#ifndef ASTROID_IO_COMPRESSION_HPP
#define ASTROID_IO_COMPRESSION_HPP

#include <astroid/common.hpp>
#include <astroid/io/forward.hpp>
#include <boost/scoped_array.hpp>
#include <vector>

namespace astroid {

// When decompressing, we assume the caller already knows the size of the
// uncompressed data (based on other info related to the data), so the caller
// is expected to allocate the full block of data and pass in its size.
void
decompress(
    void* dst, std::size_t dst_size, void const* src, std::size_t src_size);
void
decompress(void* dst, std::size_t dst_size, c_file& src);

// Compress to memory. The destination array is allocated according to the
// maximum possible size of the compressed data.
// *dst_size is set to the actual size of the compressed data.
void
compress(
    boost::scoped_array<uint8_t>* dst,
    std::size_t* dst_size,
    void const* src,
    std::size_t src_size);
// Compress to a file.
void
compress(c_file& dst, void const* src, std::size_t src_size);

// This is thrown when zlib reports an error.
class zlib_error : public exception
{
 public:
    zlib_error(int error_code);
    ~zlib_error() throw()
    {
    }
    int
    get_error_code() const
    {
        return error_code_;
    }

 private:
    int error_code_;
};

class decompression_error : public exception
{
 public:
    decompression_error(string const& message) : exception(message)
    {
    }
    ~decompression_error() throw()
    {
    }
};

} // namespace astroid

#endif
