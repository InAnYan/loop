import os
import re
from passes.read_file import read_loop_file

from looprun import run_and_compile_loop

TEST_FOLDER = "looptest"

if __name__ == "__main__":
    for i, path in enumerate(os.listdir(os.path.join(TEST_FOLDER, "fail"))):
        if not path.endswith(".loop"):
            continue

        if p := run_and_compile_loop(
            os.path.join(TEST_FOLDER, "fail", path),
            silent_el=True,
            silent_stderr=True,
        ):
            if p.returncode != 0:
                res = True
            else:
                res = False
        else:
            res = True

        print(f"{'PASS' if res else 'FAIL'}: {path}")

    for i, path in enumerate(os.listdir(os.path.join(TEST_FOLDER, "pass"))):
        if not path.endswith(".loop"):
            continue

        file = read_loop_file(os.path.join(TEST_FOLDER, "pass", path)).contents

        if p := run_and_compile_loop(
            os.path.join(TEST_FOLDER, "pass", path),
            silent_stderr=True,
            pipe_out=True,
            silent_el=True,
        ):
            if p.returncode == 0:
                read = (
                    p.stdout.read().decode().replace("\r\n", "\n").replace("\r", "\n")
                )
                should = "".join(
                    match[3:] + "\n" for match in re.findall(r"// [^\n]*", file)
                )
                if read == should:
                    res = "PASS"
                else:
                    res = "FAIL (output mismatch)"
            else:
                res = f"FAIL ({p.returncode})"
        else:
            res = "FAIL (compile)"

        print(f"{res}: {path}")
