#include <cradle/common.hpp>
#include <cradle/test.hpp>
#include <cstdlib>

namespace cradle {

file_path
test_data_directory()
{
    char const* root = std::getenv("CRADLE_ROOT");
    if (!root)
        throw exception("CRADLE_ROOT not set");
    return file_path(root) / "data/test";
}

} // namespace cradle
