#ifndef ASTROID_DATE_TIME_H
#define ASTROID_DATE_TIME_H

#include <astroid/common.hpp>

#include <boost/functional/hash.hpp>

namespace astroid {

date
parse_thinknode_date(std::string const& s);

string
to_thinknode_string(date const& d);

// Get a string representation of a date.
std::string
to_string(date const& d);

// Get a string representation of a date/time.
std::string
to_string(datetime const& t);

// Get a string representation of a date/time adjust to the local time zone.
std::string
to_local_string(datetime const& t);

// Given a date/time, convert it to the local time zone and get a string
// represention of just the date portion.
std::string
to_local_date_string(datetime const& t);

// Given a date/time, convert it to the local time zone and get a string
// represention of just the time portion.
std::string
to_local_time_string(datetime const& t);

struct value;

datetime
parse_thinknode_datetime(std::string const& s);

string
to_thinknode_string(datetime const& t);

} // namespace astroid

// Hmm... Is there a better way to manage namespaces/lookup?

namespace cradle {

template<>
struct type_info_query<astroid::date>
{
    static void
    get(api_type_info* info)
    {
        *info = make_api_type_info_with_string_type(api_string_type());
    }
};

template<>
struct type_info_query<astroid::datetime>
{
    static void
    get(api_type_info* info)
    {
        *info = make_api_type_info_with_datetime_type(api_datetime_type());
    }
};

inline void
to_dynamic(cradle::dynamic* v, astroid::date const& x)
{
    *v = astroid::to_thinknode_string(x);
}

inline void
from_dynamic(astroid::date* x, cradle::dynamic const& v)
{
    *x = astroid::parse_thinknode_date(cast<std::string>(v));
}

void
to_dynamic(cradle::dynamic* v, astroid::datetime const& x);

void
from_dynamic(astroid::datetime* x, cradle::dynamic const& v);

inline void
update_unique_hash(cradle::unique_hasher& hasher, astroid::datetime const& x)
{
    auto const t = x.time_since_epoch().count();
    hasher.encode_bytes(&t, sizeof(t));
}

inline void
update_unique_hash(cradle::unique_hasher& hasher, astroid::date const& x)
{
    auto const d = std::chrono::sys_days(x).time_since_epoch().count();
    hasher.encode_bytes(&d, sizeof(d));
}

} // namespace cradle

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
{
    namespace adaptor {

    template<>
    struct convert<std::chrono::year_month_day>
    {
        msgpack::object const&
        operator()(
            msgpack::object const& o, std::chrono::year_month_day& v) const
        {
            // Convert from msgpack object to string then parse
            std::string s;
            o.convert(s);
            v = astroid::parse_thinknode_date(s);
            return o;
        }
    };

    template<>
    struct pack<std::chrono::year_month_day>
    {
        template<typename Stream>
        msgpack::packer<Stream>&
        operator()(
            msgpack::packer<Stream>& p,
            std::chrono::year_month_day const& v) const
        {
            // Convert to string representation and pack
            p.pack(astroid::to_thinknode_string(v));
            return p;
        }
    };

    template<>
    struct object_with_zone<std::chrono::year_month_day>
    {
        void
        operator()(
            msgpack::object::with_zone& o,
            std::chrono::year_month_day const& v) const
        {
            std::string s = astroid::to_thinknode_string(v);
            o.type = msgpack::type::STR;
            o.via.str.size = static_cast<uint32_t>(s.size());
            char* ptr = static_cast<char*>(
                o.zone.allocate_align(s.size(), MSGPACK_ZONE_ALIGNOF(char)));
            std::memcpy(ptr, s.data(), s.size());
            o.via.str.ptr = ptr;
        }
    };

    } // namespace adaptor
}

} // namespace msgpack

template<>
struct cradle::serializable_via_cereal<astroid::date>
{
    static constexpr bool value = true;
};

#endif
