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

#ifndef CPLIB_INITIALIZERS_TESTLIB_CHECKER_HPP_
#define CPLIB_INITIALIZERS_TESTLIB_CHECKER_HPP_

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <string_view>
#include <vector>

#include "cplib.hpp"

namespace cplib_initializers::testlib::checker {

namespace detail {
inline auto xml_escape(std::string_view s) -> std::string {
  std::stringbuf buf(std::ios_base::out);
  for (auto c : s) {
    switch (c) {
      case '&':
        buf.sputn("&amp;", 5);
        break;
      case '<':
        buf.sputn("&lt;", 4);
        break;
      case '>':
        buf.sputn("&gt;", 4);
        break;
      case '\"':
        buf.sputn("&quot;", 6);
        break;
      default:
        if (('\x00' <= c && c <= '\x1f') || c == '\x7f') {
          buf.sputc('.');
        } else {
          buf.sputc(c);
        }
        break;
    }
  }
  return buf.str();
}
}  // namespace detail

enum class ExitCode {
  ACCEPTED = 0,
  WRONG_ANSWER = 1,
  INTERNAL_ERROR = 3,
  PARTIALLY_CORRECT = 7,
};

struct Reporter : cplib::checker::Reporter {
  bool appes_mode;
  bool print_status{};
  bool percent_mode;
  std::ostream stream;
  std::unique_ptr<std::streambuf> buf;

  explicit Reporter(std::optional<std::string> report_file, bool appes_mode, bool percent_mode)
      : appes_mode(appes_mode),
        percent_mode(percent_mode),
        stream(std::ostream(nullptr)),
        buf(nullptr) {
    if (report_file.has_value()) {
      cplib::var::detail::make_ostream_by_path(*report_file, buf, stream);
    } else {
      cplib::var::detail::make_ostream_by_fileno(fileno(stderr), buf, stream);
      print_status = true;
    }
  }

  auto print_score(double score) -> void {
    if (percent_mode) {
      stream << std::llround(score * 100);
    } else {
      stream << score;
    }
  }

  [[noreturn]] auto report(const cplib::checker::Report &report) -> void override {
    stream << std::fixed << std::setprecision(9);

    if (appes_mode) {
      stream << R"(<?xml version="1.0" encoding="utf-8"?><result outcome = ")";
      switch (report.status) {
        case cplib::checker::Report::Status::INTERNAL_ERROR:
          stream << "fail";
          break;
        case cplib::checker::Report::Status::ACCEPTED:
          stream << "accepted";
          break;
        case cplib::checker::Report::Status::WRONG_ANSWER:
          stream << "wrong-answer";
          break;
        case cplib::checker::Report::Status::PARTIALLY_CORRECT:
          stream << "points\" points = \"";
          print_score(report.score);
          break;
        default:
          stream << "FAIL invalid status\n";
          std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
      }
      stream << "\">";
      if (report.status == cplib::checker::Report::Status::PARTIALLY_CORRECT) {
        print_score(report.score);
        stream << ' ';
      }
      stream << detail::xml_escape(report.message) << "</result>\n";
    } else {
      if (print_status) {
        switch (report.status) {
          case cplib::checker::Report::Status::INTERNAL_ERROR:
            stream << "FAIL ";
            break;
          case cplib::checker::Report::Status::ACCEPTED:
            stream << "ok ";
            break;
          case cplib::checker::Report::Status::WRONG_ANSWER:
            stream << "wrong answer ";
            break;
          case cplib::checker::Report::Status::PARTIALLY_CORRECT:
            stream << "points ";
            break;
          default:
            stream << "FAIL invalid status\n";
            std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
        }
      }
      if (report.status == cplib::checker::Report::Status::PARTIALLY_CORRECT) {
        print_score(report.score);
        stream << ' ';
      }
      stream << report.message << '\n';
    }

    switch (report.status) {
      case cplib::checker::Report::Status::INTERNAL_ERROR:
        std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
      case cplib::checker::Report::Status::ACCEPTED:
        std::exit(static_cast<int>(ExitCode::ACCEPTED));
      case cplib::checker::Report::Status::WRONG_ANSWER:
        std::exit(static_cast<int>(ExitCode::WRONG_ANSWER));
      case cplib::checker::Report::Status::PARTIALLY_CORRECT:
        std::exit(static_cast<int>(ExitCode::PARTIALLY_CORRECT));
      default:
        stream << "FAIL invalid status\n";
        std::exit(static_cast<int>(ExitCode::INTERNAL_ERROR));
    }
  }
};

namespace detail {
constexpr std::string_view ARGS_USAGE =
    "<input_file> <output_file> <answer_file> [<report_file> [-appes [...]]]";

inline auto print_help_message(std::string_view program_name) -> void {
  std::string msg = cplib::format(CPLIB_STARTUP_TEXT
                                  "\n"
                                  "Initialized with testlib checker initializer\n"
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
  bool percent_mode;

  explicit Initializer(bool percent_mode) : percent_mode(percent_mode) {}

  auto init(std::string_view arg0, const std::vector<std::string> &args) -> void override {
    auto &state = this->state();

    // Use PlainTextReporter to handle errors during the init process
    state.reporter = std::make_unique<cplib::checker::PlainTextReporter>();

    auto parsed_args = cplib::cmd_args::ParsedArgs(args);

    if (parsed_args.has_flag("help")) {
      detail::print_help_message(arg0);
    }

    if (parsed_args.ordered.size() < 3) {
      cplib::panic("Program must be run with the following arguments:\n  " +
                   std::string(detail::ARGS_USAGE));
    }

    set_inf_path(parsed_args.ordered[0], cplib::var::Reader::TraceLevel::NONE);
    set_ouf_path(parsed_args.ordered[1], cplib::var::Reader::TraceLevel::NONE);
    set_ans_path(parsed_args.ordered[2], cplib::var::Reader::TraceLevel::NONE);

    std::optional<std::string> report_file = std::nullopt;
    if (parsed_args.ordered.size() >= 4) report_file = parsed_args.ordered[3];

    // Some platforms may pass some platform-specific command line arguments to testlib, ignore them

    bool appes_mode = false;

    for (size_t i = 4; i < parsed_args.ordered.size(); ++i) {
      if (parsed_args.ordered[i] == "-appes") {
        appes_mode = true;
      }
    }

    state.reporter = std::make_unique<Reporter>(report_file, appes_mode, percent_mode);
  }
};
}  // namespace cplib_initializers::testlib::checker

#endif
