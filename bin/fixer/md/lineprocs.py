#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################

class LTx2:
    lsub_re: str = r"([^\w])<<(\w)"
    lreplace: str = r"\1«\2"


class GTx2:
    lsub: str = r"([\w.])>>([^\w]))"
    lreplace: str = r"\1»\2"


# Replace two -- with a dash
class EmDash:
    # lsub_re: str = r"([a-zA-Z .])--([a-zA-Z .])"
    lsub_re: str = r"--([a-zA-Z ])"
    lreplace: str = r"—\1"


# Typographic Quotes
class DoubleQuotesTypo_Begin:
    lsub_re: str = r'([^\w])"'
    lreplace: dict[str] = {
        "de": r"\1„",
        "en": r"\1“",
        "es": r"\1«",
    }


# Typographic Quotes
class DoubleQuotesTypo_End:
    lsub_re: str = r'"([^\w])'
    lreplace: dict[str] = {
        "de": r"“\1",
        "en": r"”\1",
        "es": r"»\1",
    }
