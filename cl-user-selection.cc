#include <utility>
#include <type_traits>
#include <algorithm>
#include <initializer_list>
#include <vector>
#include <string>
#include <functional>
#include <locale>
#include <iostream>

#if defined(__APPLE__) || defined(__MACOSX__)
#include <OpenCL/cl2.hpp>
#else
#include <CL/cl2.hpp>
#endif

#include "cl-user-selection.hh"

using std::vector;
using std::string;
using std::array;
using std::pair;
using std::move;
using std::find;
using std::find_if;
using std::transform;
using std::toupper;
using std::bind;
using std::placeholders::_1;
using std::locale;
using std::cerr;
using std::endl;

using cl::Error;
using cl::Platform;
using cl::Device;

class ExactPlatformDeviceInfo: public UserDeviceSelection::PlatformDeviceInfo
{
protected:
    string platformName;
    vector<string> platformDevices;

public:
    virtual string const &name() const override;
    virtual void loadPlatform(Platform &platform) override;
    virtual void loadDevices(cl::vector<Device> &devices) override;
    virtual bool checkDeviceName(unsigned deviceIdx, string const &deviceName) override;
    virtual bool checkPlatformName(string const &name) override;
    virtual void updateUserSelector(string &selector) override;
};

class MatchPlatformDeviceInfo: public UserDeviceSelection::PlatformDeviceInfo
{
protected:
    string platformName;
    array<string, 4> platformInfo;
    vector<string> platformDevices;
    static locale current_locale;

public:
    virtual string const &name() const override;
    virtual void loadPlatform(Platform &platform) override;
    virtual void loadDevices(cl::vector<Device> &devices) override;
    virtual bool checkDeviceName(unsigned deviceIdx, string const &deviceName) override;
    virtual bool checkPlatformName(string const &name) override;
    virtual void updateUserSelector(string &selector) override;
};

inline string const &ExactPlatformDeviceInfo::name() const
{
    return platformName;
}

inline void ExactPlatformDeviceInfo::loadPlatform(Platform &platform)
{
    platformName = platform.getInfo<CL_PLATFORM_NAME>();
}

inline void ExactPlatformDeviceInfo::loadDevices(cl::vector<Device> &devices)
{
    platformDevices.resize(devices.size());

    transform
	(
	    devices.begin(),
	    devices.end(),
	    platformDevices.begin(),
	    [](Device &device) -> string
	    {
		return move(device.getInfo<CL_DEVICE_NAME>());
	    }
	);
}

inline bool ExactPlatformDeviceInfo::checkDeviceName(unsigned deviceIdx, string const &deviceName)
{
    return platformDevices[deviceIdx] == deviceName;
}

inline bool ExactPlatformDeviceInfo::checkPlatformName(string const &name)
{
    return platformName == name;
}

inline void ExactPlatformDeviceInfo::updateUserSelector(string &)
{
}

locale MatchPlatformDeviceInfo::current_locale("");

inline string const &MatchPlatformDeviceInfo::name() const
{
    return platformName;
}

inline void MatchPlatformDeviceInfo::loadPlatform(Platform &platform)
{
    platformName = platform.getInfo<CL_PLATFORM_NAME>();

    platformInfo =
    {
	platformName,
	platform.getInfo<CL_PLATFORM_VENDOR>(),
	platform.getInfo<CL_PLATFORM_VERSION>(),
	platform.getInfo<CL_PLATFORM_ICD_SUFFIX_KHR>(),
    };

    for (string &str: platformInfo)
	MatchPlatformDeviceInfo::updateUserSelector(str);
}

inline void MatchPlatformDeviceInfo::loadDevices(cl::vector<Device> &devices)
{
    platformDevices.resize(devices.size());

    transform
	(
	    devices.begin(),
	    devices.end(),
	    platformDevices.begin(),
	    [this](Device &device) -> string
	    {
		string deviceName = device.getInfo<CL_DEVICE_NAME>();
		MatchPlatformDeviceInfo::updateUserSelector(deviceName);

		return move(deviceName);
	    }
	);
}

inline bool MatchPlatformDeviceInfo::checkDeviceName(unsigned deviceIdx, string const &deviceName)
{
    return platformDevices[deviceIdx].find(deviceName) != platformDevices[deviceIdx].npos;
}

inline bool MatchPlatformDeviceInfo::checkPlatformName(string const &name)
{
    return
	find_if
	    (
		platformInfo.cbegin(),
		platformInfo.cend() - 1,
		[&name](string const &prop) -> bool
		{
		    return prop.find(name) != prop.npos;
		}
	    )
	    !=
	platformInfo.cend() - 1
	    ||
	*platformInfo.rbegin() == name;
}

inline void MatchPlatformDeviceInfo::updateUserSelector(string &selector)
{
    transform(selector.begin(), selector.end(), selector.begin(), bind(toupper<char>, _1, current_locale));
}

inline static void insertSelectedDevice(vector<unsigned> &deviceList, unsigned deviceIdx)
{
    auto selected_device_it = find(deviceList.cbegin(), deviceList.cend(), deviceIdx);

    if (selected_device_it == deviceList.cend())
	deviceList.push_back(deviceIdx);
}

inline static void sortSelectedDeviceList(vector<pair<unsigned, vector<unsigned>>> &selectedDeviceList)
{
    sort
	(
	    selectedDeviceList.begin(),
	    selectedDeviceList.end(),
	    [](pair<unsigned, vector<unsigned>> const &lhs, pair<unsigned, vector<unsigned>> const &rhs) -> bool
	    {
		return lhs.first < rhs.first;
	    }
	);

    for (auto &selectedPlatformDevices: selectedDeviceList)
	sort(selectedPlatformDevices.second.begin(), selectedPlatformDevices.second.begin());
}

void UserDeviceSelection::loadPlatformsAndDevices(cl::vector<Platform> &platforms, bool exactMatch)
{
    nativePlatformDevices.resize(platforms.size());
    availablePlatforms.resize(platforms.size());

    auto it = availablePlatforms.begin();
    auto itPlatformDevices = nativePlatformDevices.begin();

    for (auto &platform: platforms)
    {
	it->reset
	    (
		exactMatch ?
		    static_cast<PlatformDeviceInfo *>(new ExactPlatformDeviceInfo()) :
		    static_cast<PlatformDeviceInfo *>(new MatchPlatformDeviceInfo())
	    );

	(*it)->loadPlatform(platform);

	auto &nativeDevices = *itPlatformDevices++;

	try
	{
	    platform.getDevices(CL_DEVICE_TYPE_ALL, &nativeDevices);
	}
	catch (Error const &ex)
	{
	    if (ex.err() == CL_DEVICE_NOT_FOUND)
		nativeDevices.clear();
	    else
		throw;
	}

	(*it)->loadDevices(nativeDevices);

	it++;
    }
}

vector<bool> UserDeviceSelection::insertSelectedDevices(unsigned platformIdx, vector<char const *> const &deviceList, bool reportMissing)
{
    auto it = find_if
	(
	    selectedDeviceList.begin(),
	    selectedDeviceList.end(),
	    [platformIdx](pair<unsigned, vector<unsigned>> const &platform) -> bool
	    {
		return platform.first == platformIdx;
	    }
	);

    PlatformDeviceInfo &platformInfo = *availablePlatforms[platformIdx];
    vector<bool> deviceFound(deviceList.size());
    auto device_found_it = deviceFound.begin();

    if (deviceList.empty())
    {
	if (it == selectedDeviceList.end())
	    it = selectedDeviceList.emplace(selectedDeviceList.end(), platformIdx, vector<unsigned>());
    }
    else
	for (char const *deviceName: deviceList)
	{
	    if (deviceName)
	    {
		string deviceNameStr = deviceName;

		platformInfo.updateUserSelector(deviceNameStr);

		for (unsigned idx = 0; idx < nativePlatformDevices[platformIdx].size(); idx++)
		    if (platformInfo.checkDeviceName(idx, deviceNameStr))
		    {
			*device_found_it = true;

			if (it == selectedDeviceList.end())
			    it = selectedDeviceList.emplace(selectedDeviceList.end(), platformIdx, vector<unsigned> { idx });
			else
			    insertSelectedDevice(it->second, idx);
		    }

		if (!*device_found_it)
		{
		    if (reportMissing)
			cerr << "No such device: " << deviceName << " for platform: " << platformInfo.name() << endl;
		}
	    }
	    else
	    {
		*device_found_it = true;

		for (unsigned idx = 0; idx < nativePlatformDevices[platformIdx].size(); idx++)
		{
		    if (it == selectedDeviceList.end())
			it = selectedDeviceList.emplace(selectedDeviceList.end(), platformIdx, vector<unsigned> { idx });
		    else
			insertSelectedDevice(it->second, idx);
		}
	    }

	    device_found_it++;
	}

    return move(deviceFound);
}

bool UserDeviceSelection::selectDeviceTree(vector<pair<char const *, vector<char const *>>> const &selectionSet, bool keepOrder)
{
    bool selectionFound = true;

    for (pair<char const *, vector<char const *>> const &selectionCmd: selectionSet)
    {
	bool platformFound = false;
	vector<bool> deviceFound(selectionCmd.second.size());
	string platformName = selectionCmd.first ? selectionCmd.first : string();

	if (!availablePlatforms.empty())
	    (*availablePlatforms.begin())->updateUserSelector(platformName);

	if (selectionCmd.first || selectionCmd.second.empty())
	    for (unsigned platformIdx = 0; platformIdx < availablePlatforms.size(); platformIdx++)
		if (selectionCmd.first == nullptr || availablePlatforms[platformIdx]->checkPlatformName(platformName))
		{
		    auto found = insertSelectedDevices(platformIdx, selectionCmd.second, false);

		    for
			(
			    auto it = deviceFound.begin(), jt = found.begin();
			    it != deviceFound.end() && jt != found.end();
			    it++, jt++
			)
		    {
			*it = *it || *jt;
		    }

		    platformFound = true;
		}
		else
		    ;
	else
	{
	    vector<char const *> currentDevice(1);
	    auto it = deviceFound.begin();

	    for (char const *device: selectionCmd.second)
	    {
		*currentDevice.begin() = device;

		for (unsigned platformIdx = 0; platformIdx < availablePlatforms.size(); platformIdx++)
		{
		    bool found = insertSelectedDevices(platformIdx, currentDevice, false)[0];
		    *it = *it || found;
		}

		it++;
	    }

	    platformFound = true;
	}

	if (platformFound)
	{
	    for (unsigned i = 0; i < deviceFound.size(); i++)
	    {
		if (!deviceFound[i])
		{
		    selectionFound = false;
		    cerr << "No such OpenCL device: " << selectionCmd.second[i] << endl;
		}
	    }
	}
	else
	{
	    if (selectionCmd.first)
		cerr << "No such OpenCL platform: " << selectionCmd.first << endl;
	    else
		cerr << "No OpenCL platforms." << endl;

	    selectionFound = false;
	}
    }

    if (keepOrder)
	sortSelectedDeviceList(selectedDeviceList);

    return selectionFound;
}
