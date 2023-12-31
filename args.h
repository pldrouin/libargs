#ifndef _ARGS_
#define _ARGS_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
  int getnextparam(FILE **fptra, int *fptri, const bool isarg, const int nargs, char const** args, int *parc, char *param);
  void safegetnextparam(FILE **fptra, int *fptri, const bool isarg, const int nargs, char const** args, int *parc, char *param);
}
#endif

int getnextparam(FILE **fptra, int *fptri, const bool isarg, const int nargs, char const** args, int *parc, char *param);
void safegetnextparam(FILE **fptra, int *fptri, const bool isarg, const int nargs, char const** args, int *parc, char *param);

inline static bool argsdiffer(const char* arg1, const char* arg2) {int i=0; while(1) {if(arg1[i]!=arg2[i]) return true; if(!arg1[i]) return false; ++i;}}

#endif
