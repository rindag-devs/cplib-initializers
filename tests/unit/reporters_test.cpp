#include <unistd.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <optional>
#include <string>

#include "coci/interactor.hpp"
#include "cplib.hpp"
#include "syzoj/interactor.hpp"
#include "testlib/interactor_two_step.hpp"
#include "testlib/validator.hpp"

TEST_CASE("COCI interactor maps supported results to platform exit codes") {
  namespace interactor = cplib_initializers::coci::interactor;
  interactor::Reporter reporter;

  CHECK(reporter.report({cplib::interactor::Report::Status::ACCEPTED, 1.0, ""}) ==
        static_cast<int>(interactor::ExitCode::ACCEPTED));
  CHECK(reporter.report({cplib::interactor::Report::Status::WRONG_ANSWER, 0.0, ""}) ==
        static_cast<int>(interactor::ExitCode::WRONG_ANSWER));
  CHECK(reporter.report({cplib::interactor::Report::Status::PARTIALLY_CORRECT, 0.5, ""}) ==
        static_cast<int>(interactor::ExitCode::PARTIALLY_CORRECT));
  CHECK(reporter.report({cplib::interactor::Report::Status::INTERNAL_ERROR, 0.0, ""}) ==
        static_cast<int>(interactor::ExitCode::INTERNAL_ERROR));
}

TEST_CASE("two-step reporter returns internal error when its output cannot be opened") {
  namespace two_step = cplib_initializers::testlib::interactor_two_step;
  two_step::Reporter reporter("/definitely/missing/cplib-initializers/report.txt");

  CHECK(reporter.report({cplib::interactor::Report::Status::ACCEPTED, 1.0, ""}) ==
        static_cast<int>(two_step::ExitCode::INTERNAL_ERROR));
}

TEST_CASE("testlib validator returns internal error when overview output fails") {
  namespace validator = cplib_initializers::testlib::validator;
  validator::Reporter reporter(
      std::optional<std::string>("/definitely/missing/cplib-initializers/overview.txt"));

  CHECK(reporter.report({cplib::validator::Report::Status::VALID, ""}) ==
        static_cast<int>(validator::ExitCode::INTERNAL_ERROR));
}

TEST_CASE("SYZOJ interactor reports score output failure") {
  char directory_template[] = "/tmp/cplib-initializers-syzoj-XXXXXX";
  const auto directory = std::filesystem::path(mkdtemp(directory_template));
  const auto previous_directory = std::filesystem::current_path();
  std::filesystem::create_directory(directory / "score.txt");
  std::filesystem::current_path(directory);

  cplib_initializers::syzoj::interactor::Reporter reporter;
  CHECK(reporter.report({cplib::interactor::Report::Status::ACCEPTED, 1.0, ""}) == 1);

  std::filesystem::current_path(previous_directory);
  std::filesystem::remove_all(directory);
}
