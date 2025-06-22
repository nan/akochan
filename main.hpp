#ifdef WINSTD
#define _WIN32_WINNT 0x0502
#endif

#include "share/include.hpp"
#include "share/types.hpp"
#include "mjai_manager.hpp"
#include "mjai_client.hpp"
#include "stats.hpp"
#include "analyze.hpp"
#include "pseudo_game.hpp"
#include "import.hpp"

#include <string> // Required for std::string

// Stores the directory of the executable
extern std::string executable_path_str;

// Function to get the directory of the executable
std::string get_executable_dir();