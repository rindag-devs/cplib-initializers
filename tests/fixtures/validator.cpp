#include "testlib/validator.hpp"

#include <cstdint>
#include <optional>
#include <vector>

#include "cplib.hpp"

struct Input {
  std::int32_t value;

  static auto read(cplib::var::Reader &in) -> Input {
    const auto value = in.read(cplib::var::i32("value", 0, std::nullopt));
    in.inner().next_line();
    return {value};
  }
};

auto traits(const Input &input) -> std::vector<cplib::validator::Trait> {
  return {{"non-negative", [value = input.value]() -> bool { return value >= 0; }, {}}};
}

CPLIB_REGISTER_VALIDATOR_OPT(Input, traits, cplib_initializers::testlib::validator::Initializer());
