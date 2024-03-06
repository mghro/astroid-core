#include <astroid/geometry/common.hpp>
#include <astroid/math/common.hpp>
#include <astroid/math/interpolate.hpp>

namespace astroid {

bool
is_power_of_two(unsigned n)
{
    return (n & (n - 1)) == 0 && n != 0;
}

// INTERPOLATED FUNCTIONS

void
shift(regularly_sampled_function& f, double amount)
{
    f.x0 += amount;
}

void
shift(irregularly_sampled_function& f, double amount)
{
    for (auto& sample : f.samples)
        sample[0] += amount;
}

void
initialize(
    interpolated_function& f,
    double x0,
    double x_spacing,
    std::vector<double> const& samples,
    astroid::outside_domain_policy outside_domain_policy)
{
    assert(x_spacing > 0);

    f = interpolated_function();

    f.x0 = x0;
    f.x_spacing = x_spacing;

    if (samples.size() > 1)
    {
        size_t n_samples = samples.size();
        function_sample* f_samples = allocate(&f.samples, n_samples - 1);
        for (size_t i = 0; i < n_samples - 1; ++i)
        {
            function_sample& s = f_samples[i];
            s.value = samples[i];
            s.delta = samples[i + 1] - samples[i];
        }
    }

    f.outside_domain_policy = outside_domain_policy;
}

api(fun)
optional<min_max<double>>
regularly_sampled_function_range(regularly_sampled_function const& f)
{
    if (f.samples.empty())
        return none;
    double y0 = f.samples.front();
    double min = y0, max = y0;
    for (auto const& sample : f.samples)
    {
        double y = sample;
        if (y < min)
            min = y;
        if (y > max)
            max = y;
    }
    return min_max<double>(min, max);
}

std::vector<vector<2, double>>
regularly_sampled_function_as_point_list(regularly_sampled_function const& f)
{
    std::vector<vector<2, double>> points;
    points.reserve(f.samples.size());
    auto x = f.x0;
    for (auto const& sample : f.samples)
    {
        points.push_back(make_vector(x, sample));
        x += f.x_spacing;
    }
    return points;
}

void
initialize(interpolated_function& f, regularly_sampled_function const& data)
{
    initialize(
        f, data.x0, data.x_spacing, data.samples, data.outside_domain_policy);
}

void
interpolate_and_initialize(
    interpolated_function& f,
    std::vector<double> const& x,
    std::vector<double> const& y,
    astroid::outside_domain_policy outside_domain_policy)
{
    auto grid = compute_interpolation_grid(x);

    std::vector<double> interpolated_values;
    interpolate(&interpolated_values, grid, y, x);

    initialize(
        f,
        grid.p0[0],
        grid.spacing[0],
        interpolated_values,
        outside_domain_policy);
}

void
initialize(interpolated_function& f, irregularly_sampled_function const& data)
{
    size_t n_samples = data.samples.size();
    std::vector<double> x(n_samples), y(n_samples);
    for (size_t i = 0; i != n_samples; ++i)
    {
        x[i] = data.samples[i][0];
        y[i] = data.samples[i][1];
    }
    interpolate_and_initialize(f, x, y, data.outside_domain_policy);
}

api(fun)
optional<min_max<double>>
irregularly_sampled_function_range(irregularly_sampled_function const& f)
{
    if (f.samples.empty())
        return none;
    double y0 = f.samples.front()[1];
    double min = y0, max = y0;
    for (auto const& sample : f.samples)
    {
        double y = sample[1];
        if (y < min)
            min = y;
        if (y > max)
            max = y;
    }
    return min_max<double>(min, max);
}

std::vector<vector<2, double>>
irregularly_sampled_function_as_point_list(
    irregularly_sampled_function const& f)
{
    return f.samples;
}

double
sample(interpolated_function const& f, double x)
{
    if (x < f.x0)
    {
        switch (f.outside_domain_policy)
        {
            case outside_domain_policy::ALWAYS_ZERO:
                return 0;
            case outside_domain_policy::EXTEND_WITH_COPIES:
            default:
                return f.samples.size() == 0 ? 0 : f.samples[0].value;
        }
    }
    else
    {
        double adjusted_x = (x - f.x0) / f.x_spacing;
        size_t index = static_cast<size_t>(adjusted_x);
        if (index >= f.samples.size())
        {
            switch (f.outside_domain_policy)
            {
                case outside_domain_policy::ALWAYS_ZERO:
                    return 0;
                case outside_domain_policy::EXTEND_WITH_COPIES:
                default: {
                    if (f.samples.size() == 0)
                        return 0;
                    auto const& last_sample = f.samples[f.samples.size() - 1];
                    return last_sample.value + last_sample.delta;
                }
            }
        }
        function_sample const& s = f.samples[index];
        return s.value + (adjusted_x - double(index)) * s.delta;
    }
}

regular_grid<1, double>
get_sample_grid(interpolated_function const& f)
{
    return regular_grid<1, double>(
        make_vector(f.x0),
        make_vector(f.x_spacing),
        make_vector(unsigned(f.samples.size() + 1)));
}

void
shift(interpolated_function& f, double amount)
{
    f.x0 += amount;
}

double
sample_map(std::map<double, double> const& data, double x)
{
    auto high = data.upper_bound(x);
    if (high == data.begin())
    {
        // x is less than first value, return first value
        return data.begin()->second;
    }
    else if (high == data.end())
    {
        // x is past the end value, return last value
        return data.rbegin()->second;
    }

    auto low = high;
    --low;
    double percent = (x - low->first) / (high->first - low->first);
    return low->second + percent * (high->second - low->second);
}

void
rescale(interpolated_function& f, double scale)
{
    array<function_sample> new_sample_array;
    size_t n_samples = f.samples.size();
    function_sample* new_samples = allocate(&new_sample_array, n_samples);
    for (size_t i = 0; i != n_samples; ++i)
    {
        auto const& original_sample = f.samples[i];
        auto& new_sample = new_samples[i];
        new_sample.value = original_sample.value * scale;
        new_sample.delta = original_sample.delta * scale;
    }
    f.samples = new_sample_array;
}

} // namespace astroid
