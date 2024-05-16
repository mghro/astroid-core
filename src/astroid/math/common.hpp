#ifndef ASTROID_MATH_COMMON_HPP
#define ASTROID_MATH_COMMON_HPP

#include <cassert>
#include <cmath>
#include <limits>

#include <astroid/array.hpp>
#include <astroid/common.hpp>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace astroid {

// Compute the square of a number.
template<class T>
T
square(T x)
{
    return x * x;
}

template<class T>
T
round(T x)
{
    return floor(x + 0.5);
}

// Stores double min/max values
api(struct with(T : double))
template<class T>
struct min_max
{
    // Minimum value
    T min;
    // Maximum value
    T max;
};

template<class T>
T
center_of_range(min_max<T> const& range)
{
    return (range.min + range.max) / 2;
}

api(struct with(T : double))
template<class T>
struct statistics
{
    optional<T> min, max, mean;
    double n_samples;
    optional<size_t> max_element_index;
};

// Compute the mean of the given range of values.
template<typename Value, class Iterator>
Value
compute_mean(Iterator begin, Iterator end, Value zero)
{
    Value sum = zero;
    int n = 0;
    for (Iterator i = begin; i != end; ++i)
    {
        sum += *i;
        ++n;
    }
    return sum / n;
}

// Similar to above, but uses a range instead of a pair of iterators.
template<typename Value, class Range>
Value
compute_mean(Range const& range, Value zero)
{
    return compute_mean(range.begin(), range.end(), zero);
}

// computes a % b, but ensures that the result is in the range [0, b)
template<typename T>
T
nonnegative_mod(T a, T b)
{
    assert(b > 0);
    T r = a % b;
    if (r < 0)
        r += b;
    assert(r >= 0 && r < b);
    return r;
}

bool
is_power_of_two(unsigned n);

// LINEAR FUNCTION - f(x) = mx + b form
api(struct with(T : double))
template<class T>
struct linear_function
{
    // Intercept of the linear function
    T intercept;
    // Slope of the linear function
    T slope;
};

// Simple function that evaluates a linear function with the independent
// variable value of x.
api(fun trivial with(T : double; X : double) name(apply_linear_function))
template<typename T, typename X>
// Value of linear function at x.
T
apply(
    // The function to evaluate.
    linear_function<T> const& f,
    // The value of independent variable at which to evaluate.
    X x)
{
    return f.slope * x + f.intercept;
}

// an error to indicate that the inverse of a 0-slope function was requested
class undefined_inverse : public exception
{
 public:
    template<typename T>
    undefined_inverse(linear_function<T> const& f)
        : exception("inverse undefined for function: " + to_string(f))
    {
    }
};
// Get the inverse of a linear function.
template<typename T>
linear_function<T>
inverse(linear_function<T> const& f)
{
    if (f.slope == 0)
        throw undefined_inverse(f);
    return linear_function<T>(-f.intercept / f.slope, 1 / f.slope);
}

// QUADRATIC FUNCTION - f(x) = ax^2 + bx + c
api(struct with(T : double))
template<class T>
struct quadratic_function
{
    // Coefficient on the x^2 term.
    T a;
    // Coefficient on the x term.
    T b;
    // Constant term.
    T c;
};

// Simple function that evaluates a quadratic function with the independent
// variable value of x.
template<typename T, typename X>
// Value of quadratic function at x.
T
apply(
    // The function to evaluate.
    quadratic_function<T> const& f,
    // The value of independent variable at which to evaluate.
    X x)
{
    return f.a * x * x + f.b * x + f.c;
}

api(fun trivial)
double
apply_quadratic_function(quadratic_function<double> const& f, double x)
{
    return apply(f, x);
}

// Tests for approximate floating point equality...

template<typename T>
T
default_equality_tolerance()
{
    return std::numeric_limits<T>::epsilon() * 100;
}

// Test if a and b are within the given tolerance.
template<typename T>
inline bool
almost_equal(T a, T b, T tolerance = default_equality_tolerance<T>())
{
    return std::fabs(a - b) <= tolerance;
}
// Test if a and b are within the given tolerance or a is less than b.
template<typename T>
inline bool
almost_less(T a, T b, T tolerance = default_equality_tolerance<T>())
{
    return a - b <= tolerance;
}
// Test if a and b are within the given tolerance or a is greater than b.
template<typename T>
inline bool
almost_greater(T a, T b, T tolerance = default_equality_tolerance<T>())
{
    return almost_less(b, a, tolerance);
}

static const double pi = 3.1415926535897932384626433832795;

api(fun trivial)
double
addition(double x, double y)
{
    return x + y;
}

api(fun trivial)
double
multiplication(double x, double y)
{
    return x * y;
}

api(fun remote execution_class(cpu.x1) with(T : double))
template<class T>
// Scaled array
array<T>
scale_array(
    // Array to scale
    array<T> const& value_array,
    // Multiplicative scale factor
    double scale_factor)
{
    array<T> result;
    auto res_ptr = allocate(&result, value_array.size());
    std::transform(
        value_array.begin(), value_array.end(), res_ptr, [=](T value) {
            return value * scale_factor;
        });
    return result;
}

api(fun remote execution_class(cpu.x4) with(T : float))
template<class T>
// Summed result array
array<T>
sum_array_list(
    // List of arrays to sum
    std::vector<array<T>> const& array_list)
{
    array<T> summed_values;
    if (array_list.empty())
    {
        return summed_values;
    }
    auto value_ptr = allocate(&summed_values, array_list[0].size());
    std::fill_n(value_ptr, array_list[0].size(), T(0));

    for (auto const& a : array_list)
    {
        if (a.size() != summed_values.size())
        {
            throw exception("sum_array_list: array sizes cannot be different");
        }
        for (size_t j = 0; j < a.size(); ++j)
        {
            value_ptr[j] += a[j];
        }
    }

    return summed_values;
}

} // namespace astroid

#endif
