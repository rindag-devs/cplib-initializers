function(add_checker_fixture target header initializer)
  set(INITIALIZER_HEADER "${header}")
  set(INITIALIZER "${initializer}")
  set(NO_ANSWER 0)
  if(ARGC GREATER 3)
    set(NO_ANSWER "${ARGV3}")
  endif()
  configure_file(checker.cpp.in "${target}.cpp" @ONLY)
  add_executable("${target}" "${CMAKE_CURRENT_BINARY_DIR}/${target}.cpp")
  target_link_libraries("${target}" PRIVATE cplib-initializers::cplib-initializers)
endfunction()

function(add_interactor_fixture target header initializer)
  set(INITIALIZER_HEADER "${header}")
  set(INITIALIZER "${initializer}")
  configure_file(interactor.cpp.in "${target}.cpp" @ONLY)
  add_executable("${target}" "${CMAKE_CURRENT_BINARY_DIR}/${target}.cpp")
  target_link_libraries("${target}" PRIVATE cplib-initializers::cplib-initializers)
endfunction()

add_checker_fixture(checker_arbiter "arbiter/checker.hpp"
                    "cplib_initializers::arbiter::checker::Initializer()"
)
add_checker_fixture(checker_ccr "ccr/checker.hpp"
                    "cplib_initializers::ccr::checker::Initializer()"
)
add_checker_fixture(checker_cms "cms/checker.hpp"
                    "cplib_initializers::cms::checker::Initializer()"
)
add_checker_fixture(checker_coci "coci/checker.hpp"
                    "cplib_initializers::coci::checker::Initializer()"
)
add_checker_fixture(checker_hello_judge "hello_judge/checker.hpp"
                    "cplib_initializers::hello_judge::checker::Initializer()"
)
add_checker_fixture(checker_hustoj "hustoj/checker.hpp"
                    "cplib_initializers::hustoj::checker::Initializer()"
)
add_checker_fixture(checker_kattis "kattis/checker.hpp"
                    "cplib_initializers::kattis::checker::Initializer()"
)
add_checker_fixture(checker_lemon "lemon/checker.hpp"
                    "cplib_initializers::lemon::checker::Initializer()"
)
add_checker_fixture(checker_nowcoder "nowcoder/checker.hpp"
                    "cplib_initializers::nowcoder::checker::Initializer()"
)
add_checker_fixture(checker_qduoj "qduoj/checker.hpp"
                    "cplib_initializers::qduoj::checker::Initializer()" 1
)
add_checker_fixture(checker_spoj "spoj/checker.hpp"
                    "cplib_initializers::spoj::checker::Initializer()"
)
target_sources(checker_spoj PRIVATE spoj_checker_api.cpp)
add_checker_fixture(checker_syzoj "syzoj/checker.hpp"
                    "cplib_initializers::syzoj::checker::Initializer()"
)
add_checker_fixture(checker_testlib "testlib/checker.hpp"
                    "cplib_initializers::testlib::checker::Initializer(true)"
)

add_interactor_fixture(interactor_cms "cms/interactor.hpp"
                       "cplib_initializers::cms::interactor::Initializer()"
)
add_interactor_fixture(interactor_coci "coci/interactor.hpp"
                       "cplib_initializers::coci::interactor::Initializer()"
)
add_interactor_fixture(interactor_kattis "kattis/interactor.hpp"
                       "cplib_initializers::kattis::interactor::Initializer()"
)
add_interactor_fixture(interactor_spoj "spoj/interactor.hpp"
                       "cplib_initializers::spoj::interactor::Initializer()"
)
add_interactor_fixture(interactor_syzoj "syzoj/interactor.hpp"
                       "cplib_initializers::syzoj::interactor::Initializer()"
)
add_interactor_fixture(interactor_testlib "testlib/interactor.hpp"
                       "cplib_initializers::testlib::interactor::Initializer(true)"
)
add_interactor_fixture(interactor_two_step "testlib/interactor_two_step.hpp"
                       "cplib_initializers::testlib::interactor_two_step::Initializer()"
)

add_executable(validator_testlib validator.cpp)
target_link_libraries(validator_testlib PRIVATE cplib-initializers::cplib-initializers)

add_executable(checker_two_step "${PROJECT_SOURCE_DIR}/include/testlib/checker_two_step.cpp")
target_link_libraries(checker_two_step PRIVATE cplib-initializers::cplib-initializers)

add_executable(fd_launcher fd_launcher.cpp)
