LIB := libgnugocontroller.a
SRCS := go_controller.c
OBJS := $(SRCS:.c=.o)
LIBS := $(LIB)

TEST_BIN := test
TEST_SRCS := test.c
TEST_OBJS := $(TEST_SRCS:.c=.o)

CFLAGS := -Wall -g -O2
LDFLAGS := -L. -lgnugocontroller

all: $(LIB) $(TEST_BIN)

$(TEST_BIN): $(LIB) $(TEST_OBJS)
	gcc -o $@ $(TEST_OBJS) $(LDFLAGS)

$(LIB): $(OBJS)
	ar rcs $@ $^
	ranlib $@

clean:
	-rm $(OBJS) $(LIB) $(TEST_OBJS) $(TEST_BIN)
