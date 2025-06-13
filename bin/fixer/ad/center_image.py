#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
class EquationCenterFixer:
    block_begin: str = "[.text-center]"
    block_confirm: str = r" image"
    keep_confirm: bool = True
    confirm_content: bool = True  # confirm is part of content
    contentdent = True  # dedent content

    def __call__(self, lines: list[str]) -> list[str]:
        return lines[:]  # copy of lines
