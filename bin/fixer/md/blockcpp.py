#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import os.path
import re
from typing import Any

from .. import TARGET

CODEBLOCK_BEGIN = r"```cpp"
CODEBLOCK_END = r"```"

class BlockCPP:
    # all targets are valid. this is to avoid line processors manipulating
    # anything inside the cpp block
    block_begin_re: str = CODEBLOCK_BEGIN + r'(\s+title="(?P<title>[^"]+)")?'
    block_end: str = CODEBLOCK_END

    def __call__(self, lines: list[str], match_begin: Any, target: int) -> list[str]:
        if target != TARGET.MD2AD:  # keep original content in other modes
            return lines[:], False  # False to skip block processors

        # MD2AD place the title before the block and replace with untitled block
        olines = [f".{title}"] if (title := match_begin.group("title")) else []
        return (olines + [CODEBLOCK_BEGIN] + lines[1:]), False


DESCRIPTION = """
It takes a ```cpp code to place the title as a an asciidoc block title just before the
block which is then sent unmodified (minus the title) down the chain, avoiding line
processing to avoid double quote reprocessing
"""

COLON = ":"
COMMA = ","
SEMICOLON = ";"
TWODOTS = ".."

SOURCE_DIR = "sourcedir"

class BlockCPPInclude:
    targets: int = -TARGET.AD2AD

    SNIPPET = r'(?:\A-+8<-+)'
    block_begin_re: str = (
        rf'(?P<blockbegin>{CODEBLOCK_BEGIN}(,\w+)?)'
        r'('
        r'(\s+(?P<title>title)(="(?P<titletxt>[^"]+)")?)'
        r'|'
        r'(\s+(?P<lnum>linesnums)="[0-9]+")'
        r')*'
    )
    keep_begin: bool = False
    block_confirm_re: str = SNIPPET + rf'(?P<ignore>{SEMICOLON})?(\s+"(?P<filename>[^"]+)")?'
    keep_confirm = True
    block_end: str = CODEBLOCK_END

    @staticmethod
    def file_linenums(filename: str) -> tuple[str, str]:
        adocranges = ""
        dranges = []
        filename, *rest = filename.split(COLON, maxsplit=1)
        for range in (rest and rest[0].split(COMMA)):
            x, *y = range.split(COLON, maxsplit=1)
            dranges.append((x or "1", (y and y[0]) or "-1"))

        if dranges:
            aranges = SEMICOLON.join(f"{x}{TWODOTS}{y}" for x, y in dranges)
            adocranges = f"lines={aranges}"

        return filename, adocranges

    def __call__(
            self, lines: list[str],
            match_begin: Any, match_confirm: Any,
            adocvars: dict[str, str], target: int,
    ) -> list[str]:
        olines = []

        # if "title has been specified but with no title text, add it if possible"
        title = match_begin.group("title")
        titletxt = match_begin.group("titletxt")
        xtitle = ""
        if target == TARGET.MD2AD:
            if titletxt:
                olines += [f".{titletxt}"]
            else:
                olines += [""]  # to add auto-title below if possible

            # if linenums is present, append it with a , to ```cpp
            # kramdoc will ignore the , and linenums will be an
            # attribute for the source code block, activating linenums
            bbegin = match_begin.group("blockbegin")
            if lnum := match_begin.group("lnum"):
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
        if filename := match_confirm.group("filename"):
            if not match_confirm.group("ignore"):
                if target == TARGET.MD2AD:
                    filename, adocranges = self.file_linenums(filename)
                    olines += [f"include::{filename}[{adocranges}]"]
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
                olines += [lines[0]]  # confirm block

            for line in lines[1:-1]:
                if re.match(self.SNIPPET, line):
                    if target != TARGET.MD2AD:
                        olines += [line]  # exit block
                    break

                if target == TARGET.MD2AD:
                    filename, adocranges = self.file_linenums(line)
                    olines += [f"include::{filename}[{adocranges}]"]
                else:
                    sourcedir = adocvars[SOURCE_DIR]
                    include = line.replace(f"{{{SOURCE_DIR}}}", sourcedir)
                    olines += [include]
                    # for multiple files remove the line numbering from the title
                    filename, *_ = include.split(":", maxsplit=1)

                if not xtitle and not titletxt and title:
                    xtitle = os.path.basename(filename)

        # check if we need to add the calculated title
        if xtitle:
            if target == TARGET.MD2AD:
                olines[0] = f".{xtitle}"
            else:
                olines[0] = olines[0].replace("title", f'title="{xtitle}"')

        olines += lines[-1:]  # end of block - copy it
        return olines, False  # prevent line processing of codeblock
