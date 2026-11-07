#!/usr/bin/env python3
"""check_package_links.py DRAWER: every relative link in the drawer resolves.

Walks every Markdown file under DRAWER (the release drawer the package
target builds) and checks each relative link and image in it: inline
links and images, reference definitions, and HTML <a href> and <img src>.
A link passes when the file or directory it names exists inside DRAWER.
Links with a scheme (https:, mailto: and so on) and links to an anchor in
the same file are not checked; a "#fragment" or "?query" on a relative
link is ignored.  Code spans and fenced code blocks are skipped.

Prints each broken link as FILE:LINE: TARGET (why) and exits 1 if there
are any, else prints a one-line count and exits 0.

  python3 check_package_links.py build-canvas-skies/package/CanvasSkies
"""

import re
import sys
from pathlib import Path
from urllib.parse import unquote

INLINE = re.compile(r"!?\[(?:[^\[\]]|\[[^\]]*\])*\]\(\s*(<[^>]*>|[^)\s]+)"
                    r"(?:\s+(?:\"[^\"]*\"|'[^']*'|\([^)]*\)))?\s*\)")
REFDEF = re.compile(r"^\s{0,3}\[[^\]]+\]:\s*(<[^>]*>|\S+)")
HTML = re.compile(r"<(?:a|img)\b[^>]*?\b(?:href|src)\s*=\s*[\"']([^\"']*)[\"']",
                  re.IGNORECASE)
SCHEME = re.compile(r"^[A-Za-z][A-Za-z0-9+.-]*:")
FENCE = re.compile(r"^\s{0,3}(`{3,}|~{3,})")
CODESPAN = re.compile(r"(`+)(?:(?!\1).)+?\1")


def targets(text):
    """(line number, target) for every link and image in a Markdown text."""
    fence = None
    for number, line in enumerate(text.splitlines(), 1):
        m = FENCE.match(line)
        if fence:
            if m and m.group(1)[0] == fence[0] and len(m.group(1)) >= len(fence):
                fence = None
            continue
        if m:
            fence = m.group(1)
            continue
        if line.startswith("    ") or line.startswith("\t"):
            continue                    # an indented code block
        line = CODESPAN.sub("", line)
        for pattern in (INLINE, REFDEF, HTML):
            for found in pattern.finditer(line):
                yield number, found.group(1)


def check(root):
    root = root.resolve()
    broken = []
    files = sorted(p for p in root.rglob("*")
                   if p.is_file() and p.suffix.lower() in (".md", ".markdown"))
    checked = 0
    for md in files:
        text = md.read_text(encoding="utf-8", errors="replace")
        for number, target in targets(text):
            target = target.strip()
            if target.startswith("<") and target.endswith(">"):
                target = target[1:-1].strip()
            if not target or target.startswith("#") or SCHEME.match(target):
                continue
            path = unquote(re.split(r"[#?]", target, maxsplit=1)[0])
            if not path:
                continue
            checked += 1
            where = f"{md.relative_to(root)}:{number}: {target}"
            if path.startswith("/"):
                broken.append(f"{where} (absolute path)")
                continue
            resolved = (md.parent / path).resolve()
            if resolved != root and root not in resolved.parents:
                broken.append(f"{where} (outside the package)")
            elif not resolved.exists():
                broken.append(f"{where} (missing)")
    return files, checked, broken


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__.strip().splitlines()[-1].strip())
    root = Path(sys.argv[1])
    if not root.is_dir():
        sys.exit(f"{root}: not a directory")
    files, checked, broken = check(root)
    for line in broken:
        print(line)
    if broken:
        print(f"{len(broken)} of {checked} relative links in {len(files)} "
              f"Markdown files do not resolve inside {root}")
        sys.exit(1)
    print(f"{checked} relative links in {len(files)} Markdown files all "
          f"resolve inside {root}")


if __name__ == "__main__":
    main()
