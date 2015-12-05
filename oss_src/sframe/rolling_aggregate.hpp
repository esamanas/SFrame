#ifndef GRAPHLAB_SFRAME_ROLLING_AGGREGATE_HPP
#define GRAPHLAB_SFRAME_ROLLING_AGGREGATE_HPP

#include <flexible_type/flexible_type.hpp>
#include <sframe/sarray.hpp>

namespace graphlab {
namespace rolling_aggregate {

/**
 * Apply an aggregate function over a moving window.
 * 
 * \param input The input SArray (expects to be materialized)
 * \param fn_name The string representation of the aggregation function to use.
 * The mapping is kept in the `agg_string_to_fn` function.
 * \param window_start The start of the moving window relative to the current
 * value being calculated, inclusive. For example, 2 values behind the current
 * would be -2, and 0 indicates that the start of the window is the current
 * value.
 * \param window_end The end of the moving window relative to the current value
 * being calculated, inclusive. Must be greater than `window_start`. For
 * example, 0 would indicate that the current value is the end of the window,
 * and 2 would indicate that the window ends at 2 data values after the
 * current.
 * \param min_observations The minimum allowed number of non-NULL values in the
 * moving window for the emitted value to be non-NULL. 0 indicates that all
 * values must be non-NULL.
 *
 * Returns an SArray of the same length as the input, with a type that matches
 * the type output by the aggregation function.
 * 
 * Throws an exception if:
 *  - window_end < window_start
 *  - The window size is excessively large (currently hardcoded to UINT_MAX).
 *  - The given function name corresponds to a function that will not operate
 *  on the data type of the input SArray.
 *  - The aggregation function returns more than one non-NULL types.
 *  
 * 
 */
std::shared_ptr<sarray<flexible_type>> rolling_apply(
    const sarray<flexible_type> &input,
    std::string fn_name, 
    ssize_t window_start,
    ssize_t window_end,
    size_t min_observations=0);


/// Aggregate functions
template<typename Iterator>
flexible_type mean(Iterator first, Iterator last) {
  flexible_type f = 0.0f;
  size_t count = 0;
  size_t non_null_count = 0;
  for(; first != last; ++first, ++count) {
    if(first->get_type() == flex_type_enum::UNDEFINED)
      continue;
    f += *first;
    ++non_null_count;
  }

  // Avoid division by zero
  if(!non_null_count)
    return flex_undefined();

  //TODO: Should I divide at end, or each iteration?
  return f / float(non_null_count);
}

template<typename Iterator>
flexible_type sum(Iterator first, Iterator last) {
  flexible_type f = 0.0f;
  for(; first != last; ++first) {
    if(first->get_type() != flex_type_enum::UNDEFINED)
      f += *first;
  }

  return f;
}

template<typename Iterator>
flexible_type min(Iterator first, Iterator last) {
  flexible_type min_val = flex_undefined();
  for(; first != last; ++first) {
    if(first->get_type() != flex_type_enum::UNDEFINED) {
      if(min_val.get_type() != flex_type_enum::UNDEFINED && *first < min_val) {
        min_val = *first;
      }
    }
  }

  return min_val;
}

template<typename Iterator>
flexible_type max(Iterator first, Iterator last) {
  flexible_type max_val = flex_undefined();
  for(; first != last; ++first) {
    if(first->get_type() != flex_type_enum::UNDEFINED) {
      if(max_val.get_type() != flex_type_enum::UNDEFINED && *first > max_val) {
        max_val = *first;
      }
    }
  }

  return max_val;
}

template<typename Iterator>
flexible_type count(Iterator first, Iterator last) {
  flexible_type ret_count = flex_int(0);
  for(; first != last; ++first) {
    if(first->get_type() != flex_type_enum::UNDEFINED) {
      ++ret_count;
    }
  }

  return ret_count;
}


template<typename Iterator>
flexible_type median(Iterator first, Iterator last) {
  flexible_type median_val;

  // Count non-NULL 
  std::vector<flexible_type> non_null_values;
  for(; first != last; ++first) {
    if(first->get_type() != flex_type_enum::UNDEFINED) {
      non_null_values.push_back(*first);
    }
  }

  auto nn_size = non_null_values.size();
  if(nn_size == 0) {
    median_val = flex_undefined();
  } else if(nn_size == 1) {
    median_val = non_null_values[0];
  } else if((nn_size % 2) == 0) {
    auto last_val = nn_size / 2;
    auto first_val = last_val-1;
    median_val =
      float(non_null_values[last_val]+non_null_values[first_val]) / 2.0f;
  } else {
    median_val = float(non_null_values[nn_size / 2]);
  }

  return median_val;
}
    
/**
 * Scans the current window to check for the number of non-NULL values.
 *
 * Returns true if the number of non-NULL values is >= min_observations, false
 * otherwise.
 */
template<typename Iterator>
bool has_min_observations(size_t min_observations,
                          Iterator first,
                          Iterator last) {
  size_t observations = 0;
  size_t count = 0;
  bool need_all = (min_observations == 0);
  for(; first != last; ++first, ++count) {
    if(first->get_type() != flex_type_enum::UNDEFINED) {
      ++observations;
      if(!need_all && (observations >= min_observations)) {
        return true;
      }
    }
  }

  if(need_all)
    return (observations == count);

  return false;
}

/**
 * Given the string name of the aggregation function, returns the corresponding
 * function.
 * 
 * Throws exception if:
 *  - The name does not correspond to an implemented function.
 */
template <typename Iterator>
std::function<flexible_type(Iterator,Iterator)> agg_string_to_fn(
    const std::string &fn_name) {
  if(fn_name == "mean") {
    return mean<Iterator>;
  } else {
    log_and_throw("Invalid function name: " + fn_name);
  }

  // Should never get here
  return nullptr;
}

} // namespace rolling_aggregate
} // namespace graphlab
#endif // GRAPHLAB_SFRAME_ROLLING_AGGREGATE_HPP
