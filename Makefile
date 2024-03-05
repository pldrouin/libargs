CFLAGS		+=	
STCOM		=	ar rcs

LIB		=	libargs.so
STLIB		=	libargs.a
SSTLIB		=	libargs.sa
CSRCS		=	$(wildcard *.c)
OBJS		=	$(CSRCS:.c=.o)
OBJSS		=	$(CSRCS:.c=.os)
CLHD		=	$(CSRCS:.c=.h)
ODEP		=	$(OBJS:.o=.d)

all: $(SSTLIB) $(STLIB) $(LIB)

$(SSTLIB): $(OBJS)
	rm -rf $@ && $(STCOM) $@ $^

$(STLIB): $(OBJSS)
	rm -rf $@ && $(STCOM) $@ $^

$(LIB): $(OBJS)
	$(CC) $(CFLAGS) -shared $(LIBFLAGS) -o $@ $^

clear: clean
	rm -f $(LIB) $(STLIB) $(SSTLIB)

bindist: $(LIB) clean
	rm -f $(CSRCS) Makefile

clean:
	rm -f *.d
	rm -f *.o
	rm -f *.os

$(ODEP): %.d: %.c %.h
	@echo "Generating dependency file $@"
	@set -e; rm -f $@
	@$(CC) -M $(CFLAGS) $< > $@.tmp
	@sed 's,\($*\)\.o[ :]*,\1.o \1.os $@ : ,g' < $@.tmp > $@
	@rm -f $@.tmp

include $(ODEP)

$(OBJS): %.o: %.c
	 $(CC) -c -o $@ $(CFLAGS) -fPIC $<

$(OBJSS): %.os: %.c
	 $(CC) -c -o $@ $(CFLAGS) $<
