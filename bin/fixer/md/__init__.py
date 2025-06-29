#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from . import admonition
from . import blockcpp
from . import heading

from . import lineprocs
from . import stdref

PROCESSORS = [
    (heading.Heading, True),
    (admonition.Admonition, True),
    (admonition.BlockAdmonition, True),
    (blockcpp.BlockCPPInclude, True),
    (blockcpp.BlockCPP, True),
]

LINE_PROCESSORS = [
    (lineprocs.DoubleQuotesTypo_Begin, True),
    (lineprocs.DoubleQuotesTypo_End, True),
    (lineprocs.AutoLink_mk, True),
    (lineprocs.AutoLink, True),
    (lineprocs.VarSub, True),
    (stdref.stdref, True),
    (stdref.cppref, True),
    (stdref.starref, True),
]
