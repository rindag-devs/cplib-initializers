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

/**
 * @file checker_two_step.cpp
 *
 * This is NOT a checker initializer, this is a checker executable for use with
 * interactor_two_step.hpp.
 *
 * See two_step_interaction_help.md for details.
 */

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"
#include "testlib/checker.hpp"

inline auto xml_escape(std::string_view s) -> std::string {
  std::stringbuf buf(std::ios_base::out);
  for (auto c : s) {
    switch (c) {
      case '&':
        buf.sputn("&amp;", 5);
        break;
      case '<':
        buf.sputn("&lt;", 4);
        break;
      case '>':
        buf.sputn("&gt;", 4);
        break;
      case '\"':
        buf.sputn("&quot;", 6);
        break;
      default:
        if (('\x00' <= c && c <= '\x1f') || c == '\x7f') {
          buf.sputc('.');
        } else {
          buf.sputc(c);
        }
        break;
    }
  }
  return buf.str();
}

constexpr std::array<std::uint8_t, 256> decode_table{
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x3E, 0x64, 0x64, 0x64, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
    0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64};

inline auto is_valid_base64_char(char c) -> bool {
  const auto decode_byte = decode_table[static_cast<unsigned char>(c)];
  return decode_byte != 0x64;
}

inline auto is_valid_base64_str(std::string_view encoded_str) -> bool {
  if ((encoded_str.size() % 4) == 1) {
    return false;
  }

  if (!std::all_of(begin(encoded_str), end(encoded_str) - 2,
                   [](char c) { return is_valid_base64_char(c); })) {
    return false;
  }

  const auto last = rbegin(encoded_str);
  if (!is_valid_base64_char(*next(last))) {
    return (*next(last) == '=') && (*last == '=');
  }

  return is_valid_base64_char(*last) || (*last == '=');
}

inline auto decode_quad(char a, char b, char c, char d) -> std::array<std::uint8_t, 3> {
  const std::uint32_t concat_bytes =
      (static_cast<std::uint32_t>(decode_table[static_cast<unsigned char>(a)]) << 18) |
      (static_cast<std::uint32_t>(decode_table[static_cast<unsigned char>(b)]) << 12) |
      (static_cast<std::uint32_t>(decode_table[static_cast<unsigned char>(c)]) << 6) |
      static_cast<std::uint32_t>(decode_table[static_cast<unsigned char>(d)]);

  const std::uint8_t byte1 = (concat_bytes >> 16) & 0b1111'1111;
  const std::uint8_t byte2 = (concat_bytes >> 8) & 0b1111'1111;
  const std::uint8_t byte3 = concat_bytes & 0b1111'1111;
  return {byte1, byte2, byte3};
}

inline auto base64_decode(std::string_view encoded_str)
    -> std::optional<std::vector<std::uint8_t>> {
  if (encoded_str.empty()) {
    return std::vector<std::uint8_t>{};
  }

  if (!is_valid_base64_str(encoded_str)) {
    return std::nullopt;
  }

  auto const unpadded_encoded_str = encoded_str.substr(0, encoded_str.find_first_of('='));
  auto const full_quadruples = unpadded_encoded_str.size() / 4;

  std::vector<std::uint8_t> decoded_bytes;
  decoded_bytes.reserve(((full_quadruples + 2) * 3) / 4);

  for (std::size_t i = 0; i < full_quadruples; ++i) {
    auto const quad = unpadded_encoded_str.substr(i * 4, 4);
    auto const bytes = decode_quad(quad[0], quad[1], quad[2], quad[3]);
    std::ranges::copy(bytes, std::back_inserter(decoded_bytes));
  }

  if (auto const last_quad = unpadded_encoded_str.substr(full_quadruples * 4);
      last_quad.size() == 0) {
    return decoded_bytes;
  } else if ((last_quad.size() == 2) || (last_quad[2] == '=')) {
    auto const bytes = decode_quad(last_quad[0], last_quad[1], 'A', 'A');
    decoded_bytes.push_back(bytes[0]);
  } else {
    auto const bytes = decode_quad(last_quad[0], last_quad[1], last_quad[2], 'A');
    std::copy_n(begin(bytes), 2, back_inserter(decoded_bytes));
  }

  return decoded_bytes;
}

enum struct ExitCode : std::uint8_t {
  ACCEPTED = 0,
  WRONG_ANSWER = 1,
  INTERNAL_ERROR = 3,
  PARTIALLY_CORRECT = 7,
};

struct Input {
  static auto read(cplib::var::Reader&) -> Input { return {}; }
};

struct Output {
  int status;
  double score;
  std::string message;

  static auto read(cplib::var::Reader& in, const Input&) -> Output {
    auto status = in.read(cplib::var::i32("status"));
    auto score = in.read(cplib::var::f64("score"));

    // Check if there's an optional message
    std::string encoded_message_str;
    if (!in.inner().seek_eof()) {
      encoded_message_str = in.read(cplib::var::String("encoded_message"));
      auto bytes = base64_decode(encoded_message_str);
      if (!bytes.has_value()) {
        in.fail(cplib::format("Invalid Base64 encoding for message: {}", encoded_message_str));
      }
      return {
          .status = status, .score = score, .message = std::string(bytes->begin(), bytes->end())};
    } else {
      return {.status = status, .score = score, .message = ""};
    }
  }

  static auto evaluate(cplib::evaluate::Evaluator& ev, const Output& pans, const Output&,
                       const Input&) -> cplib::evaluate::Result {
    cplib::interactor::Report::Status interactor_status =
        static_cast<cplib::interactor::Report::Status::Value>(pans.status);

    cplib::evaluate::Result::Status status;

    switch (interactor_status) {
      case cplib::interactor::Report::Status::INTERNAL_ERROR:
        ev.fail(pans.message);
        break;
      case cplib::interactor::Report::Status::ACCEPTED:
        status = cplib::evaluate::Result::Status::ACCEPTED;
        break;
      case cplib::interactor::Report::Status::WRONG_ANSWER:
        status = cplib::evaluate::Result::Status::WRONG_ANSWER;
        break;
      case cplib::interactor::Report::Status::PARTIALLY_CORRECT:
        status = cplib::evaluate::Result::Status::PARTIALLY_CORRECT;
        break;
      default:
        ev.fail(cplib::format("Unknown interactor report status: {}", pans.status));
        break;
    }

    return {status, pans.score, pans.message};
  }
};

CPLIB_REGISTER_CHECKER_OPT(chk, Input, Output,
                           cplib_initializers::testlib::checker::Initializer(true));
