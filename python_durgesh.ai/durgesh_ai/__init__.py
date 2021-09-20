import os
import sys


from .durgesh_ai_cffi import ffi as _ffi

def open_dll():
    dlldir = os.path.abspath(os.path.dirname(__file__))
    return _ffi.dlopen(os.path.join(dlldir, "libdurgesh_ai.so"))

_c = open_dll()

class Model(object):

    def __init__(self, model_path,flag):
        self._handle = _c.durgesh_ai_model_new(model_path.encode('utf-8'),flag)

    def __del__(self):
        _c.durgesh_ai_model_free(self._handle)

    def durgesh_ai_model_find_word(self, word):
        return _c.durgesh_ai_model_find_word(self._handle, word.encode('utf-8'))

    def GetKwsLoadStatus(self):
        return _c.durgesh_ai_model_check_kws_load_status(self._handle)

    def GetAsrLoadStatus(self):
        return _c.durgesh_ai_model_check_asr_load_status(self._handle)

class SpkModel(object):

    def __init__(self, model_path):
        self._handle = _c.durgesh_ai_spk_model_new(model_path.encode('utf-8'))

    def __del__(self):
        _c.durgesh_ai_spk_model_free(self._handle)

class KaldiRecognizer(object):

    def __init__(self, *args):
        if len(args) == 2:
            self._handle = _c.durgesh_ai_recognizer_new(args[0]._handle, args[1])
        elif len(args) == 3 and type(args[1]) is SpkModel:
            self._handle = _c.durgesh_ai_recognizer_new_spk(args[0]._handle, args[1]._handle, args[2])
        elif len(args) == 3 and type(args[2]) is str:
            self._handle = _c.durgesh_ai_recognizer_new_grm(args[0]._handle, args[1], args[2].encode('utf-8'))
        elif len(args) == 4 and type(args[1]) is SpkModel:
            self._handle = _c.durgesh_ai_recognizer_new_grm_spk(args[0]._handle, args[1]._handle, args[2], args[3].encode('utf-8'))
        else:
            raise TypeError("Unknown arguments")

    def __del__(self):
        _c.durgesh_ai_recognizer_free(self._handle)

    def AcceptWaveform(self, data, flag):
        return _c.durgesh_ai_recognizer_accept_waveform(self._handle, data, len(data),flag)

    def Result(self):
        return _ffi.string(_c.durgesh_ai_recognizer_result(self._handle)).decode('utf-8')

    def KwsResult(self):
        return _ffi.string(_c.durgesh_ai_recognizer_kws_result(self._handle)).decode('utf-8')

    def PartialResult(self):
        return _ffi.string(_c.durgesh_ai_recognizer_partial_result(self._handle)).decode('utf-8')

    def FinalResult(self):
        return _ffi.string(_c.durgesh_ai_recognizer_final_result(self._handle)).decode('utf-8')

    def CheckWakeAvailability(self,word):
        return _c.durgesh_ai_recognizer_check_wakeword_status(self._handle,word.encode('utf-8'))


def SetLogLevel(level):
    return _c.durgesh_ai_set_log_level(level)


def GpuInit():
    _c.durgesh_ai_gpu_init()

def GpuThreadInit():
    _c.durgesh_ai_gpu_thread_init()
