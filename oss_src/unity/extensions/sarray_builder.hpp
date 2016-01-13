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

namespace graphlab {

class sarray_builder: public graphlab::toolkit_class_base {
 public:
  void init(size_t num_segments);
  void append(const flexible_type &val);
  gl_sarray close();
 private:
  /// Methods

  /// Variables
  bool m_inited = false;
  sarray<flexible_type> m_sarray;
  sarray<flexible_type>::iterator m_out_iter;
  flex_type_enum m_ary_type = flex_type_enum::UNDEFINED;
 public:
  BEGIN_CLASS_MEMBER_REGISTRATION("sarray_builder")
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::init, "num_segments")
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::append, "val")
  REGISTER_CLASS_MEMBER_FUNCTION(sarray_builder::close)
  END_CLASS_MEMBER_REGISTRATION
};

} // namespace graphlab
#endif // GRAPHLAB_SARRAY_BUILDER_HPP
