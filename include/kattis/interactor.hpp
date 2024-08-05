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

#ifndef CPLIB_INITIALIZERS_KATTIS_INTERACTOR_HPP_
#define CPLIB_INITIALIZERS_KATTIS_INTERACTOR_HPP_

#include <sys/stat.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::kattis::interactor {

constexpr int EXITCODE_JE = 1;
constexpr int EXITCODE_AC = 42;
constexpr int EXITCODE_WA = 43;

constexpr std::string_view FILENAME_JUDGE_MESSAGE = "judgemessage.txt";
constexpr std::string_view FILENAME_JUDGE_ERROR = "judgeerror.txt";
constexpr std::string_view FILENAME_SCORE = "score.txt";

namespace detail {
inline auto is_directory(std::string_view path) -> bool {
  struct stat entry;
  return stat(path.data(), &entry) == 0 && S_ISDIR(entry.st_mode);
}
}  // namespace detail

struct Reporter : cplib::interactor::Reporter {
  using Report = cplib::interactor::Report;
  using Status = Report::Status;

  explicit Reporter(std::string_view feedback_dir)
      : judge_message(cplib::format("%s/%s", feedback_dir.data(), FILENAME_JUDGE_MESSAGE.data()),
                      std::ios_base::binary),
        judge_error(cplib::format("%s/%s", feedback_dir.data(), FILENAME_JUDGE_ERROR.data()),
                    std::ios_base::binary),
        score(cplib::format("%s/%s", feedback_dir.data(), FILENAME_SCORE.data()),
              std::ios_base::binary) {}

  [[noreturn]] auto report(const Report& report) -> void override {
    switch (report.status) {
      case Status::INTERNAL_ERROR:
        judge_error << "FAIL " << report.message << '\n';
        std::exit(EXITCODE_JE);
      case Status::ACCEPTED:
        judge_message << "OK\n";
        std::exit(EXITCODE_AC);
      case Status::WRONG_ANSWER:
        judge_message << "WA " << report.message << '\n';
        std::exit(EXITCODE_WA);
      case Status::PARTIALLY_CORRECT:
        judge_message << "PC\n";
        score << std::fixed << std::setprecision(9) << report.score << '\n';
        std::exit(EXITCODE_AC);
      default:
        judge_error << "FAIL invalid status\n";
        std::exit(EXITCODE_JE);
    }
  }

 private:
  std::ofstream judge_message, judge_error, score;
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "<input_file> <dummy> <feedback_dir> [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with kattis interactor initializer\n"
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

    // Use PlainTextReporter to handle errors during the init process
    state.reporter = std::make_unique<cplib::interactor::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() < 3) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    const auto& inf = parsed_args.ordered[0];
    const auto& feedback_dir = parsed_args.ordered[2];

    if (!detail::is_directory(feedback_dir)) {
      cplib::panic(feedback_dir + " is not a directory");
    }

    state.reporter = std::make_unique<Reporter>(feedback_dir);

    set_inf_path(inf, cplib::var::Reader::TraceLevel::NONE);
    set_from_user_fileno(fileno(stdin), cplib::var::Reader::TraceLevel::NONE);
    set_to_user_fileno(fileno(stdout));
  }
};
}  // namespace cplib_initializers::kattis::interactor

#endif
