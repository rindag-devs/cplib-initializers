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

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::lemon::checker {

struct LemonReporter : cplib::checker::Reporter {
  std::int32_t max_score;
  std::ofstream score_stream, report_stream;

  explicit LemonReporter(std::int32_t max_score, std::string_view score_path,
                         std::string_view report_path)
      : max_score(max_score),
        score_stream(score_path.data(), std::ios_base::binary),
        report_stream(report_path.data(), std::ios_base::binary) {}

  [[noreturn]] auto report(const cplib::checker::Report &report) -> void override {
    score_stream << std::llround(1. * report.score / max_score);
    report_stream << report.status.to_string() << '\n';
    report_stream << report.message << '\n';
    std::exit(0);
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE =
    "<input_file> <output_file> <answer_file> <max_score> <score_file> <report_file>";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with lemon checker initializer\n"
                                  "https://github.com/rindag-devs/cplib-initializers/ by Rindag "
                                  "Devs, copyright(c) 2024-present\n"
                                  "\n"
                                  "Usage:\n"
                                  "  %s %s\n",
                                  program_name.data(), ARGS_USAGE.data());
  cplib::panic(msg);
}
}  // namespace detail

struct LemonInitializer : cplib::checker::Initializer {
  auto init(std::string_view arg0, const std::vector<std::string> &args) -> void override {
    auto &state = this->state();

    // Use PlainTextReporter to handle errors during the init process
    state.reporter = std::make_unique<cplib::checker::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() != 6) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    set_inf_path(parsed_args.ordered[0], cplib::var::Reader::TraceLevel::NONE);
    set_ouf_path(parsed_args.ordered[1], cplib::var::Reader::TraceLevel::NONE);
    set_ans_path(parsed_args.ordered[2], cplib::var::Reader::TraceLevel::NONE);

    std::int32_t max_score =
        cplib::var::i32("max_score", 0, std::nullopt).parse(parsed_args.ordered[3]);

    state.reporter =
        std::make_unique<LemonReporter>(max_score, parsed_args.ordered[4], parsed_args.ordered[5]);
  }
};
}  // namespace cplib_initializers::lemon::checker
