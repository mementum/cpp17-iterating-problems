#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import re

COLALIGNMENTS_RE = "([<>^.])(?:,)?"
COLHEADERS_RE = r'\|\s+(?::)?(\d{1,2}%?[a-z]?)?\s?([*\w\s\.,"-]*)'


class TableWidth:
    block_begin: str = "[cols="
    block_confirm: str = "|==="
    block_end: str = "|==="

    def __call__(self, lines: list[str]) -> list[str]:
        # get the alignments
        colalignments = re.findall(COLALIGNMENTS_RE, lines[0])

        # get header widths/names
        colwh = re.findall(COLHEADERS_RE, lines[2])
        colwidths, colheaders = list(zip(*colwh))

        # no headers and no colwidths
        if len(colwidths) == 1 and not colwidths[0]:
            colwidths = [""] * len(colalignments)

        # replace colalignments with alignment-width pairs
        colaw = ",".join(f"{ca}{cw}" for ca, cw in zip(colalignments, colwidths))
        lines[0] = f'[cols="{colaw}"]'

        # redo the headers without the widths unless there are no headers
        if any(x.strip() for x in colheaders):
            lines[2] = "| ".join(('',) + colheaders)  # Force join to add a leading pipe
        else:
            lines = lines[0:2] + lines[3:]

        return lines
