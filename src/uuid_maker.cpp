#include "uuid_maker.hpp"

linda::UuidMaker::UuidMaker() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  static std::mt19937 generator(seq);
  gen = std::make_unique<uuids::uuid_random_generator>(
      uuids::uuid_random_generator{generator});
}