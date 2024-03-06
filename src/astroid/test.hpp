#ifndef ASTROID_TEST_HPP
#define ASTROID_TEST_HPP

#include <algorithm>
#include <astroid/io/file.hpp>
#include <astroid/math/common.hpp>

// When using Catch, warnings seem to inevitably creep in about parentheses.
#ifdef __clang__
#pragma clang diagnostic ignored "-Wlogical-op-parentheses"
#elif __GNUC__
#pragma GCC diagnostic ignored "-Wparentheses"
#endif

#include <catch2/catch.hpp>

namespace astroid {

// Get the directory that contains the test data.
file_path
test_data_directory();

} // namespace astroid

// Check that the given expressions are almost equal.
#define ASTROID_CHECK_ALMOST_EQUAL(expr1, expr2)                              \
    {                                                                         \
        using astroid::almost_equal;                                          \
        if (!almost_equal(expr1, expr2))                                      \
        {                                                                     \
            FAIL(                                                             \
                "ASTROID_CHECK_ALMOST_EQUAL() failed"                         \
                "\n    " #expr1 " != " #expr2 "\n    "                        \
                << expr1 << " != " << expr2);                                 \
        }                                                                     \
    }

// Check that the given expressions are within the given tolerance.
#define ASTROID_CHECK_WITHIN_TOLERANCE(expr1, expr2, tolerance)               \
    {                                                                         \
        using astroid::almost_equal;                                          \
        if (!almost_equal(expr1, expr2, tolerance))                           \
        {                                                                     \
            FAIL(                                                             \
                "ASTROID_CHECK_WITHIN_TOLERANCE() failed"                     \
                "\n    " #expr1 " != " #expr2 "\n    "                        \
                << expr1 << " != " << expr2                                   \
                << "\n    tolerance: " << tolerance);                         \
        }                                                                     \
    }

// Check that the given ranges are equal.
#define ASTROID_CHECK_RANGES_EQUAL_(begin1, end1, begin2, end2)               \
    {                                                                         \
        if (std::distance(begin1, end1) != std::distance(begin2, end2))       \
        {                                                                     \
            FAIL(                                                             \
                "ASTROID_CHECK_RANGES_EQUAL() failed"                         \
                "\n    (" #begin1 ", " #end1 ") != (" #begin2 ", " #end2      \
                ")"                                                           \
                "\n    sizes: "                                               \
                << std::distance(begin1, end1)                                \
                << " != " << std::distance(begin2, end2));                    \
        }                                                                     \
        else if (std::mismatch(begin1, end1, begin2).first != end1)           \
        {                                                                     \
            FAIL(                                                             \
                "ASTROID_CHECK_RANGES_EQUAL() failed"                         \
                "\n    (" #begin1 ", " #end1 "] != (" #begin2 ", " #end2      \
                "]"                                                           \
                "\n    at index "                                             \
                << std::distance(                                             \
                       begin1, std::mismatch(begin1, end1, begin2).first)     \
                << ": " << *std::mismatch(begin1, end1, begin2).first         \
                << " != " << *std::mismatch(begin1, end1, begin2).second);    \
        }                                                                     \
    }
#define ASTROID_CHECK_RANGES_EQUAL(range1, range2)                            \
    ASTROID_CHECK_RANGES_EQUAL_(                                              \
        range1.begin(), range1.end(), range2.begin(), range2.end())

namespace astroid {
namespace impl {

template<class IteratorT1, class IteratorT2>
bool
check_ranges_almost_equal(
    IteratorT1 const& begin1,
    IteratorT1 const& end1,
    IteratorT2 const& begin2,
    IteratorT2 const& end2,
    string& error)
{
    IteratorT1 i1 = begin1;
    IteratorT2 i2 = begin2;
    for (; i1 != end1; ++i1, ++i2)
    {
        using astroid::almost_equal;
        if (!almost_equal(*i1, *i2))
        {
            error = "at index " + to_string(std::distance(begin1, i1)) + ": "
                    + to_string(*i1) + " != " + to_string(*i2);
            return false;
        }
    }
    return true;
}
} // namespace impl
} // namespace astroid

// Check that the given ranges are almost equal.
#define ASTROID_CHECK_RANGES_ALMOST_EQUAL_(begin1, end1, begin2, end2)        \
    {                                                                         \
        if (std::distance(begin1, end1) != std::distance(begin2, end2))       \
        {                                                                     \
            FAIL(                                                             \
                "ASTROID_CHECK_RANGES_ALMOST_EQUAL() failed"                  \
                "\n    (" #begin1 ", " #end1 ") != (" #begin2 ", " #end2      \
                ")"                                                           \
                "\n    sizes: "                                               \
                << std::distance(begin1, end1)                                \
                << " != " << std::distance(begin2, end2));                    \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            std::string msg;                                                  \
            if (!astroid::impl::check_ranges_almost_equal(                    \
                    begin1, end1, begin2, end2, msg))                         \
            {                                                                 \
                FAIL(                                                         \
                    "ASTROID_CHECK_RANGES_ALMOST_EQUAL() failed"              \
                    "\n    (" #begin1 ", " #end1 ") != (" #begin2 ", " #end2  \
                    ")"                                                       \
                    << msg);                                                  \
            }                                                                 \
        }                                                                     \
    }
#define ASTROID_CHECK_RANGES_ALMOST_EQUAL(range1, range2)                     \
    ASTROID_CHECK_RANGES_ALMOST_EQUAL_(                                       \
        range1.begin(), range1.end(), range2.begin(), range2.end())

#endif
