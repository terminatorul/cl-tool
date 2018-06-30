#include <cstddef>
#include <iostream>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

extern char const *error_string(cl_int err);
extern void show_cl_device(cl::Device &device, bool showPlatform = false);
extern void show_cl_platform(cl::Platform &platform, bool list_devices);
extern bool has_extension(std::string const &ext_list, char const *ext, std::size_t length);

template <typename CharT, std::size_t length>
    inline bool has_extension(std::basic_string<CharT> const &ext_list, CharT const (&ext)[length]);

template <typename CharT, std::size_t length>
    inline bool has_extension(CharT const *ext_list, CharT const (&ext)[length]);


template <typename CharT, std::size_t length>
    inline bool has_extension(std::basic_string<CharT> const &ext_list, CharT const (&ext)[length])
{
    return has_extension(ext_list, ext, length - 1);	// skip the null terminator
}

template <typename CharT, std::size_t length>
    inline bool has_extension(CharT const *ext_list, CharT const (&ext)[length])
{
    return has_extension(ext_list, ext, length - 1);	// skip the null terminator
}
