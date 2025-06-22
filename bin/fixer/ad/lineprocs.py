#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
# Four dots in a reference link for a footnote are interpreted as a command and
# breaks processing. sizeof... is a problem therefore as the link to it contains
# the 3 dots + the extension separating dot. Let 3 dots be passed through
class FixSizeofDotDotDot:
    lsub: str = r"....html"
    lreplace: str = r"++...++.html"

# String "operator ++()" is passed-through by kramdoc, but because it is inside
# a reference link, the closing square bracket of the passthrough is escaped
# as in \]. Do simply remove the escaping sequence
class FixOperatorPP:
    lsub: str = r"pass:c[operator ++()\]"
    lreplace: str = r"pass:c[operator ++()]"

# Typographic Quotes
class DoubleQuotes_Backtick:
    lsub_re: dict[str] = {
        "de": r"([„“])`",
        "en": r"([”“])`",
        "es": r"([«»])`",
    }
    lreplace: str = r'\1'

# Typographic Quotes - Italic Fix
class DoubleQuotes_Backtick_Begin:
    lsub_re: str = r'"`'
    lreplace: dict[str] = {
        "de": r"„",
        "en": r"“",
        "es": r"«",
    }

# Typographic Quotes - Italic Fix
class DoubleQuotes_Backtick_End:
    lsub_re: str = r'`"'
    lreplace: dict[str] = {
        "de": r"“",
        "en": r"”",
        "es": r"»",
    }

# Unescape an escape {blank}
class BlankUnEscaper:
    lsub: str = r"\{blank}"
    lreplace: str = r"{blank}"

# Italic followed by footnote collide, add a blank in between
class ItalicFoot:
    lsub: str = r"_footnote"
    lreplace: str = r"_{blank}footnote"

# Unescape Admonitions
class AdmonUnquoter:
    lsub_re: str = r"^\+(====)\+$"
    lreplace: str = r"\1"

# Superscript. Kramdoc breaks it with a substitution
class SuperScript:
    lsub: str = "{caret}"
    lreplace: str = "^"

# Variable substitution ... broken by kramdoc
# {\{varname}} => {varname}
class VarSub:
    lsub_re: str = r"{\\{([^}]+)}}"
    lreplace: str = r"{\1}"
