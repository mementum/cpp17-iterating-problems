#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import os.path
import re
from typing import Any

from .. import TARGET

CODEBLOCK_BEGIN = r"```cpp"
CODEBLOCK_END = r"```"
GHUB = "GitHub"
GHPATH = "/blob/main/code/"
GHLNUM = "#L"
TARGET_BLANK = "{target=_blank}"
DIV_OPEN = "/// html | div[style='text-align: center;']"
DIV_CLOSE = "///"
GR_TITLE = TITLE = "title"
GR_TITLETXT = "titletxt"
EMPTY_LINE = []
EMPTY_STR = ""
ADOC_INC = "include::"
GR_IGNORE = "ignore"
GR_FILENAME = "filename"
GR_LNUM = "lnum"
GR_BLOCKBEGIN = "blockbegin"
LINESNUM = "linesnum"
LINES = "lines"
RANGE_BEGIN = "1"
RANGE_END = "-1"

# expects a block (title is optional)
# ```cpp title="some title"
# code
# ```
# For MD2AD, it removes the title, if any, and translates it to an asciidoc title, i.e.
# .some title
#
# The rest is left untouched
# For other targets nothing is done, except preventing line processors from touching
# the block

class BlockCPP:
    # all targets are valid. this is to avoid line processors manipulating
    # anything inside the cpp block
    block_begin_re: str = CODEBLOCK_BEGIN + r'(\s+title="(?P<title>[^"]+)")?'
    block_end: str = CODEBLOCK_END
    normalize: bool = False
    lineproc: bool = False

    def __call__(self, lines: list[str], match_begin: Any, target: int) -> list[str]:
        if target != TARGET.MD2AD:  # keep original content in other modes
            return lines

        # MD2AD place the title before the block and replace with untitled block
        olines = [f".{title}"] if (title := match_begin.group("title")) else []
        return (olines + [CODEBLOCK_BEGIN] + lines[1:])


# expects a block (title and linsnum are optional)
# (single file inclusiong)
# ```cpp title="some title" linesnum="number"
# --8<-- "filename:8:32"
# ```
# or (multiple file inclusiong)
# ```cpp title="some title" linesnum="number"
# --8<--
# filename1:a:b
# filename2:c:d
# ...
# --8<--
# ```
# The line ranges for the included files are optional
#
# A title is generated with the name of the 1st included file, keeping the line range
# In case of MD2AD the title is placed as ".the title" before the block
# and the snippet inclusion is translated to an asciidoc inclusion macro
# For other cases the content is kept as is, modifying the inclusion path (need to allow
# non-relative path inclusion in the snippet extension

COLON = ":"
COMMA = ","
SEMICOLON = ";"
TWODOTS = ".."

SOURCE_DIR = "sourcedir"
REPO_URL = "repo_url"

class BlockCPPInclude:
    targets: int = -TARGET.AD2AD

    SNIPPET = r'(?:\A-+8<-+)'
    block_begin_re: str = (
        rf'(?P<{GR_BLOCKBEGIN}>{CODEBLOCK_BEGIN}(,\w+)?)'
        r'('
        rf'(\s+(?P<{GR_TITLE}>{TITLE})(="(?P<{GR_TITLETXT}>[^"]+)")?)'
        r'|'
        rf'(\s+(?P<{GR_LNUM}>{LINESNUM})="[0-9]+")'
        r')*'
    )
    keep_begin: bool = False
    block_confirm_re: str = (
        rf'{SNIPPET}'
        rf'(?P<{GR_IGNORE}>{SEMICOLON})?(\s+"(?P<{GR_FILENAME}>[^"]+)")?'
    )
    keep_confirm = True
    block_end: str = CODEBLOCK_END
    normalize: bool = False
    lineproc: bool = False

    @staticmethod
    def file_linenums(filename: str) -> tuple[str, str]:
        adocranges = ""
        dranges = []
        filename, *rest = filename.split(COLON, maxsplit=1)
        for range in (rest and rest[0].split(COMMA)):
            x, *y = range.split(COLON, maxsplit=1)
            dranges.append((x or RANGE_BEGIN, (y and y[0]) or RANGE_END))

        if dranges:
            aranges = SEMICOLON.join(f"{x}{TWODOTS}{y}" for x, y in dranges)
            adocranges = f"{LINES}={aranges}"

        return filename, adocranges

    def __call__(
            self, lines: list[str],
            match_begin: Any, match_confirm: Any,
            adocvars: dict[str, str], target: int,
    ) -> list[str]:
        olines = []

        # if "title has been specified but with no title text, add it if possible"
        title = match_begin.group(GR_TITLE)
        titletxt = match_begin.group(GR_TITLETXT)
        xtitle = ""
        if target == TARGET.MD2AD:
            if titletxt:
                olines += [f".{titletxt}"]
            else:
                olines += [""]  # to add auto-title below if possible

            # if linenums is present, append it with a , to ```cpp
            # kramdoc will ignore the , and linenums will be an
            # attribute for the source code block, activating linenums
            bbegin = match_begin.group(GR_BLOCKBEGIN)
            if lnum := match_begin.group(GR_LNUM):
                bbegin = f"{bbegin},{lnum}"

            olines += [bbegin]  # only block begin
        else:
            olines += [match_begin.string]  # for the non 2AD cases, keep full line

        # single line filename --8<-- "filename"
        # filename always quotes
        # may be ignore with a semicolon after the marker
        # can contain comma separated ranges "filename:ranges"
        # The last index is included, index starts at 1
        # :3 => 1:3
        # 1:3 => 1:3
        # 3 or 3: => 3:-1
        if filename := match_confirm.group(GR_FILENAME):
            if not match_confirm.group(GR_IGNORE):
                if target == TARGET.MD2AD:
                    adfilename, adocranges = self.file_linenums(filename)
                    olines += [f"{ADOC_INC}{adfilename}[{adocranges}]"]
                else:
                    sourcedir = adocvars[SOURCE_DIR]
                    include = match_confirm.string.replace(f"{{{SOURCE_DIR}}}", sourcedir)
                    olines += [include]

                if not titletxt and title:
                    dirname = os.path.dirname(filename)
                    _, lastdir = os.path.split(dirname)
                    basename = os.path.basename(filename)
                    xtitle = os.path.join(lastdir, basename)

        else:  # no filename
            # block case, with multiple files potentially included
            # end marker is same as start marker
            if target != TARGET.MD2AD:
                olines += [lines[0]]  # confirm line: snippet for inclusion

            for line in lines[1:-1]:
                if re.match(self.SNIPPET, line):
                    if target != TARGET.MD2AD:
                        olines += [line]  # exit block
                    break

                filename = line
                if target == TARGET.MD2AD:
                    adfilename, adocranges = self.file_linenums(line)
                    if adfilename:
                        olines += [f"{ADOC_INC}{adfilename}[{adocranges}]"]
                    else:
                        olines += EMPTY_LINE
                else:
                    sourcedir = adocvars[SOURCE_DIR]
                    include = line.replace(f"{{{SOURCE_DIR}}}", sourcedir)
                    olines += [include]
                    # for multiple files we could remove the numbering from the title
                    # filename, *_ = include.split(COLON, maxsplit=1)

                if not xtitle and not titletxt and title:
                    dirname = os.path.dirname(filename)
                    _, lastdir = os.path.split(dirname)
                    basename = os.path.basename(filename)
                    xtitle = os.path.join(lastdir, basename)

        olines += lines[-1:]  # end of block - copy it
        if xtitle and True :
            olines += []
            repo = adocvars[REPO_URL]  # get repo_url from doc definition
            ghname, *ghlines = xtitle.split(COLON)
            llink = ""
            if ghlines:
                llink = f"{GHLNUM}{ghlines[0]}"

            oline = f"{GHUB}: [{xtitle}]({repo}{GHPATH}{ghname}{llink})"

            if target == TARGET.MD2AD:
                olines[0] = f".{oline}"
            else:
                olines[0] = olines[0].replace(TITLE, EMPTY_STR)
                oline += f"{TARGET_BLANK}"
                if target == TARGET.MD2MK:
                    # Add blank attibrute and center it
                    olines += EMPTY_LINE
                    olines += [DIV_OPEN]
                    olines += [oline]
                    olines += [DIV_CLOSE]
                else:
                    olines += [oline]

                olines += EMPTY_LINE

        return olines  # prevent line processing of codeblock
