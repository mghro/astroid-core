#ifndef ASTROID_C_ARRAY_HPP
#define ASTROID_C_ARRAY_HPP

#include <astroid/common.hpp>

namespace astroid {

// C arrays that act as regular ASTROID types
template<unsigned N, class T>
struct c_array
{
    T const&
    operator[](size_t i) const
    {
        return elements[i];
    }
    T&
    operator[](size_t i)
    {
        return elements[i];
    }

    typedef T value_type;

    static unsigned const length = N;

    T elements[N];

    // STL-like interface
    T const*
    begin() const
    {
        return &elements[0];
    }
    T const*
    end() const
    {
        return begin() + N;
    }
    T*
    begin()
    {
        return &elements[0];
    }
    T*
    end()
    {
        return begin() + N;
    }
    size_t
    size() const
    {
        return size_t(N);
    }
};
template<unsigned N, class T>
bool
operator==(c_array<N, T> const& a, c_array<N, T> const& b)
{
    for (unsigned i = 0; i != N; ++i)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}
template<unsigned N, class T>
bool
operator!=(c_array<N, T> const& a, c_array<N, T> const& b)
{
    return !(a == b);
}
template<unsigned N, class T>
bool
operator<(c_array<N, T> const& a, c_array<N, T> const& b)
{
    for (unsigned i = 0; i != N; ++i)
    {
        if (a[i] < b[i])
            return true;
        if (b[i] < a[i])
            return false;
    }
    return false;
}
} // namespace astroid
template<unsigned N, class T>
struct cradle::type_info_query<astroid::c_array<N, T>>
{
    static void
    get(api_type_info* info)
    {
        *info = make_api_type_info_with_array_type(
            api_array_info{some(integer(N)), get_type_info<T>()});
    }
};
namespace astroid {
template<unsigned N, class T>
size_t
deep_sizeof(c_array<N, T> const& x)
{
    using cradle::deep_sizeof;
    size_t size = 0;
    for (auto const& element : x)
    {
        size += deep_sizeof(element);
    }
    return size;
}
template<unsigned N, class T>
void
swap(c_array<N, T>& a, c_array<N, T>& b)
{
    for (unsigned i = 0; i != N; ++i)
        swap(a[i], b[i]);
}
template<unsigned N, class T>
void
from_dynamic(c_array<N, T>* x, cradle::dynamic const& v)
{
    cradle::dynamic_array const& array = cast<cradle::dynamic_array>(v);
    cradle::check_array_size(N, array.size());
    for (unsigned i = 0; i != N; ++i)
        from_dynamic(&(*x)[i], array[i]);
}
template<unsigned N, class T>
void
to_dynamic(cradle::dynamic* v, c_array<N, T> const& x)
{
    cradle::dynamic_array array(N);
    for (unsigned i = 0; i != N; ++i)
        to_dynamic(&array[i], x[i]);
    *v = std::move(array);
}
template<unsigned N, class T>
std::ostream&
operator<<(std::ostream& s, c_array<N, T> const& x)
{
    s << "{";
    for (unsigned i = 0; i != N; ++i)
    {
        if (i != 0)
            s << ",";
        s << x[i];
    }
    s << "}";
    return s;
}
} // namespace astroid

template<unsigned N, class T>
struct std::hash<astroid::c_array<N, T>>
{
    size_t
    operator()(astroid::c_array<N, T> const& x) const
    {
        size_t h = 0;
        for (size_t i = 0; i != N; ++i)
            h = cradle::combine_hashes(h, cradle::invoke_hash(x[i]));
        return h;
    }
};

#endif
