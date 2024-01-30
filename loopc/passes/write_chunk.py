from chunk import Chunk
import json
import os

from loop_ast.repr import *


def write_chunk(path: str, module: ModuleValue) -> bool:
    # print(f"WRITING CurDir: {os.getcwd()} Path: {path} | dirname: {os.path.dirname(path)}")

    if (dir := os.path.dirname(path)) != '':
        os.makedirs(dir, exist_ok=True)

    with open(path, "w") as fout:
        fout.write(json.dumps(module.make_json_object_data(), indent=4))
        return True
