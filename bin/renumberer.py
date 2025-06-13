#!/usr/bin/env python3
# -*- coding: utf-8; py-indent-offset:4 -*-
###############################################################################
import argparse
from collections.abc import Sequence
import logging
import os
import os.path
import subprocess
import sys


PARGS = str | Sequence[str] | None

UTF8 = "utf-8"
CMD_GIT = "git"
CMD_MV = "mv"
CMD_UNTRACKED = "git ls-files --others --exclude-standard"


def check_untracked(path: str) -> bool:
    completed = subprocess.run(
        f"{CMD_UNTRACKED} {path}",
        capture_output=True,
        shell=True,
        encoding=UTF8,
    )
    try:
        completed.check_returncode()  # raises CalledProcessError if non 0
    except subprocess.CalledProcessError as e:
        logging.error(f"FAILED: {cmd = }")
        logging.error(f"Exception {e = }")
        return False

    if completed.stdout:
        logging.error("-- CANNOT RUN - untracked files are present")
        logging.error(completed.stdout)
        return False

    return True


def mv_chapter(oldpath: str, newpath: str, git: bool) -> bool:
    cmd = f"{CMD_MV} {oldpath} {newpath}"
    if git:
        cmd = f"{CMD_GIT} {cmd}"

    completed = subprocess.run(
        cmd,
        capture_output=True,
        shell=True,
        encoding=UTF8,
    )
    try:
        completed.check_returncode()  # raises CalledProcessError if non 0
    except subprocess.CalledProcessError as e:
        logging.error(f"FAILED: {cmd = }")
        logging.error(f"Exception {e = }")
        return False

    return True

def run(args: PARGS = None) -> None:
    pargs = parse_args(args)

    if not pargs.no_git:
        logging.info("-- CHECKING UNTRACKED FILES")
        if not check_untracked(pargs.path):
            sys.exit(1)

        logging.info("-- No untracked files ... proceeding")

    chap_lo = f"{pargs.chaplo:04d}"
    chap_hi = f"{pargs.chaphi:04d}"

    chap_cur = 0
    part_seen = False
    gap_next = pargs.gap

    for chapter in sorted(os.listdir(pargs.path)):
        if not chapter[0].isdigit():  # skip files other than chaps
            continue

        # build full path and report progress
        oldpath = os.path.join(pargs.path, chapter)
        logging.info(f"-- Checking {oldpath}")

        # get actual chapter number
        chapnum, chapname = chapter.split("-", maxsplit=1)
        # string compare - chap_lo/hi were already padded to the left
        if not (chap_lo <= chapnum <= chap_hi):
            continue

        ichapnum = int(chapnum)  # get int number for arithmetic

        part_seen = chapnum.endswith(pargs.partnum)
        if part_seen:
            gap = pargs.gap - 1
            gap_next = 1
        else:
            gap = gap_next
            gap_next = pargs.gap

        newchapnum = chap_cur + gap
        # check if gap is larger/smaller than requested gap
        if ichapnum == newchapnum:
            chap_cur = ichapnum  # it is not, record current chap num
            continue

        # must renumber
        chap_cur = newchapnum  # use last chap pos + gap
        newchap = f"{chap_cur:04d}-{chapname}"  # to string
        newpath = os.path.join(pargs.path, newchap)  # full path

        logging.info(f"Renumbering: {oldpath} => {newpath}")
        if not mv_chapter(oldpath, newpath, git=not pargs.no_git):
            logging.error("Exiting to prevent further errors")
            sys.exit(1)


# ---------------------------------------------------------------------------
def parse_args(args: PARGS = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description=("Renumberer of chapters to expected gap"),
    )

    pgroup = parser.add_argument_group(title="Configuration")
    pgroup.add_argument(
        "--path", default="src/en",
        help="Path to look for files to renumber",
    )
    pgroup.add_argument(
        "--gap", type=int, default=10,
        help="Gap in between chapters",
    )

    pgroup.add_argument(
        "--chaplo", type=int, default=10,
        help="Lowest chapter number to consider",
    )
    pgroup.add_argument(
        "--chaphi", type=int, default=899,
        help="Highest chapter number to consider",
    )

    pgroup.add_argument(
        "--partnum", default="9",
        help="Part separator offset to chapter",
    )

    pgroup.add_argument(
        "--no-git", action='store_true',
        help="Do not use git to rename files",
    )

    pgroup = parser.add_argument_group(title="Verbosity")
    pgroup.add_argument(
        "--logout",
        "-lo",
        action="store_true",
        help="Log to stdout instead of stderr",
    )
    pgrpex = pgroup.add_mutually_exclusive_group()
    pgrpex.add_argument("--no-logging", action="store_true", help="Report nothing")
    pgrpex.add_argument("--error", action="store_true", help="Do only report errors")
    pgrpex.add_argument(
        "--info",
        "-info",
        action="store_true",
        help="Info level (DEFAULT)",
    )
    pgrpex.add_argument(
        "--debug",
        "--verbose",
        "-v",
        action="store_true",
        help="Increase verbosity level",
    )

    if isinstance(args, str):
        args = args.split()

    pargs = parser.parse_args(args)

    # configure logging
    if not pargs.no_logging:
        vlevel = logging.INFO
        if pargs.error:
            vlevel = logging.ERROR
        elif pargs.debug:
            vlevel = logging.DEBUG

        stream = sys.stderr if not pargs.logout else sys.stdout
        logging.basicConfig(
            format="%(asctime)s,%(levelname)-8s,%(message)s",
            level=vlevel,
            stream=stream,
            datefmt="%Y-%m-%d %H:%M:%S",
        )

    return pargs


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    run()
