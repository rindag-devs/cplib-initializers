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

#ifndef CPLIB_INITIALIZERS_CMS_CHECKER_HPP_
#define CPLIB_INITIALIZERS_CMS_CHECKER_HPP_

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

namespace cplib_initializers::cms::checker {

struct Reporter : cplib::checker::Reporter {
  using Report = cplib::checker::Report;
  using Status = Report::Status;

  [[noreturn]] auto report(const Report &report) -> void override {
    std::ostream score_stream(std::cout.rdbuf());
    std::ostream status_stream(std::clog.rdbuf());

    score_stream << std::fixed << std::setprecision(9) << report.score << '\n';

    switch (report.status) {
      case Status::INTERNAL_ERROR:
        status_stream << "FAIL " << report.message << '\n';
        std::exit(1);
      case Status::ACCEPTED:
        status_stream << (report.message.empty() ? "translate:success\n" : report.message);
        break;
      case Status::WRONG_ANSWER:
        status_stream << (report.message.empty() ? "translate:wrong\n" : report.message);
        break;
      case Status::PARTIALLY_CORRECT:
        status_stream << (report.message.empty() ? "translate:partial\n" : report.message);
        break;
      default:
        status_stream << "FAIL invalid status\n";
        std::exit(1);
    }

    std::exit(0);
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> <answer_file> <output_file> [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with cms checker initializer\n"
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

    state.reporter = std::make_unique<Reporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() < 3) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    const auto &inf = parsed_args.ordered[0];
    const auto &ouf = parsed_args.ordered[2];
    const auto &ans = parsed_args.ordered[1];

    set_inf_path(inf, cplib::var::Reader::TraceLevel::NONE);
    set_ouf_path(ouf, cplib::var::Reader::TraceLevel::NONE);
    set_ans_path(ans, cplib::var::Reader::TraceLevel::NONE);
  }
};
}  // namespace cplib_initializers::cms::checker

#endif
