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
 * Nowcoder compatible checker initializer.
 *
 * WARNING: Nowcoder does not support returning PARTIALLY_CORRECT, so all PARTIALLY_CORRECTs without
 * full score are considered WRONG_ANSWER.
 */

#ifndef CPLIB_INITIALIZERS_NOWCODER_CHECKER_HPP_
#define CPLIB_INITIALIZERS_NOWCODER_CHECKER_HPP_

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"
#include "trace.hpp"

namespace cplib_initializers::nowcoder::checker {

constexpr std::string_view FILENAME_INF = "input";
constexpr std::string_view FILENAME_OUF = "user_output";
constexpr std::string_view FILENAME_ANS = "output";

enum struct ExitCode : std::uint8_t {
  ACCEPTED = 0,
  ERROR = 1,
};

struct Reporter : cplib::checker::Reporter {
  using Report = cplib::checker::Report;
  using Status = Report::Status;

  auto report(const Report& report) -> int override {
    if (report.status == Status::ACCEPTED || report.score == 1.0) {
      return static_cast<int>(ExitCode::ACCEPTED);
    }

    return static_cast<int>(ExitCode::ERROR);
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "[...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with nowcoder checker initializer\n"
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

    // Nowcoder's reporter does not have any ability to report error information, so use
    // PlainTextReporter to handle the error exit during init to provide clearer information.
    state.reporter = std::make_unique<cplib::checker::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    set_inf_path(FILENAME_INF, cplib::trace::Level::NONE);
    set_ouf_path(FILENAME_OUF, cplib::trace::Level::NONE);
    set_ans_path(FILENAME_ANS, cplib::trace::Level::NONE);
    set_evaluator(cplib::trace::Level::NONE);

    state.reporter = std::make_unique<Reporter>();
  }
};
}  // namespace cplib_initializers::nowcoder::checker

#endif
