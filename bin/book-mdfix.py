#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import argparse
from collections.abc import Sequence
from dataclasses import dataclass, field, KW_ONLY
import io
import logging
from logging import debug as ldebug
import re
import os.path
import sys
import typing

import dateparser

from upzip import UpdateableZipFile

# -----------------------------------------------------------------------------
PARGS = str | Sequence[str] | None

DEBUG = False

UTF8 = "utf-8"

UNNUMBERED = ".unnumbered"
TYPE_UNNUMBERED = ["dedication", "preface", "colophon", "appendix"]

UNLISTED = ".unlisted"
TYPE_UNLISTED = []

APPENDIX = "appendix"
APPENDIX_TITLE = APPENDIX.capitalize()
APPENDIX_CAPTION = "appendix-caption"
APPENDIX_START = "A"


EPUB_TYPE = "epub:type"
EPUB_TYPE_SEP = "="

PART = "part"

SECTNUMLEVELS = "sectnumlevels"
NUMBERSECTIONS = "number-sections"

SECTNUMS = "sectnums"
SECTNUMS_ON = ":sectnums:"
SECTNUMS_OFF = ":sectnums!:"

LANG = "lang"
LANG_DEFAULT = "en"

TOC = "toc"
TOC_DEFAULT = False
TOC_DEPTH_ADOC = "toclevels"
TOC_DEPTH = "toc-depth"
TOC_DEPTH_DEFAULT = "4"

EMPTY_LINE = ""

HEADER_PREFIX = "#"
ATTRS_OPEN = "{"
ATTRS_CLOSE = "}"

NO_ATTRS = ""

HEADER_ID_PREFIX = "#"
NO_HEADER_ID = ""

YAML_MARKER = "---"
ADOC_HEADER_MARKER = "= "
ADOC_HEADER_SEP = ":"
ADOC_BOOL_FALSE = "!"
ADOC_HEADER_IFDEF = "ifdef"

ADOC_TITLE = "title"
ADOC_SUBTITLE = "subtitle"
ADOC_AUTHOR = "author"
ADOC_DATE = "revdate"
YAML_DATE = "date"

VAR_AUTHOR = "{{author}}"
VAR_REVDATE = "{{revdate}}"

STDIN = "-"
STDOUT = "-"

# ![ seen with no closing ]
FOLDED_IMAGE_LINK_1 = r".*!\[[^\]]+$"
# a closing ] followed by (content) where content has no whitespace
FOLDED_IMAGE_LINK_2 = r".*\]\([^\s]+\)"

# To fix epub2 date requirementes - group 2 to be replaced with YYYY-MM
CONTENT_OPF = "EPUB/content.opf"
OPF_DATE_RE = r'(\s*<dc:date id=".+">)(.*)(</dc:date>)'
OPF_DATE_FMT = "%Y-%m"


# -----------------------------------------------------------------------------
@dataclass
class MD_Fixer:
    # Init
    _: KW_ONLY
    pargs: argparse.Namespace  # maximum level to allow numbering

    # Non-init definitions
    adoc: bool = field(init=False, default=False)  # maximum level to allow numbering
    pdefaults: str = field(init=False, default="")  # maximum level to allow numbering

    linenum: int = field(init=False, default=-1)

    yamlblock: bool = field(init=False, default=False)
    yamlvars: dict = field(init=False, default_factory=dict)

    adoc_header: bool = field(init=False, default=False)

    top_level: bool = field(init=False, default=False)
    # regular output lines
    olines: list[str] = field(init=False, default_factory=list)
    # output lines for finalize
    flines: list[str] = field(init=False, default_factory=list)

    # counter for appendices to modify the title
    app_idx: str = field(init=False, default=ord(APPENDIX_START) - 1)

    # unnumbering stack
    unnumbering: list[int] = field(init=False, default_factory=lambda: [0])

    # whether a blank line is separating input. True at the beginning
    blank_line: bool = field(init=False, default=True)

    # whether the first part of a folded image link has been seen
    folded_image_link: bool = field(init=False, default=False)

    # if finalize is blocked due to header processing
    # where an intermediate blank line will ruin the header
    do_not_finalize: bool = field(init=False, default=False)

    # -------------------------------------------------------------------------
    def __post_init__(self) -> None:
        self.adoc = not self.pargs.pdefaults
        self.pdefaults = self.pargs.pdefaults

    # -------------------------------------------------------------------------
    # property for quick and easy access of yamlvars values
    # -------------------------------------------------------------------------
    @property
    def sectnums(self) -> bool:
        return self.yamlvars.get(SECTNUMS, False)

    @property
    def sectnums_var(self) -> int:
        return SECTNUMS_ON if self.sectnums else SECTNUMS_OFF

    @property
    def sectnumlevels(self) -> int:
        return int(self.yamlvars.get(SECTNUMLEVELS, 3))

    @property
    def numbersections(self) -> bool:
        return self.sectnums

    @property
    def toc(self) -> bool:
        return self.yamlvars.get(TOC, TOC_DEFAULT)

    @property
    def toc_depth(self) -> str:
        return self.yamlvars.get(TOC_DEPTH_ADOC, TOC_DEPTH_DEFAULT)

    @property
    def lang(self) -> str:
        return self.yamlvars.get(LANG, LANG_DEFAULT)

    @property
    def adoc_date(self) -> str:
        return self.yamlvars[ADOC_DATE]

    @property
    def adoc_title(self) -> str:
        return self.yamlvars[ADOC_TITLE]

    @property
    def adoc_subtitle(self) -> str:
        return self.yamlvars[ADOC_SUBTITLE]

    @property
    def adoc_author(self) -> str:
        return self.yamlvars[ADOC_AUTHOR]

    @property
    def appendix_caption(self) -> str:
        appendix_title = self.yamlvars.get(APPENDIX_CAPTION, APPENDIX_TITLE)
        self.app_idx += 1
        return f"{appendix_title} {chr(self.app_idx)}: "

    # -------------------------------------------------------------------------
    # the header format is: # title {#id .class epub:type=x}
    # where everything but the title is optional
    # the kramdoc (md => asciidoc) does only understand the #id and pass the others
    # unmodified
    # this method splits the header and returns a tuple
    #   - int: heading level
    #   - str: title (without the leading "# "
    #   - str: #id (with the leading "id")
    #   - list[str]: the other attributes inside {} (.class epub:type=x)
    # -------------------------------------------------------------------------
    def split_header(self, line: str) -> tuple[int, str, str, list[str]]:
        # An ending } indicates there are attributes, else only a title is there
        hhashes, header = line.split(maxsplit=1)

        if not line.endswith(ATTRS_CLOSE):
            title, s_attrs = header, NO_ATTRS
        else:
            # after seeing a terminating }, assume there is a corresponding opening
            # {. If not theis will blow up forcing the wrongly coded attrs to be fixed
            title, s_attrs = header.rstrip(ATTRS_CLOSE).rsplit(ATTRS_OPEN, maxsplit=1)

        # break the string of attrs in tokens
        attrs = s_attrs.split()
        hid = NO_HEADER_ID
        if attrs and attrs[0].startswith(HEADER_ID_PREFIX):
            hid, *attrs = attrs

        return len(hhashes), title, hid, attrs

    # -------------------------------------------------------------------------
    def parse_date(self, datestring: str, lang: str) -> str:
        dt = dateparser.parse(datestring, languages=[lang])
        return dt.strftime(OPF_DATE_FMT)

    # -------------------------------------------------------------------------
    def date_num(self) -> str:
        return self.parse_date(self.adoc_date, self.lang)

    # -------------------------------------------------------------------------
    def adoc_header_yaml(self) -> None:
        # generate the yaml header from the parsed content
        self.olines += [YAML_MARKER]

        self.olines += [f"{ADOC_TITLE}: {self.adoc_title}"]
        self.olines += [f"{ADOC_SUBTITLE}: {self.adoc_subtitle}"]
        self.olines += [f"{YAML_DATE}: {self.adoc_date}"]
        self.olines += [f"{ADOC_AUTHOR}:"]
        self.olines += [f"  - {self.adoc_author}"]

        self.olines += [YAML_MARKER]

        self.adoc_header_yaml_defaults()  # write defaults

    # -------------------------------------------------------------------------
    def adoc_header_yaml_defaults(self) -> None:
        if not self.pdefaults:
            return

        try:
            with open_outfile(self.pdefaults) as defaults_file:
                deflines = []
                deflines += [f"{NUMBERSECTIONS}: {str(self.numbersections).lower()}"]
                deflines += [f"{TOC}: {str(self.toc).lower()}"]
                if self.toc:
                    deflines += [f"{TOC_DEPTH}: {self.toc_depth}"]

                deflines += [EMPTY_LINE]
                defaults_file.write("\n".join(deflines))

        except IOError as e:
            logging.error(f"Failed to open defaults file: '{self.pdefaults}'")
            logging.error(f"Reason: {e}")
            sys.exit(1)

    # -------------------------------------------------------------------------
    def adoc_header_parse(self, line: str) -> bool:
        # in asciidoc target mode, output the line. In any mode, parse the content
        if self.adoc:
            self.olines += [line]

        if not line:
            return False

        if self.linenum == 0:
            title, subtitle = line.lstrip(ADOC_HEADER_MARKER).split(ADOC_HEADER_SEP)
            self.yamlvars[ADOC_TITLE] = title.strip()
            self.yamlvars[ADOC_SUBTITLE] = subtitle.strip()
            return True

        if self.linenum == 1:
            if not line.startswith(ADOC_HEADER_SEP):
                self.yamlvars[ADOC_AUTHOR] = line
                return True

        if self.linenum == 2:
            if not line.startswith(ADOC_HEADER_SEP):
                self.yamlvars[ADOC_DATE] = line
                return True

        if line.startswith(ADOC_HEADER_SEP):
            _, key, value = line.split(ADOC_HEADER_SEP)
            if not value:
                if key[-1] == ADOC_BOOL_FALSE:
                    key = key[:-1]
                    value = False
                else:
                    value = True

                self.yamlvars[key] = value

            else:
                self.yamlvars[key] = value.strip()

            return True

        elif line.startswith(ADOC_HEADER_IFDEF):
            # past 1st line can be an ifdef
            return True

        # some othre line, throw an exception
        raise NotImplementedError(f"Support of {line} in asciidoc header")

    # -------------------------------------------------------------------------
    # for each line there are two processing possibilites
    # 1. It is a header
    #   - split the header in title, id and attrs
    #   - it the target is asciidoc, leave only title and id (kramdoc does nothing with
    #     the others and prepend an asciidoc section type or (example) prepend/append
    #     section numbering activation/deactivation
    #   - if the target is pandoc, add unnumbered/unlisted if not present and the
    #     pub:type should be it. In the case of appendices prepend "Appendix X: ",
    #     where X starts at "A" and increases
    #
    # 2. it is a regular line
    #   - do nothing so far. store it and done
    #
    def check_line(self, line: str) -> None:
        line = line.rstrip()
        if DEBUG:
            ldebug("== Checking line ==")
            ldebug(line)
            ldebug("== Checked line == ")

        self.linenum += 1

        # Check for starting block: yaml or asciidoc. Only on first line.
        if self.linenum == 0:
            if line.startswith(YAML_MARKER):
                self.yamlblock = True
                self.olines += [line]
                return

            elif line.startswith(ADOC_HEADER_MARKER):
                self.adoc_header = True
                # let the block later parse the content

        # if a yaml block is active store things until the eo-yaml mark is seen
        if self.yamlblock:
            self.olines += [line]
            if line.startswith(YAML_MARKER):
                self.yamlblock = False
                return

            if not line or line[0].isspace():
                return

            key, val = line.split(":")
            self.yamlvars[key] = val

            return

        # if an asciidoc header block is active check for its end
        if self.adoc_header:
            self.adoc_header = self.adoc_header_parse(line)
            if not self.adoc_header:  # header block ended
                if not self.adoc:  # check if yaml has to be generated
                    self.adoc_header_yaml()  # generate yaml header

            return

        prev_blank_line = self.blank_line
        self.blank_line = not line

        # check for a folded link before anything to nullify the bool
        if self.folded_image_link:
            self.folded_image_link = False
            if re.match(FOLDED_IMAGE_LINK_2, line) is not None:
                self.olines[-1] += line
                return

        # if current line is blank do nothing
        if self.blank_line:
            self.olines += [line]
            return

        # a link can be in a line of its own or in a paragraph. check after the blank
        # line check
        if re.match(FOLDED_IMAGE_LINK_1, line) is not None:
            self.folded_image_link = True

        # check for an attribute
        if r"{\{" in line:
            line = line.replace(r"{\{", "{").replace("}}", "}")

        if not self.adoc:  # do attribute substibution, which asciidoctor does
            if VAR_AUTHOR in line:
                line = line.replace(VAR_AUTHOR, self.adoc_author)

            if VAR_REVDATE in line:
                line = line.replace(VAR_REVDATE, self.adoc_date)

        # if content was seen in the previous line
        if not prev_blank_line:
            self.olines += [line]
            return

        # current line is not blank and previous line was blank. A header can be there
        # (it needs a blank line before it)

        # check it is not a header, i.e: standard paragraph
        if not line.startswith(HEADER_PREFIX):
            self.olines += [line]
            return

        # it is a HEADER
        # look for part signifier to ignore it
        if f"{PART}}}" in line:
            self.olines += [line]
            return

        hlevel, title, hid, attrs = self.split_header(line)

        out_attrs = [] if not hid else [hid]

        epub_type_val = ""
        for attr in attrs:
            # section behavior through epub:type definition
            if attr.startswith(EPUB_TYPE):  # check for specific section type
                # break it in type/value
                _, epub_type_val = attr.split(EPUB_TYPE_SEP, maxsplit=1)

                # if a section type should carry unlisted, add it
                if epub_type_val in TYPE_UNNUMBERED:
                    if UNNUMBERED not in attrs:
                        out_attrs += [UNNUMBERED]

                # if a section type should carry unlisted, add it
                if epub_type_val in TYPE_UNLISTED:
                    if UNLISTED not in attrs:
                        out_attrs += [UNLISTED]

                if epub_type_val == APPENDIX:
                    if not self.adoc:
                        # prepend "Appendix X: " to the title.
                        title = f"{self.appendix_caption}{title}"

            out_attrs += [attr]

        # post-process numbering in out_attrs if needed
        if DEBUG:
            ldebug(f"{hlevel = } {title = } {hid = } {attrs = }")
            ldebug(f"1. {self.unnumbering = }")

        # check unnumbering level
        while True:
            unnumlevel = self.unnumbering[-1]
            if unnumlevel >= hlevel:
                self.unnumbering.pop()
            else:
                unnumlevel = self.unnumbering[-1]
                break

        # if any non-zero level is left
        if unnumlevel:
            # add unnumbering unless already present
            if UNNUMBERED not in out_attrs:
                out_attrs += [UNNUMBERED]

        # last check to see if it's need to be unnumbered
        if UNNUMBERED not in out_attrs:
            if self.sectnumlevels >= 0:
                if hlevel > self.sectnumlevels:
                    out_attrs += ['.unnumbered']

        # mark this as the current level
        if UNNUMBERED in out_attrs:
            self.unnumbering += [hlevel]

            if self.adoc:
                if epub_type_val not in TYPE_UNNUMBERED:
                    self.olines += [SECTNUMS_OFF]
        else:
            if self.adoc:
                self.olines += [self.sectnums_var]

        if DEBUG:
            ldebug(f"2. {self.unnumbering = } {out_attrs = }")

        # fabricate final header
        if self.adoc:
            if epub_type_val:
                self.olines += [f"[{epub_type_val}]"]

            if hid:  # add id
                self.olines += [f"[{hid}]"]

        oline = f"{HEADER_PREFIX * hlevel} {title}"

        if not self.adoc:
            if out_attrs:
                oline += " " + f"{ATTRS_OPEN}{' '.join(out_attrs)}{ATTRS_CLOSE}"

        self.olines += [oline]

    def finalize(self) -> None:
        # add empty line to allow chaining and closing the adoc_header
        if self.do_not_finalize:
            return

        self.check_line(EMPTY_LINE)

        if self.flines:  # see if extra lines are needed
            self.olines += self.flines
            self.flines = []  # clear the extra output buffer

        # Separate streams, as the header is usually on the 1st line of the next
        # and it could be directly added to the last paragraph if an empty line
        # is not inserted. Blank lines are just ignored whitespace in markdown and
        # asciidoc
        self.olines += [EMPTY_LINE]

    def output_lines(self) -> list[str]:
        return self.olines

    def output_string(self) -> str:
        return "\n".join(self.output_lines())


# -----------------------------------------------------------------------------
def open_infile(infile: str) -> typing.TextIO:
    if infile == STDIN:
        sys.stdin.reconfigure(encoding=UTF8)
        return sys.stdin

    return io.open(infile, mode="r", encoding=UTF8)


# -----------------------------------------------------------------------------
def open_outfile(outfile: str) -> typing.TextIO:
    if outfile == STDOUT:
        sys.stdout.reconfigure(encoding=UTF8, newline="\n")
        return sys.stdout

    return io.open(outfile, mode="w", encoding=UTF8)


# -----------------------------------------------------------------------------
def run(args: PARGS = None) -> None:
    pargs = parse_args(args)

    # create a fixer indicating which translation
    md_fixer = MD_Fixer(pargs=pargs)

    # initialize control token to "finalize"
    last_base_name = os.path.splitext(os.path.basename(pargs.infiles[0]))[0]

    # and loop over the files
    for infile in pargs.infiles:
        # check if the file still has the same stem (name with no extension) as the
        # previous. If that is the case finalize will not be called
        # Only when the stem changes, the previous block will be "finalize"d
        base_name_infile = os.path.splitext(os.path.basename(infile))[0]
        if base_name_infile != last_base_name:
            last_base_name = base_name_infile
            md_fixer.finalize()  # previous iteration

        try:
            with open_infile(infile) as ifile:
                for line in ifile:
                    md_fixer.check_line(line)

        except IOError as e:
            logging.error(f"Failed to open file: '{infile}'")
            logging.error(f"Reason: {e}")
            sys.exit(1)

    else:
        md_fixer.finalize()  # for the last item in the list

    if pargs.pfix_epub_date:
        with UpdateableZipFile(pargs.outfile) as epub:
            opf = epub.read(CONTENT_OPF).decode(UTF8)
            opflines = opf.splitlines()
            for i, opfline in enumerate(opflines):
                if (dm := re.match(OPF_DATE_RE, opfline)) is not None:
                    opflines[i] = f"{dm.group(1)}{md_fixer.date_num()}{dm.group(3)}"
                    break

            epub.writestr(CONTENT_OPF, "\n".join(opflines).encode(UTF8))

        sys.exit(0)

    try:
        with open_outfile(pargs.outfile) as ofile:
            ofile.write(md_fixer.output_string())

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

    pgroup.add_argument(
        "--outfile",
        "--output",
        "-o",
        default="-",
        help="Output file (defaults to stdout)",
    )

    pgroup = parser.add_argument_group(title="Behavior")
    pgroup.add_argument(
        "--pdefaults", default="", help="Where to store defaults for pandoca"
    )

    pgroup.add_argument(
        "--pfix-epub-date",
        action="store_true",
        help="Set date to a YYYY[-MM[-DD]] format for epub check pass",
    )

    pgroup = parser.add_argument_group(title="Verbosity")
    pgroup.add_argument(
        "--logout", "-lo", action="store_true", help="Log to stdout instead of stderr"
    )
    pgrpex = pgroup.add_mutually_exclusive_group()
    pgrpex.add_argument("--no-logging", action="store_true", help="Report nothing")
    pgrpex.add_argument("--error", action="store_true", help="Do only report errors")
    pgrpex.add_argument(
        "--info", "-i", action="store_true", help="Info level (DEFAULT)"
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
