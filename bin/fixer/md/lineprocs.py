#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import re

from .. import TARGET

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

# A link marked as: $[link]$
# will be substituted to: [link](link){target=_blank}
class AutoLink_mk:
    targets = [TARGET.MD2MK]
    lsub_re: str = r'(\s+)\$\[([^]]+)\]\$'
    lreplace: str = r'\1[\2](\2){target=_blank}'

# A link marked as: $[link]$
# will be substituted to: [link](link)
class AutoLink:
    targets = -TARGET.MD2MK
    lsub_re: str = r'(\s+)\$\[([^]]+)\]\$'
    lreplace: str = r'\1[\2](\2)'

# Substitute vars using the format {:varname:} using the values
# defined in the adoc header
class VarSub:
    targets = -TARGET.AD2AD
    lsearch_re: str = r'{:(?P<varname>[^:]+):}'

    def __call__(self, line:str, lmatch: re.Match[str], adocvars: dict[str]) -> str:
        if (varval := adocvars.get(lmatch.group("varname"), None)) is None:
            return line

        return re.sub(self.lsearch_re, varval, line)
