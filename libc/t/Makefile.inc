OBJDIR=obj
LIBDIR=../lib
CC=gcc
RANLIB=ranlib
VPATH=.:../../
TESTDIR=../
TEST=$(TESTDIR)/$(TESTNAME).t
OBJS=$(CFILES:%.c=%.o)
NOBUILTINS=$(NOBUILTIN:%=-fno-builtin-%)

all: $(TEST)

$(TEST): $(OBJS)
	$(CC) -DTEST -o $@ $(OBJS)

$(OBJS): $(HEADERS) Makefile
$(OBJS): %.o: %.c
	$(CC) -o $@ -c $< $(NOBUILTINS)

.PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f $(TEST)
