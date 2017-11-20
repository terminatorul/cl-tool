#include <iostream>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl.hpp>
#endif

extern char const *error_string(cl_int err);
extern void show_cl_device(cl::Device &device, bool showPlatform = false);
extern void show_cl_platform(cl::Platform &platform, bool list_devices, cl::Platform const &defaultPlatform);
extern void list_default_cl_devices();
