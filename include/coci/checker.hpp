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

#ifndef CPLIB_INITIALIZERS_COCI_CHECKER_HPP_
#define CPLIB_INITIALIZERS_COCI_CHECKER_HPP_

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::coci::checker {

enum struct ExitCode {
  ACCEPTED = 0,
  WRONG_ANSWER = 1,
  INTERNAL_ERROR = 3,
  PARTIALLY_CORRECT = 7,
};

struct Reporter : cplib::checker::Reporter {
  using Report = cplib::checker::Report;
  using Status = Report::Status;

  [[noreturn]] auto report(const Report &report) -> void override {
    std::ostream score(std::clog.rdbuf());
    std::ostream message(std::cout.rdbuf());

    if (report.status == Status::PARTIALLY_CORRECT) {
      // ^partial ((\d+)\/(\d*[1-9]\d*))$
      score << "partial " << std::llround(report.score * 10000.0) << "/10000\n";
    }

    message << std::fixed << std::setprecision(2) << report.status.to_string() << ", scores "
            << report.score * 100.0 << " of 100.\n";

    if (report.status != Status::ACCEPTED || !report.message.empty()) {
      message << report.message << '\n';
    }

    if (trace_stack_.has_value()) {
      message << "\nReader trace stack (most recent variable last):\n";
      for (const auto &line : trace_stack_->to_plain_text_lines()) {
        message << "  " << line << '\n';
      }
    }

    switch (report.status) {
      case Status::INTERNAL_ERROR:
        std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
      case Status::ACCEPTED:
        std::exit(static_cast<int>(ExitCode::ACCEPTED));
      case Status::WRONG_ANSWER:
        std::exit(static_cast<int>(ExitCode::WRONG_ANSWER));
      case Status::PARTIALLY_CORRECT:
        std::exit(static_cast<int>(ExitCode::PARTIALLY_CORRECT));
      default:
        std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> <output_file> <answer_file> [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with coci checker initializer\n"
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

    set_inf_path(parsed_args.ordered[0], cplib::var::Reader::TraceLevel::STACK_ONLY);
    set_ouf_path(parsed_args.ordered[1], cplib::var::Reader::TraceLevel::STACK_ONLY);
    set_ans_path(parsed_args.ordered[2], cplib::var::Reader::TraceLevel::STACK_ONLY);
  }
};
}  // namespace cplib_initializers::coci::checker

#endif
