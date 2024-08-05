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

#ifndef CPLIB_INITIALIZERS_TESTLIB_INTERACTOR_HPP_
#define CPLIB_INITIALIZERS_TESTLIB_INTERACTOR_HPP_

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::testlib::interactor {

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

enum struct ExitCode {
  ACCEPTED = 0,
  WRONG_ANSWER = 1,
  INTERNAL_ERROR = 3,
  PARTIALLY_CORRECT = 7,
};

struct Reporter : cplib::interactor::Reporter {
  bool percent_mode;
  std::ostream stream;

  explicit Reporter(bool percent_mode) : percent_mode(percent_mode), stream(std::clog.rdbuf()) {}

  auto print_score(double score) -> void {
    if (percent_mode) {
      stream << std::llround(score * 100.0);
    } else {
      stream << score;
    }
  }

  [[noreturn]] auto report(const cplib::interactor::Report &report) -> void override {
    stream << std::fixed << std::setprecision(10);

    switch (report.status) {
      case cplib::interactor::Report::Status::INTERNAL_ERROR:
        stream << "FAIL ";
        break;
      case cplib::interactor::Report::Status::ACCEPTED:
        stream << "ok ";
        break;
      case cplib::interactor::Report::Status::WRONG_ANSWER:
        stream << "wrong answer ";
        break;
      case cplib::interactor::Report::Status::PARTIALLY_CORRECT:
        stream << "points ";
        break;
      default:
        stream << "FAIL invalid status\n";
        std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
    }
    if (report.status == cplib::interactor::Report::Status::PARTIALLY_CORRECT) {
      print_score(report.score);
      stream << ' ';
    }
    stream << report.message << '\n';

    switch (report.status) {
      case cplib::interactor::Report::Status::INTERNAL_ERROR:
        std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
      case cplib::interactor::Report::Status::ACCEPTED:
        std::exit(static_cast<int>(ExitCode::ACCEPTED));
      case cplib::interactor::Report::Status::WRONG_ANSWER:
        std::exit(static_cast<int>(ExitCode::WRONG_ANSWER));
      case cplib::interactor::Report::Status::PARTIALLY_CORRECT:
        std::exit(static_cast<int>(ExitCode::PARTIALLY_CORRECT));
      default:
        stream << "FAIL invalid status\n";
        std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with testlib interactor initializer\n"
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
  bool percent_mode;

  explicit Initializer(bool percent_mode) : percent_mode(percent_mode) {}

  auto init(std::string_view arg0, const std::vector<std::string> &args) -> void override {
    auto &state = this->state();

    state.reporter = std::make_unique<Reporter>(percent_mode);

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() < 1) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    set_inf_path(parsed_args.ordered[0], cplib::var::Reader::TraceLevel::NONE);
    set_from_user_fileno(fileno(stdin), cplib::var::Reader::TraceLevel::NONE);
    set_to_user_fileno(fileno(stdout));
  }
};
}  // namespace cplib_initializers::testlib::interactor

#endif
