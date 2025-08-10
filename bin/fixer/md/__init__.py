#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from . import admonition
from . import blockcpp
from . import heading
from . import literal
from . import pagebreak

from . import lineprocs
from . import stdref

PROCESSORS = [
    (pagebreak.PageBreak, True),
    (heading.Heading, True),
    (admonition.Admonition, True),
    (admonition.BlockAdmonition, True),
    (admonition.GenericAdmonition, True),
    (admonition.GenericBlockAdmonition, True),
    (blockcpp.BlockCPPInclude, True),
    (blockcpp.BlockCPP, True),
    (literal.GenericLiteral, True),
    (literal.GenericRefFootNote, True),
]

LINE_PROCESSORS = [
    (lineprocs.AutoFootnoteLinkMk, True),
    (lineprocs.FixFootnoteLinkMk, True),
    (lineprocs.AutoFootnoteLinkAd, True),
    (lineprocs.FixFootnoteLinkAd, True),
    (lineprocs.FixInternalLinkIds, True),
    (lineprocs.AutoLinkMk, True),
    (lineprocs.DoubleQuotesTypo, True),
    (lineprocs.VarSub, True),
    (stdref.stdref, True),
    (stdref.cppref, True),
    (stdref.starref, True),
]
