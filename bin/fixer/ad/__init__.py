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
    (lineprocs.VarSub, True),
    (lineprocs.DoubleQuotes_Backtick, True),
    (lineprocs.DoubleQuotes_Backtick_Begin, True),
    (lineprocs.DoubleQuotes_Backtick_End, True),
    (lineprocs.FixSizeofDotDotDot, True),
    (lineprocs.LiteralPass, True),
    (lineprocs.LiteralPassEscapeFix, True),
    (lineprocs.LiteralPassPlusFix, True),
]
