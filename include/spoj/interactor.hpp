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

#ifndef CPLIB_INITIALIZERS_SPOJ_INTERACTOR_HPP_
#define CPLIB_INITIALIZERS_SPOJ_INTERACTOR_HPP_

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <streambuf>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"
#include "spoj/spoj_interactive.h"

namespace cplib_initializers::spoj::interactor {

struct Reporter : cplib::interactor::Reporter {
  using Report = cplib::interactor::Report;
  using Status = Report::Status;

  auto report(const Report& report) -> int override {
    std::unique_ptr<std::streambuf> message_buf, score_buf;
    std::ostream message(nullptr), score(nullptr);
    cplib::io::detail::make_ostream_by_fileno(SPOJ_SCORE_FD, score_buf, score);
    cplib::io::detail::make_ostream_by_fileno(SPOJ_P_INFO_FD, message_buf, message);

    if (report.status == Status::PARTIALLY_CORRECT) {
      score << std::llround(report.score * 100.0) << '\n';
    }

    message << report.status.to_string() << ".\n";

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

    switch (report.status) {
      case Status::INTERNAL_ERROR:
        return SPOJ_RV_SE;
      case Status::WRONG_ANSWER:
        return SPOJ_RV_WA;
      case Status::ACCEPTED:
      case Status::PARTIALLY_CORRECT:
        return SPOJ_RV_AC;
      default:
        message << "FAIL invalid status\n";
        return SPOJ_RV_SE;
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "[...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with spoj interactor initializer\n"
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
  auto init(std::string_view arg0, const std::vector<std::string>& args) -> void override {
    auto& state = this->state();

    state.reporter = std::make_unique<Reporter>();

    spoj_init();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    set_inf_fileno(SPOJ_P_IN_FD, cplib::var::Reader::TraceLevel::STACK_ONLY);
    set_from_user_fileno(SPOJ_T_OUT_FD, cplib::var::Reader::TraceLevel::STACK_ONLY);
    set_to_user_fileno(SPOJ_FOR_TESTED_FD);
  }
};

}  // namespace cplib_initializers::spoj::interactor

#endif
