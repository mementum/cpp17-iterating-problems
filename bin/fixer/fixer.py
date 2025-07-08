#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import argparse
from collections import deque
from collections.abc import Callable, Iterable, Sequence
from dataclasses import dataclass, field, KW_ONLY
import inspect
import io
import logging
from logging import debug as ldebug, info as linfo
import os
import os.path
import re
import sys
import textwrap
import typing
from typing import Any

from . import TARGET

NORMALIZE = True

# -----------------------------------------------------------------------------
PARGS = str | Sequence[str] | None

DEBUG = False

UTF8 = "utf-8"
STDIN = "-"
STDOUT = "-"
MODE_READ = "r"
MODE_WRITE = "w"

EMPTY_LINE = ""
EMPTY_STR = EMPTY_STRING = ""
NEWLINE = "\n"
SPACE = " "
MD_HARD_LINE_BREAK = "\\"
ADOC_HARD_LINE_BREAK = "+"
ADOC_HEADER_START = "="
ADOC_TITLESUB_SEP = ":"
ADOC_IFDEF = "ifdef::"
ADOC_TITLE = "title"
ADOC_SUBTITLE = "subtitle"
ADOC_VARNAME_GRP = "varname"
ADOC_VARVAL_GRP = "varval"
ADOC_VAR_RE = rF":(?P<{ADOC_VARNAME_GRP}>[^:]+):\s*(?P<{ADOC_VARVAL_GRP}>.+)?\Z"

RETURN = "return"

PROC_CONFIG = "ProcConfig"
PROC = "proc"

BLOCK_BEGIN = "block_begin"

LSEARCH_RE = "lsearch_re"
LMATCH = "lmatch"
LMATCH_RE = "lmatch_re"
LREPLACE = "lreplace"
LSUB = "lsub"
LSUB_RE = "lsub_re"

LATTRS = [LMATCH, LMATCH_RE, LREPLACE, LSUB, LSUB_RE]

CALLATTR = "__call__"
TARGETS_ATTR = "targets"
DEBUG_ATTR = "debug"
BOOKLANG_ATTR = "blang"

TARGET_LANG_ENV = "TLANG"


# -----------------------------------------------------------------------------
@dataclass
class Fixer:
    # Init
    _: KW_ONLY
    pargs: argparse.Namespace  # parsed arguments
    target: int
    processors: list[tuple[Callable, bool]] = tuple()
    line_proc: list[tuple[Callable, bool]] = tuple()

    # Non-init definitions
    linenum: int = field(init=False, default=-1)

    # regular output lines
    olines: list[str] = field(init=False, default_factory=list)
    # output lines for finalize
    flines: list[str] = field(init=False, default_factory=list)

    # whether a blank line is separating input. True at the beginning
    blank_line: bool = field(init=False, default=True)

    # lines returned to the buffer for further processing
    ret_lines: deque = field(init=False, default_factory=deque)

    # if finalize is blocked due to header processing
    # where an intermediate blank line will ruin the header
    do_not_finalize: bool = field(init=False, default=False)

    match_begin: Any = field(init=False, default=None)
    match_confirm: Any = field(init=False, default=None)
    match_end: Any = field(init=False, default=None)
    lmatch: Any = field(init=False, default=None)

    adocvars: dict[str] = field(init=False, default_factory=dict[str])
    adocheaderdone: bool = field(init=False, default=False)
    linenum: int = field(init=False, default=0)

    postlines: list[str] = field(init=False, default_factory=list[str])
    filenum: int = field(init=False, default=0)

    def __post_init__(self) -> None:
        self.blang = os.getenv(TARGET_LANG_ENV, None)

    def set_adocvar(self, name: str, val: str) -> None:
        self.adocvars[name] = val

    def get_adocvar(self, name: str, default: str | None = None) -> str:
        if default is not None:
            return self.adocvars.get(name, default)

        return self.adocvars[name]

    def output_lines(self) -> list[str]:
        return self.olines

    def output_string(self) -> str:
        return NEWLINE.join(self.output_lines())

    def proc_line(self, proc: Any, line: str) -> str | list[str]:
        lorig = line  # keep copy

        self.lmatch = None

        if (targets := getattr(proc, TARGETS_ATTR, None)):
            if not self.check_target(targets):
                return line

        if not (isclass := inspect.isclass(proc)):  # no class ... assume callable
            for lattr in LATTRS:
                if hasattr(proc, lattr):
                    break  # else will be ignored
            else:
                # none of the attributes found ...
                return self.call_block(proc, line)  # else let exception

        lsearch_re = lmatch_re = lsub_re = lproc_re = EMPTY_STR

        # it must be a class
        if debug := getattr(proc, DEBUG_ATTR, False):
            linfo("=" * 30)
            linfo(f"{str(proc)}")

        # it must be a class - see if a match was requested before processing
        if lsearch_re := getattr(proc, LSEARCH_RE, EMPTY_STR):
            if debug:
                linfo(f"{lsearch_re = }")
            # check if per-lang dict
            if isinstance(lsearch_re, dict):
                lsearch_re = lsearch_re.get(self.blang, lmatch.get(None, ""))
            if isinstance(lsearch_re, str):
                if not (mobj := re.search(lsearch_re, line)):  # only str considered in _re
                    return line

                self.lmatch = mobj

            elif not self.call_block(lsearch_re, line):  # if not str ... callable
                return line

        elif lmatch := getattr(proc, LMATCH, EMPTY_STR):
            if debug:
                linfo(f"{lmatch = }")
            # check if per-lang dict
            if isinstance(lmatch, dict):
                lmatch = lmatch.get(self.blang, lmatch.get(None, ""))
            if isinstance(lmatch, str):
                if lmatch not in line:
                    return line
            elif not self.call_block(lmatch, line):  # if not str ... callable
                return line
        elif lmatch_re := getattr(proc, LMATCH_RE, EMPTY_STR):
            if debug:
                linfo(f"{lmatch_re = }")
            # check if per-lang dict
            if isinstance(lmatch_re, dict):
                lmatch_re = lmatch_re.get(self.blang, lmatch_re.get(None, ""))

            if not (mobj := re.match(lmatch_re, line)):  # only str considered in _re
                return line

            self.lmatch = mobj

        # there was a match or no match was requested
        # lreplace can be a dict with per-languange replacements, check it
        lreplace = getattr(proc, LREPLACE, EMPTY_STR)
        if debug:
            linfo(f"{lreplace = }")

        if isinstance(lreplace, dict):
            lreplace = lreplace.get(self.blang, lreplace.get(None, ""))

        if lreplace:
            if lsub := getattr(proc, LSUB, EMPTY_STR):
                # check if per-lang dict
                if debug:
                    linfo(f"{lsub = }")
                if isinstance(lsub, dict):
                    lsub = lsub.get(self.blang, lsub.get(None, ""))
                if isinstance(lsub, str):
                    line = line.replace(lsub, lreplace)
                else:
                    line = self.call_block(lsub, lreplace, line)
            elif lsub_re := getattr(proc, LSUB_RE, EMPTY_STR):
                if debug:
                    linfo(f"{lsub_re = }")
                # check if per-lang dict
                if isinstance(lsub_re, dict):
                    lsub_re = lsub_re.get(self.blang, lsub_re.get(None, ""))
                    if debug:
                        linfo(f"{lsub_re = }")

                if debug:
                    linfo(f"Line before: {line = }")
                line = re.sub(lsub_re, lreplace, line)
                if debug:
                    linfo(f"Line after: {line = }")

        # we have a class, check if it has a __call__ method and call it
        if CALLATTR in dir(proc):
            if self.lmatch is None:  # no match ... record if sub_re requested
                lproc_re = lsub_re

            if isclass:
                line = self.call_line_block(lproc_re, lorig, proc(), line)
            else:  # it had the attributes but not a class ... assume instance
                line = self.call_line_block(lproc_re, lorig, proc, line)

        return line  # Final default

    def add_postline(self, line: str) -> None:
        self.postlines.append(line)

    def add_line(self, line: str, lineproc: bool = True) -> None:
        lines = [line] if isinstance(line, str) else line

        if lineproc:
            for lproc in (x for x, enabled in self.line_proc if enabled):
                if (proclang := getattr(lproc, BOOKLANG_ATTR, EMPTY_STR)):
                    # only valid for a language
                    if proclang != self.blang:  # check if current language
                        continue
                newlines = []
                for line in lines:
                    line = self.proc_line(lproc, line)
                    newlines += [line] if isinstance(line, str) else line

                lines = newlines

        self.olines += lines

    def add_lines(self, lines: list[str], lineproc: bool = True) -> None:
        if lineproc:
            for line in lines:
                self.add_line(line, lineproc=lineproc)
        else:
            self.olines += lines

    def return_lines(self, lines: list[str]) -> None:
        self.ret_lines += lines

    def return_line(self, line: str) -> None:
        self.ret_lines += [line]

    def get_line_safe(self) -> str | None:
        return self.get_line(sentinel=True)

    def reget_line(self) -> str | None:
        return regotten if (regotten := self.reget_lines()) is None else regotten[0]

    def reget_lines(self, num: int = 1) -> list[str] | None:
        if len(self.olines) < num:
            return None

        regotten, self.olines = self.olines[-num:], self.olines[:-num]
        return regotten

    def unreget_line(self, line: str) -> None:
        self.olines += [line]

    def unreget_lines(self, lines: list[str]) -> None:
        self.olines += lines

    def peek_line(self) -> str | None:
        """Let the caller see the line but keep it as the next one to be processed
        the caller will not have to return if it will not process it"""
        self.return_line(line := self.get_line())
        return line

    def get_lines(self, num: int = 1) -> list[str]:
        return [self.get_line() for x in range(num)]

    def get_line(self, sentinel: bool = False) -> str | None:
        if self.ret_lines:
            return self.ret_lines.popleft()

        if (line := next(self.lineit, None)) is None:
            if sentinel:
                return None

            raise StopIteration("UNEXPECTED: REACHED END OF FILE")

        return line.rstrip()

    def check_target(self, targets: Iterable | int) -> bool:
        # check if this processor is meant for the current target.
        # if no target has been specified, all targets are valid
        if isinstance(targets, Iterable):
            if self.target not in targets:
                return False
        elif -self.target == targets:
            return False

        return True  # it is a target

    def _get_block(self, proc: Any) -> tuple[None | list[str], bool]:
        # use the arguments defined in get_block as key for the possible
        # values the processor proc could define. If not defined, used the defaults
        # defined in get_block. This makes it handy to define default behavior without
        # modifiying existing processors.
        kw = {
            name: getattr(proc, name, default) for name, default in zip(
                list(self.get_block.__annotations__)[:-1],  # skip return annot
                self.get_block.__defaults__
            )
        }

        # check if this processor is meant for the current target.
        # if no target has been specified, all targets are valid
        if (targets := kw.get(TARGETS_ATTR, None)):
            if not self.check_target(targets):
                return None, True

        return self.get_block(**kw)  # valid, target, get lines and line proc status

    def _clean_oneline(
        self,
        line: str,
        block: str | Iterable[str],
        block_re: str | Iterable[str],
        begin: bool = True,
        keep: bool = False,
    ) -> str:
        # Transform str in 1-element list, keep everything else as i
        blocks = [(isinstance(x, str) and [x]) or x for x in (block, block_re)]

        for regex, blks in enumerate(blocks):
            for blk in (x for x in blks if x):  # filter out empty strings/False ...
                if begin:
                    if not regex:
                        if line.startswith(blk):
                            line = line.removeprefix(blk).lstrip()
                            if keep:
                                line = f"{blk * keep}{line}"
                            return line

                    elif lmatch := re.match(rf"{blk}", line):
                        _, line = re.split(rf"{blk}", line)
                        line = line.lstrip()
                        if keep:
                            line = f"{lmatch.group(0)}{line}"
                        return line
                else:
                    if not regex:
                        if line.endswith(blk):
                            line = line.removesuffix(blk).rstrip()
                            if keep:
                                line = f"{line}{blk}"
                            return line

                    elif lsearch := re.search(rf"{blk}$", line):
                        # _, line = re.sub(rf"{blk}$", EMPTY_STR, line, count=1)
                        line, _ = re.split(rf"{blk}$", line, maxsplit=1)
                        line = line.rstrip()
                        if keep:
                            line = f"{line}{lsearch.group(0)}"
                        return line

        return line

    def _check_line(
        self,
        line: str,
        block: str | Iterable[str] | Callable,
        block_re: str | Iterable[str] | Callable,
        begin: bool = True,
    ) -> bool:

        blocks = [(isinstance(x, str) and [x]) or x for x in (block, block_re)]

        self.current_match = None
        for regex, blks in enumerate(blocks):
            for blk in (x for x in blks if x):  # filter out empty strings/False ...
                if callable(blk):
                    if blk(line):
                        return True
                elif not isinstance(blk, str):
                    raise ValueError(f"Unknown type for {blk = }")

                # all iterable
                if begin:
                    if not regex:
                        if line.startswith(blk):
                            return True
                    else:
                        self.current_match = re.match(blk, line)
                        if self.current_match:
                            return True
                else:
                    if not regex:
                        if line.endswith(blk):
                            return True
                    else:
                        self.current_match = re.search(rf"{blk}$", line)
                        if self.current_match:
                            return True

        return False

    def get_block(
        self,
        block_begin: str | Iterable[str] | Callable | None = EMPTY_STR,
        block_begin_re: str | Iterable[str] = EMPTY_STR,
        keep_begin: bool = True,
        block_confirm: str | Callable = EMPTY_STR,
        block_confirm_re: str | Iterable[str] = EMPTY_STR,
        keep_confirm: bool = True,
        confirm_content: bool = False,
        block_end: str | Iterable[str] | Callable = EMPTY_STR,
        block_end_re: str | Iterable[str] = EMPTY_STR,
        block_end_rex: str = EMPTY_STR,
        keep_end: bool = True,
        block_oneline: bool = False,
        skip: int = 0,
        strip_blanks: bool = True,
        contentdent: bool = False,  # to dedent content als
        debug: bool = False,  # debug block actions
        targets: list[int] = [],
        normalize: bool = True,
        lineproc: bool = True,
    ) -> tuple[list[str] | None, bool]:

        line = self.current_line  # get current line (block_begin)
        wspaces = self.current_wspaces  # get its leading indentation
        lstripped = self.current_lstripped  # get line without leading indentation

        self.one_line = False
        self.match_begin = None
        self.match_confirm = None
        self.match_end = None

        if debug:
            linfo("=" * 70)
            linfo(f"{lstripped = }")
            linfo(f"{wspaces = }")
            linfo(f"{block_begin = }")
            linfo(f"{block_begin_re = }")

        if block_begin is None:
            pass  # flag for custom block proc callable: it already has the 1st line
        elif not self._check_line(lstripped, block_begin, block_begin_re):
            return None, lineproc

        self.match_begin = self.current_match

        if block_end_rex:
            block_end_re = self.match_begin.expand(block_end_rex)

        # Shortcut if the block may be a one-liner
        # there is no need to confirm and no need to keep end
        if block_oneline:  # needs single-line dedent
            if self._check_line(lstripped, block_end, block_end_re, begin=False):
                # confirmed block end, clean begin and end, i.e:
                # l/rstrip the content and remove block/begin-end marks if requested
                lstripped = self._clean_oneline(
                    lstripped, block_end, block_end_re, begin=False, keep=keep_end
                )
                # There was a block_begin[_re], no need to check. Do either/or
                lstripped = self._clean_oneline(
                    lstripped, block_begin, block_begin_re, begin=True, keep=keep_begin
                )

                self.one_line = True
                return [lstripped], lineproc

        line_confirm = None
        # Check block_confirm line accounting for indentation
        if block_confirm or block_confirm_re:
            conf_indent = wspaces * (not confirm_content)
            plstripped = self.peek_line()[conf_indent:]
            if not self._check_line(plstripped, block_confirm, block_confirm_re):
                return None, lineproc

            self.match_confirm = self.current_match
            line_confirm = self.get_line()  # remove from queue and store it

        # lines are gathered without removing any indentation.
        # Before returning textwrap.dedent will remove any indentation
        # Indentation readded by block processor manager
        olines = []
        boc = 0  # begin of real content

        if keep_begin:
            olines += [line]
            boc += 1

        if line_confirm is not None and keep_confirm:
            olines += [line_confirm]
            boc += not confirm_content

        isblank = False  # Status of first line is for sure not blank
        lblanks = []  # keep last blank lines

        _ = self.get_lines(skip)  # skip as many lines as needed

        blend_mark = bool(block_end_re or block_end)  # look for block_end(_re) mark

        # if no block_end is defined, there could be a block at the end of the file and
        # an exception would be raised on EOF. With the sentinel set to true in this
        # case, a None will be returned on EOF. A block with begin/end marks has to raise
        # an exception if EOF is reached before seeing the block_end mark
        normline = ""
        while (line := self.get_line(sentinel=not blend_mark)) is not None:
            if debug:
                linfo(f"block loop {line = }")

            lastblank = isblank  # record last line blank status

            if isblank := (not line.lstrip()):
                # store intervening blanks, or initial blanks unless skipping blanks
                if normline:
                    olines += [normline]
                    normline = EMPTY_STR

                if len(olines) > boc or not strip_blanks:
                    lblanks += [line]

                continue

            if blend_mark:
                # detect it at the same line height as the opening one
                lleft, lright = line[:wspaces], line[wspaces:]

                # block_begin/block_end share indentation, no part of block to the left
                if lleft and not lleft.isspace():
                    blend_error = block_end_re or block_end
                    raise ValueError(
                        f"Awaiting {blend_error = }. Found non-whitespace where "
                        f"indentation should be present:\n{line = }"
                    )

                if self._check_line(lright, block_end, block_end_re):
                    self.match_end = self.current_match
                    if not strip_blanks and lblanks:
                        olines += lblanks
                    if normline:
                        olines += [normline]
                    if keep_end:
                        olines += [line]

                    break  # finish looping

            elif lastblank:  # block end detection after intervening blank
                if debug:
                    linfo(f"{lastblank = }")
                    linfo(f"{lblanks = }")

                if (lleft := line[: wspaces + 1]) and not lleft.isspace():
                    # a non-whitespace character is present after a blank ... end
                    if normline:
                        olines += [normline]

                    if not strip_blanks:
                        olines += lblanks[:-1]  # last blank was for detection

                    try:
                        # ret last blank (if any) + cur line
                        retblanks = lblanks[-1:]
                        if not retblanks and not blend_mark:
                            retblanks = [EMPTY_LINE]

                        self.return_lines(retblanks + [line])
                    except Exception as e:
                        print(f"{olines =}")
                        raise e

                    break  # finish looping

            if normline:
                normline += SPACE + lright  # part after indentation if any
                if normline.endswith(MD_HARD_LINE_BREAK):  # hard-break -- must store
                    olines += [normline]
                    normline = EMPTY_STR

                continue

            # line not blank, not block_end and no end detected. Store it
            if lblanks:  # first ... any intervening blank lines
                olines += lblanks
                lblanks = []

            if self.target != TARGET.AD2AD or not normalize:
                olines += [line]  # store line in output buffer
            else:
                # normline is empty
                normline = line  # keep left indentation for dedent
                if normline.endswith(MD_HARD_LINE_BREAK):  # hard-break -- must store
                    olines += [line]  # keep indentation of first line
                    normline = EMPTY_STR

        # dedent everything to return a block which can be processed by the caller
        # without worrying about whitespace
        odented = textwrap.dedent(NEWLINE.join(olines))
        olines = odented.splitlines()

        # once everything is dedented we can think about dedenting the content if so
        # requested, or else we would have dedented content with the begin/end blocks
        # still indented and no longer dedentable as a whole after this block
        if contentdent:  # requested dedented content
            # boc was precalulated above
            # calculate eoc to leave the block_end out if there and "keep"
            eoc = -1 if (blend_mark and keep_end) else len(olines)
            # separate begin + content + end and dedent only content
            olines = (
                olines[:boc]
                + textwrap.dedent(NEWLINE.join(olines[boc:eoc])).splitlines()
                + olines[eoc:]
            )

        return olines, lineproc

    def finalize(self) -> None:
        # add empty line to allow chaining and closing the adoc_header
        if self.do_not_finalize:
            return

        self.add_line(EMPTY_LINE)

        if self.flines:  # see if extra lines are needed
            self.add_lines(self.flines)
            self.flines = []  # clear the extra output buffer

        # Separate streams, as the header is usually on the 1st line of the next
        # and it could be directly added to the last paragraph if an empty line
        # is not inserted. Blank lines are just ignored whitespace in markdown and
        # asciidoc
        self.add_line(EMPTY_LINE)

    def proc_adoc(self) -> bool:
        if self.adocheaderdone:
            return False

        line = self.current_line
        if not line:
            return True

        if line[0] == ADOC_HEADER_START:
            _, title_sub = line.split(ADOC_HEADER_START, maxsplit=1)
            title, subtitle = title_sub.split(ADOC_TITLESUB_SEP, maxsplit=1)
            self.set_adocvar(ADOC_TITLE, title.strip())
            self.set_adocvar(ADOC_SUBTITLE, subtitle.strip())
            return True

        # match for a variable ... store it
        if (vmatch := re.match(ADOC_VAR_RE, line)):
            self.set_adocvar(
                vmatch.group(ADOC_VARNAME_GRP),
                vmatch.group(ADOC_VARVAL_GRP) or EMPTY_STR
            )
            return True

        if line.startswith(ADOC_IFDEF):
            return True

        # line contains something and it is not a match for us. Let it be processed
        self.adocheaderdone = True  # set marker for future calls
        return False

    def process_file(self, lineit: Iterable, filenum: int) -> None:
        self.filenum = filenum
        self.lineit = lineit  # get_line will use this iterator
        blank_line = True  # before the document, everything is "blank"
        block_ended = False  # new block can start right after previous ended

        normlines = []  # for lines to be normalized

        while (line := self.get_line_safe()) is not None:
            self.current_line = line
            self.current_lstripped = lstripped = line.lstrip()
            self.current_wspaces = wspaces = len(line) - len(lstripped)
            self.linenum += 1

            if self.proc_adoc():
                if self.target == TARGET.AD2AD:
                    self.add_line(line, lineproc=False)
                continue  # get next line inmmediately

            prev_blank_line = blank_line  # get from last
            if not (blank_line := not lstripped) and self.target == TARGET.AD2AD:
                blank_line = line == ADOC_HARD_LINE_BREAK

            block_start = (prev_blank_line or block_ended) and not blank_line
            block_ended = False

            if not block_start:  # paragraph or not processed block
                if NORMALIZE and self.target != TARGET.AD2AD:
                    # a blank line will be added, so check if anything has been buffered
                    # to be normalized, normalize and and the blank line
                    if blank_line:
                        if normlines:
                            self.add_line(SPACE.join(normlines))
                            normlines = []

                        self.add_line(line)
                    else:
                        # no block-start and non-blank, would get added.
                        # Buffer it for normalization
                        normlines += [lstripped] if normlines else [line]
                        if line.endswith(MD_HARD_LINE_BREAK):
                            self.add_line(SPACE.join(normlines))
                            normlines = []

                else:
                    # for non-ad2ad targets, use regular logic ... add_line
                    self.add_line(line)

                continue

            for proc in (x for x, enabled in self.processors if enabled):
                retlines, lineproc = self.proc_block(proc)
                if retlines is not None:
                    block_ended = True  # indicate a block has been taken
                    rettxt = NEWLINE.join(retlines)
                    retindented = textwrap.indent(rettxt, SPACE * wspaces)
                    self.add_lines(retindented.splitlines(), lineproc=lineproc)
                    break  # line taken, break skips else
            else:
                # no processor took the line to define a block
                if NORMALIZE and self.target != TARGET.AD2AD:
                    # if non-ad2ad target non-empty lines may follow this non-taken line.
                    # store it in the normalization buffer
                    normlines += [line]
                    if line.endswith(MD_HARD_LINE_BREAK):
                        self.add_line(SPACE.join(normlines))
                        normlines = []
                else:
                    # regular logic, add it
                    self.add_line(line)  # no processor took it ... store the line

        # if anything is left to normalize do it. No need to check target. Because it
        # will only contain something if the target was right above.
        if normlines:
            self.add_line(SPACE.join(normlines))
            normlines = []  # superflous

        # Add any extra lines
        if self.postlines:
            self.add_line(EMPTY_LINE, lineproc=False)  # add initial separation line
            self.add_lines(self.postlines, lineproc=False)  # add postline, no proc
            self.postlines = []  # clear buffer

    def get_kwargs(self, func: Callable) -> dict:
        KWARGS_FULL = {
            "parent": self,
            "linenum": self.linenum,
            "oneline": getattr(self, "one_line", False),
            "wspaces": self.current_wspaces,
            "rawline": self.current_line,
            "match_begin": self.match_begin,
            "match_confirm": self.match_confirm,
            "match_end": self.match_end,
            "lmatch": self.lmatch,
            "adocvars": self.adocvars,
            "target": self.target,
            "postlines": self.postlines,
            "filenum": self.filenum,
        }
        kwargs_to_pass = {}

        for name, param in inspect.signature(func).parameters.items():
            if param.kind == param.VAR_KEYWORD:
                kwargs_to_pass = KWARGS_FULL
                break

            if param.kind in [param.POSITIONAL_OR_KEYWORD or param.KEYWORD_ONLY]:
                if name in KWARGS_FULL:
                    kwargs_to_pass[name] = KWARGS_FULL[name]

        return kwargs_to_pass

    def proc_block(self, proc: Any) -> tuple[None | list[str], bool]:
        # auto-get lines and further line processsin
        lines, lineproc = self._get_block(proc)
        if lines is not None:  # on positive answer
            block_proc = None  # check if a __call__ is possible to postprocesslines
            if inspect.isclass(proc):  # use dir to see if an instance has __call__
                if CALLATTR in dir(proc):
                    block_proc = proc()
            elif hasattr(proc, BLOCK_BEGIN):  # not class but defines it: instance
                if hasattr(proc, CALLATTR):
                    block_proc = proc

            if block_proc is not None:  # use the calculated block_proc
                lines = self.call_block(block_proc, lines)

        return lines, lineproc  # return (post)-processed lines + line processing status

    def call_line_block(
        self,
        lproc_re: str,
        lorig: str,
        func: Callable,
        line,
        *args,
    ) -> None | list[str] | tuple[None | list[str], bool]:
        kw = self.get_kwargs(func)

        # if lproc_re is there, self.lmatch was None and we've got lsub_re
        if  lproc_re and LMATCH in kw:
            kw[LMATCH] = self.lmatch = re.search(lproc_re, lorig)
            if self.lmatch is None:  # no match, nothing to do here
                return line

        return func(line, **kw)

    def call_block(
        self,
        func: Callable,
        *args,
    ) -> None | list[str] | tuple[None | list[str], bool]:
        return func(*args, **self.get_kwargs(func))


# -----------------------------------------------------------------------------
def open_infile(infile: str) -> typing.TextIO:
    if infile == STDIN:
        sys.stdin.reconfigure(encoding=UTF8)
        return sys.stdin

    return io.open(infile, mode=MODE_READ, encoding=UTF8)


# -----------------------------------------------------------------------------
def open_outfile(outfile: str) -> typing.TextIO:
    if outfile == STDOUT:
        sys.stdout.reconfigure(encoding=UTF8, newline=NEWLINE)
        return sys.stdout

    return io.open(outfile, mode=MODE_WRITE, encoding=UTF8)


# -----------------------------------------------------------------------------
def run(
    target: int,
    processors: list[None | tuple[Callable, bool]] = None,
    line_proc: list[None | tuple[Callable, bool]] = None,
    args: PARGS = None,
) -> None:

    processors = processors or tuple()
    line_proc = line_proc or tuple()

    pargs = parse_args(args)

    # create a fixer indicating which translation
    fixer = Fixer(pargs=pargs, processors=processors, line_proc=line_proc, target=target)

    # initialize control token to "finalize"
    last_base_name = os.path.splitext(os.path.basename(pargs.infiles[0]))[0]

    if pargs.in_place:
        if len(pargs.infiles) > 1:
            logging.error("Can only edit 'in-place' for single file input")
            sys.exit(1)

        pargs.outfile = pargs.infiles[0]

    # and loop over the files
    for filenum, infile in enumerate(pargs.infiles):
        ldebug(f"Processing: {filenum = }: {infile = }")
        # check if the file still has the same stem (name with no extension) as the
        # previous. If that is the case finalize will not be called
        # Only when the stem changes, the previous block will be "finalize"d
        base_name_infile = os.path.splitext(os.path.basename(infile))[0]
        if base_name_infile != last_base_name:
            last_base_name = base_name_infile
            fixer.finalize()  # previous iteration

        try:
            with open_infile(infile) as ifile:
                fixer.process_file(iter(ifile), filenum)

        except IOError as e:
            logging.error(f"Failed to open file: '{infile}'")
            logging.error(f"Reason: {e}")
            sys.exit(1)

    else:
        fixer.finalize()  # for the last item in the list

    try:
        with open_outfile(pargs.outfile) as ofile:
            ofile.write(fixer.output_string())

    except IOError as e:
        logging.error(f"Failed to open file: '{pargs.outfile}'")
        logging.error(f"Reason: {e}")
        sys.exit(1)


# ---------------------------------------------------------------------------
def parse_args(args: PARGS = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description=("Asciidoc metadata to pandoc frontmatter"),
    )

    pgroup = parser.add_argument_group(title="Input")
    pgroup.add_argument("infiles", nargs="*", default=[STDIN], help="Input file(s)")

    pgroup = parser.add_argument_group(title="Output")
    pgroup.add_argument(
        "--in-place",
        "-i",
        action="store_true",
        help="Edit file in place (only for single file input)",
    )
    pgroup.add_argument(
        "--outfile",
        "--output",
        "-o",
        default=STDOUT,
        help="Output file (defaults to stdout)",
    )

    pgroup = parser.add_argument_group(title="Verbosity")
    pgroup.add_argument(
        "--logout",
        "-lo",
        action="store_true",
        help="Log to stdout instead of stderr",
    )
    pgrpex = pgroup.add_mutually_exclusive_group()
    pgrpex.add_argument("--no-logging", action="store_true", help="Report nothing")
    pgrpex.add_argument("--error", action="store_true", help="Do only report errors")
    pgrpex.add_argument(
        "--info",
        "-info",
        action="store_true",
        help="Info level (DEFAULT)",
    )
    pgrpex.add_argument(
        "--debug",
        "--verbose",
        "-v",
        action="store_true",
        help="Increase verbosity level",
    )

    if isinstance(args, str):
        args = args.split()

    pargs = parser.parse_args(args)

    # configure logging
    if not pargs.no_logging:
        vlevel = logging.INFO
        if pargs.error:
            vlevel = logging.ERROR
        elif pargs.debug:
            vlevel = logging.DEBUG
            global DEBUG
            DEBUG = True

        stream = sys.stderr if not pargs.logout else sys.stdout
        logging.basicConfig(
            format="%(asctime)s,%(levelname)-8s,%(message)s",
            level=vlevel,
            stream=stream,
            datefmt="%Y-%m-%d %H:%M:%S",
        )

    return pargs


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    run()
