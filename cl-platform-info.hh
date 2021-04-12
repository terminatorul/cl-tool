#ifndef CL_PLATFORM_INFO_HH
#define CL_PLATFORM_INFO_HH

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

extern char const *error_string(cl_int err);
extern bool list_all;
std::string trim_name(std::string name);
extern void show_cl_device(cl::Device &device);
extern void show_cl_platform(cl::Platform &platform, cl::vector<cl::Device> &devices);
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

#endif // CL_PLATFORM_INFO_HH
