#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <locale>
#include <initializer_list>
#include <vector>
#include <array>
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

#include "cl-platform-info.hh"
#include "cl-platform-probe.hh"
#include "cl-user-selection.hh"
#include "parse-cmd-line.hh"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::array;
using std::vector;
using std::pair;
using std::transform;
using std::bind;
using std::toupper;
using std::placeholders::_1;
using std::locale;
using std::exception;
using std::count;
using std::find;

using cl::Error;
using cl::Context;
using cl::Platform;
using cl::Device;

bool enumerate_cl_platforms
    (
	cl::vector<Platform>			   &nativePlatforms,
	UserDeviceSelection	    		   &userDeviceSelection,
	vector<pair<unsigned, vector<unsigned>>>   &platformSelection,
	bool					    probe
    )
{
    bool result = true;
    unsigned deviceCount = 0U;

    for (pair<unsigned, vector<unsigned>> const &platform: platformSelection)
    {
	Platform &nativePlatform = nativePlatforms[platform.first];
	cl::vector<Device> &platformDevices = userDeviceSelection.platformDevices(platform.first);

	if (probe)
	    probe_cl_platform(nativePlatform);
	else
	    show_cl_platform(nativePlatform, platformDevices);

	for (unsigned device: platform.second)
	    if (probe)
		result = result && probe_cl_device(platformDevices[device]);
	    else
		show_cl_device(platformDevices[device]);

	deviceCount += static_cast<unsigned>(platform.second.size());

	if (platform.second.empty())
	    cout << endl;
    }

    if (!platformSelection.empty())
    {
	cout << platformSelection.size() << " OpenCL platform" << (platformSelection.size() > 1 ? "s" : "");

	if (deviceCount)
	    cout << ", " << deviceCount << " device" << (deviceCount > 1 ? "s" : "") << " total";

	cout << '.' << endl;
    }

    return result;
}

int main(int argc, char const *argv[])
try
{
    CmdLineArgs args;
    args.parse(argv + 1);

    vector<pair<unsigned, vector<unsigned>>> listDevices, probeDevices;
    bool result = true;
    cl::vector<Platform> platformList;

    Platform::get(&platformList);

    UserDeviceSelection userDeviceSelection;
    userDeviceSelection.loadPlatformsAndDevices(platformList, args.exact_match);

    result = userDeviceSelection.selectDeviceTree(args.listSet, args.opencl_order);
    userDeviceSelection.selectedDevices().swap(listDevices);

    userDeviceSelection.clearSelection();
    result = result && userDeviceSelection.selectDeviceTree(args.probeSet, args.opencl_order);
    userDeviceSelection.selectedDevices().swap(probeDevices);

    if (result)
    {
	result = result && enumerate_cl_platforms(platformList, userDeviceSelection, listDevices, false);

	if (!listDevices.empty() && !probeDevices.empty())
	    cout << endl;

	result = result && enumerate_cl_platforms(platformList, userDeviceSelection, probeDevices, true);
    }

    return result ? EXIT_SUCCESS : EXIT_FAILURE ;
}
catch(SyntaxError const &err)
{
    err.showSyntax("cl-tool");

    if (err.what() && *err.what())
    {
	cerr << endl;
	cerr << err.what() << endl;
    }

    return EXIT_FAILURE;
}
catch(Error const &err)
{
    if (err.err() == CL_PLATFORM_NOT_FOUND_KHR)
	cerr << "No available OpenCL platforms are installed." << endl;
    else
	cerr << "OpenCL error " << error_string(err.err()) << " in call to function " << err.what() << "()" << endl;

    return EXIT_FAILURE;
}
catch(exception const &ex)
{
    cerr << "Application error: " << ex.what() << endl;
    return EXIT_FAILURE;
}
catch(...)
{
    cerr << "Application error!" << endl;
    return EXIT_FAILURE;
}
