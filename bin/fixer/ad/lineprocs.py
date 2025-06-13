#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import os
import re

NL = NEWLINE = "\n"

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


# Italic followe by footnote collide, add a blank in between
class ItalicFoot:
    lsub: str = r"_footnote"
    lreplace: str = r"_{blank}footnote"


# Unescape Admonitions
class AdmonUnquoter:
    lsub_re: str = r"^\+(====)\+$"
    lreplace: str = r"\1"


# Add a caption to images
class ImgCaption:
    # if a caption starts with ":" it has to be ignored
    lmatch_re: str = r"image:{1,2}(?P<path>[^\[]+)\[(?P<capt>[^\]]+)\](?:{width=(?P<w>[^\}]+)})?"
    LREPLACE: str = rf".\g<capt>{NL}image::\g<path>[\g<capt>,align=center]"

    WIDTHS = ["width", "pdfwidth", "scalewidth"]

    def __call__(self, line: str, lmatch):
        if not (capt := lmatch.group("capt")).startswith(":"):
            line = re.sub(self.lmatch_re, self.LREPLACE, line)
        else:
            line = f'{line.rstrip("]")},align="center"]'

        if not capt.endswith(":"):
            if (w := lmatch.group("w")) is None:
                if (w := os.getenv("IMGWIDTH", None)):
                    if not w.endswith("%"):
                        w += "%"

            if w is not None:
                widths = ",".join(f"{x}={w}" for x in self.WIDTHS)
                line = f"{line.rstrip(']')},{widths}]"

        return f"[.text-center]{NL}{NL}{line}"


# Undo pp substition for stemp processing
# it simply replaces the pattern <<something>> with {lt}{lt}something{gt}{gt}
# for proper asciidoc processing
class PPStem:
    lsub: str = "{pp}{pp}"
    lreplace: str = "++++"


# Superscript. Kramdoc breaks it with a substitution
class SuperScript:
    lsub: str = "{caret}"
    lreplace: str = "^"


# Variable substitution ... broken by kramdoc
# {{varname}} => {varname}
class VarSub:
    lsub_re: str = r"\{\\{([^}]+)}}"
    lreplace: str = r"{\1}"


# { in frac will be changed to \{ by kramdoc, fix it
class StemFrac:
    lmatch: str = "stem:["
    lsub: str = r"\{"
    lreplace: str = "{"
