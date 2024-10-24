#ifndef ASTROID_GEOMETRY_COMMON_HPP
#define ASTROID_GEOMETRY_COMMON_HPP

#include <cmath>
#include <ostream>

#include <cradle/typing/core/omissible.h>
#include <cradle/typing/core/type_definitions.h>

#include <astroid/c_array.hpp>
#include <astroid/common.hpp>
#include <astroid/geometry/forward.hpp>
#include <astroid/math/common.hpp>

namespace astroid {

// VECTOR

// componentwise arithmetic assignment operators
#define ASTROID_COMPONENT_OPERATOR(op)                                        \
    template<unsigned N, class T>                                             \
    vector<N, T>& operator op(vector<N, T>& a, vector<N, T> const& b)         \
    {                                                                         \
        for (unsigned i = 0; i < N; ++i)                                      \
            a[i] op b[i];                                                     \
        return a;                                                             \
    }
ASTROID_COMPONENT_OPERATOR(+=)
ASTROID_COMPONENT_OPERATOR(-=)
ASTROID_COMPONENT_OPERATOR(*=)
ASTROID_COMPONENT_OPERATOR(/=)
#undef ASTROID_COMPONENT_OPERATOR

// scalar arithmetic assignment operators
#define ASTROID_SCALAR_OPERATOR(op)                                           \
    template<unsigned N, class T, class Scalar>                               \
    vector<N, T>& operator op(vector<N, T>& a, Scalar b)                      \
    {                                                                         \
        for (unsigned i = 0; i < N; ++i)                                      \
            a[i] op b;                                                        \
        return a;                                                             \
    }
ASTROID_SCALAR_OPERATOR(*=)
ASTROID_SCALAR_OPERATOR(/=)
ASTROID_SCALAR_OPERATOR(%=)
#undef ASTROID_SCALAR_OPERATOR

// componentwise operators
#define ASTROID_COMPONENT_OPERATOR(op)                                        \
    template<unsigned N, class T>                                             \
    vector<N, T> operator op(vector<N, T> const& a, vector<N, T> const& b)    \
    {                                                                         \
        vector<N, T> r;                                                       \
        for (unsigned i = 0; i < N; ++i)                                      \
            r[i] = a[i] op b[i];                                              \
        return r;                                                             \
    }
ASTROID_COMPONENT_OPERATOR(+)
ASTROID_COMPONENT_OPERATOR(-)
ASTROID_COMPONENT_OPERATOR(*)
ASTROID_COMPONENT_OPERATOR(/)
#undef ASTROID_COMPONENT_OPERATOR

// scalar operators
#define ASTROID_COMMUTATIVE_SCALAR_OPERATOR(op)                               \
    template<unsigned N, class T, class Scalar>                               \
    vector<N, T> operator op(vector<N, T> const& a, Scalar b)                 \
    {                                                                         \
        vector<N, T> r;                                                       \
        for (unsigned i = 0; i < N; ++i)                                      \
            r[i] = a[i] op b;                                                 \
        return r;                                                             \
    }                                                                         \
    template<unsigned N, class T, class Scalar>                               \
    vector<N, T> operator op(Scalar a, vector<N, T> const& b)                 \
    {                                                                         \
        vector<N, T> r;                                                       \
        for (unsigned i = 0; i < N; ++i)                                      \
            r[i] = a op b[i];                                                 \
        return r;                                                             \
    }
#define ASTROID_SCALAR_OPERATOR(op)                                           \
    template<unsigned N, class T, class Scalar>                               \
    vector<N, T> operator op(vector<N, T> const& a, Scalar b)                 \
    {                                                                         \
        vector<N, T> r;                                                       \
        for (unsigned i = 0; i < N; ++i)                                      \
            r[i] = a[i] op b;                                                 \
        return r;                                                             \
    }
ASTROID_COMMUTATIVE_SCALAR_OPERATOR(*)
ASTROID_SCALAR_OPERATOR(/)
ASTROID_SCALAR_OPERATOR(%)
#undef ASTROID_COMMUTATIVE_SCALAR_OPERATOR
#undef ASTROID_SCALAR_OPERATOR

// unary negation
template<unsigned N, class T>
vector<N, T>
operator-(vector<N, T> const& v)
{
    vector<N, T> r;
    for (unsigned i = 0; i < N; ++i)
        r[i] = -v[i];
    return r;
}

// dot product of two vectors
template<unsigned N, class T>
T
dot(vector<N, T> const& a, vector<N, T> const& b)
{
    T result = 0;
    for (unsigned i = 0; i < N; ++i)
        result += a[i] * b[i];
    return result;
}

// vector length
template<unsigned N, class T>
T
length(vector<N, T> const& v)
{
    return std::sqrt(length2(v));
}

// vector length squared - This is more efficient, and it's sufficient if
// you're using the length for comparison purposes.
template<unsigned N, class T>
T
length2(vector<N, T> const& v)
{
    return dot(v, v);
}

// Get the unit-length version of the given vector.
template<unsigned N, class T>
vector<N, T>
unit(vector<N, T> const& v)
{
    return v / length(v);
}

// Create a vector whose components all have the same value.
template<unsigned N, class T>
vector<N, T>
uniform_vector(T value)
{
    vector<N, T> r;
    for (unsigned i = 0; i < N; ++i)
        r[i] = value;
    return r;
}

// hash function
template<unsigned N, class T>
size_t
hash_value(astroid::vector<N, T> const& v)
{
    size_t h = 0;
    for (unsigned i = 0; i != N; ++i)
        h = cradle::combine_hashes(h, cradle::invoke_hash(v[i]));
    return h;
}

} // namespace astroid

#define ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(N, type)                     \
    template<>                                                                \
    struct cradle::normalization_uuid_str<astroid::vector<N, type>>           \
    {                                                                         \
        static const inline std::string func{"normalization<vector_" #N       \
                                             "D_" #type ",func>"};            \
        static const inline std::string coro{"normalization<vector_" #N       \
                                             "D_" #type ",coro>"};            \
    };                                                                        \
                                                                              \
    template<>                                                                \
    struct cradle::normalization_uuid_str<                                    \
        std::vector<astroid::vector<N, type>>>                                \
    {                                                                         \
        static const inline std::string func{                                 \
            "normalization<vector_vector_" #N "D_" #type ",func>"};           \
        static const inline std::string coro{                                 \
            "normalization<vector_vector_" #N "D_" #type ",coro>"};           \
    };

ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(1, unsigned)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(1, int)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(1, double)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(1, float)

ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(2, unsigned)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(2, int)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(2, double)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(2, float)

ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(3, unsigned)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(3, int)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(3, double)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(3, float)

ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(4, unsigned)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(4, int)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(4, double)
ASTROID_DEFINE_VECTOR_NORMALIZATION_UUID(4, float)

namespace astroid {

// BOX

// A box is an N-dimensional generalization of a rectangle. In one dimension,
// it's a range; in two, a rectangle; in three, a rectangular prism; etc.
// It's always axis-aligned. It's represented by a corner point (the one with
// the smaller coordinates) and its size.
template<unsigned N, class T>
struct box
{
    box()
    {
    }

    box(vector<N, T> const& corner, vector<N, T> const& size)
        : corner(corner), size(size)
    {
    }

    // explicit conversion from a box with a different coordinate type
    template<class OtherT>
    explicit box(box<N, OtherT> const& other)
        : corner(other.corner), size(other.size)
    {
    }

    vector<N, T> corner, size;

    MSGPACK_DEFINE(corner, size);
};

template<unsigned N, typename T>
std::ostream&
operator<<(std::ostream& out, astroid::box<N, T> const& box)
{
    out << "[corner: " << box.corner << ", size: " << box.size << "]";
    return out;
}

// equality operators
template<unsigned N, class T>
bool
operator==(box<N, T> const& a, box<N, T> const& b)
{
    return a.corner == b.corner && a.size == b.size;
}
template<unsigned N, class T>
bool
operator!=(box<N, T> const& a, box<N, T> const& b)
{
    return !(a == b);
}
// < operator
template<unsigned N, class T>
bool
operator<(box<N, T> const& a, box<N, T> const& b)
{
    return a.corner < b.corner || a.corner == b.corner && a.size < b.size;
}

template<unsigned N, class T>
vector<N, T>
get_center(box<N, T> const& b)
{
    return b.corner + b.size / 2;
}

template<unsigned N, class T>
vector<N, T>
get_low_corner(box<N, T> const& b)
{
    return b.corner;
}

template<unsigned N, class T>
vector<N, T>
get_high_corner(box<N, T> const& b)
{
    return b.corner + b.size;
}

// Is the point p inside the given box?
template<unsigned N, class T>
bool
is_inside(astroid::box<N, T> const& box, vector<N, T> const& p)
{
    for (unsigned i = 0; i < N; ++i)
    {
        if (p[i] < box.corner[i] || p[i] >= box.corner[i] + box.size[i])
            return false;
    }
    return true;
}

// Does the projection of p along the given axis lie within the projection of
// the box along the same axis?
template<unsigned N, class T>
bool
is_inside(astroid::box<N, T> const& box, unsigned axis, T p)
{
    assert(axis < N);
    return p >= box.corner[axis] && p < box.corner[axis] + box.size[axis];
}

// Compute the intersection of the two boxes.
// The return value is whether or not the boxes actually intersect.
// The result is only valid if they intersect.
template<unsigned N, class T>
bool
compute_intersection(
    box<N, T>* result,
    astroid::box<N, T> const& box1,
    astroid::box<N, T> const& box2)
{
    for (unsigned i = 0; i < N; i++)
    {
        T low = box1.corner[i] > box2.corner[i] ? box1.corner[i]
                                                : box2.corner[i];
        T high1 = box1.corner[i] + box1.size[i];
        T high2 = box2.corner[i] + box2.size[i];
        T high = high1 < high2 ? high1 : high2;
        if (low >= high)
            return false;
        result->corner[i] = low;
        result->size[i] = high - low;
    }
    return true;
}

// Test if the two boxes are overlapping.
template<unsigned N, class T>
bool
overlapping(astroid::box<N, T> const& box1, astroid::box<N, T> const& box2)
{
    for (unsigned i = 0; i < N; i++)
    {
        T low = (std::max)(box1.corner[i], box2.corner[i]);
        T high
            = (std::min)(get_high_corner(box1)[i], get_high_corner(box2)[i]);
        if (low > high)
            return false;
    }
    return true;
}

// Add a uniform border around the given box and return the expanded box.
template<unsigned N, class T>
box<N, T>
add_border(astroid::box<N, T> const& box, T border)
{
    return astroid::box<N, T>(
        box.corner - uniform_vector<N, T>(border),
        box.size + uniform_vector<N, T>(border * 2));
}

// Add a border around the given box and return the expanded box.
template<unsigned N, class T>
box<N, T>
add_border(astroid::box<N, T> const& box, vector<N, T> const& border)
{
    return astroid::box<N, T>(box.corner - border, box.size + border * 2);
}

template<typename T>
void
make_polygon(vector<2, T>* vertices, box<2, T> const& box)
{
    vertices[0] = box.corner;
    vertices[1] = box.corner + make_vector<T>(box.size[0], 0);
    vertices[2] = box.corner + box.size;
    vertices[3] = box.corner + make_vector<T>(0, box.size[1]);
}

// Expand a box (if necessary) to include the given point. - If an expansion is
// necessary, it's done so that the point lies on the edge of the box.
template<unsigned N, class T>
void
expand_box_to_include_point(box<N, T>& box, vector<N, T> const& point)
{
    for (unsigned i = 0; i != N; ++i)
    {
        // Expand in the negative direction.
        if (box.corner[i] > point[i])
        {
            box.size[i] += box.corner[i] - point[i];
            box.corner[i] = point[i];
        }
        // Expand in the positive direction.
        if (box.corner[i] + box.size[i] < point[i])
        {
            box.size[i] = point[i] - box.corner[i];
        }
    }
}

template<typename T>
int
get_edge_index(box<2, T> const& box, vector<2, T> const& point)
{
    double tol = 1.0e-10;
    int index = -1;
    if ((std::fabs(point[1] - box.corner[1]) <= tol)
        && (point[0] >= box.corner[0]))
    {
        index = 0;
    }
    if ((std::fabs(point[0] - (box.corner[0] + box.size[0])) <= tol)
        && (point[1] >= box.corner[1]))
    {
        index = 1;
    }
    if ((std::fabs(point[1] - (box.corner[1] + box.size[1])) <= tol)
        && (point[0] > box.corner[0]))
    {
        index = 2;
    }
    if ((std::fabs(point[0] - box.corner[0]) <= tol)
        && (point[1] > box.corner[1]))
    {
        index = 3;
    }
    return index;
}

api(struct preexisting(comparisons, iostream) with(N : 1, 2, 3, 4; T : double))
template<unsigned N, class T>
struct box
{
    // Vector of N dimensions, with N being 1 thru 4, that defines the lower
    // left corner
    vector<N, T> corner;
    // Vector of N dimensions, with N being 1 thru 4, that defines the length,
    // width, etc
    vector<N, T> size;
};

// Get the area of a box.
template<typename T>
T
area(astroid::box<2, T> const& box)
{
    return box.size[0] * box.size[1];
}

// Clamp the given point such that it lies within the given box.
template<unsigned N, typename T>
vector<N, T>
clamp(vector<N, T> const& p, astroid::box<N, T> const& box)
{
    vector<N, T> result;
    for (unsigned i = 0; i < N; i++)
    {
        result[i]
            = astroid::clamp(p[i], box.corner[i], box.corner[i] + box.size[i]);
    }
    return result;
}

// check to see if the given point lies within the given box
template<unsigned N, typename T>
bool
contains(astroid::box<N, T> const& box, vector<N, T> const& p)
{
    for (unsigned i = 0; i < N; ++i)
    {
        if ((p[i] < box.corner[i]) || (box.corner[i] + box.size[i] < p[i]))
        {
            return false;
        }
    }
    return true;
}

// Slice an axis off the given box.
template<unsigned N, typename T>
box<N - 1, T>
slice(astroid::box<N, T> const& box, unsigned axis)
{
    assert(axis < N);
    return astroid::box<N - 1, T>(
        slice(box.corner, axis), slice(box.size, axis));
}

template<unsigned N, typename T>
box<N + 1, T>
unslice(box<N, T> const& b, unsigned axis, box<1, T> const& slice)
{
    assert(axis < N + 1);
    return astroid::box<N + 1, T>(
        unslice(b.corner, axis, slice.corner[0]),
        unslice(b.size, axis, slice.size[0]));
}

template<unsigned N, typename T>
box<N, T>
scale(astroid::box<N, T> const& box, double factor)
{
    for (unsigned i = 0; i != N; ++i)
    {
        box.corner[i] *= factor;
        box.size[i] *= factor;
    }
}

template<unsigned N, typename T>
box<N, T>
bounding_box(box<N, T> const& b)
{
    return b;
}

template<unsigned N, typename T>
void
compute_bounding_box(optional<box<N, T>>& box, astroid::box<N, T> const& b)
{
    if (box)
    {
        for (unsigned i = 0; i < N; ++i)
        {
            if (b.corner[i] < box.get().corner[i])
            {
                box.get().corner[i] = b.corner[i];
            }
            if (get_high_corner(b)[i] > get_high_corner(box.get())[i])
            {
                box.get().size[i]
                    = get_high_corner(b)[i] - box.get().corner[i];
            }
        }
    }
    else
        box = b;
}

// MATRIX

// M x N matrix class - T is the type of the scalar elements, M is the number
// of rows, and N is the number of columns.
template<unsigned M, unsigned N, class T>
class matrix
{
 public:
    matrix()
    {
    }

    // construct from a matrix of the same size but a different element type
    template<class OtherT>
    explicit matrix(matrix<M, N, OtherT> const& other)
    {
        typename matrix<M, N, OtherT>::const_iterator j = other.begin();
        iterator end = this->end();
        for (iterator i = begin(); i != end; ++i, ++j)
            *i = static_cast<T>(*j);
    }

    // access individual elements
    T const&
    operator()(unsigned row, unsigned column) const
    {
        assert(row < M && column < N);
        return data_[row * N + column];
    }
    T&
    operator()(unsigned row, unsigned column)
    {
        assert(row < M && column < N);
        return data_[row * N + column];
    }

    // iterator interface
    typedef T* iterator;
    iterator
    begin()
    {
        return data_;
    }
    iterator
    end()
    {
        return data_ + M * N;
    }

    // const iterator interface
    typedef T const* const_iterator;
    const_iterator
    begin() const
    {
        return data_;
    }
    const_iterator
    end() const
    {
        return data_ + M * N;
    }

    // scalar *
    matrix&
    operator*=(T s)
    {
        iterator end = this->end();
        for (iterator i = begin(); i != end; ++i)
            *i *= s;
        return *this;
    }
    matrix
    operator*(T s) const
    {
        matrix r = (*this);
        r *= s;
        return r;
    }
    // scalar /
    matrix&
    operator/=(T s)
    {
        iterator end = this->end();
        for (iterator i = begin(); i != end; ++i)
            *i /= s;
        return *this;
    }
    matrix
    operator/(T s) const
    {
        matrix r = (*this);
        r /= s;
        return r;
    }

    // componentwise +
    matrix&
    operator+=(matrix const& other)
    {
        const_iterator j = other.begin();
        iterator end = this->end();
        for (iterator i = begin(); i != end; ++i, ++j)
            *i += *j;
        return *this;
    }
    matrix
    operator+(matrix const& other) const
    {
        matrix r = (*this);
        r += other;
        return r;
    }
    // componentwise -
    matrix&
    operator-=(matrix const& other)
    {
        const_iterator j = other.begin();
        iterator end = this->end();
        for (iterator i = begin(); i != end; ++i, ++j)
            *i -= *j;
        return *this;
    }
    matrix
    operator-(matrix const& other) const
    {
        matrix r = (*this);
        r -= other;
        return r;
    }

    // comparisons
    bool
    operator==(matrix const& other) const
    {
        return std::mismatch(begin(), end(), other.begin()).first == end();
    }
    bool
    operator!=(matrix const& other) const
    {
        return !(*this == other);
    }
    bool
    operator<(matrix const& other) const
    {
        for (unsigned i = 0; i != M * N; ++i)
        {
            if (data_[i] < other.data_[i])
                return true;
            else if (data_[i] > other.data_[i])
                return false;
        }
        return false;
    }

    // get the ith column as a vector
    vector<M, T>
    get_column(unsigned i) const
    {
        vector<M, T> v;
        for (unsigned j = 0; j < M; ++j)
        {
            v[j] = (*this)(j, i);
        }
        return v;
    }

    // set the ith column using a vector
    void
    set_column(unsigned i, vector<M, T> const& v)
    {
        for (unsigned j = 0; j < M; ++j)
            (*this)(j, i) = v[j];
    }

    // *= operator
    matrix<M, N, T>&
    operator*=(matrix<N, N, T> const& other)
    {
        *this = *this * other;
        return *this;
    }

 private:
    T data_[M * N];
};

template<unsigned M, unsigned N, typename T>
std::ostream&
operator<<(std::ostream& out, matrix<M, N, T> const& m)
{
    out << "[";
    for (unsigned i = 0; i != M; ++i)
    {
        for (unsigned j = 0; j != N; ++j)
        {
            if (i != 0 || j != 0)
                out << ", ";
            out << m(i, j);
        }
    }
    out << "]";
    return out;
}

// 2x2 constructor
template<class T>
matrix<2, 2, T>
make_matrix(T v00, T v01, T v10, T v11)
{
    matrix<2, 2, T> m;
    m(0, 0) = v00;
    m(0, 1) = v01;
    m(1, 0) = v10;
    m(1, 1) = v11;
    return m;
}
// 3x3 constructor
template<class T>
matrix<3, 3, T>
make_matrix(T v00, T v01, T v02, T v10, T v11, T v12, T v20, T v21, T v22)
{
    matrix<3, 3, T> m;
    m(0, 0) = v00;
    m(0, 1) = v01;
    m(0, 2) = v02;
    m(1, 0) = v10;
    m(1, 1) = v11;
    m(1, 2) = v12;
    m(2, 0) = v20;
    m(2, 1) = v21;
    m(2, 2) = v22;
    return m;
}
// 4x4 constructor
template<class T>
matrix<4, 4, T>
make_matrix(
    T v00,
    T v01,
    T v02,
    T v03,
    T v10,
    T v11,
    T v12,
    T v13,
    T v20,
    T v21,
    T v22,
    T v23,
    T v30,
    T v31,
    T v32,
    T v33)
{
    matrix<4, 4, T> m;
    m(0, 0) = v00;
    m(0, 1) = v01;
    m(0, 2) = v02;
    m(0, 3) = v03;
    m(1, 0) = v10;
    m(1, 1) = v11;
    m(1, 2) = v12;
    m(1, 3) = v13;
    m(2, 0) = v20;
    m(2, 1) = v21;
    m(2, 2) = v22;
    m(2, 3) = v23;
    m(3, 0) = v30;
    m(3, 1) = v31;
    m(3, 2) = v32;
    m(3, 3) = v33;
    return m;
}

template<unsigned M, unsigned N, class T, class Scalar>
matrix<M, N, T>
operator*(Scalar s, matrix<M, N, T> const& m)
{
    matrix<M, N, T> r = m;
    r *= s;
    return r;
}

// matrix multiplication
template<class T, unsigned M, unsigned N, unsigned O>
matrix<M, O, T>
operator*(matrix<M, N, T> const& a, matrix<N, O, T> const& b)
{
    matrix<M, O, T> r;
    for (unsigned i = 0; i < M; i++)
    {
        for (unsigned j = 0; j < O; j++)
        {
            r(i, j) = 0;
            for (unsigned k = 0; k < N; k++)
                r(i, j) += a(i, k) * b(k, j);
        }
    }
    return r;
}

// Get the identity matrix.
template<unsigned N, class T>
matrix<N, N, T>
identity_matrix()
{
    matrix<N, N, T> m;
    typename matrix<N, N, T>::iterator p = m.begin();
    for (unsigned i = 0; i < N; ++i)
        for (unsigned j = 0; j < N; ++j, ++p)
            *p = static_cast<T>((i == j) ? 1 : 0);
    return m;
}

// Get the inverse of the given matrix.
// 3x3 case
template<class T>
matrix<3, 3, T>
inverse(matrix<3, 3, T> const& m)
{
    matrix<3, 3, T> inv = make_matrix<T>(
        m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1),
        m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2),
        m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1),
        m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2),
        m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0),
        m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2),
        m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0),
        m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1),
        m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0));
    T det = m(0, 0) * inv(0, 0) + m(0, 1) * inv(1, 0) + m(0, 2) * inv(2, 0);
    if (det == 0)
        return make_matrix<T>(0, 0, 0, 0, 0, 0, 0, 0, 0);
    inv *= T(1) / det;
    return inv;
}
// 2x2 case
template<class T>
matrix<2, 2, T>
inverse(matrix<2, 2, T> const& m)
{
    T det = m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
    if (det == 0)
        return make_matrix<T>(0, 0, 0, 0);
    T inv_det = static_cast<T>(1.0) / det;
    matrix<2, 2, T> inv = make_matrix(m(1, 1), -m(0, 1), -m(1, 0), m(0, 0));
    inv *= inv_det;
    return inv;
}

// hash function
} // namespace astroid
template<unsigned M, unsigned N, class T>
struct std::hash<astroid::matrix<M, N, T>>
{
    size_t
    operator()(astroid::matrix<M, N, T> const& m) const
    {
        size_t h = 0;
        for (unsigned i = 0; i != M; ++i)
        {
            for (unsigned j = 0; j != N; ++j)
                h = cradle::combine_hashes(h, cradle::invoke_hash(m(i, j)));
        }
        return h;
    }
};

#define ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(M, N, type)                  \
    template<>                                                                \
    struct cradle::normalization_uuid_str<astroid::matrix<M, N, type>>        \
    {                                                                         \
        static const inline std::string func{"normalization<matrix_" #M       \
                                             "x" #N "_" #type ",func>"};      \
        static const inline std::string coro{"normalization<matrix_" #M       \
                                             "x" #N "_" #type ",coro>"};      \
    };

ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(1, 1, double)
ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(1, 1, float)

ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(2, 2, double)
ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(2, 2, float)

ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(3, 3, double)
ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(3, 3, float)

ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(4, 4, double)
ASTROID_DEFINE_MATRIX_NORMALIZATION_UUID(4, 4, float)

namespace astroid {

// ANGLES

static inline double
degrees_to_radians(double degrees)
{
    return degrees * (pi / 180);
}

static inline double
radians_to_degrees(double radians)
{
    return radians * (180 / pi);
}

// 2D TRANSFORMATIONS

// Generate a rotation matrix.
template<class T>
matrix<3, 3, T>
rotation_matrix(T a)
{
    T c = std::cos(a), s = std::sin(a);
    return make_matrix<T>(c, -s, 0, s, c, 0, 0, 0, 1);
}

// Generate a translation matrix.
template<class T>
matrix<3, 3, T>
translation_matrix(vector<2, T> const& v)
{
    return make_matrix<T>(1, 0, v[0], 0, 1, v[1], 0, 0, 1);
}

// Generate a scaling matrix.
template<class T>
matrix<3, 3, T>
scaling_matrix(vector<2, T> const& v)
{
    return make_matrix<T>(v[0], 0, 0, 0, v[1], 0, 0, 0, 1);
}

// Transform a vector by a transformation matrix.
template<class T>
vector<2, T>
transform(matrix<3, 3, T> const& m, vector<2, T> const& v)
{
    return make_vector(
        v[0] * m(0, 0) + v[1] * m(0, 1) + m(0, 2),
        v[0] * m(1, 0) + v[1] * m(1, 1) + m(1, 2));
}

// Transform a box by a transformation matrix.
template<typename T>
box<2, T>
transform_box(matrix<3, 3, T> const& m, box<2, T> const& b)
{
    // Start with a box that just includes the transformation of one corner of
    // the original box.
    box<2, T> result = make_box(transform(m, b.corner), make_vector<T>(0, 0));
    // Now expand that box to include the transformation of each of the other
    // corners.
    expand_box_to_include_point(
        result, transform(m, b.corner + make_vector<T>(b.size[0], 0)));
    expand_box_to_include_point(result, transform(m, b.corner + b.size));
    expand_box_to_include_point(
        result, transform(m, b.corner + make_vector<T>(0, b.size[1])));
    return result;
}

// CUBIC BEZIERS

// unit_cubic_bezier represents a cubic bezier whose end points are (0, 0)
// and (1, 1).
struct unit_cubic_bezier
{
    vector<2, double> p1, p2;

    unit_cubic_bezier()
    {
    }
    unit_cubic_bezier(vector<2, double> const& p1, vector<2, double> const& p2)
        : p1(p1), p2(p2)
    {
    }
    unit_cubic_bezier(double p1x, double p1y, double p2x, double p2y)
    {
        p1[0] = p1x;
        p1[1] = p1y;
        p2[0] = p2x;
        p2[1] = p2y;
    }
};

// Evaluate a unit_cubic_bezier at the given x value.
// Since this is an approximation, the caller must specify an epsilon value
// to control the error in the result.
double
eval_curve_at_x(unit_cubic_bezier const& curve, double x, double epsilon);

// 1D constructor
template<class T>
vector<1, T>
make_vector(T x)
{
    vector<1, T> v;
    v[0] = x;
    return v;
}
// 4D constructor
template<class T>
vector<4, T>
make_vector(T x, T y, T z, T w)
{
    vector<4, T> v;
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
    return v;
}

// Given a vector, this returns a corresponding vector with one less dimension
// by removing the value at index i.
template<unsigned N, class T>
vector<N - 1, T>
slice(vector<N, T> const& p, unsigned i)
{
    assert(i < N);
    vector<N - 1, T> r;
    for (unsigned j = 0; j < i; ++j)
        r[j] = p[j];
    for (unsigned j = i; j < N - 1; ++j)
        r[j] = p[j + 1];
    return r;
}

// Given a vector, this returns a corresponding vector with one more dimension
// by inserting the given value at index i.
template<unsigned N, class T, class OtherValue>
vector<N + 1, T>
unslice(vector<N, T> const& p, unsigned i, OtherValue value)
{
    assert(i <= N);
    vector<N + 1, T> r;
    for (unsigned j = 0; j < i; ++j)
        r[j] = p[j];
    r[i] = static_cast<T>(value);
    for (unsigned j = i; j < N; ++j)
        r[j + 1] = p[j];
    return r;
}

// 3D cross product of two vectors
template<class T>
vector<3, T>
cross(vector<3, T> const& a, vector<3, T> const& b)
{
    return make_vector<T>(
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]);
}

// 2D cross product of two vectors
template<class T>
T
cross(vector<2, T> const& a, vector<2, T> const& b)
{
    return a[0] * b[1] - a[1] * b[0];
}

// Get a unit vector that's perpendicular to the given one.
template<class T>
vector<3, T>
get_perpendicular(vector<3, T> const& v)
{
    vector<3, T> u;
    if ((std::abs)(v[0]) < (std::abs)(v[1]))
    {
        if ((std::abs)(v[0]) < (std::abs)(v[2]))
            u = make_vector<T>(1, 0, 0);
        else
            u = make_vector<T>(0, 0, 1);
    }
    else
    {
        if ((std::abs)(v[1]) < (std::abs)(v[2]))
            u = make_vector<T>(0, 1, 0);
        else
            u = make_vector<T>(0, 0, 1);
    }
    return unit(cross(u, v));
}

template<unsigned N, typename T>
T
product(vector<N, T> const& v)
{
    T r = 1;
    for (unsigned i = 0; i != N; ++i)
        r *= v[i];
    return r;
}

// Determine if the two vectors are almost equal.
template<unsigned N, class Value>
bool
almost_equal(vector<N, Value> const& a, vector<N, Value> const& b)
{
    for (unsigned i = 0; i < N; ++i)
    {
        if (!astroid::almost_equal<Value>(a[i], b[i]))
            return false;
    }
    return true;
}
template<unsigned N, class Value, class Tolerance>
bool
almost_equal(
    vector<N, Value> const& a, vector<N, Value> const& b, Tolerance tolerance)
{
    for (unsigned i = 0; i < N; ++i)
    {
        if (!astroid::almost_equal(a[i], b[i], static_cast<Value>(tolerance)))
        {
            return false;
        }
    }
    return true;
}

template<unsigned N, class T>
size_t
deep_sizeof(vector<N, T>)
{
    using cradle::deep_sizeof;
    // We assume that T has a static deep_sizeof.
    return N * deep_sizeof(T());
}
template<unsigned N, class T>
void
from_dynamic(vector<N, T>* x, cradle::dynamic const& v)
{
    using cradle::from_dynamic;
    cradle::dynamic_array const& array = cast<cradle::dynamic_array>(v);
    cradle::check_array_size(N, array.size());
    for (unsigned i = 0; i != N; ++i)
        from_dynamic(&(*x)[i], array[i]);
}
template<unsigned N, class T>
void
to_dynamic(cradle::dynamic* v, vector<N, T> const& x)
{
    using cradle::to_dynamic;
    cradle::dynamic_array array(N);
    for (unsigned i = 0; i != N; ++i)
        to_dynamic(&array[i], x[i]);
    *v = std::move(array);
}

// MATRIX

using astroid::identity_matrix;
using astroid::make_matrix;
using astroid::matrix;

typedef matrix<3, 3, double> transformation_matrix_2d;
typedef matrix<4, 4, double> transformation_matrix_3d;

} // namespace astroid
template<unsigned M, unsigned N, class T>
struct cradle::type_info_query<astroid::matrix<M, N, T>>
{
    static void
    get(api_type_info* info)
    {
        *info = make_api_type_info_with_array_type(api_array_info{
            some(integer(M)),
            make_api_type_info_with_array_type(
                api_array_info{some(integer(N)), get_type_info<T>()})});
    }
};
namespace astroid {

template<unsigned M, unsigned N, class T>
size_t
deep_sizeof(matrix<M, N, T>)
{
    using cradle::deep_sizeof;
    // We assume that T has a static deep_sizeof.
    return M * N * deep_sizeof(T());
}
template<unsigned M, unsigned N, class T>
void
from_dynamic(matrix<M, N, T>* x, cradle::dynamic const& v)
{
    using cradle::from_dynamic;
    cradle::dynamic_array const& array = cast<cradle::dynamic_array>(v);
    cradle::check_array_size(M, array.size());
    for (unsigned i = 0; i != M; ++i)
    {
        cradle::dynamic_array const& row
            = cast<cradle::dynamic_array>(array[i]);
        cradle::check_array_size(N, row.size());
        for (unsigned j = 0; j != N; ++j)
            from_dynamic(&(*x)(i, j), row[j]);
    }
}
template<unsigned M, unsigned N, class T>
void
to_dynamic(cradle::dynamic* v, matrix<N, M, T> const& x)
{
    using cradle::to_dynamic;
    cradle::dynamic_array array(M);
    for (unsigned i = 0; i != M; ++i)
    {
        cradle::dynamic_array row(N);
        for (unsigned j = 0; j != N; ++j)
            to_dynamic(&row[j], x(i, j));
        array[i] = std::move(row);
    }
    *v = std::move(array);
}

// Get the inverse of the given 4x4 matrix.
template<typename T>
matrix<4, 4, T>
inverse(matrix<4, 4, T> const& m)
{
    T fa0 = m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
    T fa1 = m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0);
    T fa2 = m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0);
    T fa3 = m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1);
    T fa4 = m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1);
    T fa5 = m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2);
    T fb0 = m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0);
    T fb1 = m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0);
    T fb2 = m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0);
    T fb3 = m(2, 1) * m(2, 2) - m(2, 2) * m(2, 1);
    T fb4 = m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1);
    T fb5 = m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2);

    T det = fa0 * fb5 - fa1 * fb4 + fa2 * fb3 + fa3 * fb2 - fa4 * fb1
            + fa5 * fb0;
    if (det == 0)
        return make_matrix<T>(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    T inv_det = static_cast<T>(1.0) / det;

    matrix<4, 4, T> inv = make_matrix<T>(
        +m(1, 1) * fb5 - m(1, 2) * fb4 + m(1, 3) * fb3,
        -m(0, 1) * fb5 + m(0, 2) * fb4 - m(0, 3) * fb3,
        +m(3, 1) * fa5 - m(3, 2) * fa4 + m(3, 3) * fa3,
        -m(2, 1) * fa5 + m(2, 2) * fa4 - m(2, 3) * fa3,
        -m(1, 0) * fb5 + m(1, 2) * fb2 - m(1, 3) * fb1,
        +m(0, 0) * fb5 - m(0, 2) * fb2 + m(0, 3) * fb1,
        -m(3, 0) * fa5 + m(3, 2) * fa2 - m(3, 3) * fa1,
        +m(2, 0) * fa5 - m(2, 2) * fa2 + m(2, 3) * fa1,
        +m(1, 0) * fb4 - m(1, 1) * fb2 + m(1, 3) * fb0,
        -m(0, 0) * fb4 + m(0, 1) * fb2 - m(0, 3) * fb0,
        +m(3, 0) * fa4 - m(3, 1) * fa2 + m(3, 3) * fa0,
        -m(2, 0) * fa4 + m(2, 1) * fa2 - m(2, 3) * fa0,
        -m(1, 0) * fb3 + m(1, 1) * fb1 - m(1, 2) * fb0,
        +m(0, 0) * fb3 - m(0, 1) * fb1 + m(0, 2) * fb0,
        -m(3, 0) * fa3 + m(3, 1) * fa1 - m(3, 2) * fa0,
        +m(2, 0) * fa3 - m(2, 1) * fa1 + m(2, 2) * fa0);
    inv *= inv_det;
    return inv;
}

template<unsigned N, typename T>
matrix<N - 1, N - 1, T>
slice(matrix<N, N, T> const& m, unsigned axis)
{
    matrix<N - 1, N - 1, T> r;
    for (unsigned i = 0; i < N; ++i)
    {
        for (unsigned j = 0; j < N; ++j)
            r(i, j) = m(i >= axis ? i + 1 : i, j >= axis ? j + 1 : j);
    }
}

// OTHER STUFF

// a circle
api(struct with(T : double))
template<typename T>
struct circle
{
    vector<2, T> center;
    T radius;
};

template<typename T>
T
area(circle<T> const& circle)
{
    return pi * circle.radius * circle.radius;
}

template<typename T>
bool
is_inside(circle<T> const& circle, vector<2, T> const& p)
{
    return length(p - circle.center) <= circle.radius;
}

// a plane in 3D place
api(struct with(T : double))
template<typename T>
struct plane
{
    vector<3, T> point;
    vector<3, T> normal;
};

// A ray is the combination of an origin point and a direction vector.
// The direction vector should be a unit vector.
/// N is the number of dimensions of the space in which the ray exists.
/// T is the type of the coordinates.
api(struct with(N : 2, 3; T : double))
template<unsigned N, typename T>
struct ray
{
    vector<N, T> origin;
    vector<N, T> direction;
};

ASTROID_GEOMETRY_DEFINE_FLOATING_TYPEDEFS(ray)

template<unsigned N, typename T>
struct line_segment : c_array<2, vector<N, T>>
{
    line_segment()
    {
    }
    line_segment(vector<N, T> const& v0, vector<N, T> const& v1)
    {
        (*this)[0] = v0;
        (*this)[1] = v1;
    }
};

} // namespace astroid
template<unsigned N, class T>
struct std::hash<astroid::line_segment<N, T>>
{
    size_t
    operator()(astroid::line_segment<N, T> const& x) const
    {
        return cradle::combine_hashes(
            cradle::invoke_hash(x[0]), cradle::invoke_hash(x[1]));
    }
};
namespace astroid {

template<unsigned N, typename T>
line_segment<N, T>
make_line_segment(vector<N, T> const& v0, vector<N, T> const& v1)
{
    return line_segment<N, T>(v0, v1);
}

// Get a vector from the first point on the given line segment to the second.
template<unsigned N, typename T>
vector<N, T>
as_vector(line_segment<N, T> const& segment)
{
    return segment[1] - segment[0];
}

// Get a point along the given line segment u[0,1].
template<unsigned N, typename T>
vector<N, T>
point_along(line_segment<N, T> const& segment, double u)
{
    return segment[0] + (u * (segment[1] - segment[0]));
}
// Get a point along the given line segment u[0,1].
template<unsigned N, typename T>
vector<N, T>
point_along(vector<N, T> const& s0, vector<N, T> const& s1, double u)
{
    return s0 + (u * (s1 - s0));
}

// Calculate the length of the line segment.
template<unsigned N, typename T>
T
length(line_segment<N, T> const& segment)
{
    return length(as_vector(segment));
}

template<unsigned N, typename T>
struct triangle : c_array<3, vector<N, T>>
{
    typedef vector<N, T> vertex_type;

    triangle()
    {
    }

    triangle(
        vertex_type const& v0, vertex_type const& v1, vertex_type const& v2)
    {
        (*this)[0] = v0;
        (*this)[1] = v1;
        (*this)[2] = v2;
    }
};

// hash function
} // namespace astroid
template<unsigned N, class T>
struct std::hash<astroid::triangle<N, T>>
{
    size_t
    operator()(astroid::triangle<N, T> const& x) const
    {
        return cradle::combine_hashes(
            cradle::invoke_hash(x[0]),
            cradle::combine_hashes(
                cradle::invoke_hash(x[1]), cradle::invoke_hash(x[2])));
    }
};
namespace astroid {

template<typename T>
vector<3, T>
get_normal(triangle<3, T> const& tri)
{
    return unit(cross(tri[1] - tri[0], tri[2] - tri[0]));
}

template<typename T>
bool
is_ccw(triangle<2, T> const& tri)
{
    vector<2, T> edge0 = tri[1] - tri[0];
    vector<2, T> edge1 = tri[2] - tri[1];
    return (edge0[0] * edge1[1]) - (edge0[1] * edge1[0]) > 0;
}

template<typename T>
T
get_area(triangle<2, T> const& tri)
{
    return std::fabs(cross(tri[1] - tri[0], tri[2] - tri[0])) * 0.5;
}

template<typename T>
T
get_area(triangle<3, T> const& tri)
{
    return length(cross(tri[1] - tri[0], tri[2] - tri[0])) * 0.5;
}

template<unsigned N, typename T>
box<N, T>
bounding_box(triangle<N, T> const& tri)
{
    vector<N, T> min = tri[0];
    vector<N, T> max = tri[0];
    for (unsigned j = 0; j < N; ++j)
    {
        if (tri[1][j] < min[j])
        {
            min[j] = tri[1][j];
        }
        if (tri[2][j] < min[j])
        {
            min[j] = tri[2][j];
        }
        if (tri[1][j] > max[j])
        {
            max[j] = tri[1][j];
        }
        if (tri[2][j] > max[j])
        {
            max[j] = tri[2][j];
        }
    }
    return astroid::box<N, T>(min, max - min);
}

// API functions

// Expand or contract a box by adding a uniform margin around the edge.
api(fun trivial with(N : 1, 2, 3, 4))
template<unsigned N>
box<N, double>
add_margin_to_box(
    astroid::box<N, double> const& box, vector<N, double> const& size)
{
    return add_border(box, size);
}

// Compute the inverse of a matrix.
api(fun with(N : 2, 3, 4))
template<unsigned N>
// The matrix representing the inverse of the provided matrix
matrix<N, N, double>
matrix_inverse(
    // The matrix that is to inversed
    matrix<N, N, double> const& m)
{
    return inverse(m);
}

// Compute the product of two matrices.
api(fun with(N : 1, 2, 3, 4))
template<unsigned N>
// The matrix equivalent to the product of the provided matrices.
matrix<N, N, double>
matrix_product(
    // The provided matrix that is to be multiplied by the second
    matrix<N, N, double> const& a,
    // The provided matrix that is to be multiplied by the first
    matrix<N, N, double> const& b)
{
    return a * b;
}

template<unsigned N, typename T>
void
compute_bounding_box(
    optional<box<N, T>>& box, std::vector<vector<N, T>> const& points)
{
    typename std::vector<vector<N, T>>::const_iterator i = points.begin(),
                                                       end = points.end();
    if (i == end)
        return;

    vector<N, T> min, max;
    if (box)
    {
        min = box->corner;
        max = get_high_corner(*box);
    }
    else
    {
        min = *i;
        max = *i;
        ++i;
    }

    for (; i != end; ++i)
    {
        for (unsigned j = 0; j < N; ++j)
        {
            if ((*i)[j] < min[j])
                min[j] = (*i)[j];
            if ((*i)[j] > max[j])
                max[j] = (*i)[j];
        }
    }

    box = astroid::box<N, T>(min, max - min);
}

// Compute the bounding box of a list of points.
api(fun with(N : 1, 2, 3, 4; T : double) name(point_list_bounding_box))
template<unsigned N, typename T>
// Bounding box size N of the given point list
box<N, T>
bounding_box(
    // List of point vectors in which to compute bounding box
    std::vector<vector<N, T>> const& points)
{
    optional<box<N, T>> box;
    compute_bounding_box(box, points);
    return box ? *box
               : astroid::box<N, T>(
                     uniform_vector<N, T>(0), uniform_vector<N, T>(0));
}

} // namespace astroid

#endif
