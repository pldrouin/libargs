#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "args.h"

int main(int nargs, char const* args[])
{
  args++;
  nargs--;

  FILE **fptra=NULL;
  int fptri=-1;
  char pbuf[1024];
  int parc=0;

  int plength=1;

  int oldstdout, oldstderr;
  int oout=1, eout=2; //stdout==1, stderr==2

  oldstdout=dup(1);
  oldstderr=dup(2);
  
  while(plength>0) {

    while((plength=getnextparam(fptra,&fptri,false,nargs,args,&parc,pbuf))>0) {

      if(!argsdiffer(pbuf,"config")) {
	safegetnextparam(fptra,&fptri,true,nargs,args,&parc,pbuf);

	//If we switch to interactive mode (reading instructions from standard
	//input)
	if(!argsdiffer(pbuf,"-")) {

	  if(fptri==-1 || fptra[fptri] != stdin) {
	    fptri++;
	    fptra=(FILE**)realloc(fptra,(fptri+1)*sizeof(FILE*));
	    fptra[fptri]=stdin;
	    write(oldstderr,"@INTERACTIVE\n",13);
	  }

	//Otherwise, if reading from a configuration file
	} else {
	  fptri++;
	  fptra=(FILE**)realloc(fptra,(fptri+1)*sizeof(FILE*));

	  if(!(fptra[fptri]=fopen(pbuf,"r"))) {
	    fprintf(stderr,"Error: Cannot open file '%s' in read mode\n",pbuf);
	    return 1;
	  }
	}

        //Output file log, redirecting the standard output
      } else if(!argsdiffer(pbuf,"olog")) {
	safegetnextparam(fptra,&fptri,true,nargs,args,&parc,pbuf);
	fflush(stdout);
	close(oout);

	if((oout=open(pbuf,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
	  fprintf(stderr,"Error: Cannot open file '%s' in write mode\n",pbuf);
	  return 1;
	}
	dup2(oout,1);

	//Output error file log, redirecting the standard error
      } else if(!argsdiffer(pbuf,"elog")) {
	safegetnextparam(fptra,&fptri,true,nargs,args,&parc,pbuf);
	fflush(stderr);
	close(eout);

	if((eout=open(pbuf,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
	  fprintf(stderr,"Error: cannot open file '%s' in write mode\n",pbuf);
	  return 1;
	}
	dup2(eout,2);

	//Example of a command with a single argument
      } else if(!argsdiffer(pbuf,"ldfile")) {
	safegetnextparam(fptra,&fptri,true,nargs,args,&parc,pbuf);
	//ldfile=pbuf;

	//Example of a command without any argument
      } else if(!argsdiffer(pbuf,"execute")) {
	//A condition is met where a sufficient set of instructions has been
	//read to allow performing a job. We stop reading more arguments and
	//exit the inner loop
	break;

	//Handling of invalid parameter
      } else {
	fprintf(stderr,"Error: Unknown parameter: '%s'\n",pbuf);
	return 1;
      }
    }

    //Assertion: We are ready to perform a task whether a break statement has
    //been reached in the inner loop or plength<=0. Otherwise the value of
    //plength can be assessed and appropriate actions can be taken

    //All the parameters for the current job have been read, the job can now be
    //performed. Do it here

    //If reading argument from the standard input (interactive mode), we
    //indicate that the job is completed through the standard output
    if(fptri!=-1 && fptra[fptri] == stdin) write(oldstderr,"@DONE\n",6);
    fflush(stdout);
    fflush(stderr);
    //Continue reading other commands for the next job
  }

  close(oout);
  close(eout);
  dup2(oldstdout,1);
  dup2(oldstderr,2);
  close(oldstdout);
  close(oldstderr);

  free(fptra);
  return 0;
}
