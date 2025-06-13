#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from . import lineprocs
from . import unliteralizer
from . import admonfixer
from . import center_image
from . import tablewidth


PROCESSORS = [
    (admonfixer.AdmonFixer, True),
    (unliteralizer.UnLiteralizer, True),
    (center_image.EquationCenterFixer, True),
    (tablewidth.TableWidth, True),
]

LINE_PROCESSORS = [
    (lineprocs.AdmonUnquoter, True),
    (lineprocs.BlankUnEscaper, True),
    (lineprocs.ImgCaption, True),
    (lineprocs.ItalicFoot, True),
    (lineprocs.PPStem, False),
    (lineprocs.SuperScript, True),
    (lineprocs.VarSub, True),
    (lineprocs.StemFrac, True),
    (lineprocs.DoubleQuotes_Backtick, True),
    (lineprocs.DoubleQuotes_Backtick_Begin, True),
    (lineprocs.DoubleQuotes_Backtick_End, True),
]
