#include <iostream>
#include <algorithm>

#include "parse-cmd-line.hh"

using std::cerr;
using std::endl;

void SyntaxError::ShowSyntax(char const *cmd_name)
{
    cerr << "Syntax:" << endl;
    cerr << "\t" << cmd_name << endl;
    cerr << "\t" << cmd_name << " [ [--list] [--probe] --platforms [--devices] ] " << endl;
    cerr << "\t" << cmd_name << " [ [--list] [--probe] --platform \"Name\" [--devices | --device \"Name\" ]... ]... " << endl;
    cerr << endl;
    cerr << cmd_name << " will by default attempt to probe the default OpenCL device(s) using a trivial matrix" << endl;
    cerr << "multiplication and report the number of floating-point operations per second in GFLOPS." << endl;
    cerr << endl;
    cerr << "Options:" << endl;
    cerr << "\t--probe" << endl;
    cerr << "\t     Attempt to run a test function for each available device, using floating-point operations," << endl;
    cerr << "\t     and show the resulting speed (usually in GFLOPS). Without other options, this will be the default" << endl;
    cerr << "\t     action. With no device given, --probe implies --devices, meaning all platform devices will be" << endl;
    cerr << "\t     probed." << endl;
    cerr << endl;
    cerr << "\t[--list][ [--probe] --platforms [--devices]" << endl;
    cerr << "\t     With --list (default) show details on the available OpenCL platforms." << endl;
    cerr << "\t     With --devices also show details on available OpenCL devices in the platforms." << endl;
    cerr << "\t     With --probe report the resulting floating-point speed of the devices." << endl;
    cerr << "\t     To both show details and test all devices in your system use:" << endl;
    cerr << "\t          " << cmd_name << " --list --probe --platforms --devices" << endl;
    cerr << endl;
    cerr << "\t[--list] [--probe] --platform \"OpenCL Platform or Vendor Name\" [--devices]" << endl;
    cerr << "\t     With --list (default) show details on selected OpenCL platform/vendor." << endl;
    cerr << "\t     With --devices show details on available OpenCL devices in selected platform." << endl;
    cerr << "\t     With --probe report the resulting floating-point speed of the devices," << endl;
    cerr << "\t     usually in GFLOPS." << endl;
    cerr << endl;
    cerr << "\t[--list] [--probe] --platform \"OpenCL Platform Name\" --device \"OpenCL Device Name\" " << endl;
    cerr << "\t     With --list (default) show selected OpenCL device, from the selected platform." << endl;
    cerr << "\t     With --probe show the resulting floating-point speed of the device." << endl;
    cerr << endl;
    cerr << "The platform and device names can also be substrings or prefixes thereof. You can list and probe several" << endl;
    cerr << "platforms and devices with multiple options. Option order is significant, specify the platform before the" << endl;
    cerr << "device(s)." << endl;
    cerr << endl;
    cerr << "OpenCL allows multiple devices with the same name (or multiple platforms with the same name). In this" << endl;
    cerr << "case there will be no way to distinguish between the devices using " << cmd_name << "." << endl;
    cerr << endl;
    cerr << "See the OpenCL specification at https:://www.khronos.org/registry/OpenCL/ for more information" << endl;
    cerr << "about the platform and device details listed by " << cmd_name << '.' << endl;
    cerr << endl;
}

void CmdLineArgs::newAction(SelectionSet &selectionSet, char const *platform, bool allDevices, std::vector<char const *> const &devices)
{
    if (platform)
    {
	selectionSet.platforms.emplace_back();
	PlatformDeviceSet &deviceSet = *selectionSet.platforms.rbegin();

	deviceSet.platformSelector = platform;
	deviceSet.platformUsage = false;
	deviceSet.allDevices = allDevices;
	deviceSet.devices.assign(devices.cbegin(), devices.cend());
	deviceSet.deviceUsage.resize(devices.size());
    }
    else
    {
	selectionSet.all_platforms = true;

	if (allDevices)
	    selectionSet.all_devices = true;
    }
}

void CmdLineArgs::newAction()
{
    if (listAction)
	newAction(list, platform, allDevices, devices);

    if (probeAction)
	newAction(probe, platform, allDevices, devices);
}

void CmdLineArgs::startSelection(bool resetActions)
{
    if (resetActions)
    {
	listAction = false;
	probeAction = false;
    }

    allDevices = false;
    platform = nullptr;
    devices.clear();

    state = ReadActions;
}

void CmdLineArgs::parse(char const * const argv[])
{
    while (argv[0])
    {
	char const *arg = argv[0];

	if (!strncmp("-h", arg, sizeof "-h") || !strncmp("--help", arg, sizeof "--help") || !strncmp("--usage", arg, sizeof "--usage"))
	    throw SyntaxError();

	if (!strncmp("--probe", arg, sizeof "--probe"))
	{
	    if (state != ReadActions)
	    {
		newAction();
		startSelection(true);
	    }

	    probeAction = true;

	    argv++;
	    continue;
	}

	if (!strncmp("--list", arg, sizeof "--list"))
	{
	    if (state != ReadActions)
	    {
		newAction();
		startSelection(true);
	    }

	    listAction = true;

	    argv++;
	    continue;
	}

	if (!strncmp("--platforms", arg, sizeof "--platforms"))
	{
	    switch (state)
	    {
	    case ReadActions:
		if (!(listAction || probeAction))
		    listAction = true;
		break;
	    case ReadPlatform:
		break;
	    case ReadDevices:
		newAction();
		startSelection();
		break;
	    }

	    platform = nullptr;
	    state = ReadDevices;

	    argv++;
	    continue;
	}

	if (!strncmp("--platform", arg, sizeof "--platform"))
	{
	    switch (state)
	    {
	    case ReadActions:
		if (!(listAction || probeAction))
		    listAction = true;
		break;
	    case ReadPlatform:
		break;
	    case ReadDevices:
		newAction();
		startSelection();
		break;
	    }

	    argv++;

	    if (argv[0])
	    {
		platform = argv[0];
		state = ReadDevices;
	    }
	    else
		throw SyntaxError("Missing platform name after \"--platform\" argument.");

	    argv++;
	    continue;
	}

	if (!strncmp("--devices", arg, sizeof "--devices"))
	{
	    if (state != ReadDevices)
		throw SyntaxError("Specify actions (\"--list\" and/or \"--probe\") and \"--platform\" before device name.");
	    
	    allDevices = true;

	    argv++;
	    continue;
	}

	if (!strncmp("--device", arg, sizeof "--device"))
	{
	    if (state != ReadDevices)
		throw SyntaxError("Specify actions (\"--list\"/\"--probe\") and \"--platform\" before device name.");

	//     if (allDevices)
	// 	throw SyntaxError("Ambigous device selection, use either \"--devices\", either \"--device Name\".");

	    if (!platform)
		throw SyntaxError("Specify a platform name with \"--platform\" before using \"--device\".");

	    argv++;

	    if (argv[0])
		devices.push_back(argv[0]);
	    else
		throw SyntaxError("Command line option " + std::string(arg) + " missing device name argument.");

	    argv++;
	    continue;
	}

	throw SyntaxError("Unknown command line option " + std::string(arg));
    }

    if (!probeAction && !listAction)
	probeAction = true;

    newAction();
}

