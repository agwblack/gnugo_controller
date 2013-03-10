LIB := libgnugocontroller.a
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

CFLAGS := -Wall -g -O2
LOADLIBES := 	-L./

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $@ $^
	ranlib $@

clean:
	rm $(OBJS) $(LIB)
