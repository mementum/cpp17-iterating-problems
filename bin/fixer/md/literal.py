#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from typing import Any

from .. import TARGET

class GenericLiteral:
    targets: int = -TARGET.AD2AD
    block_begin: str = "```"  # matches lines starting with this
    block_end: str = block_begin
    normalize: bool = False

    def __call__(self, lines: list[str]) -> tuple[list[str], bool]:
        return lines, False  # return a simple copy
