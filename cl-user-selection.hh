#if !defined(CL_TOOL_HH)
#define CL_TOOL_HH

#include <cstddef>
#include <type_traits>
#include <memory>
#include <vector>
#include <utility>
#include <array>
#include <string>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

class UserDeviceSelection
{
public:
    class PlatformDeviceInfo
    {
    public:
	virtual std::string const &name() const = 0;

	virtual void loadPlatform(cl::Platform &platform) = 0;
	virtual void loadDevices(cl::vector<cl::Device> &devices) = 0;
	virtual bool checkDeviceName(unsigned deviceIdx, std::string const &deviceName) = 0;
	virtual bool checkPlatformName(std::string const &name) = 0;
	virtual void updateUserSelector(std::string &selector) = 0;
    };

protected:
    std::vector<std::unique_ptr<PlatformDeviceInfo>>
	availablePlatforms;

    std::vector<cl::vector<cl::Device>>
	nativePlatformDevices;

    std::vector<std::pair<unsigned, std::vector<unsigned>>>
	selectedDeviceList;

    std::vector<bool>
	insertSelectedDevices(unsigned platformIdx, std::vector<char const *> const &deviceList, bool outputMissing);

public:
    void loadPlatformsAndDevices(cl::vector<cl::Platform> &platforms, bool exactMatch);
    void clearSelection();
    bool selectDeviceTree(std::vector<std::pair<char const *, std::vector<char const *>>> const &selectionSet, bool openclOrder);

    decltype(selectedDeviceList) &selectedDevices();
    cl::vector<cl::Device> &platformDevices(unsigned platformIdx);
    std::size_t totalDeviceCount(void) const;
};


inline cl::vector<cl::Device> &UserDeviceSelection::platformDevices(unsigned platformIdx)
{
    return nativePlatformDevices[platformIdx];
}

inline std::size_t UserDeviceSelection::totalDeviceCount(void) const
{
    std::size_t count = 0u;

    for (auto const &platform: nativePlatformDevices)
	count += platform.size();

    return count;
}

inline void UserDeviceSelection::clearSelection()
{
    selectedDeviceList.clear();
}

inline decltype(UserDeviceSelection::selectedDeviceList) &UserDeviceSelection::selectedDevices()
{
    return selectedDeviceList;
}

#endif /* !defined(CL_TOOL_HH) */
