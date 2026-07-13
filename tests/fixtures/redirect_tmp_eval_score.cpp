#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {
constexpr const char *kArbiterReportPath = "/tmp/_eval.score";
constexpr const char *kRedirectEnv = "CPLIB_INITIALIZERS_ARBITER_REPORT";

auto redirected_path(const char *pathname) -> const char * {
  const char *const target = std::getenv(kRedirectEnv);
  if (target != nullptr && std::strcmp(pathname, kArbiterReportPath) == 0) {
    return target;
  }
  return pathname;
}
}  // namespace

extern "C" auto open(const char *pathname, int flags, ...) -> int {
  using Open = int (*)(const char *, int, ...);
  static auto real_open = reinterpret_cast<Open>(dlsym(RTLD_NEXT, "open"));

  pathname = redirected_path(pathname);
  if ((flags & O_CREAT) == 0) {
    return real_open(pathname, flags);
  }

  va_list args;
  va_start(args, flags);
  const mode_t mode = va_arg(args, mode_t);
  va_end(args);
  return real_open(pathname, flags, mode);
}

extern "C" auto open64(const char *pathname, int flags, ...) -> int {
  using Open64 = int (*)(const char *, int, ...);
  static auto real_open64 = reinterpret_cast<Open64>(dlsym(RTLD_NEXT, "open64"));

  pathname = redirected_path(pathname);
  if ((flags & O_CREAT) == 0) {
    return real_open64(pathname, flags);
  }

  va_list args;
  va_start(args, flags);
  const mode_t mode = va_arg(args, mode_t);
  va_end(args);
  return real_open64(pathname, flags, mode);
}

extern "C" auto openat(int dirfd, const char *pathname, int flags, ...) -> int {
  using Openat = int (*)(int, const char *, int, ...);
  static auto real_openat = reinterpret_cast<Openat>(dlsym(RTLD_NEXT, "openat"));

  pathname = redirected_path(pathname);
  if ((flags & O_CREAT) == 0) {
    return real_openat(dirfd, pathname, flags);
  }

  va_list args;
  va_start(args, flags);
  const mode_t mode = va_arg(args, mode_t);
  va_end(args);
  return real_openat(dirfd, pathname, flags, mode);
}

extern "C" auto openat64(int dirfd, const char *pathname, int flags, ...) -> int {
  using Openat64 = int (*)(int, const char *, int, ...);
  static auto real_openat64 = reinterpret_cast<Openat64>(dlsym(RTLD_NEXT, "openat64"));

  pathname = redirected_path(pathname);
  if ((flags & O_CREAT) == 0) {
    return real_openat64(dirfd, pathname, flags);
  }

  va_list args;
  va_start(args, flags);
  const mode_t mode = va_arg(args, mode_t);
  va_end(args);
  return real_openat64(dirfd, pathname, flags, mode);
}

extern "C" auto fopen(const char *pathname, const char *mode) -> FILE * {
  using Fopen = FILE *(*)(const char *, const char *);
  static auto real_fopen = reinterpret_cast<Fopen>(dlsym(RTLD_NEXT, "fopen"));
  return real_fopen(redirected_path(pathname), mode);
}

extern "C" auto fopen64(const char *pathname, const char *mode) -> FILE * {
  using Fopen64 = FILE *(*)(const char *, const char *);
  static auto real_fopen64 = reinterpret_cast<Fopen64>(dlsym(RTLD_NEXT, "fopen64"));
  return real_fopen64(redirected_path(pathname), mode);
}
