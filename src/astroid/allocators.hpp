#ifndef ASTROID_ALLOCATORS_HPP
#define ASTROID_ALLOCATORS_HPP

#include <astroid/common.hpp>

namespace astroid {

template<class T>
struct allocator_interface
{
    // Allocate a cradle::data_owner with sufficient size and alignment to hold
    // `n` elements of type `T`.
    virtual std::shared_ptr<cradle::data_owner>
    allocate(size_t n) = 0;
};

template<class T>
struct unique_ptr_data_owner : public cradle::data_owner
{
    virtual std::uint8_t*
    data()
    {
        return reinterpret_cast<std::uint8_t*>(ptr.get());
    }

    std::unique_ptr<T> ptr;
};

template<class T>
std::shared_ptr<cradle::data_owner>
make_unique_ptr_owner(std::unique_ptr<T> ptr)
{
    unique_ptr_data_owner<T> owner;
    owner.ptr = std::move(ptr);
    return std::make_shared<unique_ptr_data_owner<T>>(std::move(owner));
}

template<class T>
struct standard_allocator : allocator_interface<T>
{
    std::shared_ptr<cradle::data_owner>
    allocate(size_t n) override
    {
        return make_unique_ptr_owner<T[]>(std::make_unique<T[]>(n));
    }
};

template<class T>
allocator_interface<T>&
default_allocator()
{
    static standard_allocator<T> the_allocator;
    return the_allocator;
}

struct standard_allocation_policy
{
    template<class T>
    standard_allocator<T>
    make_allocator()
    {
        return standard_allocator<T>();
    }
};

} // namespace astroid

#endif
