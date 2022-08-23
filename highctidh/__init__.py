#!/usr/bin/env python3

import ctypes
import ctypes.util
from ctypes import LibraryLoader

class ctidh(object):
    def __init__(self, field_size):
        self._ctidh_sizes = (511, 512, 1024, 2048)
        self.field_size = field_size
        if self.field_size not in self._ctidh_sizes:
            raise Exception(f"Unsupported size: {repr(self.field_size)}")
        if self.field_size == 511:
            self.pk_size = 64
            self.sk_size = 74
        elif self.field_size == 512:
            self.pk_size = 64
            self.sk_size = 74
        elif self.field_size == 1024:
            self.pk_size = 128
            self.sk_size = 130
        elif self.field_size == 2048:
            self.pk_size = 256
            self.sk_size = 231
        self.base = b'\x00' * self.pk_size
        class private_key(ctypes.Structure):
            __slots__ = [ 'e', ]
            _fields_ = [ ('e', ctypes.c_ubyte * self.sk_size), ]
        self.private_key = private_key
        class public_key(ctypes.Structure):
            __slots__ = [ 'A', ]
            _fields_ = [ ('A', ctypes.c_ubyte * self.pk_size) ]
        self.public_key = public_key
        self._lib = ctypes.CDLL(ctypes.util.find_library('highctidh_' + str(self.field_size)))
        csidh_private = self._lib.__getattr__('highctidh_'+str(self.field_size)+'_csidh_private')
        csidh_private.restype = None
        csidh_private.argtypes = [ctypes.POINTER(self.private_key)]
        self.csidh_private = csidh_private
        csidh = self._lib.__getattr__('highctidh_'+str(self.field_size)+'_csidh')
        csidh.restype = bool
        csidh.argtypes = [ctypes.POINTER(self.public_key), ctypes.POINTER(self.public_key), ctypes.POINTER(self.private_key)]
        self.csidh = csidh
        validate = self._lib.__getattr__('highctidh_'+str(self.field_size)+'_validate')
        validate.restype = bool
        validate.argtypes = [ctypes.POINTER(self.public_key)]
        self.validate = validate
    def private_key_from_hex(self, h):
       return ctypes.cast(ctypes.create_string_buffer(h),
               ctypes.POINTER(self.private_key)).contents
    def public_key_from_hex(self, h):
       return ctypes.cast(ctypes.create_string_buffer(h),
               ctypes.POINTER(self.public_key)).contents

