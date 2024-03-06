#ifndef ASTROID_IO_FORWARD_HPP
#define ASTROID_IO_FORWARD_HPP

#include <boost/version.hpp>

#if BOOST_VERSION >= 105100
#define ASTROID_IO_BOOST_FILESYSTEM_NAMESPACE filesystem
#else
#define ASTROID_IO_BOOST_FILESYSTEM_NAMESPACE filesystem3
#endif

namespace boost {
namespace ASTROID_IO_BOOST_FILESYSTEM_NAMESPACE {

class path;

}
} // namespace boost

namespace astroid {

typedef boost::ASTROID_IO_BOOST_FILESYSTEM_NAMESPACE::path file_path;

struct c_file;

namespace impl {
namespace config {
class structure;
template<typename T>
class list;
} // namespace config
} // namespace impl

struct file_error;

struct simple_file_parser;

struct filesystem_item;
struct filesystem_item_contents;

struct text_parser;

} // namespace astroid

#endif
