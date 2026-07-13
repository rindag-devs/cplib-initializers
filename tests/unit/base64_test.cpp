#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <string>
#include <vector>

#include "testlib/interactor_two_step.hpp"

namespace {
using cplib_initializers::testlib::interactor_two_step::detail::base64_encode;

auto bytes(std::string value) -> std::vector<std::uint8_t> { return {value.begin(), value.end()}; }
}  // namespace

TEST_CASE("two-step Base64 encoding handles complete and partial groups") {
  CHECK(base64_encode(bytes("")) == "");
  CHECK(base64_encode(bytes("f")) == "Zg==");
  CHECK(base64_encode(bytes("fo")) == "Zm8=");
  CHECK(base64_encode(bytes("foo")) == "Zm9v");
  CHECK(base64_encode(bytes("foob")) == "Zm9vYg==");
  CHECK(base64_encode(bytes("fooba")) == "Zm9vYmE=");
  CHECK(base64_encode(bytes("foobar")) == "Zm9vYmFy");
  CHECK(base64_encode(bytes("foobarb")) == "Zm9vYmFyYg==");
}
