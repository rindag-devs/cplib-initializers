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

struct CmsReporter : cplib::checker::Reporter {
  [[noreturn]] auto report(const cplib::checker::Report& report) -> void override {
    std::ostream score_stream(std::cout.rdbuf());
    std::ostream status_stream(std::clog.rdbuf());

    score_stream << std::fixed << std::setprecision(9) << report.score << '\n';

    switch (report.status) {
      case cplib::checker::Report::Status::INTERNAL_ERROR:
        status_stream << "FAIL " << report.status << '\n';
        std::exit(1);
      case cplib::checker::Report::Status::ACCEPTED:
        status_stream << "translate:success\n";
        break;
      case cplib::checker::Report::Status::WRONG_ANSWER:
        status_stream << "translate:wrong\n";
        break;
      case cplib::checker::Report::Status::PARTIALLY_CORRECT:
        status_stream << "translate:partial\n";
        break;
      default:
        status_stream << "FAIL invalid status\n";
        std::exit(1);
    }
    std::exit(0);
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> <answer_file> <output_file>";
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

struct CmsInitializer : cplib::checker::Initializer {
  auto init(std::string_view arg0, const std::vector<std::string>& args) -> void override {
    auto& state = this->state();

    state.reporter = std::make_unique<CmsReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() != 3) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    const auto& inf = parsed_args.ordered[0];
    const auto& ouf = parsed_args.ordered[2];
    const auto& ans = parsed_args.ordered[1];

    set_inf_path(inf, cplib::var::Reader::TraceLevel::NONE);
    set_ouf_path(ouf, cplib::var::Reader::TraceLevel::NONE);
    set_ans_path(ans, cplib::var::Reader::TraceLevel::NONE);
  }
};
}  // namespace cplib_initializers::cms::checker
