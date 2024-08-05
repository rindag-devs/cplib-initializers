#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SPOJ_H__
#define __SPOJ_H__

#include <stdio.h>

/* return values */
#define SPOJ_RV_POSITIVE 0
#define SPOJ_RV_NEGATIVE 1
#define SPOJ_RV_IE 2

/* descriptor numbers */
#define SPOJ_P_IN_FD 0
#define SPOJ_P_OUT_FD 4
#define SPOJ_T_OUT_FD 3
#define SPOJ_T_SRC_FD 5
#define SPOJ_SCORE_FD 1
#define SPOJ_P_INFO_FD 6
#define SPOJ_U_INFO_FD 7

/* file handles */
extern FILE *spoj_p_in, /* problem's input */
    *spoj_p_out,        /* problem's output */
    *spoj_t_out,        /* tested program's output */
    *spoj_t_src,        /* tested program's source */
    *spoj_score,        /* score for the program, for challenge problems */
    *spoj_p_info,       /* additional info - problemsetter only */
    *spoj_u_info;       /* additional info - psetter and solution's owner */

/* must be called before using other functions or variables */
void spoj_init();

/* spoj "files" are really pipes, this is the recommended way to tell size */
/* the file's data is unaccessible later, so use it only if it doesn't matter */
unsigned spoj_file_length(FILE *);

/* use spoj_assert()! */
/* usual assert() from assert.h will cause "judge's internal error" in the future! */
#define spoj_assert(x) \
  __spoj_assert_fail(__FILE__ ":" __spoj_xstr(__LINE__) ": SPOJ assertion `" #x "' failed.", x)
#define __spoj_str(x) #x
#define __spoj_xstr(x) __spoj_str(x)
void __spoj_assert_fail(char *, int);

#endif
#ifdef __cplusplus
}
#endif
