#ifndef ASTROID_IMAGING_MERGE_SLICES_HPP
#define ASTROID_IMAGING_MERGE_SLICES_HPP

#include <astroid/imaging/slicing.hpp>
#include <astroid/imaging/variant.hpp>
#include <astroid/math/common.hpp>

namespace astroid {

// Given a range of N-dimensional slices, this merges the slices to create an
// (N + 1)-dimensional image.
api(fun disk_cached monitored with(N : 2; T
                                   : variant;
                                   SP
                                   : shared) execution_class(cpu.x4))
template<unsigned N, class T, class SP>
image<N + 1, T, shared>
merge_slices(std::vector<image_slice<N, T, SP>> const& slices);

// With this version, you can manually specify the interpolation grid.
template<unsigned N, class T, class SP>
image<N + 1, T, shared>
merge_slices(
    std::vector<image_slice<N, T, SP>> const& slices,
    regular_grid<1, double> const& interpolation_grid);

} // namespace astroid

#include <astroid/imaging/merge_slices.ipp>

#endif
