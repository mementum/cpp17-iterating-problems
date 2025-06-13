#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
CODEBLOCK_BEGIN = "```unliteralize_admonition"
CODEBLOCK_END = "```"

ADOC_QUOTE = "+"
ADMONITION_BEGIN = ADMONITION_END = "===="
EXAMPLE = "example"

ADLIST = ["example", "note", "tip", "important", "caution", "warning"]

ADTRANS = dict(
    hint="tip",
)


class Admonition:
    block_begin_re: str = r"!!!\s+\w+"
    contentdent: bool = True  # to dedent content as required by asciidoc

    def __call__(self, lines: list[str]) -> list[str]:
        _, adtype, *title = lines[0].split(maxsplit=2)

        adtype = adtype.lower()
        adtype = ADTRANS.get(adtype, adtype)  # try to translate alternative notation
        if adtype not in ADLIST:
            raise ValueError(f"Unknown admonition type: {adtype = }")

        olines = []
        # asciidoc - begin
        olines += [f"[{adtype.upper()}]"] if adtype != EXAMPLE else []
        olines += [f".{title[0]}"] if title else []
        olines += ["+====+"]  # admonition block ... but quoted
        olines += lines[1:]
        olines += ["+====+"]
        # asciidoc - end
        return olines


DESCRIPTION = """
It takes a (python-)markdown admonition and translates it to an asciidoc admonition. It
is embedded in a codeblock, which is later removed to let the naked admonition be
processed.

# INPUT
!!! adtype [title]

  content
  content

  more content

something else

# OUTPUT

```admonition
[,unliteralize_admonition]
[ADTYPE]
.title (OPTIONAL)
====
content
content

more content
====
```
"""
