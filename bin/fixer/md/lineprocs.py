#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import re

from .. import TARGET

# Automate reference link and footnote
# A format [:^ref] is translated to [ref] for mkdocs
class AutoFootnoteLinkMk:
    targets: int = TARGET.MD2MK
    lsub_re: str = r"\]\[:\^([^]]+)\]"
    lreplace: str = r"][\1]{target=_blank}^:octicons-link-external-16:^"

# Fix the real footnote/ref link
# A format ""[:^ref]: footnote link" is translated to a footnote [ref]: link"
# for mkdocs
class FixFootnoteLinkMk:
    targets: int = TARGET.MD2MK
    lsub_re: str = r"^\[:\^([^]]+)\]:\s+(.+)(https?://.+)$"
    lreplace: str = r"[\1]: \3"

# Automate reference link and footnote
# A format [:^ref] is translated to a footnote [^ref] for pdf over asciidoc
class AutoFootnoteLinkAd:
    targets: int = TARGET.MD2AD
    lsub_re: str = r"\[([^]]+)\]\[:\^([^]]+)\]"
    lreplace: str = r"\1[^\2]"

# Fix the real footnote/ref link
# A format [:^ref]: footnote is translated to a footnote [^ref]: footnote"
# for pdf over asciidoc
class FixFootnoteLinkAd:
    targets: int = TARGET.MD2AD
    lsub_re: str = r"^\[:\^([^]]+)\]:\s+(.+)$"
    lreplace: str = r"[^\1]: \2"

# Change autolinks, <http.....> to [url](url){target=blank} for mkdocs
class AutoLinkMk:
    targets: int = TARGET.MD2MK
    lsub_re: str = r"<(https?:[^>]+)>"
    lreplace: str = r"[\1](\1){target=_blank} :octicons-link-external-16:"

# Mkdocs cannot reference links in other files, because each file will be rendered in a
# given page. But adding the file name breaks asciidoc, that sees all as one file and
# does not know about the .md filenames. Remove the filename and leave only the id when
# processing for md2ad
class FixInternalLinkIds:
    targets: int = TARGET.MD2AD
    lsub_re: str = r"\[([^]]+)\]\([\w-]+\.md(#[\w-]+)\)"
    lreplace: str = r"[\1](\2)"

# Change standard double quotes to be typographic quotes using the proper set
# for the target language
class DoubleQuotesTypo:
    lsub_re: str = r'"([^"]+)"'
    lreplace: dict[str] = {
        "de": r"„\1“",
        "en": r"“\1”",
        "es": r"«\1»",
    }

# Substitute vars using the format {:varname:} using the values
# defined in the adoc header
class VarSub:
    lsearch_re: str = r'{:(?P<varname>[^:]+):}'

    def __call__(self, line:str, lmatch: re.Match[str], adocvars: dict[str]) -> str:
        if (varval := adocvars.get(lmatch.group("varname"), None)) is None:
            return line

        return re.sub(self.lsearch_re, varval, line)
