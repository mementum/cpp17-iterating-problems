#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
# use comment to footnote to define text
class FootNote:
    block_begin_re: str = r"\[//\]:\s+#\s+\((?P<fntext>[^)]+)\)"
    block_confirm_re: str = r"(?P<fntitle>\[\^[a-zA-Z0-9-_]+\]:)\s*(?P<fnextra>.*)$"

    def __call__(self, lines: list[str], match_begin, match_confirm) -> list[str]:
        fntitle = match_confirm.group("fntitle")
        fntext = match_begin.group("fntext")
        fnextra = match_confirm.group("fnextra")

        return [f'{fntitle} {fntext} - {fnextra}']
