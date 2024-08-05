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

#ifndef CPLIB_INITIALIZERS_SPOJ_CHECKER_HPP_
#define CPLIB_INITIALIZERS_SPOJ_CHECKER_HPP_

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
#include "spoj.h"
#include "spoj/spoj_interactive.h"

namespace cplib_initializers::spoj::checker {

struct Reporter : cplib::checker::Reporter {
  using Report = cplib::checker::Report;
  using Status = Report::Status;

  auto report(const Report& report) -> int override {
    std::unique_ptr<std::streambuf> message_buf, score_buf;
    std::ostream message(nullptr), score(nullptr);
    cplib::var::detail::make_ostream_by_fileno(SPOJ_SCORE_FD, score_buf, score);
    cplib::var::detail::make_ostream_by_fileno(SPOJ_P_INFO_FD, message_buf, message);

    if (report.status == Status::PARTIALLY_CORRECT) {
      score << std::llround(report.score * 100.0) << '\n';
    }

    message << report.status.to_string() << ".\n";

    if (report.status != Status::ACCEPTED || !report.message.empty()) {
      message << report.message << '\n';
    }

    if (trace_stack_.has_value()) {
      message << "\nReader trace stack (most recent variable last):\n";
      for (const auto& line : trace_stack_->to_plain_text_lines()) {
        message << "  " << line << '\n';
      }
    }

    switch (report.status) {
      case Status::INTERNAL_ERROR:
        return SPOJ_RV_IE;
      case Status::WRONG_ANSWER:
        return SPOJ_RV_NEGATIVE;
      case Status::ACCEPTED:
      case Status::PARTIALLY_CORRECT:
        return SPOJ_RV_POSITIVE;
      default:
        message << "FAIL invalid status\n";
        return SPOJ_RV_IE;
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "[...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with spoj checker initializer\n"
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

    spoj_init();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    set_inf_fileno(SPOJ_P_IN_FD, cplib::var::Reader::TraceLevel::STACK_ONLY);
    set_ouf_fileno(SPOJ_T_OUT_FD, cplib::var::Reader::TraceLevel::STACK_ONLY);
    set_ans_fileno(SPOJ_P_OUT_FD, cplib::var::Reader::TraceLevel::STACK_ONLY);
  }
};

}  // namespace cplib_initializers::spoj::checker

#endif
