"""
An interface for creating an SArray over time.
"""

'''
Copyright (C) 2015 Dato, Inc.
All rights reserved.

This software may be modified and distributed under the terms
of the BSD license. See the DATO-PYTHON-LICENSE file for details.
'''

from ..connect import main as glconnect
from ..cython.cy_sarray_builder import UnitySArrayBuilderProxy
from .sarray import SArray

class SArrayBuilder(object):
    def __init__(self, num_segments=1, history_size=10, dtype=None):
        self._builder = UnitySArrayBuilderProxy(glconnect.get_client())
        if dtype is None:
            dtype = type(None)
        self._builder.init(num_segments, history_size, dtype)
        self._block_size = 1024

    def append(self, data, segment=0):
        self._builder.append(data, segment)
        
    def append_multiple(self, data, segment=0):
        if not hasattr(data, '__iter__'):
            raise TypeError("append_multiple must be passed an iterable object")
        tmp_list = []
        block_pos = 0
        first = True
        while block_pos == self._block_size or first:
            first = False
            for i in data:
                tmp_list.append(i)
                ++block_pos
                if block_pos == self._block_size:
                    break
            self._builder.append_multiple(tmp_list, segment)
            tmp_list = []

    def get_type(self):
        return self._builder.get_type()

    def read_history(self, num=10):
        if num < 0:
          num = 0
        return self._builder.read_history(num)

    def close(self):
        return SArray(_proxy=self._builder.close())
