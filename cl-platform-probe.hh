#if !defined(CL_PLATFORM_PROBE_HH)
#define CL_PLATFORM_PROBE_HH

#if defined(__APPLE__) || defined(__MACOSX__)
#include <OpenCL/cl2.hpp>
#else
#include <CL/cl2.hpp>
#endif

extern bool probe_cl_device(cl::Device &device, unsigned long simulation_count);
extern void probe_cl_platform(cl::Platform &platform);

#endif // !defined(CL_PLATFORM_PROBE_HH)
