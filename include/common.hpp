#pragma once

#include <fmt/core.h>
#include <uuid.h>

#include <loguru.hpp>
#include <random>

namespace linda {

inline void loggerInit(int argc, char** argv,
                       const std::string& logfile = "lindux.log",
                       loguru::Verbosity verbosity = loguru::Verbosity_MAX) {
    loguru::init(argc, argv);
    loguru::add_file(logfile.c_str(), loguru::Append, verbosity);
    // Only show most relevant things on stderr:
    loguru::g_stderr_verbosity = 1;
}

class UuidMaker {
public:
    /**
     * Provides reference to the singleton instance of UuidMaker.
     */
    static UuidMaker& Instance() {
        static UuidMaker uuid_maker;
        return uuid_maker;
    }

    /**
     * Generates random UUID using default settings.
     */
    uuids::uuid GenUuid() { return (*gen)(); }

private:
    UuidMaker() {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size>{};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        static std::mt19937 generator(seq);
        gen = std::make_unique<uuids::uuid_random_generator>(
            uuids::uuid_random_generator{generator});
    }
    ~UuidMaker() = default;
    UuidMaker(const UuidMaker&) = delete;
    UuidMaker& operator=(const UuidMaker&) = delete;

    std::unique_ptr<uuids::uuid_random_generator> gen;
};
}  // namespace linda
