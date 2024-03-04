#include "args.h"

void args_init(args_info* a_h, int nargs, const char** args)
{
  a_h->fptra=NULL;
  a_h->fptri=-1;
  a_h->parc=0;
  a_h->nargs=nargs;
  a_h->args=args;
}

void args_terminate(args_info* a_h)
{
  args_close_all_files_down_to_level(a_h, -1);
  free(a_h->fptra);
}

int args_open_conf_file(args_info* a_h, const char* filename)
{
  if(!argsdiffer(filename,"-")) {

    if(a_h->fptri==-1 || a_h->fptra[a_h->fptri] != stdin) {
      a_h->fptra=(FILE**)realloc(a_h->fptra,(a_h->fptri+2)*sizeof(FILE*));

      if(a_h->fptra == NULL) return errno;
      ++(a_h->fptri);
      a_h->fptra[a_h->fptri]=stdin;
    }

  } else {
    a_h->fptra=(FILE**)realloc(a_h->fptra,(a_h->fptri+2)*sizeof(FILE*));

    if(a_h->fptra == NULL) return errno;

    if(!(a_h->fptra[a_h->fptri+1]=fopen(filename,"r"))) {
      fprintf(stderr,"%s: Error: Cannot open file '%s' in read mode\n",__func__,filename);
      return errno;
    }
    ++(a_h->fptri);
  }
  return 0;
}

int args_prompt(args_info* a_h, const char* prompt, char* pbuf, const int maxlen, int (*cb)(const char* param, void* ptr), void* ptr)
{
  char* lineptr;
  char* charptr;
  size_t n;
  ssize_t ret;
  const char** oldargs=a_h->args;
  int plength;
  a_h->args=(char const**)&charptr;

  a_h->fptra=(FILE**)realloc(a_h->fptra,(a_h->fptri+2)*sizeof(FILE*));

  if(a_h->fptra == NULL) return errno;
  ++(a_h->fptri);
  a_h->fptra[a_h->fptri]=NULL;

  const int initptri=a_h->fptri;

  int cret;

  do {
next_line:
    printf("%s", prompt);
    lineptr=NULL;
    n=0;
    ret=getline(&lineptr, &n, stdin);
    charptr=lineptr;

    if(ret<0) {
      free(lineptr);
      a_h->args=oldargs;
      --(a_h->fptri);
      break;
    }

    for(;;) {
      //printf("Inner loop\n");
      //Prioritise reading from prompt if a new configuration file has not been loaded from prompt

      plength=getnextparam(a_h,false,pbuf,maxlen);

      if(plength<=0) {

	if(a_h->fptri>initptri) continue;

	else {
	  free(lineptr);
	  goto next_line;
	}
      }
      cret=cb(pbuf, ptr);

      if(cret == ARGS_CB_EXIT) goto exit_prompt;

      if(a_h->fptri>initptri) {

	//Continue reading config file or interactive mode only if no
	//error is returned
	if(cret<0) {
	  args_close_all_files_down_to_level(a_h,initptri);
	  free(lineptr);
	  goto next_line;
	}

      } else {
	free(lineptr);
	goto next_line;
      }
    }

  } while(charptr == lineptr+ret);

exit_prompt:
  free(lineptr);
  printf("\n");
  a_h->args=oldargs;
  --(a_h->fptri);
  return 0;
}

int getnextparam(args_info* a_h, const bool isarg, char *param, const int maxlen)
{
  int i=0;
  //Priority order is:
  //1) Reading from file handles
  //2) Reading from a string
  //3) Reading from arguments
  //Reading from a string occur when fptri>0 and fptra[fptri]=NULL, in which case args[0] is assumed
  //to point to the current character to be read.

  while(a_h->fptri>-1 || a_h->nargs>a_h->parc) {

    //If reading from args
    if(a_h->fptri==-1) {
      if(!isarg) while(a_h->args[a_h->parc][i] == '-') i++;
      int j=strlen(a_h->args[a_h->parc]+i);
      j=(j>=maxlen?maxlen-1:j);
      memcpy(param, a_h->args[a_h->parc]+i, j);
      param[j]=0;
      (a_h->parc)++;
      //printf("param is '%s'\n",param);
      return j;

    } else {
      signed char c;
      bool brs=false, brd=false, com=false;

      //If reading from file
      if(a_h->fptra[a_h->fptri]!=NULL) {
        //printf("Reading from file level %i\n",a_h->fptri);

	while(isspace((c=fgetc(a_h->fptra[a_h->fptri]))) || ((c == '=' || c == ':') && isarg) || c == '#' || (com && c != 0 && c != EOF)) {if(com && c == '\n') com=false; if(c == '#') com=true;}
	if(!isarg && c == '-') while((c=fgetc(a_h->fptra[a_h->fptri])) == '-') {}

	if(c != 0 && c != EOF) {

	  if(c=='\'') brs=!brs;
	  else if(c=='\"') brd=!brd;
	  else param[i++]=c;

	  while(((!isspace((c=fgetc(a_h->fptra[a_h->fptri]))) && ((c != '=' && c != ':') || isarg) && c != '#') || brs || brd) && c != 0 && c != EOF) {

	    if(c=='\'')
	      brs=!brs;

	    else if(c=='\"')
	      brd=!brd;

	    else {
	      param[i++]=c;

	      if(i==maxlen-1) break;
	    }
	  }
	  param[i++]=0;

	  if(c == 0 || c == EOF) {
	    if(a_h->fptra[a_h->fptri]!=stdin) fclose(a_h->fptra[a_h->fptri]);
	    (a_h->fptri)--;
	  }
	  //printf("param is '%s'\n",param);
	  return i;

	} else {
	  if(a_h->fptra[a_h->fptri]!=stdin) fclose(a_h->fptra[a_h->fptri]);
	  (a_h->fptri)--;

	  //If reading from a string up in the stack, only decrement one level
	  //of file handle
	  if(a_h->nargs<0) break;
	}

      //Else if reading from string
      } else {
        //printf("Reading from string\n");

	while(isspace((c=(a_h->args[0]++)[0])) || ((c == '=' || c == ':') && isarg) || c == '#' || (com && c != 0)) {if(com && c == '\n') com=false; if(c == '#') com=true;;}
	if(!isarg && c == '-') while((c=(a_h->args[0]++)[0]) == '-') {}

	if(c != 0) {

	  if(c=='\'') brs=!brs;
	  else if(c=='\"') brd=!brd;
	  else param[i++]=c;

	  while(((!isspace((c=(a_h->args[0]++)[0])) && ((c != '=' && c != ':') || isarg) && c != '#') || brs || brd) && c != 0) {

	    if(c=='\'')
	      brs=!brs;

	    else if(c=='\"')
	      brd=!brd;

	    else {
	      param[i++]=c;

	      if(i==maxlen-1) break;
	    }
	  }
	  param[i++]=0;
	  //printf("param is '%s'\n",param);
	  return i;

	} else break;
      }
    }
  }
  return -1;
}

void safegetnextparam(args_info* a_h, const bool isarg, char *param, const int maxlen)
{
  if(getnextparam(a_h,isarg,param,maxlen)<0) {
    fprintf(stderr,"Error: Missing parameter\n");
    exit(1);
  }
}

void args_close_all_files_down_to_level(args_info* a_h, const int ptri_stop)
{
  for(; a_h->fptri>ptri_stop; --(a_h->fptri)) {

    if(a_h->fptra[a_h->fptri]!=NULL && a_h->fptra[a_h->fptri]!=stdin) fclose(a_h->fptra[a_h->fptri]);
  }
}
