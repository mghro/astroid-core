#ifndef ASTROID_COMMON_HPP
#define ASTROID_COMMON_HPP

#include <algorithm>
#include <cassert>
#include <cradle/inner/core/type_definitions.h>
#include <cradle/inner/core/type_interfaces.h>
#include <cstdint>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <vector>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#ifndef _WIN32
// ignore warnings for GCC
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// General-purpose utility functions and types...

namespace astroid {

typedef long long counter_type;

using std::int16_t;
using std::int8_t;
using std::uint16_t;
using std::uint8_t;

typedef std::string string;

typedef char const* utf8_ptr;

struct utf8_string
{
    utf8_ptr begin, end;

    utf8_string()
    {
    }
    utf8_string(utf8_ptr begin, utf8_ptr end) : begin(begin), end(end)
    {
    }
};

static inline bool
is_empty(utf8_string const& text)
{
    return text.begin == text.end;
}

static inline utf8_string
as_utf8_string(string const& text)
{
    return utf8_string(text.c_str(), text.c_str() + text.length());
}

template<typename T>
T
clamp(T x, T min, T max)
{
    assert(min <= max);
    return (std::min)((std::max)(x, min), max);
}

// A flag_set is a set of flags, each of which represents a boolean property.
// It is implemented as a simple unsigned integer, where each bit represents
// a different property.
// (The property codes must be defined manually as constants.)
// The advantage of using this over plain unsigned integers is that this is
// type-safe and slightly more explicit.
// A flag_set has a Tag type that identifies the set of properties that go with
// it. Only properties/sets with the same tag can be combined.

// NO_FLAGS can be implicitly converted to any type of flag_set.
struct null_flag_set
{
};
static null_flag_set const NO_FLAGS = null_flag_set();

template<class Tag>
struct flag_set
{
    unsigned code;
    flag_set()
    {
    }
    flag_set(null_flag_set) : code(0)
    {
    }
    explicit flag_set(unsigned code) : code(code)
    {
    }
    // allows use within if statements without other unintended conversions
    typedef unsigned flag_set::*unspecified_bool_type;
    operator unspecified_bool_type() const
    {
        return code != 0 ? &flag_set::code : 0;
    }
};

template<class Tag>
flag_set<Tag>
operator|(flag_set<Tag> a, flag_set<Tag> b)
{
    return flag_set<Tag>(a.code | b.code);
}
template<class Tag>
flag_set<Tag>&
operator|=(flag_set<Tag>& a, flag_set<Tag> b)
{
    a.code |= b.code;
    return a;
}
template<class Tag>
flag_set<Tag>
operator&(flag_set<Tag> a, flag_set<Tag> b)
{
    return flag_set<Tag>(a.code & b.code);
}
template<class Tag>
flag_set<Tag>&
operator&=(flag_set<Tag>& a, flag_set<Tag> b)
{
    a.code &= b.code;
    return a;
}
template<class Tag>
bool
operator==(flag_set<Tag> a, flag_set<Tag> b)
{
    return a.code == b.code;
}
template<class Tag>
bool
operator!=(flag_set<Tag> a, flag_set<Tag> b)
{
    return a.code != b.code;
}
template<class Tag>
bool
operator<(flag_set<Tag> a, flag_set<Tag> b)
{
    return a.code < b.code;
}
template<class Tag>
flag_set<Tag>
operator~(flag_set<Tag> a)
{
    return flag_set<Tag>(~a.code);
}

} // namespace astroid

namespace std {
template<class Tag>
struct hash<astroid::flag_set<Tag>>
{
    size_t
    operator()(astroid::flag_set<Tag> const& set) const
    {
        return hash<unsigned>()(set.code);
    }
};
} // namespace std

namespace astroid {

#define ASTROID_DEFINE_FLAG_TYPE(type_prefix)                                 \
    struct type_prefix##_flag_tag                                             \
    {                                                                         \
    };                                                                        \
    typedef astroid::flag_set<type_prefix##_flag_tag> type_prefix##_flag_set;

#define ASTROID_DEFINE_FLAG(type_prefix, code, name)                          \
    static unsigned const name##_CODE = code;                                 \
    static astroid::flag_set<type_prefix##_flag_tag> const name(code);

// Inspired by Boost, inheriting from noncopyable disables copying for a type.
// The namespace prevents unintended ADL if used by applications.
namespace impl {
namespace noncopyable_ {
struct noncopyable
{
    noncopyable()
    {
    }

 private:
    noncopyable(noncopyable const& other);
    noncopyable&
    operator=(noncopyable const& other);
};
} // namespace noncopyable_
} // namespace impl
typedef impl::noncopyable_::noncopyable noncopyable;

struct exception : std::exception
{
    exception(string const& msg) : msg_(new string(msg))
    {
    }

    ~exception() throw()
    {
    }

    virtual char const*
    what() const throw()
    {
        return msg_->c_str();
    }

    // Add another level of context to the error messsage.
    void
    add_context(string const& str)
    {
        *msg_ += "\n" + str;
    }

    // If this returns true, it indicates that the condition that caused the
    // exception is transient, and thus retrying the same operation at a later
    // time may solve the problem.
    virtual bool
    is_transient() const
    {
        return false;
    }

 private:
    std::shared_ptr<string> msg_;
};

// vector<N,T> represents an N-dimensional geometric vector with elements of
// type T.
template<unsigned N, class T>
struct vector
{
    // allow external access to template parameters
    typedef T value_type;
    static const unsigned dimensionality = N;

    // element accessors
    T const&
    operator[](unsigned i) const
    {
        assert(i < N);
        return elements[i];
    }
    T&
    operator[](unsigned i)
    {
        assert(i < N);
        return elements[i];
    }

    vector()
    {
    }

    // explicit conversion from vectors with other element types
    template<class OtherT>
    explicit vector(vector<N, OtherT> const& other)
    {
        for (unsigned i = 0; i < N; ++i)
            (*this)[i] = static_cast<T>(other[i]);
    }

 private:
    T elements[N];
};
// 2D constructor
template<class T>
vector<2, T>
make_vector(T x, T y)
{
    vector<2, T> v;
    v[0] = x;
    v[1] = y;
    return v;
}
// 3D constructor
template<class T>
vector<3, T>
make_vector(T x, T y, T z)
{
    vector<3, T> v;
    v[0] = x;
    v[1] = y;
    v[2] = z;
    return v;
}
// equality operators
namespace impl {
template<unsigned N, class T, unsigned I>
struct vector_equality_test
{
    static bool
    apply(vector<N, T> const& a, vector<N, T> const& b)
    {
        return a[I - 1] == b[I - 1]
               && vector_equality_test<N, T, I - 1>::apply(a, b);
    }
};
template<unsigned N, class T>
struct vector_equality_test<N, T, 0>
{
    static bool
    apply(vector<N, T> const& a, vector<N, T> const& b)
    {
        return true;
    }
};
} // namespace impl
template<unsigned N, class T>
bool
operator==(vector<N, T> const& a, vector<N, T> const& b)
{
    return impl::vector_equality_test<N, T, N>::apply(a, b);
}
template<unsigned N, class T>
bool
operator!=(vector<N, T> const& a, vector<N, T> const& b)
{
    return !(a == b);
}
// < operator
template<unsigned N, class T>
bool
operator<(vector<N, T> const& a, vector<N, T> const& b)
{
    for (unsigned i = 0; i < N; ++i)
    {
        if (a[i] < b[i])
            return true;
        if (b[i] < a[i])
            return false;
    }
    return false;
}
// streaming
template<unsigned N, class T>
std::ostream&
operator<<(std::ostream& out, vector<N, T> const& v)
{
    out << "(";
    for (unsigned i = 0; i != N; ++i)
    {
        if (i != 0)
            out << ", ";
        out << v[i];
    }
    out << ")";
    return out;
}

} // namespace astroid
template<unsigned N, class T>
struct cradle::type_info_query<astroid::vector<N, T>>
{
    static void
    get(api_type_info* info)
    {
        *info = make_api_type_info_with_array_type(
            api_array_info{some(integer(N)), get_type_info<T>()});
    }
};
namespace astroid {

using cradle::none;
using cradle::some;
using std::optional;

template<class Container>
struct raii_adaptor : Container
{
    ~raii_adaptor()
    {
        Container::end();
    }
};

struct id_interface;

// Accessors are the means by which UI elements access model state and values
// computed by the application for display in the UI.
// The goals of the accessor library are as follows.
// - Unify the interface to C-style data structures and OOP-style classes
//   (whose data members may be protected behind accessor functions).
// - Provide standard mechanisms for transforming the UI's view of model state
//   or applying constraints to its manipulations of that state.
// - Provide mechanisms for efficiently detecting changes in displayed values.
// - Ensure that the passing of values to the UI is as efficient and lazy as
//   possible.
//
// Accessors are passed by const reference into UI functions.
// They're typically created directly at the call site as function arguments
// and are only valid for the life of the function call.
// Accessor wrappers are templated and store copies of the actual wrapped
// accessor, which allows them to be easily composed at the call site, without
// requiring any memory allocation.
// UI functions are untemplated and lose the actual type of the accessor.
// One consequence of this is that a UI container cannot store its accessor
// for its entire scope and thus only has access to it within its begin
// function. If a container needs to set its accessor's value from within its
// scope, it can do so by reinvoking the UI context with a set_value_event
// that is processed by the container's begin function.
//
struct untyped_accessor_base
{
    // If this returns false, the underlying state has no value, so get()
    // should not be called.
    virtual bool
    is_gettable() const
        = 0;

    // An accessor must supply an ID which uniquely identifies its value.
    // The ID is required to be valid if is_gettable() returns true.
    // (It may be valid even if is_gettable() returns false, which would mean
    // that the accessor can identify its value but doesn't know it yet.)
    // The ID reference is only valid as long as the accessor itself is valid.
    virtual id_interface const&
    id() const
        = 0;

    // If is_settable() returns false, the accessor is currently read-only and
    // any UI controls associated with it should disallow user input.
    virtual bool
    is_settable() const
        = 0;
};
template<class T>
struct accessor : untyped_accessor_base
{
    typedef T value_type;

    // Get the value. The reference returned here is only guaranteed to be
    // valid as long as the accessor itself is valid.
    virtual T const&
    get() const
        = 0;

    // Set the value. (Only call if is_settable returns true.)
    virtual void
    set(T const& value) const
        = 0;
};

using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::int8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

// Although the api() statements are removed by the preprocessor, IDE code
// analysis on the original headers doesn't like them. This macro causes the
// IDE to ignore them.
#define api(x)

// A lot of ASTROID algorithms use callbacks to report progress or check in
// with their callers (which can, for example, pause or terminate the
// algorithm). The following are the interface definitions for these callbacks
// and some utilities for constructing them.

// A progress reporter object gets called periodically with the progress of the
// algorithm (0 is just started, 1 is done).
struct progress_reporter_interface
{
    virtual void
    operator()(float)
        = 0;
};
// If you don't want to know about the progress, pass one of these.
struct null_progress_reporter : progress_reporter_interface
{
    void
    operator()(float)
    {
    }
};

// When an algorithm is divided into subtasks, this can be used to translate
// the progress of the subtasks into the overall progress of the main task.
// When creating the progress reporter for each subtask, you specify the
// portion of the overall job that the subtask represents. The state variable
// must be shared across all subtasks and tracks the overall progress of the
// main task. It's up to the caller to ensure that the sum of the portions of
// all subtasks is 1.
struct task_subdivider_state
{
    float offset;
    task_subdivider_state() : offset(0)
    {
    }
};
struct subtask_progress_reporter : progress_reporter_interface
{
    subtask_progress_reporter(
        progress_reporter_interface& parent_reporter,
        task_subdivider_state& state,
        float portion)
        : parent_reporter_(&parent_reporter), state_(state), portion_(portion)
    {
        offset_ = state_.offset;
        state_.offset += portion;
    }

    void
    operator()(float progress)
    {
        (*parent_reporter_)(offset_ + progress * portion_);
    }

 private:
    progress_reporter_interface* parent_reporter_;
    task_subdivider_state& state_;
    float offset_;
    float portion_;
};

// When an algorithm is divided into subtasks, and called from within a loop
// this sub_progress_reporter can be used.
// Takes in the parent reporter which it will call to report progress
// Takes in an offset which is the current progress when then reporter is made
// Takes in a scale which is used when this reporter is created in a loop
struct sub_progress_reporter : progress_reporter_interface
{
    sub_progress_reporter(
        progress_reporter_interface& parent_reporter,
        float offset,
        float scale)
        : parent_reporter_(&parent_reporter), offset_(offset), scale_(scale)
    {
    }

    void
    operator()(float progress)
    {
        (*parent_reporter_)(offset_ + progress * scale_);
    }

 private:
    progress_reporter_interface* parent_reporter_;
    float offset_;
    float scale_;
};

// Algorithms call this to check in with the caller every few milliseconds.
// This can be used to abort the algorithm by throwing an exception.
struct check_in_interface
{
    virtual void
    operator()()
        = 0;
};
// If you don't need the algorithm to check in, pass one of these.
struct null_check_in : check_in_interface
{
    void
    operator()()
    {
    }
};

// If you need an algorithm to check in with two different controllers, you
// can use this to merge the check_in objects supplied by the two controllers.
struct merged_check_in : check_in_interface
{
    merged_check_in(check_in_interface* a, check_in_interface* b) : a(a), b(b)
    {
    }

    void
    operator()()
    {
        (*a)();
        (*b)();
    }

 private:
    check_in_interface* a;
    check_in_interface* b;
};

using std::swap;

template<class T>
struct remove_const_reference
{
    typedef T type;
};
template<class T>
struct remove_const_reference<T const>
{
    typedef T type;
};
template<class T>
struct remove_const_reference<T const&>
{
    typedef T type;
};

// Given a vector of optional values, return a vector containing all values
// inside those optionals.
template<class Value>
std::vector<Value>
filter_optionals(std::vector<optional<Value>> const& values)
{
    std::vector<Value> filtered;
    for (auto const& v : values)
    {
        if (v)
            filtered.push_back(get(v));
    }
    return filtered;
}

// functional map over a vector
template<class Item, class Fn>
auto
map(Fn const& fn, std::vector<Item> const& items)
    -> std::vector<typename remove_const_reference<decltype(fn(Item()))>::type>
{
    typedef typename remove_const_reference<decltype(fn(Item()))>::type
        mapped_item_type;
    size_t n_items = items.size();
    std::vector<mapped_item_type> result(n_items);
    for (size_t i = 0; i != n_items; ++i)
        result[i] = fn(items[i]);
    return result;
}

// functional map over a map
template<class Key, class Value, class Fn>
auto
map(Fn const& fn, std::map<Key, Value> const& items)
    -> std::map<Key, decltype(fn(Value()))>
{
    typedef decltype(fn(Value())) mapped_item_type;
    std::map<Key, mapped_item_type> result;
    for (auto const& item : items)
        result[item.first] = fn(item.second);
    return result;
}

// functional map from a std::map to a vector
template<class Key, class Value, class Fn>
auto
map_to_vector(Fn const& fn, std::map<Key, Value> const& items)
    -> std::vector<decltype(fn(Key(), Value()))>
{
    typedef decltype(fn(Key(), Value())) mapped_item_type;
    std::vector<mapped_item_type> result;
    result.reserve(items.size());
    for (auto const& item : items)
        result.push_back(fn(item.first, item.second));
    return result;
}

// Get a pointer to the elements of a vector.
template<class T>
T const*
get_elements_pointer(std::vector<T> const& vector)
{
    return vector.empty() ? 0 : &vector[0];
}

// Convert the given value to a string.
template<class T>
string
to_string(T const& x)
{
    std::ostringstream s;
    s << x;
    return s.str();
}

// Convert the given value to a formatted string.
template<class T>
string
to_string(T const& x, int precision)
{
    std::ostringstream s;
    s.precision(precision);
    s << x;
    return s.str();
}

// forward declared for use by another forward declaration below
} // namespace astroid
namespace ClipperLib {
struct IntPoint;
typedef std::vector<std::vector<IntPoint>> Paths;
typedef Paths Polygons;
} // namespace ClipperLib
namespace astroid {

// OTHER UTILITIES

using ownership_holder = std::shared_ptr<cradle::data_owner>;

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
