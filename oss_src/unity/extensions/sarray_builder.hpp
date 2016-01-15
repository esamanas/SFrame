/**
 * Copyright (C) 2015 Dato, Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */
#ifndef GRAPHLAB_SARRAY_BUILDER_HPP
#define GRAPHLAB_SARRAY_BUILDER_HPP

#include <unity/lib/toolkit_class_macros.hpp>
#include <sframe/sarray.hpp>
#include <boost/circular_buffer.hpp>

namespace graphlab {

class sarray_builder: public graphlab::toolkit_class_base {
 public:
  void init(size_t num_segments=1,
            size_t history_size=10,
            flex_type_enum dtype=flex_type_enum::UNDEFINED);

  void append(const flexible_type &val, size_t segment);
  void append_multiple(const std::vector<flexible_type> &vals, size_t segment);

  flex_type_enum get_type();
  std::vector<flexible_type> read_history(size_t num_elems=size_t(-1));
  gl_sarray close();
 private:
  /// Methods

  /// Variables
  bool m_inited = false;
  bool m_closed = false;
  sarray<flexible_type> m_sarray;
  std::vector<sarray<flexible_type>::iterator> m_out_iters;
  flex_type_enum m_ary_type = flex_type_enum::UNDEFINED;
  flex_type_enum m_given_dtype = flex_type_enum::UNDEFINED;
  std::set<flex_type_enum> m_types_inserted;

  std::shared_ptr<boost::circular_buffer<flexible_type>> m_history;
 public:
  BEGIN_CLASS_MEMBER_REGISTRATION("sarray_builder")
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::init, "num_segments", "history_size", "dtype")
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::append, "val", "segment")
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::append_multiple, "vals", "segment")
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::get_type)
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::read_history, "num_elems")
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::close)
  END_CLASS_MEMBER_REGISTRATION
};

} // namespace graphlab
#endif // GRAPHLAB_SARRAY_BUILDER_HPP
