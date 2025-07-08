#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
# Catches generic literal blocks and delivers them as they are to avoid normalization
# and line processing
class GenericLiteral:
    block_begin_re: str = r"(?P<bticks>````?)"  # matches lines starting with backticks
    block_end_rex: str = r"\g<bticks>$"  # uses the number of matched backticks
    normalize: bool = False
    lineproc: bool = False

# This catches reference links and footnote links as a block and delivers them back
# intact. That means that each line is not "normalized" because it is seen as a block.
class GenericRefFootNote:
    block_begin_re: str = r"\[\^?[^]]+\]:\s+.*"
    block_end_re: str = r"$"
    block_oneline: bool = True
