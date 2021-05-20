#ifndef COMMON_HPP
#define COMMON_HPP

#include <loguru.hpp>
#include <fmt/core.h>

namespace Lindux {
    inline void loggerInit(
            int argc, char ** argv, const std::string& logfile = "lindux.log",
            loguru::Verbosity verbosity = loguru::Verbosity_MAX) {
        loguru::init(argc, argv);
        loguru::add_file(logfile.c_str(), loguru::Append, verbosity);
        // Only show most relevant things on stderr:
        loguru::g_stderr_verbosity = 1;
    }
}

#endif