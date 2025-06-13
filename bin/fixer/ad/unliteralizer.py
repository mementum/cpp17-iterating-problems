#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
class UnLiteralizer:
    block_begin: str = "[,unliteralize_"
    block_confirm: str = "----"
    block_end: str = "----"

    def __call__(self, lines: list[str]) -> list[str]:
        return lines[2:-1]  # skip 2 initial lines and last


DESCRIPTION = """
It takes a literal block marked as "unliteralize" and delivers the content of the block

# INPUT
[,unliteralize_admonition]
----
[ADTYPE]
.title (OPTIONAL)
====
content
content

more content
====
----

# OUTPUT
[ADTYPE]
.title (OPTIONAL)
====
content
content

more content
====
"""
