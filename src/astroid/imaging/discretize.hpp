#ifndef ASTROID_IMAGING_DISCRETIZE_HPP
#define ASTROID_IMAGING_DISCRETIZE_HPP

#include <astroid/imaging/variant.hpp>

namespace astroid {

template<unsigned N, class DiscreteT, class ContinuousT, class SourceSP>
void
discretize(
    image<N, DiscreteT, shared>& result,
    image<N, ContinuousT, SourceSP> const& src,
    unsigned result_max);

template<unsigned N, class DiscreteT, class ContinuousT, class SourceSP>
void
discretize(
    image<N, DiscreteT, shared>& result,
    image<N, ContinuousT, SourceSP> const& src,
    linear_function<double> const& value_mapping);

template<unsigned N, class DiscreteT, class SourceSP>
void
discretize(
    image<N, DiscreteT, shared>& result,
    image<N, variant, SourceSP> const& src,
    linear_function<double> const& value_mapping);

} // namespace astroid

#include <astroid/imaging/discretize.ipp>

#endif
