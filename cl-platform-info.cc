#include <cstddef>
#include <string>
#include <array>
#include <vector>
#include <list>
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
	    return "INVALID_WORK_GROUP_SIZE";
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
	case CL_PLATFORM_NOT_FOUND_KHR:
	    return "CL_PLATFORM_NOT_FOUND_KHR";
	default:
	    return "OPEN_CL_ERROR";
    }
}

template<typename Type>
    std::string list_cl_device_type(Type type_mask)
{
    std::ostringstream result_str;

    if (type_mask & CL_DEVICE_TYPE_GPU)
	result_str << "GPU";

    if (type_mask & CL_DEVICE_TYPE_CPU)
	result_str << "CPU";

    if (type_mask & CL_DEVICE_TYPE_ACCELERATOR)
	result_str << "ACCELERATOR";

    if (type_mask & CL_DEVICE_TYPE_DEFAULT)
	result_str << " DEFAULT ";

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
	str << "x" << *it++;

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
	    return "Next Partitionable";
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
	if (!str.str().empty())
	    str << ", ";

	if (props & prop)
	    str << "[+] " << queue_property(prop);
	else
	    str << "[ ] " << queue_property(prop);
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
	if (!str.str().empty())
	    str << ", ";

	if (caps & cap)
	    str << "[+] " << execution_capability(cap);
	else
	    str << "[ ] " << execution_capability(cap);
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
	    return "Inf/NaN";
	case CL_FP_ROUND_TO_NEAREST:
	    return "round to nearest";
	case CL_FP_ROUND_TO_ZERO:
	    return "round to zero";
	case CL_FP_ROUND_TO_INF:
	    return "round to Inf";
	case CL_FP_FMA:
	    return "fused multiply-add";
	case CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT:
	    return "correctly rounded divide & sqrt";
	case CL_FP_SOFT_FLOAT:
	    return "soft float";
	default:
	    return "-";
    }
}

static std::string list_float_support(cl_device_fp_config fp_config, bool single_precision)
{
    if (!fp_config)
	return "-";

    std::ostringstream str;

    for (auto config: std::vector<cl_device_fp_config> { CL_FP_DENORM, CL_FP_INF_NAN, CL_FP_ROUND_TO_NEAREST, CL_FP_ROUND_TO_ZERO, CL_FP_ROUND_TO_INF, CL_FP_FMA, CL_FP_SOFT_FLOAT, CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT })
    {
	if (config == CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT)
	    if (single_precision)
		str << ",\n\t\t\t\t";
	    else
		continue;
	else
	    if (!str.str().empty())
		    str << ", ";

	if (fp_config & config)
	    str << "[+] " << float_config(config);
	else
	    str << "[ ] " << float_config(config);
    }

    return str.str();
}

static std::string memory_size_str(cl_ulong memoryCapacity, bool showKiBytes = true)
{
    std::ostringstream memoryString;

    cl_ulong kbytes = memoryCapacity / 1024;
    cl_ulong mbytes = kbytes / 1024;
    cl_ulong gbytes = mbytes / 1024;

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

extern bool has_extension(std::string const &ext_list, char const *ext, std::size_t ext_len)
{
    std::string::const_iterator it = ext_list.cbegin();

    while
	(
	    (it = std::search(it, ext_list.cend(), ext, ext + ext_len)) != ext_list.cend()
		&&
	    ((it != ext_list.cbegin() && *(it - 1) != ' ') || (std::size_t(ext_list.cend() - it) > ext_len && *(it + ext_len) != ' ' && *(it + ext_len) != '\0'))
	)
    {
	it++;
    }

    return it != ext_list.cend();
}

static std::string show_extensions_list(std::list<std::string> &extensions, char const *indent)
{
    std::ostringstream str;

    if (!extensions.empty())
    {
	extensions.sort();
	auto it = extensions.cbegin();

	while (it != extensions.cend() && *it == std::string())
	    it++;

	if (it != extensions.cend())
	    str << *it++;

	while (it != extensions.cend())
	    str << std::endl << indent << *it++;
    }

    return str.str();
}

static std::string show_extensions_list(std::list<std::string> &&extensions, char const *indent)
{
    return show_extensions_list(extensions, indent);
}

extern std::string trim_name(std::string name)
{
    while (!name.empty() && (*name.rbegin() == '\0' || *name.rbegin() == ' ' || *name.rbegin() == '\t'))
	name.pop_back();

    while (!name.empty() && (*name.begin() == '\0' || *name.begin() == ' ' || *name.rbegin() == '\t'))
	name.erase(name.begin());

    return name;
}

extern void show_cl_device(cl::Device &device, bool showPlatform)
{
    cout << "\tDevice:                 " << trim_name(device.getInfo<CL_DEVICE_NAME>()) << endl;
    cout << "\tPlatform:               " << trim_name(cl::Platform(device.getInfo<CL_DEVICE_PLATFORM>()).getInfo<CL_PLATFORM_NAME>()) << endl;
    cout << "\tVendor:                 [0x" << std::setw(4) << std::setfill('0') << std::setiosflags(cout.right | cout.uppercase) << std::setbase(16) << device.getInfo<CL_DEVICE_VENDOR_ID>() /* << std::resetiosflags() */ << std::setbase(10) << "] "
					<< device.getInfo<CL_DEVICE_VENDOR>() << endl;
    cout << "\tDriver version:         " << device.getInfo<CL_DRIVER_VERSION>() << endl;
    cout << "\tType                    " << list_cl_device_type(device.getInfo<CL_DEVICE_TYPE>()) << endl;
    cout << "\tOpencCL version:        " << device.getInfo<CL_DEVICE_VERSION>() << endl;
    cout << "\tOpenCL C version:       " << device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << endl;
    // cout << "\tSPIR version:           " << device.getInfo<CL_DEVICE_SPIR_VERSIONS>() << endl;
    cout << "\tProfile:                " << device.getInfo<CL_DEVICE_PROFILE>() << endl;
    cout << "\tDevice available:       " << (device.getInfo<CL_DEVICE_AVAILABLE>() ? "[+]" : "[ ]") << endl;
    cout << "\tCompiler available:     " << (device.getInfo<CL_DEVICE_COMPILER_AVAILABLE>() ? "[+]" : "[ ]") << endl;
    // cout << "\tLinker available:       " << (device.getInfo<CL_DEVICE_LINKER_AVAILABLE>() ? "[+]" : "[ ]") << endl;
    cout << "\tMax clock speed:        " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << " MHz" << endl;
    cout << "\tError correction:       " << (device.getInfo<CL_DEVICE_ERROR_CORRECTION_SUPPORT>() ? "[+]" : "[ ]") << endl;
    cout << "\tMemory:                 " << memory_size_str(device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>(), false) << ", " << device.getInfo<CL_DEVICE_ADDRESS_BITS>() << "-bit "
#if (CL_HPP_TARGET_OPENCL_VERSION >= 120)
                                        << (device.getInfo<CL_DEVICE_HOST_UNIFIED_MEMORY>() ? "host memory (" : "device memory (")
#else
                                        << "("
#endif
					<< memory_size_str(device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE>()) << ' ' << memoryCacheType(device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_TYPE>()) << " cache, "
					<< device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE>() * 8 << "-bit)" << endl;
    cout << "\tMax memory object size: " << memory_size_str(device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>(), false) << endl;
    cout << "\tMax image args:         " << device.getInfo<CL_DEVICE_MAX_READ_IMAGE_ARGS>() << " read, " << device.getInfo<CL_DEVICE_MAX_WRITE_IMAGE_ARGS>() << " write" << endl;
    cout << "\tMax const buffer size:  " << memory_size_str(device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>(), true) << endl; 
    cout << "\tMax const args:         " << device.getInfo<CL_DEVICE_MAX_CONSTANT_ARGS>() << endl;
    cout << "\tMax argument size:      " << memory_size_str(device.getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>(), true) << endl;
    cout << "\tBase addres alignament: " << device.getInfo<CL_DEVICE_MEM_BASE_ADDR_ALIGN>() / 8 << " bytes (" << device.getInfo<CL_DEVICE_MEM_BASE_ADDR_ALIGN>() << " bits)" << endl;
    // cout << "\tprintf buffer size:     " << memory_size_str(device.getInfo<CL_DEVICE_PRINTF_BUFFER_SIZE>(), true) << endl;
    cout << "\tLittle endian:          " << (device.getInfo<CL_DEVICE_ENDIAN_LITTLE>() ? "[+]" : "[ ]") << endl;
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
    std::string local_string = device.getInfo<CL_DEVICE_BUILT_IN_KERNELS>();
    std::regex separator(";");
    cout << "\tBuilt-in kernels:       " << show_extensions_list(std::list<std::string>(std::sregex_token_iterator(local_string.cbegin(), local_string.cend(), separator, -1), std::sregex_token_iterator()), "\t\t\t\t") << endl;
#endif
    cout << std::setfill(' ');
    cout << "\tNative vector size:     " << "(char: " << std::setw(2) << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR>() << ", short: " << std::setw(2) << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT>()
					 << ", int: " << std::setw(2) << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_INT>() << ", long: " << std::setw(2) << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG>()
					 << ", half: " << std::setw(2) << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF>() << ", float: " << std::setw(2) << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT>()
					 << ", double: " << std::setw(2) << device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE>() << ")" << endl;
    cout << "\tPrefferred vector size: " << "(char: " << std::setw(2) << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR>() << ", short: " << std::setw(2) << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT>()
					 << ", int: " << std::setw(2) << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT>() << ", long: " << std::setw(2) << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG>()
					 << ", half: " << std::setw(2) << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF>() << ", float: " << std::setw(2) << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT>()
					 << ", double: " << std::setw(2) << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE>() << ")" << endl;
    //cout << "\tHalf float config:      " << (has_extension(device.getInfo<CL_DEVICE_EXTENSIONS>(), "cl_khr_fp16") ?
	//					list_float_support(device.getInfo<CL_DEVICE_HALF_FP_CONFIG>()) : "-") << endl;
    cout << "\tDouble float config:    " << list_float_support(device.getInfo<CL_DEVICE_DOUBLE_FP_CONFIG>(), false) << endl;
    cout << "\tSingle float config:    " << list_float_support(device.getInfo<CL_DEVICE_SINGLE_FP_CONFIG>(), true) << endl;
    cout << "\tImage support:          " << (device.getInfo<CL_DEVICE_IMAGE_SUPPORT>() ? "Yes" : "No") << endl;
    // cout << "\t1D image size:          " << device.getInfo<CL_DEVICE_IMAGE_MAX_BUFFER_SIZE>() << endl;
    cout << "\tMax 2D image sizes:     " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>() << "x" << device.getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>() << endl;
    cout << "\tMax 3D image sizes:     " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_WIDTH>() << "x" << device.getInfo<CL_DEVICE_IMAGE3D_MAX_HEIGHT>() << "x" << device.getInfo<CL_DEVICE_IMAGE3D_MAX_DEPTH>() << endl;
    // cout << "\timage array size:       " << device.getInfo<CL_DEVICE_IMAGE_MAX_ARRAY_SIZE>() << endl;
    cout << "\tMax samplers count:     " << device.getInfo<CL_DEVICE_MAX_SAMPLERS>() << endl;
    cout << "\tTimer resolution:       " << device.getInfo<CL_DEVICE_PROFILING_TIMER_RESOLUTION>() << " nanoseconds" << endl;

    local_string = device.getInfo<CL_DEVICE_EXTENSIONS>();
    separator = std::regex("[[:space:]]+");
    cout << "\tExtensions:             " << show_extensions_list(std::list<std::string>(std::sregex_token_iterator(local_string.cbegin(), local_string.cend(), separator, -1), std::sregex_token_iterator()), "\t\t\t\t") << endl;
    cout << endl;
}

extern void show_cl_platform(cl::Platform &platform, bool all_devices, std::vector<PlatformDeviceSet *> &device_list)
{
    bool result = true;

    cout << "Platform:      \t" << trim_name(platform.getInfo<CL_PLATFORM_NAME>()) << endl;
    cout << "Vendor:        \t" << trim_name(platform.getInfo<CL_PLATFORM_VENDOR>()) << endl;
    cout << "Profile:       \t" << platform.getInfo<CL_PLATFORM_PROFILE>() << endl;
    cout << "Version:       \t" << platform.getInfo<CL_PLATFORM_VERSION>() << endl;
    cout << "ICD suffix:    \t" << platform.getInfo<CL_PLATFORM_ICD_SUFFIX_KHR>() << endl;

    std::string extensions = platform.getInfo<CL_PLATFORM_EXTENSIONS>();
    std::basic_regex<char> const feature_name_regexp("[^[:space:]]*");
    std::list<std::string> ext_list;
    for
	(
	    std::regex_iterator<std::string::const_iterator> it(extensions.cbegin(), extensions.cend(), feature_name_regexp);
	    it != std::regex_iterator<std::string::const_iterator>();
	    it++
	)
    {
	if (!it->empty() && !it->str().empty())
	    ext_list.push_back(it->str());
    }

    cout << "Extensions:    \t" << show_extensions_list(ext_list, "\t\t") << endl;

    std::vector<cl::Device> clDevices;
    std::vector<cl::Device> clPerTypeDevices;

    platform.getDevices(CL_DEVICE_TYPE_ALL, &clDevices);
    cout << "Devices:       \t";
    bool devices_output = false;

    for (cl_device_type deviceType: std::array<cl_device_type, 4> { CL_DEVICE_TYPE_ACCELERATOR, CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_CUSTOM })
    {
	clPerTypeDevices.clear();

	try
	{
	    platform.getDevices(deviceType, &clPerTypeDevices);

	    if (clPerTypeDevices.size() != 0)
	    {
		if (devices_output)
		    cout << ", ";

		cout << clPerTypeDevices.size() << ' ' << list_cl_device_type(deviceType);

		if (deviceType != CL_DEVICE_TYPE_CUSTOM && clPerTypeDevices.size() > 1)
		    cout << "s";

		devices_output = true;
	    }
	}
	catch (cl::Error const &err)
	{
	    if (err.err() != CL_DEVICE_NOT_FOUND && err.err() != CL_INVALID_DEVICE_TYPE)
		throw;
	}
    }

    if (!clDevices.empty())
    {
	if (clDevices.size() > 1)
	    cout << "\n               \t    " << list_cl_device_type(clDevices[0].getInfo<CL_DEVICE_TYPE>()) << ":";

	cout << " [" << trim_name(clDevices[0].getInfo<CL_DEVICE_NAME>()) << "]";

	for (size_t i = 1; i < clDevices.size(); i++)
	{
	    cout << endl << "               \t    " << list_cl_device_type(clDevices[i].getInfo<CL_DEVICE_TYPE>())
		<< ": [" << trim_name(clDevices[i].getInfo<CL_DEVICE_NAME>()) << "]";
	}
    }
    cout << endl;

    for (auto &platformDeviceSet: device_list)
	platformDeviceSet->platformUsage = true;

    all_devices = all_devices || std::any_of(device_list.cbegin(), device_list.cend(), [](PlatformDeviceSet *deviceSet) -> bool { return deviceSet->allDevices; });

    if (all_devices)
	for (auto &clDevice: clDevices)
	{
	    std::string device_name = trim_name(clDevice.getInfo<CL_DEVICE_NAME>());

	    show_cl_device(clDevice);

	    for (auto &deviceSet: device_list)
		for (size_t i = 0; i < deviceSet->devices.size(); i++)
		    if (device_name.find(deviceSet->devices[i]) != std::string::npos)
			deviceSet->deviceUsage[i] = true;
	}
    else
    {
	std::vector<bool> listed_devices(clDevices.size());

	for (auto &platformDeviceSet: device_list)
	    for (size_t i = 0; i < platformDeviceSet->devices.size(); i++)
		for (size_t cl_device = 0; cl_device < clDevices.size(); cl_device++)
		{
		    std::string device_name = trim_name(clDevices[cl_device].getInfo<CL_DEVICE_NAME>());

		    if (device_name.find(platformDeviceSet->devices[i]) != std::string::npos)
		    {
			if (!listed_devices[cl_device])
			    show_cl_device(clDevices[cl_device]);

			listed_devices[cl_device] = true;
			platformDeviceSet->deviceUsage[i] = true;
		    }
		}
    }

    cout << endl;
}
