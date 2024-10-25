#ifndef ASTROID_MATH_INTERPOLATE_HPP
#define ASTROID_MATH_INTERPOLATE_HPP

#include <vector>

#include <astroid/geometry/regular_grid.hpp>
#include <astroid/math/common.hpp>

namespace astroid {

template<class Value, class Fraction, bool IsInteger>
struct interpolator
{
    static Value
    apply(Value a, Value b, Fraction f)
    {
        return Value(std::floor(a * (1 - f) + b * f + 0.5));
    }
};
template<class Value, class Fraction>
struct interpolator<Value, Fraction, false>
{
    static Value
    apply(Value a, Value b, Fraction f)
    {
        return Value(a * (1 - f) + b * f);
    }
};
template<class Value, class Fraction>
Value
interpolate(Value a, Value b, Fraction f)
{
    return interpolator<
        Value,
        Fraction,
        std::numeric_limits<Value>::is_integer>::apply(a, b, f);
}

// Interpolate the given data samples onto an evenly spaced grid.
// interpolated_values will receive the interpolated values.
// source_values and source_positions specify the values and positions of the
// source data.
// Note that interpolated_values should have the same number of elements as
// interpolated_grid has points, and source_values and source_points should
// have the same number of elements.
template<class Value>
void
interpolate(
    std::vector<Value>* interpolated_values,
    regular_grid<1, double> const& interpolation_grid,
    std::vector<Value> const& source_values,
    std::vector<double> const& source_positions)
{
    interpolated_values->resize(interpolation_grid.n_points[0]);

    typename std::vector<Value>::iterator interpolated_value
        = interpolated_values->begin();
    typename std::vector<Value>::iterator interpolated_values_end
        = interpolated_values->end();

    typename std::vector<Value>::const_iterator source_value
        = source_values.begin();
    typename std::vector<Value>::const_iterator next_source_value
        = source_value + 1;
    typename std::vector<Value>::const_iterator source_values_end
        = source_values.end();

    typename std::vector<double>::const_iterator source_position
        = source_positions.begin();
    typename std::vector<double>::const_iterator next_source_position
        = source_position + 1;

    double interpolated_position = interpolation_grid.p0[0];

    // Fill all interpolated values located before the first source point with
    // the value of the first source point.  In the future, it might be better
    // to have a policy to dictate this behavior.
    while (interpolated_position < *source_position
           && interpolated_value != interpolated_values_end)
    {
        *interpolated_value++ = *source_value;
        interpolated_position += interpolation_grid.spacing[0];
    }

    // Interpolate.
    while (next_source_value != source_values_end
           && interpolated_value != interpolated_values_end)
    {
        double f = (interpolated_position - *source_position)
                   / (*next_source_position - *source_position);
        *interpolated_value++
            = interpolate(*source_value, *next_source_value, f);

        interpolated_position += interpolation_grid.spacing[0];

        while (next_source_value != source_values_end
               && *next_source_position < interpolated_position)
        {
            source_value = next_source_value;
            ++next_source_value;
            source_position = next_source_position;
            ++next_source_position;
        }
    }

    // Fill all interpolated values located after the last source point with
    // the value of the last source point.
    while (interpolated_value != interpolated_values_end)
        *interpolated_value++ = *source_value;
}

// Given a list of sample positions (one-dimensional), this calculates an
// evenly spaced grid whose points will lie close to the sample positions.
// The sample positions should be sorted beforehand.
// Note that this is an optimization problem, and for certain position sets,
// different solutions may appear better depending on your preferences.
regular_grid<1, double>
compute_interpolation_grid(
    std::vector<double> const& sample_positions,
    double growth_tolerance = 10.);

// INTERPOLATED FUNCTIONS

// outside_domain_policy determines the behavior for the function when it's
// evaluated for a value outside the sample domain.
api(enum)
enum class outside_domain_policy
{
    // Alwayz zero
    ALWAYS_ZERO,
    // Extend with copies
    EXTEND_WITH_COPIES
};

api(struct)
struct function_sample
{
    // this sample
    double value;
    // the next sample minus this one
    double delta;
};

// interpolated_function represents a set of discrete data samples that are
// interpolated to create a continuous function.
// Note that the samples must be evenly spaced. Unevenly spaced data should
// be interpolated first onto a regular grid.
api(struct)
struct interpolated_function
{
    // the x value of the first sample
    double x0;

    // the spacing between consecutive samples
    double x_spacing;

    // the samples (struct: function_sample)
    array<function_sample> samples;

    // outside_domain_policy determines the behavior for the function when it's
    // evaluated for a value outside the sample domain.
    astroid::outside_domain_policy outside_domain_policy;
};

// regularly_sampled_function is the same as interpolated_function, but the
// samples haven't yet been preprocessed for fast interpolation.
// The advantage is that it doesn't contain redundant data, so it's more
// appropriate for external storage or manipulation.
api(struct)
struct regularly_sampled_function
{
    // the x value of the first sample
    double x0;

    // the spacing between consecutive samples
    double x_spacing;

    // the samples
    std::vector<double> samples;

    // outside_domain_policy determines the behavior for the function when it's
    // evaluated for a value outside the sample domain.
    astroid::outside_domain_policy outside_domain_policy;
};

api(fun)
optional<min_max<double>>
regularly_sampled_function_range(regularly_sampled_function const& f);

void
shift(regularly_sampled_function& f, double amount);

api(fun)
std::vector<vector<2, double>>
regularly_sampled_function_as_point_list(regularly_sampled_function const& f);

// irregularly_sampled_function represents irregular samples of a continuous
// function.
api(struct)
struct irregularly_sampled_function
{
    // the samples
    std::vector<astroid::vector<2, double>> samples;

    // outside_domain_policy determines the behavior for the function when it's
    // evaluated for a value outside the sample domain.
    astroid::outside_domain_policy outside_domain_policy;
};

void
shift(irregularly_sampled_function& f, double amount);

api(fun)
optional<min_max<double>>
irregularly_sampled_function_range(irregularly_sampled_function const& f);

api(fun)
std::vector<vector<2, double>>
irregularly_sampled_function_as_point_list(
    irregularly_sampled_function const& f);

void
initialize(
    interpolated_function& f,
    double x0,
    double x_spacing,
    std::vector<double> const& samples,
    astroid::outside_domain_policy outside_domain_policy);

void
initialize(interpolated_function& f, regularly_sampled_function const& data);

api(fun)
interpolated_function
make_regularly_spaced_interpolated_function(
    regularly_sampled_function const& f)
{
    interpolated_function result;
    initialize(result, f);
    return result;
}

void
initialize(interpolated_function& f, irregularly_sampled_function const& data);

api(fun)
interpolated_function
make_irregularly_spaced_interpolated_function(
    irregularly_sampled_function const& f)
{
    interpolated_function result;
    initialize(result, f);
    return result;
}

void
interpolate_and_initialize(
    interpolated_function& f,
    std::vector<double> const& x,
    std::vector<double> const& y,
    astroid::outside_domain_policy outside_domain_policy);

// Returns the data result of the given interpolated function at the given x
// value
api(fun trivial name(sample_interpolated_function))
// The data value resulting from the given function
double
sample(
    // The function that is used to find the data value
    interpolated_function const& f,
    // The x value passed into the interpolated function
    double x);

api(fun trivial name(interpolated_function_sample_grid))
regular_grid<1, double>
get_sample_grid(interpolated_function const& f);

void
shift(interpolated_function& f, double amount);

api(fun)
interpolated_function
shift_interpolated_function(
    interpolated_function const& f, double shift_amount)
{
    interpolated_function tmp = f;
    shift(tmp, shift_amount);
    return tmp;
}

} // namespace astroid

#define ASTROID_DEFINE_MAP_NORMALIZATION_UUID(key, value)                     \
    template<>                                                                \
    struct cradle::normalization_uuid_str<std::map<key, value>>               \
    {                                                                         \
        static const inline std::string func{"normalization<map_" #key        \
                                             "_" #value ",func>"};            \
        static const inline std::string coro{"normalization<map_" #key        \
                                             "_" #value ",coro>"};            \
    };

ASTROID_DEFINE_MAP_NORMALIZATION_UUID(double, double)

namespace astroid {

// Returns the data result by interpolating between the two items nearest to
// the given x value
api(fun trivial)
// The data value resulting from the given function
double
sample_map(
    // The data map that will be used to interpolate the data value
    std::map<double, double> const& data,
    // The x value passed into the interpolation data map
    double x);

void
rescale(interpolated_function& f, double scale);

api(fun)
interpolated_function
rescale_interpolated_function(
    interpolated_function const& f, double scale_factor)
{
    interpolated_function tmp = f;
    rescale(tmp, scale_factor);
    return tmp;
}

} // namespace astroid

#endif
