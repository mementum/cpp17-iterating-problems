#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################

# Unescape an escape {blank}
class CPPLines:
    lsub: str = "[,cpplines]"
    lreplace: str = "[,cpp,linesnum]"


class CPPLinesBlock:
    block_begin: str = "[,cpplines]"
    keep_begin: bool = False
    block_confirm: str = "----"
    block_confirm: str = block_confirm

    def __call__(self, lines: list[str]) -> list[str]:
        return ["[,cpp,linesnum]"] + lines
