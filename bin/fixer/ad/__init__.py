#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from . import lineprocs

PROCESSORS = [
]

LINE_PROCESSORS = [
    (lineprocs.AdmonUnquoter, True),
    (lineprocs.BlankUnEscaper, True),
    (lineprocs.ItalicFoot, True),
    (lineprocs.SuperScript, True),
    (lineprocs.DoubleQ_Backtick, True),
    (lineprocs.FixSizeofDotDotDot, True),
    (lineprocs.LiteralPass, True),
    (lineprocs.LiteralPassEscapeFix, True),
    (lineprocs.LiteralPassPlusFix, True),
]
