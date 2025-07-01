#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
# Kramdoc does already put some of the inline literals inside a pass:c
# macro, but it ignores many, especially when it comes to footnotes.
# This looks for both, thosed marked and those unmarked. All of them are
# marked with pass:c. Special care is needed for constructs like `[a, b]`. The
# square brackets can only be matched if `pass:` has been seen
class LiteralPass:
    lsub_re: str = r"`(?P<pass>(?:pass:)(?:[a-z],?)*(?:\[))?((?(pass)[^]`]+|[^`]+))(?(pass)\])?`"
    lreplace: str = r"`pass:c[\2]`"

# Kramdoc does sometimes escape the interior ] of a pass:c with \] making it
# useless. This removes the escaping. This has to be run after "LiteralPass
# to make sure the links have already been passed through
class LiteralPassEscapeFix:
    lsub_re: str   = r"(\[`pass:c\[[^]\\]+)\\\](`\])"
    lreplace: str = r"\1]\2"

# Kramdoc does sometimes escape the interior of a pass:c with + escape signs,
# which are already escaped by pass and show up in the final rendering. This
# removes them.
class LiteralPassPlusFix:
    lsub_re: str = r"(`pass:c\[)\+([^+]+)\+(\]`)"
    lreplace: str = r"\1\2\3"

# Four dots in a reference link for a footnote are interpreted as a command and
# breaks processing. sizeof... is a problem therefore as the link to it contains
# the 3 dots + the extension separating dot. Let 3 dots be passed through
class FixSizeofDotDotDot:
    lsub: str = r"....html"
    lreplace: str = r"++...++.html"

# Kramdoc changes the typographic quotes to standard quotes and uses backticks to enclose
# the content in between the pair of double quotes.
# Although we have already changed the double quotes to typographic quotes for the mdx
# intermediate target, we need to again do the change and remove the backticks for each
# target language
class DoubleQ_Backtick:
    lsub_re: str = r'"`([^"`]+)`"'
    lreplace: dict[str] = {
        "de": r"„\1“",
        "en": r"“\1”",
        "es": r"«\1»",
    }

# Unescape an escape {blank}
class BlankUnEscaper:
    lsub: str = r"\{blank}"
    lreplace: str = r"{blank}"

# Italic followed by footnote collide, add a blank in between
class ItalicFoot:
    lsub: str = r"_footnote"
    lreplace: str = r"_{blank}footnote"

# Unescape Admonitions
class AdmonUnquoter:
    lsub_re: str = r"^\+(====)\+$"
    lreplace: str = r"\1"

# Superscript. Kramdoc breaks it with a substitution
class SuperScript:
    lsub: str = "{caret}"
    lreplace: str = "^"
