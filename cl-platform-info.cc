#include <cstddef>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "cl-platform-info.hh"

using std::cerr;
using std::cout;
using std::endl;

extern char const *error_string(cl_int err)
{
    switch (err)
    {
	case CL_SUCCESS:
	    return "SUCCESS";
	case CL_DEVICE_NOT_FOUND:
	    return "DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
	    return "DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
	    return "COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
	    return "MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
	    return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
	    return "OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
	    return "PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
	    return "MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
	    return "IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
	    return "IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
	    return "BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
	    return "MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
	    return "MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
	    return "EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_COMPILE_PROGRAM_FAILURE:
	    return "COMPILE_PROGRAM_FAILURE";
	case CL_LINKER_NOT_AVAILABLE:
	    return "LINKER_NOT_AVAILABLE";
	case CL_LINK_PROGRAM_FAILURE:
	    return "LINK_PROGRAM_FAILURE";
	case CL_DEVICE_PARTITION_FAILED:
	    return "DEVICE_PARTITION_FAILED";
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
	    return "KERNEL_ARG_INFO_NOT_AVAILABLE";
	case CL_INVALID_VALUE:
	    return "INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
	    return "INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
	    return "INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
	    return "INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
	    return "INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
	    return "INVALID_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
	    return "INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
	   return "INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
	    return "INVALID_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
	    return "INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
	    return "INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
	    return "INVALID_SAMPLER";
	case CL_INVALID_BINARY:
	    return "INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
	    return "INVALID_OPTIONS";
	case CL_INVALID_PROGRAM:
	    return "INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
	    return "INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
	    return "INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
	    return "INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
	    return "INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
	    return "INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
	    return "INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
	    return "INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
	    return "INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
	    return "INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
	    return "INVALID__GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
	    return "INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
	    return "INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
	    return "INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
	    return "INVALID_EVENT";
	case CL_INVALID_OPERATION:
	    return "INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
	    return "INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
	    return "INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
	    return "INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:
	    return "INVALID_GLOBAL_WORK_SIZE";
	case CL_INVALID_PROPERTY:
	    return "INVALID_PROPERTY";
	case CL_INVALID_IMAGE_DESCRIPTOR:
	    return "INVALID_IMAGE_DESCRIPTOR";
	case CL_INVALID_COMPILER_OPTIONS:
	    return "INVALID_COMPILER_OPTIONS";
	case CL_INVALID_LINKER_OPTIONS:
	    return "INVALID_LINKER_OPTIONS";
	case CL_INVALID_DEVICE_PARTITION_COUNT:
	    return "INVALID_DEVICE_PARTITION_COUNT";
	case CL_INVALID_PIPE_SIZE:
	    return "INVALID_PIPE_SIZE";
	case CL_INVALID_DEVICE_QUEUE:
	    return "INVALID_DEVICE_QUEUE";
	default:
	    return "OPEN_CL_ERROR";
    }
}

template<typename Type>
    std::string list_cl_device_type(Type type_mask)
{
    std::ostringstream result_str;

    if (type_mask & CL_DEVICE_TYPE_GPU)
	result_str << "GPU ";

    if (type_mask & CL_DEVICE_TYPE_CPU)
	result_str << "CPU ";

    if (type_mask & CL_DEVICE_TYPE_ACCELERATOR)
	result_str << "ACCELERATOR ";

    if (type_mask & CL_DEVICE_TYPE_DEFAULT)
	result_str << "DEFAULT ";

    if (type_mask & CL_DEVICE_TYPE_CUSTOM)
	result_str << "CUSTOM";

    return result_str.str();
}

static std::string list_product(std::vector<std::size_t> const &sizes)
{
    std::ostringstream str;

    auto it = sizes.cbegin();

    str << *it++;

    while (it != sizes.cend())
	str << " x " << *it++;

    return str.str();
}

static char const *property_name(cl_device_partition_property prop)
{
    switch (prop)
    {
	case CL_DEVICE_PARTITION_EQUALLY:
	    return "Equally";
	case CL_DEVICE_PARTITION_BY_COUNTS:
	    return "By counts";
	case CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN:
	    return "By affinity domain";
	default:
	    return "-";
    }
}

static std::string list_partitions(std::vector<cl_device_partition_property> const &partition_properties)
{
    std::ostringstream str;

    auto it = partition_properties.cbegin();

    if (it != partition_properties.cend() && *it)
    {
	str << property_name(*it);

	while (it != partition_properties.cend() && *it)
	    str << ", " << property_name(*it++);
    }

    if (str.str().empty())
	return "-";

    return str.str();
}

static char const *affinity_domain(cl_device_affinity_domain affinity_domain)
{
    switch (affinity_domain)
    {
	case CL_DEVICE_AFFINITY_DOMAIN_NUMA:
	    return "NUMA";
	case CL_DEVICE_AFFINITY_DOMAIN_L4_CACHE:
	    return "L4 cache";
	case CL_DEVICE_AFFINITY_DOMAIN_L3_CACHE:
	    return "L3 cache";
	case CL_DEVICE_AFFINITY_DOMAIN_L2_CACHE:
	    return "L2 cache";
	case CL_DEVICE_AFFINITY_DOMAIN_L1_CACHE:
	    return "L1 cache";
	case CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE:
	    return "next partitionable";
	default:
	    return "-";
    }
}

static std::string list_domains(cl_device_affinity_domain domains_mask)
{
    std::ostringstream str;

    for (auto const &domain: std::array<cl_device_affinity_domain, 6> { CL_DEVICE_AFFINITY_DOMAIN_NUMA,  CL_DEVICE_AFFINITY_DOMAIN_L4_CACHE, CL_DEVICE_AFFINITY_DOMAIN_L3_CACHE, CL_DEVICE_AFFINITY_DOMAIN_L2_CACHE,
								CL_DEVICE_AFFINITY_DOMAIN_L1_CACHE, CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE } )
    {
	if (domains_mask & domain)
	{
	    if (!str.str().empty())
		str << ", ";

	    str << affinity_domain(domain);
	}
    };

    if (str.str().empty())
	return "-";

    return str.str();
}

static char const *queue_property(cl_command_queue_properties prop)
{
    switch (prop)
    {
	case CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE:
	    return "Out of order execution";
	case CL_QUEUE_PROFILING_ENABLE:
	    return "Profiling";
	default:
	    return "-";
    }
}

static std::string list_queue_props(cl_command_queue_properties props)
{
    std::ostringstream str;

    for (auto prop: std::vector<cl_command_queue_properties> { CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, CL_QUEUE_PROFILING_ENABLE })
    {
	if (props & prop)
	{
	    if (!str.str().empty())
		str << ", ";

	    str << queue_property(prop);
	}
    }

    return str.str();
}

static char const *execution_capability(cl_device_exec_capabilities cap)
{
    switch (cap)
    {
	case CL_EXEC_KERNEL:
	    return "kernel execution";
	case CL_EXEC_NATIVE_KERNEL:
	    return "native kernel execution";
	default:
	    return "-";
    }
}

static std::string list_capabilities(cl_device_exec_capabilities caps)
{
    std::ostringstream str;

    for (auto cap: std::vector<cl_device_exec_capabilities> { CL_EXEC_KERNEL, CL_EXEC_NATIVE_KERNEL })
    {
	if (caps & cap)
	{
	    if (!str.str().empty())
		str << ", ";

	    str << execution_capability(cap);
	}
    }

    return str.str();
}

static char const *float_config(cl_device_fp_config config)
{
    switch (config)
    {
	case CL_FP_DENORM:
	    return "denorms";
	case CL_FP_INF_NAN:
	    return "INF/NaN";
	case CL_FP_ROUND_TO_NEAREST:
	    return "round to nearest";
	case CL_FP_ROUND_TO_ZERO:
	    return "round to zero";
	case CL_FP_ROUND_TO_INF:
	    return "round to inf";
	case CL_FP_FMA:
	    return "fused multiply-add";
	case CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT:
	    return "correctly rounded devide & sqrt";
	case CL_FP_SOFT_FLOAT:
	    return "soft float";
	default:
	    return "-";
    }
}

static std::string list_float_support(cl_device_fp_config fp_config)
{
    std::ostringstream str;

    for (auto config: std::vector<cl_device_fp_config> { CL_FP_DENORM, CL_FP_INF_NAN, CL_FP_ROUND_TO_NEAREST, CL_FP_ROUND_TO_ZERO, CL_FP_ROUND_TO_INF, CL_FP_FMA, CL_FP_SOFT_FLOAT })
    {
	if (fp_config & config)
	{
	    if (!str.str().empty())
		    str << ", ";

	    str << float_config(config);
	}
    }

    if (str.str().empty())
	return "-";

    return str.str();
}

static std::string memory_size_str(size_t memoryCapacity, bool showKiBytes = true)
{
    std::ostringstream memoryString;

    size_t kbytes = memoryCapacity/ 1024;
    size_t mbytes = kbytes / 1024;
    size_t gbytes = mbytes / 1024;

    kbytes %= 1024;
    mbytes %= 1024;

    if (gbytes)
	memoryString << gbytes << " GiBytes ";

    if (mbytes)
	memoryString << (mbytes % 1024) << " MiBytes ";

    if (showKiBytes && kbytes)
	memoryString << (kbytes % 1024) << " KiBytes ";

    std::string result = memoryString.str();

    while (!result.empty() && result.back() == ' ')
	result.pop_back();

    if (result.empty())
	return "0 bytes";

    return result;
}

static char const *memoryCacheType(cl_device_mem_cache_type memCacheType)
{
    switch(memCacheType)
    {
    case CL_NONE:
	return "";
    case CL_READ_ONLY_CACHE:
	return "ReadOnly";
    case CL_READ_WRITE_CACHE:
	return "ReadWrite";
    default:
	return "";
    }
}

extern bool has_extension(std::string const &ext_list, char const *ext, std::size_t ext_size)
{
    ext_size--;	    // ignore the null terminator
    std::string::const_iterator it = std::search(ext_list.cbegin(), ext_list.cend(), ext, ext + ext_size);

    if (it != ext_list.cend())
    {
	if (it != ext_list.cbegin())
	    if (*(it - 1) != ' ')
		return false;

	if (it + ext_size != ext_list.cend())
	    if (*(it + ext_size) != ' ' && *(it + ext_size) != '\0')
		    return false;

	return true;
    }

    return false;
}

extern void show_cl_device(cl::Device &device, bool showPlatform)
{
    cout << "\tPlatfrom:               " << cl::Platform(device.getInfo<CL_DEVICE_PLATFORM>()).getInfo<CL_PLATFORM_NAME>() << endl;
#if defined(CL_HPP_PARAM_NAME_INFO_1_0_)
    cout << "\tDevice ID:              " << std::setiosflags(cout.uppercase) << device.Wrapper<cl_device_id>::get() << endl;
#else
    cout << "\tDevice ID:              " << std::setiosflags(cout.uppercase) << device() << endl;
#endif
    cout << "\tVendor:                 [0x" << std::setw(4) << std::setfill('0') << std::setiosflags(cout.right | cout.uppercase) << std::setbase(16) << device.getInfo<CL_DEVICE_VENDOR_ID>() /* << std::resetiosflags() */ << std::setbase(10) << "] "
					<< device.getInfo<CL_DEVICE_VENDOR>() << endl;
    cout << "\tDevice name:            " << device.getInfo<CL_DEVICE_NAME>() << endl;
    cout << "\tDriver version:         " << device.getInfo<CL_DRIVER_VERSION>() << endl;
    cout << "\tType                    " << list_cl_device_type(device.getInfo<CL_DEVICE_TYPE>()) << endl;
    cout << "\tOpencCL version:        " << device.getInfo<CL_DEVICE_VERSION>() << endl;
    cout << "\tOpenCL C version:       " << device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << endl;
    // cout << "\tSPIR version:           " << device.getInfo<CL_DEVICE_SPIR_VERSIONS>() << endl;
    cout << "\tProfile:                " << device.getInfo<CL_DEVICE_PROFILE>() << endl;
    cout << "\tMax clock speed:        " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << " MHz" << endl;
    cout << "\tMemory:                 " << memory_size_str(device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>(), false) << ", " << device.getInfo<CL_DEVICE_ADDRESS_BITS>() << "-bit " 
#if (CL_HPP_TARGET_OPENCL_VERSION >= 120)
                                        << (device.getInfo<CL_DEVICE_HOST_UNIFIED_MEMORY>() ? "host memory (" : "device memory (")
#else
                                        << "("
#endif
					<< memory_size_str(device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE>()) << ' ' << memoryCacheType(device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_TYPE>()) << " cache, "
					<< device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE>() * 8 << "-bit)" << endl;
    cout << "\tByte order:             " << (device.getInfo<CL_DEVICE_ENDIAN_LITTLE>() ? "Little endian" : "Big endian or other") << endl;
    cout << "\tCompute units:          " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << endl;
    cout << "\tCompute units memory:   " << memory_size_str(device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>()) << ' ' << (device.getInfo<CL_DEVICE_LOCAL_MEM_TYPE>() == CL_LOCAL ? "local memory" : "global memory") << endl;
    cout << "\tWork group size:        " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << endl;
    cout << "\tWork item sizes:        " << list_product(device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()) << endl;
#if (CL_HPP_TARGET_OPENCL_VERSION >= 120)
    cout << "\tPartition properties:   " << list_partitions(device.getInfo<CL_DEVICE_PARTITION_PROPERTIES>()) << endl;
    cout << "\t  affinity domains:     " << list_domains(device.getInfo<CL_DEVICE_PARTITION_AFFINITY_DOMAIN>()) << endl;
#endif
    // cout << "\tMax sub-devices:        " << device.getInfo<CL_DEVICE_PARTITION_MAX_SUB_DEVICES>() << endl;
    cout << "\tExecution queue flags:  " << list_queue_props(device.getInfo<CL_DEVICE_QUEUE_PROPERTIES>()) << endl;
    cout << "\tExecution capabilities: " << list_capabilities(device.getInfo<CL_DEVICE_EXECUTION_CAPABILITIES>()) << endl;
#if (CL_HPP_TARGET_OPENCL_VERSION >= 120)
    cout << "\tBuilt-in kernels:       " << std::regex_replace(device.getInfo<CL_DEVICE_BUILT_IN_KERNELS>(), std::regex(";"), "\n\t\t\t\t") << endl;
#endif
    cout << "\tNative vector size:     " << "(char: " << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR>() << ", short: " << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT>()
					 << ", int: " << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_INT>() << ", long: " << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG>()
					 << ", half: " << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF>() << ", float: " << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT>()
					 << ", double: " << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE>() << ")" << endl;
    cout << "\tPrefferred vector size: " << "(char: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR>() << ", short: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT>()
					 << ", int: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT>() << ", long: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG>()
					 << ", half: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF>() << ", float: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT>()
					 << ", double: " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE>() << ")" << endl;
    cout << "\tHalf float config:      " << (has_extension(device.getInfo<CL_DEVICE_EXTENSIONS>(), "cl_khr_fp16", sizeof "cl_khr_fp16") ?
						list_float_support(device.getInfo<CL_DEVICE_HALF_FP_CONFIG>()) : "-") << endl;
    cout << "\tSingle float config:    " << list_float_support(device.getInfo<CL_DEVICE_SINGLE_FP_CONFIG>()) << endl;
    cout << "\tDouble float config:    " << list_float_support(device.getInfo<CL_DEVICE_DOUBLE_FP_CONFIG>()) << endl;
    cout << "\tImage support:          " << (device.getInfo<CL_DEVICE_IMAGE_SUPPORT>() ? "Yes" : "No") << endl;
    // cout << "\t1D image size:          " << device.getInfo<CL_DEVICE_IMAGE_MAX_BUFFER_SIZE>() << endl;
    cout << "\t2D image size:          " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>() << " x " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>() << endl;
    cout << "\t3D image size:          " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_WIDTH>() << " x " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_HEIGHT>() << " x " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_DEPTH>() << endl;
    // cout << "\timage array size:       " << device.getInfo<CL_DEVICE_IMAGE_MAX_ARRAY_SIZE>() << endl;
    cout << "\tMax samplers count:     " << device.getInfo<CL_DEVICE_MAX_SAMPLERS>() << endl;
    cout << "\tExtensions:             " << std::regex_replace(device.getInfo<CL_DEVICE_EXTENSIONS>(), std::regex("[[:space:]]+"), "\n\t\t\t\t") << endl;
    cout << endl;
}

extern void show_cl_platform(cl::Platform &platform, bool list_devices, cl::Platform const &defaultPlatform)
{
#if defined(CL_HPP_PARAM_NAME_INFO_1_0_)
    cout << "Default:       \t" << (platform.Wrapper<cl_platform_id>::get() == defaultPlatform.Wrapper<cl_platform_id>::get() ? "*** DEFAULT ***" : "") << endl;
    cout << "Platform ID:   \t" << platform.Wrapper<cl_platform_id>::get() << endl;
#else
    cout << "Default:       \t" << (platform() == defaultPlatform() ? "*** DEFAULT ***" : "") << endl;
    cout << "Platform ID:   \t" << platform() << endl;
#endif
    cout << "Vendor:        \t" << platform.getInfo<CL_PLATFORM_VENDOR>() << endl;
    cout << "Platform name: \t" << platform.getInfo<CL_PLATFORM_NAME>() << endl;
    cout << "Profile:       \t" << platform.getInfo<CL_PLATFORM_PROFILE>() << endl;
    cout << "Version:       \t" << platform.getInfo<CL_PLATFORM_VERSION>() << endl;

    cout << "Extensions:    \t" << endl;
    std::string extensions = platform.getInfo<CL_PLATFORM_EXTENSIONS>();
    std::basic_regex<char> const feature_name_regexp("[^[:space:]]*");
    for
	(
	    std::regex_iterator<std::string::const_iterator> it(extensions.cbegin(), extensions.cend(), feature_name_regexp);
	    it != std::regex_iterator<std::string::const_iterator>();
	    it++
	)
    {
	if (!it->empty() && !it->str().empty())
	    cout << "\t\t" << it->str() << endl;
    }

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    cout << "Devices:       \t" << devices.size() << endl;

    if (list_devices)
    {
	for (auto &device: devices)
	    show_cl_device(device);
    }

    cout << endl;
}

extern void list_default_cl_devices()
{
    cl_uint device_count = 0;
    ::clGetDeviceIDs(nullptr, CL_DEVICE_TYPE_ALL, 0, nullptr, &device_count);
    std::vector<cl_device_id> devices(device_count);
    cl_int result = ::clGetDeviceIDs(nullptr, CL_DEVICE_TYPE_ALL, devices.size(), devices.data(), &device_count);

    switch (result)
    {
	case CL_SUCCESS:
	    cout << "Default devices:" << endl;
	    for (auto cl_device: devices)
	    {
		cl::Device device(cl_device);
		show_cl_device(device);
	    }
	    break;
	case CL_DEVICE_NOT_FOUND:
	    cerr << "No default devices !";
	    break;
	case CL_INVALID_PLATFORM:
	    cerr << "NULL platform invalid !";
	    break;
	default:
	    throw cl::Error(result, "clGetDeviceIDs");
    }
}
