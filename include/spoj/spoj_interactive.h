#ifndef __SPOJ_INTERACTIVE_H__
#define __SPOJ_INTERACTIVE_H__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

/* return values */
#define SPOJ_RV_AC			0
#define SPOJ_RV_WA			1
#define SPOJ_RV_SE			2
#define SPOJ_RV_CE			3
#define SPOJ_RV_RE			4
#define SPOJ_RV_TLE			5
#define SPOJ_RV_MLE			6
#define SPOJ_RV_EOF			7
#define SPOJ_RV_IE			255

/* descriptor numbers */
#define SPOJ_P_IN_FD			0
#define SPOJ_P_OUT_FD			4
#define SPOJ_T_OUT_FD			3
#define SPOJ_T_SRC_FD			5
#define SPOJ_SCORE_FD			1
#define SPOJ_P_INFO_FD			6
#define SPOJ_U_INFO_FD			7
#define SPOJ_FOR_TESTED_FD		8

/* file handles */
FILE *spoj_p_in;		/* problem's input */
FILE *spoj_p_out;		/* problem's output */
FILE *spoj_t_out;		/* tested program's output */
FILE *spoj_t_src;		/* tested program's source */
FILE *spoj_score;		/* score for the program, for challenge problems */
FILE *spoj_p_info;		/* additional info - psetter only */
FILE *spoj_u_info;		/* additional info - psetter and solution's owner */
FILE *spoj_for_tested;		/* data for tested program */

/* spoj_assert() --> WA */
#define spoj_assert(x) { \
	if (!(x)) exit(SPOJ_RV_WA); \
}

/* spoj_scanf() - ... */
#define spoj_scanf(t, z) { \
	int ret = fscanf(spoj_t_out, (t), (z)); \
	if (ret == EOF) exit(SPOJ_RV_EOF); \
	if (ret != 1) exit(SPOJ_RV_WA); \
}

/* spoj_printf() - ... */
#define spoj_printf(t, z) { \
	int ret = fprintf(spoj_for_tested, (t), (z)); \
	if (ret < 0) exit(SPOJ_RV_EOF); \
}

#define __BUF_SIZE 1024

unsigned spoj_srclen() {
	char buffer[__BUF_SIZE];
	unsigned size = 0;
	unsigned temp;
	while ((temp = read(SPOJ_T_SRC_FD, buffer, __BUF_SIZE)) > 0) size += temp;
	return size;
}

/* ... */
static void spoj_finalize() {
	fclose(spoj_p_in);
	fclose(spoj_p_out);
	fclose(spoj_t_out);
	fclose(spoj_t_src);
	fclose(spoj_score);
	fclose(spoj_p_info);
	fclose(spoj_u_info);
	fclose(spoj_for_tested);
}

#define __WARN_IF_NULL(x) if (errno=0, (x)==NULL)\
	fprintf(stderr, "warning: \"" #x "\" -> %s\n", strerror(errno))

/* must be called before using other functions or variables */
void spoj_init() {
	__WARN_IF_NULL(spoj_p_in = fdopen(SPOJ_P_IN_FD,"r"));
	__WARN_IF_NULL(spoj_p_out = fdopen(SPOJ_P_OUT_FD,"r"));
	__WARN_IF_NULL(spoj_t_out = fdopen(SPOJ_T_OUT_FD,"r"));
	__WARN_IF_NULL(spoj_t_src = fdopen(SPOJ_T_SRC_FD,"r"));
	__WARN_IF_NULL(spoj_score = fdopen(SPOJ_SCORE_FD,"w"));
	__WARN_IF_NULL(spoj_p_info = fdopen(SPOJ_P_INFO_FD,"w"));
	__WARN_IF_NULL(spoj_u_info = fdopen(SPOJ_U_INFO_FD,"w"));
	__WARN_IF_NULL(spoj_for_tested = fdopen(SPOJ_FOR_TESTED_FD,"w"));

	setlinebuf(spoj_for_tested);

	signal(SIGPIPE, SIG_IGN);

	atexit(spoj_finalize);
}

#endif

