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
 * @file checker.hpp
 *
 * QDUOJ compatible checker initializer.
 *
 * WARNING: QDUOJ does not support returning PARTIALLY_CORRECT, so all PARTIALLY_CORRECTs without
 * full score are considered WRONG_ANSWER.

 * WARNING: QDUOJ checker does not provide an answer file when running, so trying to call methods on
 * chk.ans will lead to undefined behavior.
 */

#ifndef CPLIB_INITIALIZERS_QDUOJ_CHECKER_HPP_
#define CPLIB_INITIALIZERS_QDUOJ_CHECKER_HPP_

#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::qduoj::checker {

enum struct ExitCode {
  ACCEPTED = 0,
  WRONG_ANSWER = 1,
  INTERNAL_ERROR = -1,
};

struct Reporter : cplib::checker::Reporter {
  using Report = cplib::checker::Report;
  using Status = Report::Status;

  [[noreturn]] auto report(const Report& report) -> void override {
    if (report.status == Status::INTERNAL_ERROR) {
      std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
    } else if (report.status == Status::ACCEPTED || report.score == 1.0) {
      std::exit(static_cast<int>(ExitCode::ACCEPTED));
    } else {
      std::exit(static_cast<int>(ExitCode::WRONG_ANSWER));
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> <output_file> [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with qduoj checker initializer\n"
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

    // QDUOJ's reporter does not have any ability to report error information, so use
    // PlainTextReporter to handle the error exit during init to provide clearer information.
    state.reporter = std::make_unique<cplib::checker::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() < 2) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    const auto& inf = parsed_args.ordered[0];
    const auto& ouf = parsed_args.ordered[1];

    set_inf_path(inf, cplib::var::Reader::TraceLevel::NONE);
    set_ouf_path(ouf, cplib::var::Reader::TraceLevel::NONE);

    state.reporter = std::make_unique<Reporter>();
  }
};
}  // namespace cplib_initializers::qduoj::checker

#endif
