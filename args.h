#ifndef _ARGS_
#define _ARGS_

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct
{
  int nargs;
  const char** args;
  FILE **fptra;
  int fptri;
  int parc;
} args_info;

enum {ARGS_CB_OK=0, ARGS_CB_EXIT=1, ARGS_CB_ERROR=-1};

#ifdef __cplusplus
extern "C" {
  void args_init(args_info* a_h, int nargs, const char** args);
  void args_terminate(args_info* a_h);
  int args_open_conf_file(args_info* a_h, const char* filename);
  int getnextparam(args_info* a_h, const bool isarg, char *param, const int maxlen);
  void safegetnextparam(args_info* a_h, const bool isarg, char *param, const int maxlen);
  void args_close_all_files_down_to_level(args_info* a_h, const int ptri_stop);
}
#endif

void args_init(args_info* a_h, int nargs, const char** args);
void args_terminate(args_info* a_h);
int args_open_conf_file(args_info* a_h, const char* filename);
inline static bool args_reading_from_stdin(args_info* a_h){return (a_h->fptri>=0 && a_h->fptra[a_h->fptri]==stdin);}
int args_prompt(args_info* a_h, const char* prompt, char* pbuf, const int maxlen, int (*cb)(const char* param, void* ptr), void* ptr);
int getnextparam(args_info* a_h, const bool isarg, char *param, const int maxlen);
void safegetnextparam(args_info* a_h, const bool isarg, char *param, const int maxlen);

void args_close_all_files_down_to_level(args_info* a_h, const int ptri_stop);

inline static bool argsdiffer(const char* arg1, const char* arg2) {int i=0; while(1) {if(arg1[i]!=arg2[i]) return true; if(!arg1[i]) return false; ++i;}}

#endif
