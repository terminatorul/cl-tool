#ifndef PARSE_COMMAND_LINE_HH
#define PARSE_COMMAND_LINE_HH

#include <stdexcept>
#include <vector>
#include <string>

#include "cl-platform-info.hh"

class SyntaxError: public std::runtime_error
{
    public:
	SyntaxError();
	SyntaxError(char const *msg);
	SyntaxError(std::string const &msg);

	static void ShowSyntax(char const *cmd_name);
};

struct CmdLineArgs
{
    struct SelectionSet
    {
	bool all_platforms = false, all_devices = false;
	std::vector<PlatformDeviceSet> platforms;
    }
	list, probe;

    bool show_defaults = false;
    void parse(char const * const argv[]);

protected:
    enum { ReadActions, ReadPlatform, ReadDevices } state = ReadActions;
    bool listAction = false, probeAction = false, allDevices = false;
    char const *platform = nullptr;
    std::vector<char const *> devices;

    void startSelection(bool resetActions = false);
    void newAction(SelectionSet &selectionSet, char const *platform, bool all_devices, std::vector<char const *> const &devices);
    void newAction();
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

#endif // PARSE_COMMAND_LINE_HH
