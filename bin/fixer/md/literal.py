#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from typing import Any

from .. import TARGET

# Catches generic literal blocks and delivers them as they are to avoid normalization
# and line processing
class GenericLiteral:
    targets: int = -TARGET.AD2AD
    block_begin: str = "```"  # matches lines starting with this
    block_end: str = block_begin
    normalize: bool = False

    def __call__(self, lines: list[str]) -> tuple[list[str], bool]:
        return lines, False  # return a simple copy

# This catches reference links and footnote links as a block and delivers them back
# intact. That means that each line is not "normalized" because it is seen as a block.
class GenericRefFootNote:
    targets: int = -TARGET.AD2AD
    block_begin_re: str = "\[\^?[^]]+\]:\s+.*"
    normalize: bool = False

    def __call__(self, lines: list[str]) -> list[str]:
        return lines  # return a simple copy
