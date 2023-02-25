#include <locale>
#include <iostream>
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include <string>

#include "parse-cmd-line.hh"

using std::cerr;
using std::endl;
using std::vector;
using std::pair;
using std::string;
using std::stoul;

void SyntaxError::showSyntax(char const *cmd_name)
{
    cerr << "Syntax:" << endl;
    cerr << "\t" << cmd_name << " [ --include-defaults ]" << endl;
    cerr << "\t" << cmd_name << " [ [--list] [--probe [--max-count 300]] --platforms [--devices] ] " << endl;
    cerr << "\t" << cmd_name << " [ [--list] [--probe [--max-count 300]] --platform \"Name\" [--devices | --device \"Name\" ]... ]... " << endl;
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
    cerr << "\t[--max-count 300]" << endl;
    cerr << "\t     Max number of simmulations to run for one pass when probing devices. The number of work items" << endl;
    cerr << "\t     increases for each simulation during a pass in a linear fashion, from 0 to a maximum that is" << endl;
    cerr << "\t     determined so that one simulation fits in around 450ms." << endl;
    cerr << endl;
    cerr << "\t[--list][ [--probe] --platforms [--devices]" << endl;
    cerr << "\t     With --list or --show (default), show details on the available OpenCL platforms." << endl;
    cerr << "\t     With --devices also show details on available OpenCL devices in the platforms." << endl;
    cerr << "\t     With --probe report the resulting floating-point speed of the devices." << endl;
    cerr << "\t     To both show details and test all devices in your system use:" << endl;
    cerr << "\t          " << cmd_name << " --list --probe --platforms --devices" << endl;
    cerr << endl;
    cerr << "\t[--list] [--probe] --platform \"OpenCL Platform or Vendor Name\" [--devices]" << endl;
    cerr << "\t     With --list or --show (default), show details on selected OpenCL platform or vendor/other info." << endl;
    cerr << "\t     With --devices show details on available OpenCL devices in selected platform." << endl;
    cerr << "\t     With --probe report the resulting floating-point speed of the devices," << endl;
    cerr << "\t     usually in GFLOPS." << endl;
    cerr << endl;
    cerr << "\t[--list] [--probe] --platform \"OpenCL Platform Name\" --device \"OpenCL Device Name\" " << endl;
    cerr << "\t     With --list or --show (default), show selected OpenCL device, from the selected platform." << endl;
    cerr << "\t     With --probe show the resulting floating-point speed of the device." << endl;
    cerr << endl;
    cerr << "\t[--include-defaults]" << endl;
    cerr << "\t     Include device details and extensions that are pre-defined for any OpenCL 1.2 device. By default" << endl;
    cerr << "\t     pre-defined values are not shown to simplify the output." << endl;
    cerr << endl;
    cerr << "\t[--opencl-order]" << endl;
    cerr << "\t     Keep platform and device order as reported by OpenCL. By default the order from the command line" << endl;
    cerr << "\t     is used, as the OpenCL order is not meant to be significant." << endl;
    cerr << endl;
    cerr << "\t[--exact-match]" << endl;
    cerr << "\t     Match platfom and device name exactly. By default names on the command line can be substrings or" << endl;
    cerr << "\t     prefixes of the OpenCL reported names." << endl;
    cerr << endl;
    cerr << "The platform and device names can also be substrings or prefixes thereof. You can show and probe several" << endl;
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

void CmdLineArgs::newCommand(vector<pair<char const *, vector<char const *>>> &selectionSet, bool clearDevices)
{
    selectionSet.emplace_back(platform, vector<char const *>());
    if (clearDevices)
	selectionSet.rbegin()->second.swap(devices);
    else
	selectionSet.rbegin()->second = devices;
}

void CmdLineArgs::flushPendingCommand()
{
    if (listAction)
	newCommand(listSet, !probeAction);

    if (probeAction)
    {
	if (devices.empty())
	    devices.push_back(nullptr);

	newCommand(probeSet, true);
    }
}

void CmdLineArgs::restartParser(bool resetActions)
{
    if (resetActions)
    {
	listAction = false;
	probeAction = false;
    }

    platform = nullptr;
    devices.clear();

    state = ReadActions;
}

char const * const *CmdLineArgs::parseGlobalOptions(char const * const argv[])
{
    if
	(
	    !strncmp("-h",	argv[0], sizeof "-h")
		||
	    !strncmp("--help",  argv[0], sizeof "--help")
		||
	    !strncmp("--usage", argv[0], sizeof "--usage")
	)
    {
	throw SyntaxError();
    }

    if (!strncmp("--include-defaults", argv[0], sizeof "--include-defaults"))
    {
	show_defaults = true;
	argv++;
    }

    if (argv[0] && !strncmp("--opencl-order", argv[0], sizeof "--opencl-order"))
    {
	opencl_order = true;
	argv++;
    }

    if (argv[0] && !strncmp("--exact-match", argv[0], sizeof "--exact-match"))
    {
	exact_match = true;
	argv++;
    }

    if (argv[0] && !strncmp("--max-count", argv[0], sizeof "--max-count"))
    {
	argv++;
	simulation_count = stoul(argv[0]);
	has_simulation_count = true;
	argv++;
    }

    return argv;
}

char const * const *CmdLineArgs::parsePlatformActions(char const * const argv[])
{
    if (!strncmp("--probe", argv[0], sizeof "--probe"))
    {
	if (state != ReadActions)
	{
	    flushPendingCommand();
	    restartParser(true);
	}

	probeAction = true;
	argv++;
    }

    if (argv[0] && (!strncmp("--list", argv[0], sizeof "--list") || !strncmp("--show", argv[0], sizeof "--show")))
    {
	if (state != ReadActions)
	{
	    flushPendingCommand();
	    restartParser(true);
	}

	listAction = true;

	argv++;
    }

    return argv;
}

char const * const *CmdLineArgs::parsePlatformSelection(char const * const argv[])
{
    if (!strncmp("--platforms", argv[0], sizeof "--platforms"))
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
	    flushPendingCommand();
	    restartParser();
	    break;
	}

	state = ReadDevices;

	argv++;
    }

    if (argv[0] && !strncmp("--platform", argv[0], sizeof "--platform"))
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
	    flushPendingCommand();
	    restartParser();
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
    }

    return argv;
}

char const * const *CmdLineArgs::parsePlatfromDevices(char const * const argv[])
{
    if (!strncmp("--devices", argv[0], sizeof "--devices"))
    {
	switch (state)
	{
	case ReadActions:
	    if (!(listAction || probeAction))
		listAction = true;
	    state = ReadDevices;
	    break;
	case ReadPlatform:
	    state = ReadDevices;
	    break;
	case ReadDevices:
	    break;
	}

	devices.push_back(nullptr);

	argv++;
    }

    if (argv[0] && !strncmp("--device", argv[0], sizeof "--device"))
    {
	switch (state)
	{
	case ReadActions:
	    if (!(listAction || probeAction))
		listAction = true;
	    state = ReadDevices;
	    break;
	case ReadPlatform:
	    state = ReadDevices;
	    break;
	case ReadDevices:
	    break;
	}

	argv++;

	if (argv[0])
	{
	    devices.push_back(argv[0]);
	    argv++;
	}
	else
	    throw SyntaxError("Command line option " + string(argv[0]) + " missing device name argument.");
    }

    return argv;
}

char const * const *CmdLineArgs::parsePlatformCommand(char const * const argv[])
{
    if (argv[0])
    {
	argv = parsePlatformActions(argv);

	if (argv[0])
	{
	    argv = parsePlatformSelection(argv);

	    if (argv[0])
		argv = parsePlatfromDevices(argv);
	}
    }

    return argv;
}

char const * const *CmdLineArgs::parseCompleted(char const * const argv[])
{
    if (!probeAction && !listAction)
	probeAction = true;

    flushPendingCommand();

    return argv;
}

void CmdLineArgs::parse(char const * const argv[])
{
    while (argv[0])
    {
	char const * const *arg = parseGlobalOptions(argv);

	if (arg[0])
	    arg = parsePlatformCommand(arg);


	if (arg == argv)
	    throw SyntaxError("Unknown command line option " + string(argv[0]));
	else
	    argv = arg;
    }

    parseCompleted(argv);

    if (has_simulation_count && probeSet.empty())
	throw SyntaxError("Max simulation count specified without devices to probe.");
}
