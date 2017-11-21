#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <string>
#include <regex>
#include <iostream>
#include <iomanip>
#include <sstream>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpneCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

#include "cl-platform-info.hh"
#include "cl-matrix-mult.hh"

using std::cerr;
using std::cout;
using std::endl;


struct args
{
    bool list_platforms = false;
    bool list_platform_devices = false;
    bool list_default_devices = false;
    bool list_all_devices = false;

    std::string select_platform_name;
    std::vector<std::string> select_devices;
};

void parse_args(char const *argv[], struct args &args)
{
    while (argv[0])
    {
	char const *arg = argv[0];

	if (!strncmp("--platforms", arg, sizeof "--platforms"))
	{
	    args.list_platforms = true;
	    argv++;
	    continue;
	}

	if (!strncmp("--default-devices", arg, sizeof "--default-devices"))
	{
	    args.list_default_devices = true;
	    argv++;
	    continue;
	}

	if (!strncmp("--all-devices", arg, sizeof "--all-devices"))
	{
	    args.list_all_devices = true;
	    argv++;
	    continue;
	}

	if (!strncmp("--devices", arg, sizeof "--devices"))
	{
	    args.list_platform_devices = true;
	    argv++;
	    continue;
	}

	if (!strncmp("--platform", arg, sizeof "--platform"))
	{
	    argv++;

	    if (argv[0])
	    {
		args.select_platform_name = argv[0];
		argv++;
		continue;
	    }
	    
	    throw std::runtime_error("Command line missing platform name");
	}

	if (!strncmp("--device", arg, sizeof "--device"))
	{
	    argv++;

	    if (argv[0])
	    {
		args.select_devices.push_back(argv[0]);
		argv++;
		continue;
	    }

	    throw std::runtime_error("Command line missing device name");
	}

	throw std::runtime_error("Unknown command line option");
    }
}

void list_context_devices(cl::Context &context)
{
    std::vector<cl::Device> devices(context.getInfo<CL_CONTEXT_DEVICES>());

    for (auto &device: devices)
	show_cl_device(device, true);
}

void CL_CALLBACK context_error_notification(char const *error_info, void const *private_info, std::size_t private_info_size, void *user_data)
{
    cerr << "Context error: " << error_info << endl;
}

int main(int argc, char const *argv[])
try
{
    struct args args;

    parse_args(&argv[1], args);

    if (args.list_platforms)
    {
	cl::Platform defaultPlatform;
	
	try
	{
	    defaultPlatform = cl::Platform::getDefault();
	}
	catch(cl::Error const &err)
	{
	    cerr << "Select default platform failed with " << error_string(err.err()) << " in call to " << err.what() << endl;
	}
	catch(std::exception const &ex)
	{
	    cerr << "Select default platform failed: " << ex.what() << endl;
	}
	catch(...)
	{
	    cerr << "Select default platform failed." << endl;
	}

	std::vector<cl::Platform> clPlatforms;

	cl::Platform::get(&clPlatforms);

	for (auto &platform: clPlatforms)
	    show_cl_platform(platform, args.list_platform_devices, defaultPlatform);

	std::cout << "OpenCL platforms: " << clPlatforms.size() << std::endl;
    }

    if (args.list_default_devices)
	list_default_cl_devices();

    cl_platform_id platform;
    std::vector<cl_context_properties> context_props;

    if (!args.select_platform_name.empty())
    {
	std::vector<cl::Platform> clPlatforms;
	bool platform_found = false;

	cl::Platform::get(&clPlatforms);

	for (auto it = clPlatforms.cbegin(); !platform_found && it != clPlatforms.cend(); it++)
	    if (!strncmp(it->getInfo<CL_PLATFORM_NAME>().data(), args.select_platform_name.data(), args.select_platform_name.length()))
	    {
		platform = it->Wrapper<cl_platform_id>::get();
		platform_found = true;
	    }

	if (!platform_found)
	    throw std::runtime_error("No such platform: " + args.select_platform_name);

	context_props.resize(3);
	context_props[0] = CL_CONTEXT_PLATFORM;
	context_props[1] = static_cast<cl_context_properties>(reinterpret_cast<intptr_t>(platform));
	context_props[2] = static_cast<cl_context_properties>(0);
    }

    cl::Context context(CL_DEVICE_TYPE_ALL, context_props.data(), context_error_notification);

    if (args.list_all_devices)
	list_context_devices(context);

    cl_ulong const lines = 1000, cols=1000, internal_size= 1000;
    cl::Buffer
	m(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, sizeof(cl_double) * lines * internal_size),
	n(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, sizeof(cl_double) * internal_size * cols),
	result(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(cl_double)* lines * cols);

    Matrix mat(context);
    mat.random_fill<cl_float>(m, lines, internal_size, -100.0f, 100.0f);
    mat.random_fill<cl_float>(n, internal_size, cols, -100.0f, 100.0f);
    // matrix_multiply<cl_float, lines, internal_size, cols>(m, n, result);
    // show_sub_buffer<cl_float, lines, cols>(result, 25, 10);
	
    return EXIT_SUCCESS;
}
catch(cl::Error const &err)
{
    cerr << "OpenCL error " << error_string(err.err()) << " in call to function " << err.what() << "()" << endl;
    return EXIT_FAILURE;
}
catch(std::exception const &ex)
{
    cerr << "Application error: " << ex.what() << endl;
    return EXIT_FAILURE;
}
catch(...)
{
    cerr << "Application error!" << endl;
    return EXIT_FAILURE;
}
