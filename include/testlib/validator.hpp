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

#ifndef CPLIB_INITIALIZERS_TESTLIB_VALIDATOR_HPP_
#define CPLIB_INITIALIZERS_TESTLIB_VALIDATOR_HPP_

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::testlib::validator {

enum struct ExitCode {
  OK = 0,
  INTERNAL_ERROR = 3,
};

struct Reporter : cplib::validator::Reporter {
  using Report = cplib::validator::Report;
  using Status = Report::Status;

  std::optional<std::ofstream> overview_log_stream;

  explicit Reporter(std::optional<std::string> overview_log_path) {
    if (overview_log_path.has_value()) {
      overview_log_stream = std::ofstream(overview_log_path.value(), std::ios_base::binary);
    }
  }

  auto report(const Report &report) -> int override {
    std::ostream message(std::clog.rdbuf());

    if (overview_log_stream.has_value()) {
      for (const auto &[name, satisfaction] : trait_status_) {
        *overview_log_stream << "feature \"" << name << "\":";
        if (satisfaction) {
          *overview_log_stream << " hit";
        }
        *overview_log_stream << '\n';
      }
    }

    switch (report.status) {
      case Status::INTERNAL_ERROR:
      case Status::INVALID:
        message << "FAIL " << report.message << '\n';
        return static_cast<int>(ExitCode::INTERNAL_ERROR);
        break;
      case Status::VALID:
        return static_cast<int>(ExitCode::OK);
        break;
      default:
        message << "FAIL invalid status\n";
        return static_cast<int>(ExitCode::INTERNAL_ERROR);
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE = "[--testOverviewLogFileName <overview_log_path>] [...]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with testlib validator initializer\n"
                                  "https://github.com/rindag-devs/cplib-initializers/ by Rindag "
                                  "Devs, copyright(c) 2024\n"
                                  "\n"
                                  "Usage:\n"
                                  "  %s %s\n",
                                  program_name.data(), ARGS_USAGE.data());
  cplib::panic(msg);
}
}  // namespace detail

struct Initializer : cplib::validator::Initializer {
  auto init(std::string_view arg0, const std::vector<std::string> &args) -> void override {
    auto &state = this->state();

    // Use PlainTextReporter to handle errors during the init process
    state.reporter = std::make_unique<cplib::validator::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    std::optional<std::string> overview_log_path;
    if (auto it = parsed_args.vars.find("testOverviewLogFileName"); it != parsed_args.vars.end()) {
      overview_log_path = it->second;
    }

    state.reporter = std::make_unique<Reporter>(overview_log_path);

    set_inf_fileno(fileno(stdin), cplib::var::Reader::TraceLevel::NONE);
  }
};
}  // namespace cplib_initializers::testlib::validator

#endif
