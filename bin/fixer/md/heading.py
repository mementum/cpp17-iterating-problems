#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from typing import Any

from .. import TARGET

NEWLINE = "\n"
EMPTYLINE = ""

# Look for a part definition and translate to asciidoc
# header with lowest level and the id if present
class Heading:
    # this is the full match of a title. but we are (right now) only interested in
    # matching if there is a "part" indicator
    block_begin_re: str = (
        r"^(?P<hlevel>#+)\s+(?P<title>[^{]+)\b\s*"
        r"(?:{\s*"
        r"(?P<hid>#[^\s}]+)?\b\s*"
        r"(?:epub:type=(?P<epubtype>[^\s}]+))?\b\s*"
        r"(?P<attrs>[^}]+)?"
        r"})?"
    )

    targets: list[int] = [TARGET.MD2AD, TARGET.MD2PD, TARGET.MD2MK]

    def __call__(self, lines: list[str], match_begin: Any, target: str) -> list[str]:
        outlines = []

        # The header is expected to be generic for ebook generation which may include an
        # epub:type attribute.
        # Do nothing if the target is "md" post-processing do nothing.
        # "ad" (asciidoc) is not target of this block processor.

        if target == TARGET.MD2PD:
            outline = match_begin.string
        else:
            hid = match_begin.group("hid")
            hlevel = match_begin.group("hlevel")
            # when pre-adapting for a translation to asciidoc, put epubtype and header id
            # in the right place to have a proper translation. It is no longer markdown
            # afterwards the header finally looks like this [epubtype] [headerid] # title
            # <= this is still markdown with the level +1
            if target == TARGET.MD2AD:
                part = False
                if (epubtype := match_begin.group("epubtype")):
                    if not (part := (epubtype.lower() == "part")):
                        outlines += [f"[{epubtype}]"]
                if hid:
                    outlines += [f"[{hid}]"]

                # heading level + 1 except for parts
                hlevel = "#" * (len(hlevel) + 1 - part)

            # create the line
            title = match_begin.group('title')
            outline = f"{hlevel} {title}"

            # When fixing for mkdocs (material) keep id and attrs
            if target == TARGET.MD2MK:
                extra = ""
                if hid:
                    extra += f" {hid} "
                if (attrs := match_begin.group("attrs")):
                    extra += f" {attrs} "

                if extra:
                    outline += f" {{{extra}}}"

        outlines += [outline]
        return outlines
