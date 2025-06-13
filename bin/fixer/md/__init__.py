#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from . import admonition
from . import blockcpp
from . import footnote
from . import heading
from . import quotedtable

from . import lineprocs


PROCESSORS = [
    (heading.Heading, True),
    (admonition.Admonition, True),
    (quotedtable.QuotedTable, True),
    (footnote.FootNote, True),
    (blockcpp.BlockCPPInclude, True),
    (blockcpp.BlockCPP, True),
]

LINE_PROCESSORS = [
    (lineprocs.EmDash, False),
    (lineprocs.DoubleQuotesTypo_Begin, True),
    (lineprocs.DoubleQuotesTypo_End, True),
]
