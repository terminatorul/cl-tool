#ifndef PARSE_COMMAND_LINE_HH
#define PARSE_COMMAND_LINE_HH

#include <stdexcept>
#include <vector>
#include <map>
#include <string>

#include "cl-platform-info.hh"

class SyntaxError: public std::runtime_error
{
    public:
	SyntaxError();
	SyntaxError(char const *msg);
	SyntaxError(std::string const &msg);

	static void showSyntax(char const *cmd_name);
};

struct CmdLineArgs
{
    typedef std::vector<std::pair<char const *, std::vector<char const *>>> SelectionSet;

    SelectionSet listSet, probeSet;

    bool show_defaults = false;
    bool opencl_order = false;
    bool exact_match = false;
    bool has_simulation_count = false;
    unsigned long simulation_count = 300;
    void parse(char const * const argv[]);

protected:
    enum { ReadActions, ReadPlatform, ReadDevices } state = ReadActions;
    bool listAction = false, probeAction = false;
    char const *platform = nullptr;
    std::vector<char const *> devices;

    void restartParser(bool resetActions = false);
    void newCommand(SelectionSet &selectionSet, bool clearDevices);
    void flushPendingCommand();

    char const * const *parseGlobalOptions(char const * const argv[]);
    char const * const *parsePlatformActions(char const * const argv[]);
    char const * const *parsePlatformSelection(char const * const argv[]);
    char const * const *parsePlatfromDevices(char const * const argv[]);
    char const * const *parsePlatformCommand(char const * const argv[]);
    char const * const *parseCompleted(char const * const argv[]);
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
