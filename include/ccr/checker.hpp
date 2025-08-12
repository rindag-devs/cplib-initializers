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

#ifndef CPLIB_INITIALIZERS_CCR_CHECKER_HPP_
#define CPLIB_INITIALIZERS_CCR_CHECKER_HPP_

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
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

namespace cplib_initializers::ccr::checker {

namespace detail {
inline auto escape(std::string_view s) -> std::string {
  std::stringbuf buf(std::ios_base::out);
  for (char c : s) {
    if (std::isgraph(c)) {
      buf.sputc(c);
    } else {
      buf.sputc(' ');
    }
  }
  return buf.str();
}
}  // namespace detail

struct Reporter : cplib::checker::Reporter {
  using Report = cplib::checker::Report;
  using Status = Report::Status;

  std::ofstream stream;

  explicit Reporter(std::string_view report_path)
      : stream(std::string(report_path), std::ios_base::binary) {}

  auto report(const Report& report) -> int override {
    stream << std::fixed << ' ' << std::setprecision(9) << report.score << '\n';
    stream << report.status.to_string() << ": " << detail::escape(report.message) << '\n';

    if (report.status == Status::INTERNAL_ERROR) {
      return 1;
    }

    return 0;
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> <answer_file> <output_file> [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with ccr checker initializer\n"
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

    // Use PlainTextReporter to handle errors during the init process
    state.reporter = std::make_unique<cplib::checker::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() < 4) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    const auto& inf = parsed_args.ordered[0];
    const auto& ouf = parsed_args.ordered[2];
    const auto& ans = parsed_args.ordered[1];

    set_inf_path(inf, cplib::trace::Level::NONE);
    set_ouf_path(ouf, cplib::trace::Level::NONE);
    set_ans_path(ans, cplib::trace::Level::NONE);
    set_evaluator(cplib::trace::Level::NONE);

    const auto& report_path = parsed_args.ordered[3];
    state.reporter = std::make_unique<Reporter>(report_path);
  }
};
}  // namespace cplib_initializers::ccr::checker

#endif
