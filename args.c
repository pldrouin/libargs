#include "args.h"

int getnextparam(FILE **fptra, int *fptri, const bool isarg, const int nargs, char const** args, int *parc, char *param)
{
  int i=0;
  //Priority order is:
  //1) Reading from a string
  //1) Reading from file handles
  //2) Reading from arguments
  //Reading from a string occur when nargs<0, in which case args[0] is assumed
  //to point to the current character to be read.

  while(*fptri>-1 || nargs>*parc) {

    //If reading from args
    if(nargs>=0 && *fptri==-1) {
      if(!isarg) while(args[*parc][i] == '-') i++;
      strcpy(param,args[*parc]+i);
      (*parc)++;
      //printf("param is '%s'\n",param);
      return strlen(param);

    //Else if (nargs<0 && nargs>*parc) || *fptri!=-1
    } else {
      signed char c;
      bool brs=false, brd=false, com=false;

      //If reading from file
      if(nargs<=*parc) {
        //printf("Reading from file level %i\n",*fptri);

	while(isspace((c=fgetc(fptra[*fptri]))) || ((c == '=' || c == ':') && isarg) || c == '#' || (com && c != 0 && c != EOF)) {if(com && c == '\n') com=false; if(c == '#') com=true;}
	if(!isarg && c == '-') while((c=fgetc(fptra[*fptri])) == '-') {}

	if(c != 0 && c != EOF) {

	  if(c=='\'') brs=!brs;
	  else if(c=='\"') brd=!brd;
	  else param[i++]=c;

	  while(((!isspace((c=fgetc(fptra[*fptri]))) && ((c != '=' && c != ':') || isarg) && c != '#') || brs || brd) && c != 0 && c != EOF) {if(c=='\'') brs=!brs; else if(c=='\"') brd=!brd; else param[i++]=c;}
	  param[i++]=0;

	  if(c == 0 || c == EOF) {
	    if(fptra[*fptri]!=stdin) fclose(fptra[*fptri]);
	    (*fptri)--;
	  }
	  //printf("param is '%s'\n",param);
	  return i;

	} else {
	  if(fptra[*fptri]!=stdin) fclose(fptra[*fptri]);
	  (*fptri)--;

	  //If reading from a string up in the stack, only decrement one level
	  //of file handle
	  if(nargs<0) break;
	}

      //Else if reading from string
      } else {
        //printf("Reading from string\n");

	while(isspace((c=(args[0]++)[0])) || ((c == '=' || c == ':') && isarg) || c == '#' || (com && c != 0)) {if(com && c == '\n') com=false; if(c == '#') com=true;;}
	if(!isarg && c == '-') while((c=(args[0]++)[0]) == '-') {}

	if(c != 0) {

	  if(c=='\'') brs=!brs;
	  else if(c=='\"') brd=!brd;
	  else param[i++]=c;

	  while(((!isspace((c=(args[0]++)[0])) && ((c != '=' && c != ':') || isarg) && c != '#') || brs || brd) && c != 0) {if(c=='\'') brs=!brs; else if(c=='\"') brd=!brd; else param[i++]=c;}
	  param[i++]=0;
	  //printf("param is '%s'\n",param);
	  return i;

	} else break;
      }
    }
  }
  return -1;
}

void safegetnextparam(FILE **fptra, int *fptri, const bool isarg, const int nargs, char const** args, int *parc, char *param)
{
  if(getnextparam(fptra,fptri,isarg,nargs,args,parc,param)<0) {
    fprintf(stderr,"Error: Missing parameter\n");
    exit(1);
  }
}

void args_close_all_files_down_to_level(FILE **fptra, int *fptri, const int ptri_stop)
{
  for(; *fptri>ptri_stop; --*fptri) {

    if(fptra[*fptri]!=stdin) fclose(fptra[*fptri]);
  }
}
