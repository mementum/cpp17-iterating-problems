#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from .. import TARGET

# Adds a pagebreak for asciidoc
# Syntax: /// pagebreak ///
class PageBreak:
    block_begin: str = "/// pagebreak"
    block_end: str = "///"
    block_oneline: bool = True
    normalize: bool = False

    def __call__(self, lines: list[str], target: int) -> list[str]:
        if target == TARGET.MD2AD:
            return ["ifdef::backend-pdf[]", "<<<", "endif::[]"]

        return []  # for the 2-markdown platform cases, remove the block
