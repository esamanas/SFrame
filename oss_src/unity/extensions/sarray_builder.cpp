/**
 * Copyright (C) 2015 Dato, Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

#include <unity/extensions/sarray_builder.hpp>
#include <unity/lib/gl_sarray.hpp>

namespace graphlab {

//TODO: Add a hint for the size?
//TODO: How shall I handle types?
void sarray_builder::init(size_t num_segments) {
  m_sarray.open_for_write(1);
  m_out_iter = m_sarray.get_output_iterator(0);
  m_inited = true;
}

void sarray_builder::append(const flexible_type &val) {
  *m_out_iter = val;
}

gl_sarray sarray_builder::close() {
  m_sarray.close();
  return gl_sarray(std::make_shared<sarray<flexible_type>>(m_sarray));
}

} // namespace graphlab

using namespace graphlab;

BEGIN_CLASS_REGISTRATION
REGISTER_CLASS(sarray_builder)
END_CLASS_REGISTRATION
