include Makefile.inc

OBJFILES=snprintf.o string.o intoa.o malloc.o unistd.o joo.o stdio.o
OBJDIR=obj
LIBDIR=../lib
VPATH=.
TESTDIR=t
INCLUDE=include $(GCCINCLUDEDIRS)

INCLUDES=$(INCLUDE:%=-I%)
OBJS=$(OBJFILES:%=$(OBJDIR)/%)
TESTS=$(OBJFILES:%.o=$(TESTDIR)/%.t)
ARFLAGS=rcs

all: $(LIBDIR)/libc.a $(LIBDIR)/crt0start.o

test:
	@echo -e "\033[34;1mMaking tests\033[0m"
	@cd t && ./make-tests
	@echo -e "\033[34;1mRunning tests\033[0m"
	@cd t && ./run-tests

$(LIBDIR)/libc.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(LIBDIR)/crt0start.o: $(OBJDIR)/crt0start.o
	cp $(OBJDIR)/crt0start.o $(LIBDIR)/crt0start.o

$(OBJS) $(OBJDIR)/crt0start.o: $(HEADERS) Makefile
$(OBJS) $(OBJDIR)/crt0start.o: $(OBJDIR)/%.o: %.c
	$(CC) -o $@ -c $< -fno-builtin -nostdinc $(INCLUDES) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(LIBDIR)/libc.a
	rm -f $(OBJS)

clean-tests:
	cd t && ./clean-tests	
