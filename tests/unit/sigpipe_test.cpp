#include <sys/wait.h>
#include <unistd.h>

#include <catch2/catch_test_macros.hpp>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "coci/interactor.hpp"
#include "cplib.hpp"
#include "kattis/interactor.hpp"
#include "syzoj/interactor.hpp"
#include "testlib/interactor.hpp"
#include "testlib/interactor_two_step.hpp"

namespace {
auto initializer_ignores_sigpipe(std::unique_ptr<cplib::interactor::Initializer> initializer,
                                 const std::vector<std::string> &arguments,
                                 const std::filesystem::path &working_directory) -> bool {
  const auto child = fork();
  if (child == 0) {
    std::filesystem::current_path(working_directory);
    signal(SIGPIPE, SIG_DFL);
    auto *state = new cplib::interactor::State(std::move(initializer));
    state->initializer->init("test-interactor", arguments);

    struct sigaction action{};
    if (sigaction(SIGPIPE, nullptr, &action) != 0) _exit(2);
    _exit(action.sa_handler == SIG_IGN ? 0 : 1);
  }
  if (child < 0) return false;

  int status = 0;
  if (waitpid(child, &status, 0) != child) return false;
  return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}
}  // namespace

TEST_CASE("stdio interactor initializers ignore SIGPIPE") {
  char directory_template[] = "/tmp/cplib-initializers-sigpipe-XXXXXX";
  const auto directory = std::filesystem::path(mkdtemp(directory_template));
  const auto input = directory / "input";
  const auto feedback = directory / "feedback";
  const auto report = directory / "report";
  std::ofstream(input) << "7\n";
  std::filesystem::create_directory(feedback);

  CHECK(initializer_ignores_sigpipe(
      std::make_unique<cplib_initializers::coci::interactor::Initializer>(), {input.string()},
      directory));
  CHECK(initializer_ignores_sigpipe(
      std::make_unique<cplib_initializers::kattis::interactor::Initializer>(),
      {input.string(), "dummy", feedback.string()}, directory));
  CHECK(initializer_ignores_sigpipe(
      std::make_unique<cplib_initializers::syzoj::interactor::Initializer>(), {}, directory));
  CHECK(initializer_ignores_sigpipe(
      std::make_unique<cplib_initializers::testlib::interactor::Initializer>(true),
      {input.string()}, directory));
  CHECK(initializer_ignores_sigpipe(
      std::make_unique<cplib_initializers::testlib::interactor_two_step::Initializer>(),
      {input.string(), report.string()}, directory));

  std::filesystem::remove_all(directory);
}
