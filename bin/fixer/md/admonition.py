#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from .. import TARGET

# Ref: https://python-markdown.github.io/extensions/admonition/
# It looks for an admonition block of this type (with the title being optional)
# !!! admonition_type "The Title"
#     Content

# Ref: https://docs.asciidoctor.org/asciidoc/latest/blocks/admonitions/
# It outputs (title is output if present)
# [ADMONITION_TYPE]
# .title
# +====+
# Content
# +====+
#
# Outputting "+====+" instead of the normative "====" asciidoc separator.
# It will be "unquoted" by the AdmonUnquoter line processor

# it considers "example" (from asciidoc), which simply indents the content to create a
# block with no specific admonition type

ADLIST = ["example", "note", "tip", "important", "caution", "warning"]
ADTRANS = dict(hint="tip")
EXAMPLE = "example"
ADBLOCK = "+====+"

class Admonition:
    targets: list[int] = [TARGET.MD2AD]
    block_begin_re: str = rf'!!!\s+(?P<adtype>\w+)(\s+"(?P<title>[^"]*)")?'
    keep_begin: bool = False
    contentdent: bool = True  # to dedent content as required by asciidoc

    def __call__(self, lines: list[str], match_begin) -> list[str]:
        olines = []  # output lines

        adtype = match_begin.group("adtype")  # get adtype and capitalize
        adtype = ADTRANS.get(adtype.lower(), adtype)  # look for an alias
        if adtype not in ADLIST:
            raise ValueError(f"Unknown admonition type: {adtype = }")

        if adtype != EXAMPLE:  # "example" does not need a block type
            olines += [f"[{adtype.upper()}]"]  # ad [ADTYPE]

        if title := match_begin.group("title"):
            olines += [f".{title}"]  # add .title only if present

        olines += [ADBLOCK]  # ==== quoted as +====+
        olines += lines  # content (dedented)
        olines += [ADBLOCK]  # ==== quoted as +====+
        return olines

# Ref: https://facelessuser.github.io/pymdown-extensions/extensions/blocks/plugins/admonition/
# It looks for an admonition block of this type (with the title being optional)
# /// admonition_type | title
# Content
# ///
# The output is as shown above

class BlockAdmonition(Admonition):
    block_begin_re: str = rf"///\s+(?P<adtype>\w+)((\s+\|\s+)(?P<title>.+))?$"
    keep_begin: bool = False
    block_end: str = "///"
    keep_end: bool = False
    contentdent: bool = False  # In block mode, content is as-is
