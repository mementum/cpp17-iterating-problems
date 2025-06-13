#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import re
import unicodedata


WSPACE = " "
EMPTY_STRING = ""


def strip_accents(s):
    return EMPTY_STRING.join(
        c for c in unicodedata.normalize("NFD", s) if unicodedata.category(c) != "Mn"
    )


class Quote:
    block_begin_re: str = r"\A\[,[\w\s,.:-]+\]"
    block_confirm: str = "____"
    block_end: str = "____"

    QCITE_SPLIT_RE = r"[^\w\s]"

    def __call__(self, lines: list[str]) -> list[str]:
        qcite = lines[0].split(",", maxsplit=1)[1][:-1]

        # print(f"{qcite = }")
        author = re.split(self.QCITE_SPLIT_RE, qcite, maxsplit=1)[0]
        author = author.rstrip().lower().replace(" ", "-")
        author = strip_accents(author)

        olines = []
        olines += lines[1:2]  # begin quote

        olines += [f"[.{author}]"]
        # used the first 2 lines, do not want to use last
        for line in lines[2:-1]:
            olines += [f"[.{author}]"] if not line else [line]

        olines += [f"[.{author}]"]
        olines += [f"— {qcite}"]

        olines += lines[-1:]  # end quote
        return olines
