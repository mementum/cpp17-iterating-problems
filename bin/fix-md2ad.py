#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import fixer


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    fixer.fixer.run(
        target=fixer.TARGET.MD2AD,
        processors=fixer.md.PROCESSORS,
        line_proc=fixer.md.LINE_PROCESSORS,
    )
