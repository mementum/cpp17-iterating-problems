#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
QTABLE_BEGIN = "```table"
QTABLE_END = "```"
EMPTY_LINE = ""

class QuotedTable:
    # Block Processing Variables
    block_begin = QTABLE_BEGIN
    keep_begin = True
    block_end = QTABLE_END
    keep_end = False

    def __call__(self, lines: list[str]) -> list[str]:
        if True:
            return lines[1:]

        _, *caption = lines[0].split(":", maxsplit=1)

        if caption:
            caption = ["[.text-center]", f"{caption[0].lstrip()}"]

        return lines[1:] + [EMPTY_LINE] + caption


DESCRIPTION = """Remove the literal ```table and ``` quoting tags from tables, to let
them be processed as tables. Th reason to quote them is to avoid reformatting when the
entire buffer is "filled" in Emacs"""
