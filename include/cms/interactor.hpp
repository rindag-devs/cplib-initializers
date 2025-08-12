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

#ifndef CPLIB_INITIALIZERS_CMS_INTERACTOR_HPP_
#define CPLIB_INITIALIZERS_CMS_INTERACTOR_HPP_

#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::cms::interactor {

constexpr std::string_view FILENAME_INF = "input.txt";

struct Reporter : cplib::interactor::Reporter {
  using Report = cplib::interactor::Report;
  using Status = Report::Status;

  auto report(const Report &report) -> int override {
    std::ostream score_stream(std::cout.rdbuf());
    std::ostream status_stream(std::cerr.rdbuf());

    score_stream << std::fixed << std::setprecision(9) << report.score << '\n';

    switch (report.status) {
      case Status::INTERNAL_ERROR:
        status_stream << "FAIL " << report.message << '\n';
        return 1;
      case Status::ACCEPTED:
        status_stream << (report.message.empty() ? "translate:success" : report.message) << '\n';
        break;
      case Status::WRONG_ANSWER:
        status_stream << (report.message.empty() ? "translate:wrong" : report.message) << '\n';
        break;
      case Status::PARTIALLY_CORRECT:
        status_stream << (report.message.empty() ? "translate:partial" : report.message) << '\n';
        break;
      default:
        status_stream << "FAIL invalid status\n";
        return 1;
    }

    return 0;
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<from_user_file> <to_user_file> [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with cms interactor initializer\n"
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

    if (parsed_args.ordered.size() < 2) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    const auto &from_user_file = parsed_args.ordered[0];
    const auto &to_user_file = parsed_args.ordered[1];

    // When the sandbox opens the other endpoints of these fifos to redirect
    // them to to stdin/out it does so first for stdin and then for stdout.
    // We must match that order as otherwise we would deadlock.

    set_to_user_path(to_user_file);
    set_from_user_path(from_user_file, cplib::trace::Level::NONE);

    set_inf_path(FILENAME_INF, cplib::trace::Level::NONE);
  }
};
}  // namespace cplib_initializers::cms::interactor

#endif
