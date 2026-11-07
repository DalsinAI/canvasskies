# Host-side tests of the game's small arithmetic: run with
#   make -C tests/host
# They need only a host C compiler, not SDL or the Amiga tool chain.

CC ?= cc
CFLAGS := -std=gnu99 -O1 -Wall -Wextra -Werror
TESTS := test_ticks test_simclock

.PHONY: all clean
all: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

%: %.c ../../src/modern/canvas_ticks.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TESTS)
