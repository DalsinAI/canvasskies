# Host-side checks, run from the repository root:
#
#   make -f tests/host/Makefile check
#   make -f tests/host/Makefile check PACKAGE=path/to/CanvasSkies
#
# PACKAGE is the release drawer built by
# `make -f ports/amigaos3/Makefile.canvas package`.

PYTHON ?= python3
PACKAGE ?= build-canvas-skies/package/CanvasSkies

.PHONY: check check-package-links

check: check-package-links

check-package-links:
	$(PYTHON) ports/amigaos3/check_package_links.py $(PACKAGE)
