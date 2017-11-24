#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
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
    bool probe = false;

    std::string probe_device;
    std::string select_platform_name;
    std::vector<std::string> select_devices;
};

class SyntaxError: public std::runtime_error
{
    public:
	SyntaxError();
	SyntaxError(char const *msg);
	SyntaxError(std::string const &msg);

	static void ShowSyntax(char const *cmd_name);
};

inline SyntaxError::SyntaxError()
    : runtime_error(std::string())
{
}

inline SyntaxError::SyntaxError(char const *msg)
    : runtime_error(msg)
{
}

inline SyntaxError::SyntaxError(std::string const &msg)
    : runtime_error(msg)
{
}

void SyntaxError::ShowSyntax(char const *cmd_name)
{
    cerr << "Syntax:" << endl;
    cerr << "\t" << cmd_name << " [--probe]" << endl;
    cerr << "\t" << cmd_name << " [--probe] --platforms [--devices]" << endl;
    cerr << "\t" << cmd_name << " [--probe] --platform \"OpencCL Platform Name\" [--devices]" << endl;
    cerr << "\t" << cmd_name << " [--probe] --platform \"OpencCL Platform Name\" --device \"OpenCL Device Name\"" << endl;
    cerr << "\t" << cmd_name << " [--probe] --default-devices" << endl;
    cerr << "\t" << cmd_name << " [--probe] [--platform \"OpenCL Platform Name\"] --all-devices" << endl;
    cerr << endl;
    cerr << cmd_name << " will by default attempt to probe the default OpenCL device(s) using a trivial matrix" << endl;
    cerr << "multiplication and report the number of floating-point operations per second in GFLOPS." << endl;
    cerr << endl;
    cerr << "Options:" << endl;
    cerr << "\t--probe" << endl;
    cerr << "\t     Attempts to run a matrix multiplication function on the device(s) using single-precision"<< endl;
    cerr << "\t     floating-point operations and report the operation speed in GFLOPS. With no other options," << endl;
    cerr << "\t     this is the default operation, run on an implicitly-selected set of default compute devices" << endl;
    cerr << "\t     chosen by the OpenCL system." << endl;
    cerr << endl;
    cerr << "\t--platforms [--devices]" << endl;
    cerr << "\t     List available OpenCL platforms on the system." << endl;
    cerr << "\t     With --devices also report details on available OpenCL devices in each platform." << endl;
    cerr << endl;
    cerr << "\t--platform \"OpenCL Platform Name\" [--devices]" << endl;
    cerr << "\t     List selected OpenCL platform." << endl;
    cerr << "\t     With --devices   report details on available OpenCL devices in selected platform." << endl;
    cerr << endl;
    cerr << "\t--platform \"OpenCL Platform Name\" --device \"OpenCL Device Name\" [--probe] " << endl;
    cerr << "\t     not currently used." << endl;
    cerr << endl;
    cerr << "\t--default-devices" << endl;
    cerr << "\t     Show details on some default OpenCL compute device(s) reported by the system." << endl;
    cerr << "\t     Note: some systems do not properly report a default OpenCL device." << endl;
    cerr << endl;
    cerr << "\t[--platform \"OpenCL Platform Name\"] --all-devices" << endl;
    cerr << "\t    Open and show details on some default OpenCL compute device(s) reported by teh system." << endl;
    cerr << "\t    With \"--platform\" opens devices in the given platform" << endl;
    cerr << "\t    Note: some systems do not properly report a set of default OpencCL devices." << endl;
    cerr << endl;
    cerr << "See the OpenCL specifications from https:://www.khronos.org/registry/OpenCL/ for more information" << endl;
    cerr << "about the platform and device details reported by " << cmd_name << '.' << endl;
    cerr << endl;
}
void parse_args(char const *argv[], struct args &args)
{
    while (argv[0])
    {
	char const *arg = argv[0];

	if (!strncmp("-h", arg, sizeof "-h") || !strncmp("--help", arg, sizeof "--help") || !strncmp("--usage", arg, sizeof "--usage"))
	    throw SyntaxError();

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

	if (!strncmp("--probe", arg, sizeof "--probe"))
	{
	    args.probe = true;
	    argv++;
	    continue;
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

	    throw SyntaxError("Command line option " + std::string(arg) + " missing device name argument.");
	}

	throw SyntaxError("Unknown command line option " + std::string(arg));
    }

    if (!(args.list_platforms || args.list_platform_devices || args.list_default_devices || args.list_all_devices || args.probe))
	args.probe = true;
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
	std::vector<cl::Platform> clPlatforms;

	cl::Platform::get(&clPlatforms);

	if (!clPlatforms.empty())
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

	    for (auto &platform: clPlatforms)
		show_cl_platform(platform, args.list_platform_devices, defaultPlatform);
	}

	cout << "OpenCL platforms: " << clPlatforms.size() << endl;
    }

    if (args.list_default_devices)
	list_default_cl_devices();

    cl_platform_id platform;
    std::array<cl_context_properties, 3> context_props = { 0, 0, 0 };

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

	context_props[0] = CL_CONTEXT_PLATFORM;
	context_props[1] = static_cast<cl_context_properties>(reinterpret_cast<intptr_t>(platform));
	context_props[2] = static_cast<cl_context_properties>(0);
    }

    cl::Context context(CL_DEVICE_TYPE_ALL, context_props[0] ? context_props.data() : nullptr, context_error_notification);

    if (args.list_all_devices)
	list_context_devices(context);

    if (args.probe)
    {
	cout << "Probing devices: \n";
	
	for (auto const &device: context.getInfo<CL_CONTEXT_DEVICES>())
	    cout << '\t' << device.getInfo<CL_DEVICE_NAME>() << endl;

	cout << endl;

	cl_ulong const lines = 2048, cols = 2048, internal_size = 2048;

	cl::Buffer
	    m(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY /* CL_MEM_HOST_NO_ACCESS */, sizeof(cl_double) * lines * internal_size),
	    n(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY /* CL_MEM_HOST_NO_ACCESS */, sizeof(cl_double) * internal_size * cols),
	    result(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(cl_double)* lines * cols);

	Matrix mat(context);
	std::vector<cl_float> sub_buffer;

	mat.random_fill<cl_float>(m, lines, internal_size, -100.0f, 100.0f);
	mat.random_fill<cl_float>(n, internal_size, cols, -100.0f, 100.0f);
	//mat.zero_fill<cl_float>(result, lines, cols);
	mat.random_fill<cl_float>(result, internal_size, cols, 0.0f, 0.0001f);
	mat.waitForCompletion();


	std::time_t startTime, endTime;
	time(&startTime);
	mat.multiply<cl_float>(m, lines, internal_size, n, internal_size, cols, result, lines, cols);
	mat.waitForCompletion();
	time(&endTime);

	cout << std::setprecision(5) << std::fixed << (static_cast<double>(lines) * internal_size * cols * 2 / 1000/1000/1000) / static_cast<double>(endTime - startTime) << " GFLOPS" << endl;


	// mat.readBufferRectAsync(result, lines, cols, 852, 718, 20, 20, sub_buffer);
	// mat.waitForCompletion();

	mat.readBufferRect(result, lines, cols, 852, 718, 20, 20, sub_buffer);

	cout << "Sample sub-matrix:\n";
	for (unsigned i = 0; i < 10; i++)
	{
	    for (unsigned j = 0; j < 10; j++)
		cout << std::setw(14) << std::setprecision(5) << std::fixed << std::setfill(' ') << std::setiosflags(cout.right) << std::showpos << sub_buffer[i * 10 + j] << ' ';

	    cout << endl;
	}
    }
	
    return EXIT_SUCCESS;
}
catch(SyntaxError const &err)
{
    err.ShowSyntax(argv[0]);

    if (err.what() && *err.what())
    {
	cerr << endl;
	cerr << err.what() << endl;
    }

    return EXIT_FAILURE;
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
