#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import textwrap


CODEBLOCK_BEGIN = "```unliteralize_conversation"
CODEBLOCK_END = "```"

NEWLINE = "\n"
EMPTY_STRING = ""
WSPACE = " "
ESCAPE = "+"

BEGIN_TABLE = END_TABLE = "|==="
CELL = "|"


# It simply changes the notation to be ```conversation, to allow standard processing by
# karmdown, whilst allowing proper editing in Emacs because things are not fenced
class Conversation:
    block_begin: str = "[[[conversation"
    keep_begin: bool = False

    block_end: str = "]]]"
    keep_end: bool = False

    contentdent: bool = True

    def __call__(self, lines: list[str]) -> list[str]:
        olines = []
        olines += [CODEBLOCK_BEGIN]
        olines += self.process(lines)
        olines += [CODEBLOCK_END]
        return olines

    # PARAMETRIZATION
    useindent: bool = True
    cols: int = 6  # total number of columns
    empty: int = 1  # columns empty on the sides (left, right, left-right)

    blanksep: bool = False  # blank line puts same conv in new cell

    # control line drawing in the table
    frame: str = "none"  # none, ends, sides
    grid: str = "none"  # none, ends, sides

    def process(self, lines: list[str]) -> list[str]:
        convkeys = []  # to store the order in which conv markers (list marker) come in
        conversations = []  # store the conversations
        lastblank = False
        for line in lines:
            if not line:
                if not self.blanksep:
                    conversations[-1] += [line]  # add it to current converation
                else:
                    lastblank = True  # skip the blank ... new conv with same marker

                continue

            # line has content
            char0 = line[0]
            if not char0.isspace():  # check if it is a list/conv marker
                if char0 not in convkeys:  # already there?
                    convkeys += [char0]  # no, add it

                # it was a conv marker, so we add the line as a new conversation
                conversations += [[line]]  # we add a list, NOT an element
            else:
                if not lastblank:  # prev was not blank conv continued
                    conversations[-1] += [line]  # wspace, next line in conv, add it

                elif not self.blanksep:  # prev was a blank line
                    # still part of current conversation
                    conversations[-1] += [line]  # add it to current conv

                else:  # create a conv without market
                    conversations += [[line]]  # adding list, NOT elem

            lastblank = False

        # parsing the conversations
        olines = []
        if not self.useindent:
            olines += [f'[cols="{self.cols}", frame={self.frame}, grid={self.grid}]']
            olines += [BEGIN_TABLE]

        # check for three participants to adjust table positions below
        trio = len(convkeys) == 3

        curconv = EMPTY_STRING  # keep track of current conv market
        for convlines in conversations:
            conv = NEWLINE.join(convlines).rstrip()

            # The first character, list marker, against the position in which it entered
            # the storage, let us know which participant in the conversation is to
            # adjust the position: left-right for 2 people or left-middle-right for 3
            try:
                convidx = convkeys.index(conv[0])
                curconv = conv[0]  # keep it
            except ValueError:
                convidx = convkeys.index(curconv)  # use the last one available

            if not conv[0:1] == WSPACE:
                # conv marker - escape for asciidoc
                conv = f"{ESCAPE}{conv[0]}{ESCAPE}{conv[1:]}"
            else:
                conv = textwrap.dedent(conv)

            if not convidx:
                # Final text: always left, leaving 1 or 2 cols empty to the right
                # "x+|content (spanning x columns)
                # |
                # | (this one only if three people)
                if not self.useindent:
                    olines += (
                        [f"{self.cols - self.empty - trio}+{CELL}{conv}"] +
                        [CELL] +
                        [CELL] * trio
                    )
                else:
                    olines += (
                        [
                            "[.convleft]",
                            conv,
                        ]
                    )

            elif convidx == 1:  # can be in the
                # Final text: middle (3 people) with empty columns left and right
                # or right (2 people) leaving 1 column empty to the left
                # |
                # "x+|content (spanning x columns)
                # | (this one only if three people)
                if not self.useindent:
                    olines += (
                        [CELL] +
                        [f"{self.cols - self.empty - trio}+{CELL}{conv}"] +
                        [CELL] * trio
                    )
                else:
                    olines += (
                        [
                            "[.convright]",
                            conv,
                        ]
                    )
            elif convidx == 2:
                # Final text: always right, leaving 2 columns empty to the left
                # |
                # | (there are three people, so it is always present)
                # "x+|content (spanning x columns)
                olines += (
                    [CELL] +
                    [CELL] +
                    [f"{self.cols - self.empty - trio}+{CELL}{conv}"]
                )

        if not self.useindent:
            olines += [END_TABLE]

        return olines


# Quick doc
DESCRIPTION = """
It takes a (python-)markdown special-defined block in the format

INPUT
-----
[[[conversation

  - person1

  = person2

]]]

and outputs a fenced code block with the type "conversation"

OUTPUT
------

```unliteralize_conversation

[.convleft]
- person1

[.convright]
= person2
```

Or in table notation if so chosen

   (Col 1) (Col2) (Col3)
   - Hi Bob
           - Hi Alice



"""
