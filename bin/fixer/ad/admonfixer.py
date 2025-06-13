#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import textwrap

NEWLINE = "\n"
ADOC_QUOTE = "+"
ADMONITION_CONFIRM = ADMONITION_END = r"\s+\+====\+"
EXAMPLE = "example"

ADLIST = ["example", "note", "tip", "important", "caution", "warning"]
BL_BE_RE: str = rf"\[(?:{'|'.join(x.upper() for x in ADLIST)})\]"

ADTRANS = dict(
    hint="tip",
)


class AdmonFixer:
    block_begin_re: str = BL_BE_RE

    block_end_re: str = r"\s+\+====\+"
    keep_end: bool = False

    block_confirm_re: str = r"\s+\+====\+"
    keep_confirm: bool = False
    content_confirm: bool = True

    contentdent: bool = True  # to dedent content as required by asciidoc

    def __call__(self, lines: list[str]) -> list[str]:
        olines = []
        olines += [lines[0]]
        olines += ["===="]
        olines += textwrap.dedent(NEWLINE.join(lines[1:])).splitlines()
        olines += ["===="]
        return olines
