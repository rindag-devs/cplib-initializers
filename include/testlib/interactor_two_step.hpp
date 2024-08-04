/*
 * This file is part of CPLibInitializers.
 *
 * CPLibInitializers is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * CPLibInitializers is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * CPLibInitializers. If not, see <https://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::testlib::interactor_two_step {

namespace detail {
constexpr std::array<char, 64> encode_table{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

inline auto encode_triplet(std::uint8_t a, std::uint8_t b, std::uint8_t c) -> std::array<char, 4> {
  const std::uint32_t concat_bits = (a << 16) | (b << 8) | c;

  const auto b64_char1 = encode_table[(concat_bits >> 18) & 0b0011'1111];
  const auto b64_char2 = encode_table[(concat_bits >> 12) & 0b0011'1111];
  const auto b64_char3 = encode_table[(concat_bits >> 6) & 0b0011'1111];
  const auto b64_char4 = encode_table[concat_bits & 0b0011'1111];
  return {b64_char1, b64_char2, b64_char3, b64_char4};
}

inline auto base64_encode(const std::vector<std::uint8_t> &input) -> std::string {
  const auto size = input.size();
  const auto full_triples = size / 3;

  std::string output;
  output.reserve((full_triples + 2) * 4);

  for (std::size_t i = 0; i < full_triples; ++i) {
    const auto triplet =
        std::vector<std::uint8_t>(input.begin() + static_cast<std::ptrdiff_t>(i) * 3,
                                  input.begin() + static_cast<std::ptrdiff_t>(i) * 3 + 4);
    const auto base64_chars = encode_triplet(triplet[0], triplet[1], triplet[2]);
    std::copy(begin(base64_chars), end(base64_chars), back_inserter(output));
  }

  if (const auto remaining_chars = size - full_triples * 3; remaining_chars == 2) {
    const auto last_two = std::vector<std::uint8_t>(input.end() - 2, input.end());
    const auto base64_chars = encode_triplet(last_two[0], last_two[1], 0x00);

    output.push_back(base64_chars[0]);
    output.push_back(base64_chars[1]);
    output.push_back(base64_chars[2]);
    output.push_back('=');
  } else if (remaining_chars == 1) {
    auto const base64_chars = encode_triplet(input.back(), 0x00, 0x00);

    output.push_back(base64_chars[0]);
    output.push_back(base64_chars[1]);
    output.push_back('=');
    output.push_back('=');
  }

  return output;
}
}  // namespace detail

enum class ExitCode {
  ACCEPTED = 0,
  WRONG_ANSWER = 1,
  INTERNAL_ERROR = 3,
  PARTIALLY_CORRECT = 7,
};

struct Reporter : cplib::interactor::Reporter {
  std::ofstream stream;

  explicit Reporter(std::string_view output_file)
      : stream(output_file.data(), std::ios_base::binary) {}

  [[noreturn]] auto report(const cplib::interactor::Report &report) -> void override {
    auto bytes = std::vector<std::uint8_t>(report.message.begin(), report.message.end());
    stream << std::fixed << std::setprecision(10);
    stream << static_cast<int>(report.status) << '\n'
           << report.score << '\n'
           << detail::base64_encode(bytes) << '\n';
    std::exit(0);
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> <report_file> [...]";
inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with testlib two-step interactor initializer\n"
                                  "https://github.com/rindag-devs/cplib-initializers/ by Rindag "
                                  "Devs, copyright(c) 2024\n"
                                  "\n"
                                  "Usage:\n"
                                  "  %s %s\n",
                                  program_name.data(), ARGS_USAGE.data());
  cplib::panic(msg);
}
}  // namespace detail

struct Initializer : cplib::interactor::Initializer {
  auto init(std::string_view arg0, const std::vector<std::string> &args) -> void override {
    auto &state = this->state();

    // Use PlainTextReporter to handle errors during the init process
    state.reporter = std::make_unique<cplib::interactor::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() < 2) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    set_inf_path(parsed_args.ordered[0], cplib::var::Reader::TraceLevel::NONE);
    set_from_user_fileno(fileno(stdin), cplib::var::Reader::TraceLevel::NONE);
    set_to_user_fileno(fileno(stdout));

    const auto &report_file = parsed_args.ordered[1];

    state.reporter = std::make_unique<Reporter>(report_file);
  }
};
}  // namespace cplib_initializers::testlib::interactor_two_step
