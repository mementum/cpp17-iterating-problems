#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
from enum import IntEnum

class TARGET(IntEnum):
    AD2AD = 1
    MD2PD = 2
    MD2AD = 3
    MD2MK = 4

from . import fixer
from . import md
from . import ad 
