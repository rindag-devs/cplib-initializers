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

#ifndef CPLIB_INITIALIZERS_COCI_INTERACTOR_HPP_
#define CPLIB_INITIALIZERS_COCI_INTERACTOR_HPP_

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::coci::interactor {

enum struct ExitCode {
  ACCEPTED = 0,
  WRONG_ANSWER = 1,
  INTERNAL_ERROR = 3,
  PARTIALLY_CORRECT = 7,
};

struct Reporter : cplib::interactor::Reporter {
  using Report = cplib::interactor::Report;
  using Status = Report::Status;

  auto report(const Report &report) -> int override {
    std::ostream score(std::clog.rdbuf());

    if (report.status == Status::PARTIALLY_CORRECT) {
      // ^partial ((\d+)\/(\d*[1-9]\d*))$
      score << "partial " << std::llround(report.score * 10000.0) << "/10000\n";
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
        return static_cast<int>(ExitCode::INTERNAL_ERROR);
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with coci interactor initializer\n"
                                  "https://github.com/rindag-devs/cplib-initializers/ by Rindag "
                                  "Devs, copyright(c) 2024-present\n"
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

    state.reporter = std::make_unique<Reporter>();

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
}  // namespace cplib_initializers::coci::interactor

#endif
