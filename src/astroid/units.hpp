#ifndef ASTROID_UNITS_HPP
#define ASTROID_UNITS_HPP

#include <astroid/common.hpp>

#ifndef _WIN32
// ignore warnings for GCC
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// This is the start of a system for checking the units used in calculations.
//
// Note that Boost already includes an extensive system for doing this.
// However, I'm choosing not to use it for various reasons...
// * It's focused on compile-time unit checking whereas Astroid needs
//   considerable runtime flexibility.  For example, it's impossible to
//   represent images that can store either Gray or HU in a single type.
// * To use it properly, a lot of ASTROID would need to be overhauled
//   (e.g., geometric and mathemetical primitives would need to carry units).
//   At the moment, there isn't really time for that.
// * I fear the impact on compilation times.
//
// The idea is to initially implement very simple unit tracking, where
// functions check that input units are correct and record their output units,
// and eventually expand the system towards a runtime version of Boost.Units
// (or perhaps even a compile-time/runtime hybrid).

namespace astroid {

using units = std::string;

// CHECKING

class unit_mismatch : public exception
{
 public:
    unit_mismatch(units const& a, units const& b)
        : astroid::exception(
            "unit mismatch"
            "\n  a: "
            + a + "\n  b: " + b),
          a_(new units(a)),
          b_(new units(b))
    {
    }
    ~unit_mismatch() throw()
    {
    }

    units const&
    a() const
    {
        return *a_;
    }
    units const&
    b() const
    {
        return *b_;
    }

 private:
    std::shared_ptr<units> a_;
    std::shared_ptr<units> b_;
};

static inline void
check_matching_units(units const& a, units const& b)
{
    if (a != b)
        throw unit_mismatch(a, b);
}

class incorrect_units : public exception
{
 public:
    incorrect_units(units const& expected, units const& actual)
        : astroid::exception(
            "incorrect units"
            "\n  expected: "
            + expected + "\n  actual: " + actual),
          expected_(new units(expected)),
          actual_(new units(actual))
    {
    }
    ~incorrect_units() throw()
    {
    }

    units const&
    expected() const
    {
        return *expected_;
    }
    units const&
    actual() const
    {
        return *actual_;
    }

 private:
    std::shared_ptr<units> expected_;
    std::shared_ptr<units> actual_;
};

static inline void
check_units(units const& expected, units const& actual)
{
    if (actual != expected)
        throw incorrect_units(expected, actual);
}

// STANDARD UNITS

static units const no_units("");

// length

static units const millimeters("mm");
static units const meters("m");

// dose

static units const gray("Gy");
static units const gray_rbe("Gy(RBE)");

// other

static units const relative_stopping_power("relative stopping power");
static units const hounsfield_units("HU");

} // namespace astroid

#endif
