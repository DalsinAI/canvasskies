# Host-side checks, run as: make -C tests/host check
#
# check            the deterministic C tests (test_*.c)
# package-links    every relative link in the release drawer resolves:
#                  make -C tests/host package-links [PACKAGE=dir]
#                  (PACKAGE defaults to the drawer the package target builds)

CC ?= cc
CFLAGS ?= -std=gnu99 -O2 -Wall -Wextra -Werror -I../../src/modern
PYTHON ?= python3
PACKAGE ?= ../../build-canvas-skies/package/CanvasSkies
TESTS := $(patsubst %.c,build/%,$(wildcard test_*.c))

.PHONY: check package-links clean

check: $(TESTS)
	@for t in $(TESTS); do echo "== $$t"; ./$$t || exit 1; done

build/%: %.c $(wildcard ../../src/modern/canvas_*.h)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $<

package-links:
	$(PYTHON) ../../ports/amigaos3/check_package_links.py $(PACKAGE)

clean:
	rm -rf build
