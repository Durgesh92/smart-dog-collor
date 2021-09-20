#!/usr/bin/env python3

import os
from cffi import FFI

durgesh_ai_root=os.environ.get("DURGESH_AI_SOURCE", "..")
cpp_command = "cpp " + durgesh_ai_root + "/src_durgesh/durgesh_ai.h"

ffibuilder = FFI()
ffibuilder.set_source("durgesh_ai.durgesh_ai_cffi", None)
ffibuilder.cdef(os.popen(cpp_command).read())

if __name__ == '__main__':
    ffibuilder.compile(verbose=True)
