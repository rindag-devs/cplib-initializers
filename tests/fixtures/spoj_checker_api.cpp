#include <cstdio>
#include <cstdlib>

#include "spoj/spoj.h"

FILE *spoj_p_in;
FILE *spoj_p_out;
FILE *spoj_t_out;
FILE *spoj_t_src;
FILE *spoj_score;
FILE *spoj_p_info;
FILE *spoj_u_info;

namespace {
auto close_spoj_files() -> void {
  fclose(spoj_p_in);
  fclose(spoj_p_out);
  fclose(spoj_t_out);
  fclose(spoj_t_src);
  fclose(spoj_score);
  fclose(spoj_p_info);
  fclose(spoj_u_info);
}
}  // namespace

extern "C" auto spoj_init() -> void {
  spoj_p_in = fdopen(SPOJ_P_IN_FD, "r");
  spoj_p_out = fdopen(SPOJ_P_OUT_FD, "r");
  spoj_t_out = fdopen(SPOJ_T_OUT_FD, "r");
  spoj_t_src = fdopen(SPOJ_T_SRC_FD, "r");
  spoj_score = fdopen(SPOJ_SCORE_FD, "w");
  spoj_p_info = fdopen(SPOJ_P_INFO_FD, "w");
  spoj_u_info = fdopen(SPOJ_U_INFO_FD, "w");
  std::atexit(close_spoj_files);
}

extern "C" auto spoj_file_length(FILE *) -> unsigned { return 0; }

extern "C" auto __spoj_assert_fail(char *, int condition) -> void {
  if (!condition) std::exit(SPOJ_RV_NEGATIVE);
}
