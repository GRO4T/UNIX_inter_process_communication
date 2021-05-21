#pragma once
#include <uuid.h>

#include <random>

namespace linda {
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
    UuidMaker();
    ~UuidMaker() = default;
    UuidMaker(const UuidMaker&) = delete;
    UuidMaker& operator=(const UuidMaker&) = delete;

    std::unique_ptr<uuids::uuid_random_generator> gen;
};
}  // namespace linda