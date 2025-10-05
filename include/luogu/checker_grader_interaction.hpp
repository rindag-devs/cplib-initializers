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

#ifndef CPLIB_INITIALIZERS_LUOGU_CHECKER_GRADER_INTERACTION_HPP_
#define CPLIB_INITIALIZERS_LUOGU_CHECKER_GRADER_INTERACTION_HPP_

#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::luogu::checker_grader_interaction {

namespace detail {
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
}  // namespace detail

enum struct ExitCode : std::uint8_t {
  ACCEPTED = 0,
  WRONG_ANSWER = 1,
  INTERNAL_ERROR = 3,
  PARTIALLY_CORRECT = 7,
};

struct Reporter : cplib::checker::Reporter {
  using Report = cplib::checker::Report;
  using Status = Report::Status;

  bool appes_mode;
  bool print_status{};
  std::ostream stream;
  std::unique_ptr<std::streambuf> buf;

  explicit Reporter(std::optional<std::string> report_file, bool appes_mode)
      : appes_mode(appes_mode), stream(std::ostream(nullptr)), buf(nullptr) {
    if (report_file.has_value()) {
      cplib::io::detail::make_ostream_by_path(*report_file, buf, stream);
    } else {
      cplib::io::detail::make_ostream_by_fileno(fileno(stderr), buf, stream);
      print_status = true;
    }
  }

  auto print_score(double score) -> void { stream << score; }

  auto report(const Report &report) -> int override {
    stream << std::fixed << std::setprecision(9);

    if (appes_mode) {
      stream << R"(<?xml version="1.0" encoding="utf-8"?><result outcome = ")";
      switch (report.status) {
        case Status::INTERNAL_ERROR:
          stream << "fail";
          break;
        case Status::ACCEPTED:
          stream << "accepted";
          break;
        case Status::WRONG_ANSWER:
          stream << "wrong-answer";
          break;
        case Status::PARTIALLY_CORRECT:
          stream << "points\" points = \"";
          print_score(report.score);
          break;
        default:
          stream << "FAIL invalid status\n";
          return static_cast<int>(ExitCode::INTERNAL_ERROR);
      }
      stream << "\">";
      if (report.status == Status::PARTIALLY_CORRECT) {
        print_score(report.score);
        stream << ' ';
      }
      stream << detail::xml_escape(report.message) << "</result>\n";
    } else {
      if (print_status) {
        switch (report.status) {
          case Status::INTERNAL_ERROR:
            stream << "FAIL ";
            break;
          case Status::ACCEPTED:
            stream << "ok ";
            break;
          case Status::WRONG_ANSWER:
            stream << "wrong answer ";
            break;
          case Status::PARTIALLY_CORRECT:
            stream << "points ";
            break;
          default:
            stream << "FAIL invalid status\n";
            return static_cast<int>(ExitCode::INTERNAL_ERROR);
        }
      }
      if (report.status == Status::PARTIALLY_CORRECT) {
        print_score(report.score);
        stream << ' ';
      }
      stream << report.message << '\n';
    }

    switch (report.status) {
      case Status::INTERNAL_ERROR:
        return static_cast<int>(ExitCode::INTERNAL_ERROR);
      case Status::ACCEPTED:
        return static_cast<int>(ExitCode::ACCEPTED);
      case Status::WRONG_ANSWER:
        return static_cast<int>(ExitCode::WRONG_ANSWER);
      case Status::PARTIALLY_CORRECT:
        return static_cast<int>(ExitCode::PARTIALLY_CORRECT);
      default:
        stream << "FAIL invalid status\n";
        return static_cast<int>(ExitCode::INTERNAL_ERROR);
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE =
    "<input_file> <dummy> <answer_file> [<report_file> [-appes [...]]]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with luogu grader-interaction checker initializer\n"
                                  "https://github.com/rindag-devs/cplib-initializers/ by Rindag "
                                  "Devs, copyright(c) 2024-present\n"
                                  "\n"
                                  "Usage:\n"
                                  "  %s %s\n",
                                  program_name.data(), ARGS_USAGE.data());
  cplib::panic(msg);
}
}  // namespace detail

struct Initializer : cplib::checker::Initializer {
  auto init(std::string_view arg0, const std::vector<std::string> &args) -> void override {
    auto &state = this->state();

    // Use PlainTextReporter to handle errors during the init process
    state.reporter = std::make_unique<cplib::checker::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() < 3) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    // Pipe all content in input file to stdout.
    {
      auto file = std::fopen(parsed_args.ordered[0].c_str(), "rb");
      if (file == nullptr) {
        cplib::panic(std::string("Error opening input file: ") + std::strerror(errno));
      }
      if (std::fseek(file, 0, SEEK_END) != 0) {
        cplib::panic(std::string("Error seeking to end of file: ") + std::strerror(errno));
      }
      auto input_file_size = std::ftell(file);
      if (input_file_size == -1) {
        cplib::panic(std::string("Error getting input file size: ") + std::strerror(errno));
      }
      if (std::fseek(file, 0, SEEK_SET) != 0) {
        cplib::panic(std::string("Error seeking to start of file: ") + std::strerror(errno));
      }
      std::vector<char> buffer(input_file_size);
      if (std::fread(buffer.data(), 1, input_file_size, file) !=
          static_cast<std::size_t>(input_file_size)) {
        cplib::panic("Failed to read the entire input file");
      }
      std::fclose(file);
      if (std::fwrite(buffer.data(), 1, input_file_size, stdout) !=
          static_cast<std::size_t>(input_file_size)) {
        cplib::panic("Failed to write the input file to stdout");
      }
      std::fflush(stdout);
    }

    set_inf_path(parsed_args.ordered[0], cplib::trace::Level::NONE);
    set_ouf_fileno(fileno(stdin), cplib::trace::Level::NONE);
    set_ans_path(parsed_args.ordered[2], cplib::trace::Level::NONE);
    set_evaluator(cplib::trace::Level::STACK_ONLY);

    std::optional<std::string> report_file = std::nullopt;
    if (parsed_args.ordered.size() >= 4) report_file = parsed_args.ordered[3];

    // Some platforms may pass some platform-specific command line arguments to testlib, ignore them

    bool appes_mode = false;

    for (size_t i = 4; i < parsed_args.ordered.size(); ++i) {
      if (parsed_args.ordered[i] == "-appes") {
        appes_mode = true;
      }
    }

    state.reporter = std::make_unique<Reporter>(report_file, appes_mode);
  }
};
}  // namespace cplib_initializers::luogu::checker_grader_interaction

#endif
