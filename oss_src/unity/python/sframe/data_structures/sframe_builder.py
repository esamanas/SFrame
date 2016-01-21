"""
An interface for creating an SFrame over time.
"""

'''
Copyright (C) 2015 Dato, Inc.
All rights reserved.

This software may be modified and distributed under the terms
of the BSD license. See the DATO-PYTHON-LICENSE file for details.
'''

from ..connect import main as glconnect
from ..cython.cy_sframe_builder import UnitySFrameBuilderProxy
from .sframe import SFrame 

class SFrameBuilder(object):
    def __init__(self, column_types, column_names=None, num_segments=1, history_size=10):

        self._column_names = column_names
        self._column_types = column_types
        self._num_segments = num_segments
        self._history_size = history_size
        if column_names is not None and column_types is not None:
            if len(column_names) != len(column_types):
                raise AssertionError("There must be same amount of column names as column types.")
        elif column_names is None and column_types is not None:
            self._column_names = self._generate_column_names(len(column_types))
        else:
            raise AssertionError("Column types must be defined!")

        self._builder = UnitySFrameBuilderProxy(glconnect.get_client())
        self._builder.init(self._column_types, self._column_names, self._num_segments, self._history_size)
        self._block_size = 1024

    def _generate_column_names(self, num_columns):
        return ["X"+str(i) for i in range(1,num_columns+1)]

    def append(self, data, segment=0):
        # Assume this case refers to an SFrame with a single column
        if not hasattr(data, '__iter__'):
            data = [data]
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
        return SFrame(_proxy=self._builder.close())

