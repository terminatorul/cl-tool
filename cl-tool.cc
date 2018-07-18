#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <locale>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>

#if defined(_WINDOWS)
#include <Windows.h>
#if !defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

#include "parse-cmd-line.hh"
#include "cl-platform-info.hh"
#include "cl-matrix-mult.hh"

using std::cerr;
using std::cout;
using std::endl;

void list_context_devices(cl::Context &context)
{
    std::vector<cl::Device> devices(context.getInfo<CL_CONTEXT_DEVICES>());

    for (auto &device: devices)
	show_cl_device(device);
}

void CL_CALLBACK context_error_notification(char const *error_info, void const *private_info, std::size_t private_info_size, void *user_data)
{
    cerr << "Context error: " << error_info << endl;
}

void select_matching_platforms(cl::Platform &clPlatform, std::vector<PlatformDeviceSet> &userSelection, std::vector<PlatformDeviceSet *> &matchSelection)
{
    std::string
	platform_name = trim_name(clPlatform.getInfo<CL_PLATFORM_NAME>()),
	vendor_name = trim_name(clPlatform.getInfo<CL_PLATFORM_VENDOR>()),
	version = trim_name(clPlatform.getInfo<CL_PLATFORM_VERSION>()),
	icd_suffix = trim_name(clPlatform.getInfo<CL_PLATFORM_ICD_SUFFIX_KHR>());

    for (auto str: std::array<std::string *, 4> { &platform_name, &vendor_name, &version, &icd_suffix })
	std::transform(str->begin(), str->end(), str->begin(), std::bind(std::toupper<char>, std::placeholders::_1, std::locale()));

    matchSelection.clear();

    for (auto &platformDeviceSet: userSelection) 
	if (platform_name.find(platformDeviceSet.platformSelector) != std::string::npos ||
	    vendor_name.find(platformDeviceSet.platformSelector) != std::string::npos ||
	    version.find(platformDeviceSet.platformSelector) != std::string::npos ||
	    icd_suffix == platformDeviceSet.platformSelector)
	{
	    matchSelection.push_back(&platformDeviceSet);
	}
}

bool show_cl_platforms(CmdLineArgs::SelectionSet &platformSelection)
{
    bool result = true;
    std::vector<cl::Platform> clPlatforms;
    std::vector<PlatformDeviceSet *> matchSelection;

    cl::Platform::get(&clPlatforms);

    if (platformSelection.all_platforms)
    {
	for (cl::Platform &clPlatform: clPlatforms)
	{
	    select_matching_platforms(clPlatform, platformSelection.platforms, matchSelection);
	    show_cl_platform(clPlatform, platformSelection.all_devices, matchSelection);
	}

	cout << "OpenCL platforms: " << clPlatforms.size() << endl;
    }
    else
    {
	std::vector<bool> listedPlatforms(clPlatforms.size());

	for (PlatformDeviceSet &platformDeviceSet: platformSelection.platforms)
	{
	    for (size_t i = 0; i < clPlatforms.size(); i++)
		if (!listedPlatforms[i])
		{
		    std::string
			platform_name = trim_name(clPlatforms[i].getInfo<CL_PLATFORM_NAME>()),
			vendor_name = trim_name(clPlatforms[i].getInfo<CL_PLATFORM_VENDOR>()),
			version = trim_name(clPlatforms[i].getInfo<CL_PLATFORM_VERSION>()),
			icd_suffix = trim_name(clPlatforms[i].getInfo<CL_PLATFORM_ICD_SUFFIX_KHR>());

		    for (auto str: std::array<std::string *, 4> { &platform_name, &vendor_name, &version, &icd_suffix })
			std::transform(str->begin(), str->end(), str->begin(), std::bind(std::toupper<char>, std::placeholders::_1, std::locale()));

		    if (platform_name.find(platformDeviceSet.platformSelector) != std::string::npos
			    ||
			vendor_name.find(platformDeviceSet.platformSelector) != std::string::npos
			    ||
			version.find(platformDeviceSet.platformSelector) != std::string::npos
			    ||
			icd_suffix == platformDeviceSet.platformSelector)
		    {
			select_matching_platforms(clPlatforms[i], platformSelection.platforms, matchSelection);
			show_cl_platform(clPlatforms[i], false, matchSelection);
			listedPlatforms[i] = true;
		    }
		}
	}

	cout << "Selected OpenCL platforms: " << std::count(listedPlatforms.cbegin(), listedPlatforms.cend(), true) << endl; 
    }

    for (auto const &platformDeviceSet: platformSelection.platforms)
	if (!platformDeviceSet.platformUsage)
	{
	    cerr << endl;
	    cerr << "No matching OpenCL platform for \"" << platformDeviceSet.platformSelector << "\"." << endl;
	    result = false;
	}
	else
	    for (size_t i = 0; i < platformDeviceSet.devices.size(); i++)
		if (!platformDeviceSet.deviceUsage[i])
		{
		    cerr << endl;
		    cerr << "No matching OpenCL device for \"" << platformDeviceSet.devices[i] <<
			"\" in platforms for \"" << platformDeviceSet.platformSelector << "\"." << endl;
		    result = false;
		}

    return result;
}

bool probe_cl_platforms(CmdLineArgs::SelectionSet const &platforms)
{
    return false;
}

int main(int argc, char const *argv[])
try
{
    CmdLineArgs args;
    bool result = true;

    args.parse(argv + 1);

    list_all = args.show_defaults;

    if (args.list.all_platforms || !args.list.platforms.empty())
	result = show_cl_platforms(args.list);

    if (args.probe.all_platforms || !args.probe.platforms.empty())
	result = result && probe_cl_platforms(args.list);

    // cl_platform_id platform;
    // std::array<cl_context_properties, 3> context_props = { 0, 0, 0 };

    // if (!args.select_platform_name.empty())
    // {
    //     std::vector<cl::Platform> clPlatforms;
    //     bool platform_found = false;

    //     cl::Platform::get(&clPlatforms);

    //     for (auto it = clPlatforms.cbegin(); !platform_found && it != clPlatforms.cend(); it++)
    //         if (!strncmp(it->getInfo<CL_PLATFORM_NAME>().data(), args.select_platform_name.data(),
    //     		std::min(it->getInfo<CL_PLATFORM_NAME>().size(), args.select_platform_name.length())))
    //         {
    //     	platform = it->Wrapper<cl_platform_id>::get();
    //     	platform_found = true;
    //         }

    //     if (!platform_found)
    //         for (auto it = clPlatforms.cbegin(); !platform_found && it != clPlatforms.cend(); it++)
    //     	if(!strncmp(it->getInfo<CL_PLATFORM_VENDOR>().data(), args.select_platform_name.data(),
    //     		std::min(it->getInfo<CL_PLATFORM_VENDOR>().size(), args.select_platform_name.length())))
    //     	{
    //     	    platform = it->Wrapper<cl_platform_id>::get();
    //     	    platform_found = true;
    //     	}

    //     if (!platform_found)
    //         throw std::runtime_error("No such platform: " + args.select_platform_name);

    //     context_props[0] = CL_CONTEXT_PLATFORM;
    //     context_props[1] = static_cast<cl_context_properties>(reinterpret_cast<intptr_t>(platform));
    //     context_props[2] = static_cast<cl_context_properties>(0);
    // }

    // cl::Context context(CL_DEVICE_TYPE_DEFAULT, context_props[0] ? context_props.data() : nullptr, context_error_notification);

    // if (args.list_all_devices)
    //     list_context_devices(context);

    // if (args.probe)
    // {
    //     cout << "Probing device" << (context.getInfo<CL_CONTEXT_DEVICES>().size() > 1 ? "s" : "") << ": \n";

    //     for (auto const &device: context.getInfo<CL_CONTEXT_DEVICES>())
    //         cout << '\t' << device.getInfo<CL_DEVICE_NAME>() << endl;

    //     cout << endl;

    //     cl_ulong const lines = 1024, cols = 1024, internal_size = 1024;

    //     cl::Buffer
    //         m(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY /* CL_MEM_HOST_NO_ACCESS */, sizeof(cl_double) * lines * internal_size),
    //         n(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY /* CL_MEM_HOST_NO_ACCESS */, sizeof(cl_double) * internal_size * cols),
    //         result(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(cl_double)* lines * cols);

    //     Matrix mat(context);
    //     std::vector<cl_float> sub_buffer;

    //     mat.random_fill<cl_float>(m, lines, internal_size, -100.0f, 100.0f);
    //     mat.random_fill<cl_float>(n, internal_size, cols, -100.0f, 100.0f);
    //     //mat.zero_fill<cl_float>(result, lines, cols);
    //     mat.random_fill<cl_float>(result, internal_size, cols, 0.0f, 0.0001f);
    //     mat.waitForCompletion();


    //     std::time_t startTime, endTime;
    //     time(&startTime);
    //     mat.multiply<cl_float>(m, lines, internal_size, n, internal_size, cols, result, lines, cols);
    //     mat.waitForCompletion();
    //     time(&endTime);

    //     cout << std::setprecision(5) << std::fixed << (static_cast<double>(lines) * internal_size * cols * 2 / 1000/1000/1000) / static_cast<double>(endTime - startTime) << " GFLOPS" << endl;


    //     // mat.readBufferRectAsync(result, lines, cols, 852, 718, 20, 20, sub_buffer);
    //     // mat.waitForCompletion();

    //     mat.readBufferRect(result, lines, cols, 852, 718, 20, 20, sub_buffer);

    //     cout << "Sample sub-matrix:\n";
    //     for (unsigned i = 0; i < 10; i++)
    //     {
    //         for (unsigned j = 0; j < 10; j++)
    //     	cout << std::setw(14) << std::setprecision(5) << std::fixed << std::setfill(' ') << std::setiosflags(cout.right) << std::showpos << sub_buffer[i * 10 + j] << ' ';

    //         cout << endl;
    //     }
    // }
	
    return result ? EXIT_SUCCESS : EXIT_FAILURE ;
}
catch(SyntaxError const &err)
{
    err.ShowSyntax("cl-tool");

    if (err.what() && *err.what())
    {
	cerr << endl;
	cerr << err.what() << endl;
    }

    return EXIT_FAILURE;
}
catch(cl::Error const &err)
{
    if (err.err() == CL_PLATFORM_NOT_FOUND_KHR)
	cerr << "No available OpenCL platforms are installed." << endl;
    else
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
