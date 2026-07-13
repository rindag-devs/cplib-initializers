find_package(Catch2 3 CONFIG REQUIRED)

add_executable(cplib_unit_tests unit/base64_test.cpp unit/reporters_test.cpp unit/sigpipe_test.cpp)
target_link_libraries(
  cplib_unit_tests
  PRIVATE cplib-initializers::cplib-initializers Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(
  cplib_unit_tests
  TEST_PREFIX "unit."
  PROPERTIES LABELS unit
)

add_subdirectory(fixtures)
