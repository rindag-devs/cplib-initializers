#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
auto fail(std::string_view operation) -> int {
  std::fprintf(stderr, "%.*s: %s\n", static_cast<int>(operation.size()), operation.data(),
               std::strerror(errno));
  return EXIT_FAILURE;
}

auto parse_fd(std::string_view token, int &value) -> bool {
  const std::string text(token);
  char *end = nullptr;
  errno = 0;
  const long long parsed = std::strtoll(text.c_str(), &end, 10);
  if (errno != 0 || end != text.c_str() + text.size() || parsed < 0 || parsed > INT_MAX) {
    return false;
  }
  value = static_cast<int>(parsed);
  return true;
}
}  // namespace

auto main(int argc, char **argv) -> int {
  if (argc < 2) {
    std::fputs("usage: fd_launcher <program> [<source_fd>:<target_fd> ...] -- [args ...]\n",
               stderr);
    return EXIT_FAILURE;
  }

  std::vector<std::pair<int, int>> mappings;
  int argument = 2;
  for (; argument < argc && std::string_view(argv[argument]) != "--"; ++argument) {
    const std::string_view mapping(argv[argument]);
    const auto separator = mapping.find(':');
    if (separator == std::string_view::npos) {
      std::fputs("invalid fd mapping\n", stderr);
      return EXIT_FAILURE;
    }

    int source = -1;
    int target = -1;
    if (!parse_fd(mapping.substr(0, separator), source) ||
        !parse_fd(mapping.substr(separator + 1), target)) {
      std::fputs("invalid fd mapping\n", stderr);
      return EXIT_FAILURE;
    }
    mappings.emplace_back(source, target);
  }

  if (argument >= argc) {
    std::fputs("missing --\n", stderr);
    return EXIT_FAILURE;
  }
  ++argument;

  for (auto &[source, target] : mappings) {
    source = fcntl(source, F_DUPFD_CLOEXEC, 64);
    if (source == -1) return fail("fcntl");
  }
  for (const auto [source, target] : mappings) {
    if (dup2(source, target) == -1) return fail("dup2");
    close(source);
  }

  std::vector<char *> exec_arguments{argv[1]};
  exec_arguments.insert(exec_arguments.end(), argv + argument, argv + argc);
  exec_arguments.push_back(nullptr);
  execv(argv[1], exec_arguments.data());
  return fail("execv");
}
