#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import fixer


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    fixer.fixer.run(
        target=fixer.fixer.TARGET.AD2AD,
        processors=fixer.ad.PROCESSORS,
        line_proc=fixer.ad.LINE_PROCESSORS,
    )
