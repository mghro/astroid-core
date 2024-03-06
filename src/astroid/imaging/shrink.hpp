#ifndef ASTROID_IMAGING_SHRINK_HPP
#define ASTROID_IMAGING_SHRINK_HPP

#include <astroid/imaging/variant.hpp>

namespace astroid {

// Shrinks a 2D image by a factor of N x N, where N is a positive integer.
// Each pixel in the result image is the average of the corresponding block of
// N x N pixels in the source image.
template<class Pixel, class SrcSP>
void
shrink_image(
    image<2, Pixel, unique>& result,
    image<2, Pixel, SrcSP> const& src,
    unsigned factor);

void
shrink_image(
    image<2, variant, shared>& result,
    image<2, variant, const_view> const& src,
    unsigned factor);

} // namespace astroid

#include <astroid/imaging/shrink.ipp>

#endif
