#include <astroid/imaging/foreach.hpp>
#include <astroid/test.hpp>

namespace astroid {

template<class Pixel>
struct sequential_fill_fn
{
    Pixel value;
    Pixel increment;
    void
    operator()(Pixel& dst)
    {
        dst = value;
        value += increment;
    }
};
template<unsigned N, class Pixel, class SP, class Value>
void
sequential_fill(image<N, Pixel, SP>& img, Value initial, Value increment)
{
    sequential_fill_fn<Pixel> fn;
    fn.value = initial;
    fn.increment = increment;
    foreach_pixel(img, fn);
}

template<class Iter>
struct image_checker
{
    image_checker() : same(true)
    {
    }
    Iter i;
    bool same;
    template<class Pixel>
    void
    operator()(Pixel const& p)
    {
        if (p != *i)
            same = false;
        ++i;
    }
};

// Check that an image matches the given reference data.
template<unsigned N, class T, class SP, class Iter>
bool
check_image(image<N, T, SP> const& img, Iter begin, Iter end)
{
    image_checker<Iter> checker;
    checker.i = begin;
    foreach_pixel(img, checker);
    return checker.same;
}
#define ASTROID_CHECK_IMAGE(view, ref_begin, ref_end)                         \
    {                                                                         \
        REQUIRE(product((view).size) == (ref_end) - (ref_begin));             \
        REQUIRE(astroid::check_image(view, ref_begin, ref_end));              \
    }

} // namespace astroid
