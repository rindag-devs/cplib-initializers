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

#ifndef CPLIB_INITIALIZERS_SYZOJ_CHECKER_HPP_
#define CPLIB_INITIALIZERS_SYZOJ_CHECKER_HPP_

#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::syzoj::checker {

constexpr std::string_view FILENAME_INF = "input";
constexpr std::string_view FILENAME_OUF = "user_out";
constexpr std::string_view FILENAME_ANS = "answer";

struct Reporter : cplib::checker::Reporter {
  using Report = cplib::checker::Report;
  using Status = Report::Status;

  auto report(const Report& report) -> int override {
    std::ostream score(std::cout.rdbuf());
    std::ostream message(std::clog.rdbuf());

    score << std::fixed << std::setprecision(9) << report.score * 100.0;

    message << std::fixed << std::setprecision(2) << report.status.to_string() << ", scores "
            << report.score * 100.0 << " of 100.\n";

    if (report.status != Status::ACCEPTED || !report.message.empty()) {
      message << report.message << '\n';
    }

    if (!trace_stacks_.empty()) {
      message << "\nReader trace stacks (most recent variable last):";
      for (const auto& [_, stack] : trace_stacks_) {
        for (const auto& line : stack.to_plain_text_lines()) {
          message << '\n' << "  " << line;
        }
        message << '\n';
      }
    }

    if (report.status == Status::INTERNAL_ERROR) {
      return 1;
    }

    return 0;
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "[...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with syzoj checker initializer\n"
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
  auto init(std::string_view arg0, const std::vector<std::string>& args) -> void override {
    auto& state = this->state();

    state.reporter = std::make_unique<Reporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    set_inf_path(FILENAME_INF, cplib::var::Reader::TraceLevel::STACK_ONLY);
    set_ouf_path(FILENAME_OUF, cplib::var::Reader::TraceLevel::STACK_ONLY);
    set_ans_path(FILENAME_ANS, cplib::var::Reader::TraceLevel::STACK_ONLY);
  }
};

}  // namespace cplib_initializers::syzoj::checker

#endif
