CFLAGS		+=	-fPIC

LIB		=	libargs.so
CSRCS		=	$(wildcard *.c)
OBJS		=	$(CSRCS:.c=.o)
CLHD		=	$(CSRCS:.c=.h)
ODEP		=	$(OBJS:.o=.d)

$(LIB): $(OBJS)
	$(CC) $(CFLAGS) -shared $(LIBFLAGS) -o $@ $^

clear: clean
	rm -f $(LIB)

bindist: $(LIB) clean
	rm -f $(CSRCS) Makefile

clean:
	rm -f *.d
	rm -f *.o

$(ODEP): %.d: %.c %.h
	@echo "Generating dependency file $@"
	@set -e; rm -f $@
	@$(CC) -M $(CFLAGS) $< > $@.tmp
	@sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@
	@rm -f $@.tmp

include $(ODEP)

$(OBJS): %.o: %.c
	 $(CC) -c -o $@ $(CFLAGS) $<
