#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from logging import debug as ldebug, info as linfo
from collections import defaultdict
import re

from .. import TARGET, fixer

CPPREF = "https://en.cppreference.com/w/cpp"
TARGET_BLANK = "{target=_blank}"

CPPREFS = {
    "{}": "language/list_initialization",
    "...": "language/parameter_pack",
    "and": "keyword/and",
    "auto": "keyword/auto",
    "bool": "keyword/bool",
    "C++": "",
    # "C++98": "",
    "C++11": "11",
    "C++14": "14",
    "C++17": "17",
    "C++20": "20",
    "C++23": "23",
    "C++26": "26",
    "const": "language/const",
    "char": "keyword/char",
    "concepts": "concepts",
    "constexpr": "language/constexpr",
    "decltype": "language/decltype",
    "do-while": "language/do",
    "double": "keyword/double",
    "false": "language/bool_literal",
    "float": "keyword/float",
    "fold expression": "language/fold",
    "for": "language/for",
    "if constexpr": "language/if",
    "#include": "preprocessor/include",
    "int": "keyword/int",
    "lambda": "language/lambda",
    "long": "keyword/long",
    "lvalue": "language/value_category",
    "main": "language/main_function",
    "nullptr": "language/nullptr",
    "operator": "language/operators",
    "preprocessor": "preprocessor",
    "rvalue": "language/value_category",
    "SFINAE": "language/sfinae",
    "size_t": "c/types/size_t",
    "sizeof...": "language/sizeof....html",
    "static": "keyword/static",
    "STL": "standard_library",
    "Structured Binding": "language/structured_binding",
    "struct": "language/struct",
    "template": "language/templates",
    "true": "language/bool_literal",
    "typename": "keyword/typename",
    "using": "keyword/using",
    "typeid": "language/typeid",
    "void": "language/types.html#void",
    "while": "language/while",
}

STDREFS = {
    "accumulate": "algorithm/accumulate",
    "any": "utility/any",
    "any_cast": "utility/any/any_cast",
    "array": "container/array",
    "back_inserter": "iterator/back_inserter",
    "front_inserter": "iterator/front_inserter",
    "basic_string": "string/basic_string",
    "begin": "iterator/begin",
    "bind": "utility/functional/bind",
    "cerr": "io/cerr",
    "cin": "io/cin",
    "conjunction": "types/conjunction",
    "conjunction_v": "types/conjunction",
    "copy": "algorithm/copy",
    "copy_if": "algorithm/copy",
    "count": "algorithm/count",
    "count_if": "algorithm/count",
    "cout": "io/cout",
    "decay_t": "types/decay",
    "deque::push_front": "container/deque/push_front",
    "deque": "container/deque",
    "declval": "utility/declval",
    "distance": "iterator/distance",
    "enable_if": "types/enable_if",
    "enable_if_t": "types/enable_if",
    "end": "iterator/end",
    "endl": "io/manip/endl",
    "false_type": "types/integral_constant",
    "find": "algorithm/find",
    "find_if": "algorithm/find_if",
    "function": "utility/functional/function",
    "get": "utility/tuple/get",
    # iterator tags - begin
    "input_iterator_tag": "iterator/iterator_tags",
    "output_iterator_tag": "iterator/iterator_tags",
    "forward_iterator_tag": "iterator/iterator_tags",
    "bidirectional_iterator_tag": "iterator/iterator_tags",
    "random_access_iterator_tag": "iterator/iterator_tags",
    "copy_n": "algorithm/copy_n",
    "contiguous_iterator_tag": "iterator/iterator_tags",
    # iterator tags - end
    "invoke_result": "result_of",
    "invoke_result_t": "result_of",
    "is_base_of": "types/is_base_of",
    "is_base_of_v": "types/is_base_of",
    "is_integral": "types/is_integral",
    "is_integral_v": "types/is_integral_v",
    "is_same": "types/is_same",
    "is_same_v": "types/is_same",
    "istream": "io/basic_istream",
    "istream_iterator": "iterator/istream_iterator",
    "iterator": "iterator/iterator",
    "iterator_traits": "iterator/iterator_traits",
    "list": "container/list",
    "make_reverse_iterator": "iterator/make_reverse_iterator",
    "make_unique": "memory/unique_ptr/make_unique",
    "map": "container/map",
    "next": "iterator/next",
    "numeric_limits": "types/numeric_limits",
    'string::operator""s': "string/basic_string/operator%2522%2522s.html",
    "ostream": "io/basic_ostream",
    "ostream_iterator": "iterator/ostream_iterator",
    "pair": "utility/pair",
    "partition": "algorithm/partition",
    "stack": "container/stack",
    "string": "string/basic_string",
    "transform": "algorithm/transform",
    "unique_ptr": "memory/unique_ptr",
    "unordered_map": "container/unordered_map",
    "variant": "utility/variant",
    "variant_alternative": "utility/variant/variant_alternative",
    "variant_alternative_t": "utility/variant/variant_alternative",
    "variant_size": "utility/variant/variant_size",
    "variant_size_v": "utility/variant/variant_size",
    "vector::emplace": "container/vector/emplace",
    "vector::push_back": "container/vector/push_back",
    "vector": "container/vector",
    "views::iota": "ranges/iota_view",
    "void_t": "types/void_t",
    "true_type": "types/integral_constant",
    "tuple": "utility/tuple",
    "type_index": "types/type_index",
    "type_info": "types/type_info",
    "visit": "utility/variant/visit2",
    "do_something": None,  # placeholder used in example
}


class baseref:
    lsearch_re: str
    xrefs: dict[str, str]
    refprefix: str
    casein: bool = False

    seen: defaultdict[int, list[str]] = defaultdict(list)

    def __call__(
        self,
        line: str,
        target: int,
        filenum: int,
        parent: fixer.Fixer,
    ) -> str:

        lresults = re.findall(self.lsearch_re, line)
        for fmatch, op, noop, sref in set(lresults):
            lsref = sref.lower()
            # try lower case can direct capture
            if (hlink := self.xrefs.get(lsref, None)) is None:
                hlink = self.xrefs.get(sref, None)

            if hlink is None:  # still None after lower and direct capture search
                # Report anything we may have missed
                if sref not in self.xrefs and lsref not in self.xrefs:
                    linfo(f"++Missing STD Ref++: {sref}")
                continue

            fulllink = f"{CPPREF}/{hlink}"  # "/{sref}"
            # replace chars that could break a reference name
            reference = f"{filenum}-{self.refprefix}-{lsref}".\
                replace(" ", "-").\
                replace(".", "o").\
                replace("*", "O").\
                replace("::", "-"). \
                replace("{}", "cbraces"). \
                replace('"', "dq"). \
                replace("+", "plus")

            if lsref not in self.seen[filenum]:
                postlink = f"[{reference}]: {fulllink}"
                parent.add_postline(postlink)

            linkfmatch = fmatch
            reflink = f"[{linkfmatch}][{reference}]"
            extralink = ""

            if target == TARGET.MD2AD:
                footname = f"[^{reference}]"
                if lsref not in self.seen[filenum]:
                    postfoot = f"{footname}: {self.get_foottext(sref)} - {fulllink}"
                    parent.add_postline(postfoot)

                extralink = f"{footname}"

            else:
                extralink = f"{TARGET_BLANK}"

            if lsref not in self.seen[filenum]:
                self.seen[filenum].append(lsref)

            line = line.replace(fmatch, f"{reflink}{extralink}")

        return line

    def get_foottext(self, sref: str) -> str:
        return sref

    def __init_subclass__(cls, /, **kwargs):
        super().__init_subclass__(**kwargs)
        if cls.casein:
            cls.lsearch_re = f"(?i){cls.lsearch_re}"


class stdref(baseref):
    lsearch_re: str = (
        r"(?P<fmatch>(?P<op>`)(?P<noop>std::(?P<sref>[^(<`\s]+)(?:[^`]+)?)(?P=op))"
    )
    xrefs: dict[str, str] = STDREFS
    refprefix: str = "std"

    def get_foottext(self, sref: str) -> str:
        return f"{self.refprefix}::{sref}"


class xref(baseref):
    casein: bool = True
    _stdkeys = "|".join(x.replace('.', r'\.') for x in CPPREFS.keys())
    _stdkeys = _stdkeys.replace("+", r"\+")
    lsearch_re: str = (
        r"(?P<fmatch>(?P<op>{delim})(?P<noop>(?P<sref>{_stdkeys})[^{delim}]*)(?P=op))"
    )
    xrefs = CPPREFS
    refprefix: str = "xref"

    def __init_subclass__(cls, /, delim, **kwargs):
        super().__init_subclass__(**kwargs)
        cls.lsearch_re = xref.lsearch_re.format(delim=delim, _stdkeys=cls._stdkeys)


class cppref(xref, delim="`"):
    refprefix: str = "cpp"


class starref(xref, delim="\*"):
    refprefix: str = "star"
