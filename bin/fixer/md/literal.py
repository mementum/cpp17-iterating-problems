#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
# Catches generic literal blocks and delivers them as they are to avoid normalization
# and line processing
class GenericLiteral:
    block_begin_re: str = r"(?P<bticks>````?)"  # matches lines starting with this
    block_end_rex: str = r"\g<bticks>"
    normalize: bool = False
    lineproc: bool = False

# This catches reference links and footnote links as a block and delivers them back
# intact. That means that each line is not "normalized" because it is seen as a block.
class GenericRefFootNote:
    block_begin_re: str = r"\[\^?[^]]+\]:\s+.*"
    normalize: bool = False
    block_oneline: bool = True
