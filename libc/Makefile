include ../Makefile.inc

HEADERS=stdio.h stdlib.h
OBJFILES=snprintf.o string.o intoa.o
OBJDIR=obj
LIBDIR=../lib
VPATH=.
TESTDIR=t
INCLUDE=../libc $(GCCINCLUDEDIRS)

INCLUDES=$(INCLUDE:%=-I%)
OBJS=$(OBJFILES:%=$(OBJDIR)/%)
TESTS=$(OBJFILES:%.o=$(TESTDIR)/%.t)
ARFLAGS=rcs

all: $(LIBDIR)/libc.a

test:
	@echo -e "\033[34;1mMaking tests\033[0m"
	@cd t && ./make-tests
	@echo -e "\033[34;1mRunning tests\033[0m"
	@cd t && ./run-tests

$(LIBDIR)/libc.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(OBJS): $(HEADERS) Makefile
$(OBJS): $(OBJDIR)/%.o: %.c
	$(CC) -o $@ -c $< -fno-builtin -nostdinc $(INCLUDES) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(LIBDIR)/libc.a
	rm -f $(OBJS)

clean-tests:
	cd t && ./clean-tests	
