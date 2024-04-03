#ifndef ASTROID_GEOMETRY_CLIPPER_HPP
#define ASTROID_GEOMETRY_CLIPPER_HPP

#include <astroid/geometry/polygonal.hpp>
#include <clipper.hpp>

// This file provides an interface between ASTROID's polygonal geometry
// representations and the Clipper library, which provides some useful
// (and very efficient) algorithms.

namespace astroid {

// Since Clipper uses integer coordinates, we need to convert the floating
// point coordinates that ASTROID uses to integers.
// This scale factor determines the relationship between integer values
// and floating point values.
// Since ASTROID spatial coordinates are generally in mm, this gives one micron
// of precision in the integer representation.
double const clipper_integer_precision = 0.001;

typedef ClipperLib::IntPoint clipper_point;

clipper_point
to_clipper(vector<2, double> const& p);

vector<2, double>
from_clipper(clipper_point const& p);

typedef ClipperLib::Path clipper_polygon2;

ClipperLib::Path
to_clipper(polygon2 const& poly);

void
to_clipper(ClipperLib::Path* cp, polygon2 const& poly);

polygon2
from_clipper(ClipperLib::Path const& cp);

void
from_clipper(polygon2* poly, ClipperLib::Path const& cp);

typedef ClipperLib::Path clipper_poly;
typedef ClipperLib::Paths clipper_polyset;

ClipperLib::Paths
to_clipper(polyset const& astroid_set);

void
to_clipper(ClipperLib::Paths* clipper_set, polyset const& astroid_set);

polyset
from_clipper(ClipperLib::Paths const& clipper_set);

void
from_clipper(polyset* astroid_set, ClipperLib::Paths const& clipper_set);

// The following provides the interface to make a Clipper polyset a regular
// ASTROID type. This allows Clipper polysets to be used directly as part of
// the ASTROID API.

} // namespace astroid

template<>
struct cradle::type_info_query<astroid::clipper_polyset>
    : cradle::type_info_query<astroid::polygon2>
{
};

namespace astroid {

bool
operator==(clipper_polyset const& a, clipper_polyset const& b);
bool
operator!=(clipper_polyset const& a, clipper_polyset const& b);
bool
operator<(clipper_polyset const& a, clipper_polyset const& b);

void
to_dynamic(cradle::dynamic* v, clipper_poly const& x);
void
from_dynamic(clipper_poly* x, cradle::dynamic const& v);
void
to_dynamic(cradle::dynamic* v, clipper_polyset const& x);
void
from_dynamic(clipper_polyset* x, cradle::dynamic const& v);

} // namespace astroid

// hash function
template<>
struct std::hash<astroid::clipper_polyset>
{
    size_t
    operator()(astroid::clipper_polyset const& x) const
    {
        return cradle::invoke_hash(astroid::from_clipper(x));
    }
};

namespace astroid {

// operations on Clipper polysets

double
get_area(clipper_polyset const& set);

} // namespace astroid

#endif
