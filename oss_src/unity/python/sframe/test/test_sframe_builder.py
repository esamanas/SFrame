'''
Copyright (C) 2015 Dato, Inc.
All rights reserved.

This software may be modified and distributed under the terms
of the BSD license. See the LICENSE file for details.
'''

from ..data_structures.sframe import SFrame
import unittest
import array
import datetime as dt
from ..util.timezone import GMT
from ..util import _assert_sframe_equal

class SFrameBuilderTest(unittest.TestCase):
    def setUp(self):
        self.int_data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        self.float_data = [1., 2., 3., 4., 5., 6., 7., 8., 9., 10.]
        self.string_data = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]
        self.vec_data = [array.array('d', [i, i+1]) for i in self.int_data]
        self.list_data = [[i, str(i), i * 1.0] for i in self.int_data]
        self.dict_data =  [{str(i): i, i : float(i)} for i in self.int_data]
        self.datetime_data = [dt.datetime(2013, 5, 7, 10, 4, 10),
                dt.datetime(1902, 10, 21, 10, 34, 10).replace(tzinfo=GMT(0.0))]
        self.all_type_cols = [self.int_data,
                              self.float_data,
                              self.string_data,
                              self.vec_data,
                              self.list_data,
                              self.dict_data,
                              self.datetime_data*5]
        self.sf_all_types = SFrame({"X"+str(i[0]):i[1] for i in zip(range(1,8),
          self.all_type_cols)})
        self.all_types = [int,float,str,array.array,list,dict,dt.datetime]

    def test_basic(self):
        from ..data_structures.sframe_builder import SFrameBuilder
        sf_data = zip(*self.all_type_cols) 

        sb = SFrameBuilder(self.all_types)
        for i in sf_data:
            sb.append(i)
        sf = sb.close()
        _assert_sframe_equal(sf, self.sf_all_types)

        sb = SFrameBuilder(self.all_types)
        sb.append_multiple(sf_data)
        sf = sb.close()
        _assert_sframe_equal(sf, self.sf_all_types)

    def test_history(self):
        from ..data_structures.sframe_builder import SFrameBuilder
        sb = SFrameBuilder([int,float], history_size=10)
        sb.append_multiple(([i,i+0.0] for i in xrange(8)))
        hist = sb.read_history(3)
        self.assertEquals(hist,[[5,5.0],[6,6.0],[7,7.0]])

        hist = sb.read_history(20)
        self.assertEquals(hist, [[i,i+0.0] for i in range(8)])
        hist = sb.read_history()
        self.assertEquals(hist, [[i,i+0.0] for i in range(8)])

        sb.append_multiple(([i,i+0.0] for i in xrange(5)))
        hist = sb.read_history(10)
        self.assertEquals(hist, [[i,i+0.0] for i in [3,4,5,6,7,0,1,2,3,4]])

        sb.append([50,50.0])
        hist = sb.read_history(10)
        self.assertEquals(hist, [[i,i+0.0] for i in [4,5,6,7,0,1,2,3,4,50]])

        hist = sb.read_history(-1)
        self.assertEquals(hist, [])
        hist = sb.read_history(0)
        self.assertEquals(hist, [])

        expected_data = [[i,i+0.0] for i in range(8)] + [[i,i+0.0] for i in range(5)] + [[50,50.0]]
        cols = [[],[]]
        for i in expected_data:
            cols[0].append(i[0])
            cols[1].append(i[1])
        expected_sf = SFrame({'X1':cols[0],'X2':cols[1]})

        sf = sb.close()
        _assert_sframe_equal(sf,expected_sf)

    def test_segments(self):
        from ..data_structures.sframe_builder import SFrameBuilder
        sb = SFrameBuilder([int],num_segments=4)

        sb.append_multiple(([i] for i in xrange(20,30)), segment=2)
        sb.append_multiple(([i] for i in xrange(10,20)), segment=1)
        sb.append_multiple(([i] for i in xrange(30,40)), segment=3)
        sb.append_multiple(([i] for i in xrange(0,10)), segment=0)

        sf = sb.close()
        expected_sf = SFrame({'X1':range(40)})
        _assert_sframe_equal(sf, expected_sf)
