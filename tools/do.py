#! /usr/bin/env python3

"""
Moirai project manager.
This script is meant for building, dependency handling and cleaning build artifacts.
In general it handles all project actions whose commands are long.
"""

# TODO: Implement verbosity levels
# TODO: Implement colours for different output messages.
# TODO: Ensure build, bin, etc. folders exist and create them if they do not.

import argparse
import pathlib
import time
import shutil
import subprocess
import sys


PROJECT_NAME = "moirai"
NECESSARY_DIRECTORIES = [
    "bin",
    "build",
    "external/raylib",
    "external/catch2",
]


def configure_argument_parser(parser):
    """
    Configures the input parser object \
    with all the command line that the program takes.

    param parser: the argparse.ArgumentParser object to configure.
    """

    parser.add_argument(
        "-c", "--clean", action="store_true", help="ACTION: Clean all build artifacts"
    )
    parser.add_argument(
        "-b",
        "--build",
        action="store_true",
        help="ACTION: Build project, in debug mode",
    )
    parser.add_argument(
        "--tests",
        action="store_true",
        help="MODIFIER: Specified actions will also apply to unit tests",
    )

    # TODO: Implement verbosity levels.


def create_required_project_directories():
    """
    Creates all directories required by the project that are not present by default (defined in NECESSARY_DIRECTORIES). \
    Will not fail if directories already exist. \
    Techinically not needed since cmake creates the directories if they are not found.
    """

    main_project_dir_path = pathlib.Path(__file__).resolve().parents[1]

    for directory in NECESSARY_DIRECTORIES:
        path = main_project_dir_path.joinpath(directory)
        try:
            if not path.exists():
                print(
                    "[DO] PROJECT_ROOT/{0} Does not exist. Creating...".format(
                        directory
                    )
                )
                path.mkdir()

        except Exception as e:
            raise Exception("[DO]: Failed to create directories. Reason: %s" % e)


def format_time(seconds):
    """
    Returns a string representing the given number of seconds in a human readable format.

    pram seconds: a float value representing the number of seconds to format.
    returns: A string containing the formatted time.
    """

    if abs(seconds) < 0.001:
        return "0s"
    elif abs(seconds) < 1.0:
        # xxx ms
        milliseconds = round(seconds * 1000.0, 0)
        return "{:.0f}".format(milliseconds) + "ms"
    elif abs(seconds) < 60.0:
        # S s
        return "{:.0f}".format(seconds) + "s"
    elif abs(seconds) < 3600.0:
        # M min SS s
        return (
            "{:.0f}".format(seconds // 60)
            + "min "
            + "{:02.0f}".format(seconds % 60)
            + "s"
        )
    else:
        # H h MM min SS s
        return (
            "{:.0f}".format(seconds // 3600)
            + "h "
            + "{:02.0f}".format((seconds // 60) % 60)
            + "min "
            + "{:02.0f}".format(seconds % 60)
            + "s"
        )

    # return "format_time error: could not format value = %s" % seconds


def delete_directory_contents(dir_path):
    """
    Deletes all files and directories in the specified path.

    param dir_path: libparth.Path object representing the parth of the directory \
    whose contents we want to delete.
    """
    print(
        "[DO][CLEAN]: Deleting all content from directory [",
        dir_path.parent.name + "/" + dir_path.name,
        "]",
    )

    for file_path in pathlib.Path.iterdir(dir_path):
        try:
            if pathlib.Path.is_file(file_path) or pathlib.Path.is_symlink(file_path):
                pathlib.Path.unlink(file_path)
                # print("  Deleting file/symling", file_path) # TODO: Implement verbosity levels.
            elif pathlib.Path.is_dir(file_path):
                shutil.rmtree(file_path)
                # print("  Deleting directory", file_path) # TODO: Implement verborisy levels.
        except Exception as e:
            raise Exception(
                "[DO][CLEAN]: Failed to delete %s. Reason: %s" % (file_path, e)
            )


def perform_clean_action():
    """
    Cleans all build artifacts from the project.
    """

    print("[DO][CLEAN]: Cleaning all previous builds")
    clean_action_start_time = time.time()

    main_project_dir_path = pathlib.Path(__file__).resolve().parents[1]

    for directory in reversed(NECESSARY_DIRECTORIES):
        path = main_project_dir_path.joinpath(directory)
        delete_directory_contents(path)

    clean_action_finish_time = time.time() - clean_action_start_time
    print("[DO][CLEAN]: Finished! Took:", format_time(clean_action_finish_time))


def perform_build_action(tests):
    """
    Build the project.

    param tests: bool that indicates whether to build the tests.
    """

    print("[DO][BUILD]: Building the {0} project".format(PROJECT_NAME))
    build_action_start_time = time.time()

    main_project_dir_path = pathlib.Path(__file__).resolve().parents[1]
    build_dir_path = main_project_dir_path.joinpath("build")

    if tests:
        build_tests_param = "ON"
    else:
        build_tests_param = "OFF"

    cmake_configure_result = subprocess.run(
        [
            "cmake",
            "-S",
            main_project_dir_path,
            "-B",
            build_dir_path,
            "-G",
            "Unix Makefiles",
            "-DMOIRAI_BUILD_TESTS:BOOL=" + build_tests_param,
        ]
    )

    if cmake_configure_result.returncode != 0:
        raise Exception("[DO][BUILD]: Error during the cmake configuration phase.")

    cmake_build_result = subprocess.run(
        ["cmake", "--build", build_dir_path, "--parallel", "10"]
    )

    if cmake_build_result.returncode != 0:
        raise Exception("[DO][BUILD]: Error during the compilation/linking step.")

    build_action_finish_time = time.time() - build_action_start_time
    print("[DO][BUILD]: Finished! Took", format_time(build_action_finish_time))


def main():
    """
    Main function. Checks command lines arguments passed to the program \
    and calls corresponding actions.
    """

    print("[DO]: Invoked the project manager script")
    parser = argparse.ArgumentParser(
        description="Script to manage the {0} project.".format(PROJECT_NAME)
    )
    configure_argument_parser(parser)
    args = parser.parse_args()

    if not args.clean and not args.build:
        parser.error("No action requested, please select one of the available actions")

    try:

        create_required_project_directories()

        if args.clean:
            perform_clean_action()

        if args.build:
            perform_build_action(args.tests)

    except Exception as e:
        print(e)
        print("[DO] Terminating program...")
        sys.exit(1)

    print("[DO]: Finished!")


if __name__ == "__main__":
    main()
