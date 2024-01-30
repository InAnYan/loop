import sys

from util.default_error_listener import DefaultErrorListener
from full_passes import full_passes


def main():
    if len(sys.argv) != 2:
        print("error: wrong arguments count")
        print("usage: main.py source_path")
        sys.exit(2)

    in_path = sys.argv[1]
    if not full_passes(DefaultErrorListener(), in_path):
        sys.exit(3)


if __name__ == "__main__":
    #full_passes(DefaultErrorListener(), "loopc/sandbox.loop")
    main()
