#!/usr/bin/env python3

import ctypes
import ctypes.util
from ctypes import LibraryLoader
import hashlib

class InvalidFieldSize(Exception):
    """ Raised when a field is not one of (511, 512, 1024, 2048) """
    pass

class InvalidPublicKey(Exception):
    """ Raised when a public key is not validated by the validate() function. """
    pass

class CSIDHError(Exception):
    """ Raised when csidh() fails to return True. """
    pass

class LibraryNotFound(Exception):
    """ Raised when the shared library cannot be located and opened. """
    pass

class ctidh(object):
    def __init__(self, field_size):
        self._ctidh_sizes = (511, 512, 1024, 2048)
        self.field_size = field_size
        if self.field_size not in self._ctidh_sizes:
            raise InvalidFieldSize(f"Unsupported size: {repr(self.field_size)}")
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
        class private_key(ctypes.Structure):
            __slots__ = [ 'e', ]
            _fields_ = [ ('e', ctypes.c_ubyte * self.sk_size), ]
        self.private_key = private_key
        class public_key(ctypes.Structure):
            __slots__ = [ 'A', ]
            _fields_ = [ ('A', ctypes.c_ubyte * self.pk_size) ]
        self.public_key = public_key
        self.base = self.public_key()
        try:
            self._lib = ctypes.CDLL(ctypes.util.find_library('highctidh_' + str(self.field_size)))
        except OSError as e:
            print('Unable to load highctidh_' + str(self.field_size) + '.so'.format(e))
            raise LibraryNotFound
        csidh_private = self._lib.__getattr__('highctidh_'+str(self.field_size)+'_csidh_private')
        csidh_private.restype = None
        csidh_private.argtypes = [ctypes.POINTER(self.private_key)]
        self.csidh_private = csidh_private
        csidh = self._lib.__getattr__('highctidh_'+str(self.field_size)+'_csidh')
        csidh.restype = bool
        csidh.argtypes = [ctypes.POINTER(self.public_key), ctypes.POINTER(self.public_key), ctypes.POINTER(self.private_key)]
        self._csidh = csidh
        validate = self._lib.__getattr__('highctidh_'+str(self.field_size)+'_validate')
        validate.restype = bool
        validate.argtypes = [ctypes.POINTER(self.public_key)]
        self._validate = validate
    def private_key_from_bytes(self, h):
        if type(h) is not bytes:
            raise Exception("Private key is not bytes")
        return ctypes.cast(ctypes.create_string_buffer(h),
               ctypes.POINTER(self.private_key)).contents
    def public_key_from_bytes(self, h):
       if type(h) is not bytes:
            raise Exception("Private key is not bytes")
       return ctypes.cast(ctypes.create_string_buffer(h),
               ctypes.POINTER(self.public_key)).contents
    def private_key_from_hex(self, h):
       if type(h) is not str:
           raise Exception("Private key is not str")
       h = bytes.fromhex(h)
       return ctypes.cast(ctypes.create_string_buffer(h),
               ctypes.POINTER(self.private_key)).contents
    def public_key_from_hex(self, h):
       if type(h) is not str:
           raise Exception("Private key is not str")
       h = bytes.fromhex(h)
       return ctypes.cast(ctypes.create_string_buffer(h),
               ctypes.POINTER(self.public_key)).contents
    def validate(self, pk):
        """ self._validate returns 1 if successful, 0 if invalid. """
        if self._validate(pk):
            return True
        else:
            raise InvalidPublicKey
    def csidh(self, pk0, pk1, sk):
        if self._csidh(pk0, pk1, sk):
            return True
        else:
            raise CSIDHError
    def generate_secret_key(self):
        """ Generate a secret key *sk*, return it. """
        sk = self.private_key()
        self.csidh_private(sk)
        return sk
    def derive_public_key(self, sk):
        """ Given a secret key *sk*, return the corresponding public key *pk*. """
        pk = self.public_key()
        self.csidh(pk, self.base, sk)
        return pk
    def dh(self, sk, pk, _hash=lambda shared,size:
            hashlib.shake_256(bytes(shared)).digest(length=size)):
        """
        This is a classic Diffie-Hellman function which takes a secret key
        *sk* and a public key *pk* and computes a random element. It then
        computes a uniformly random bit string from the random element using a
        variable length hash function. The returned value is a bytes() object.
        The size of the hash output is dependent on the field size. The *_hash*
        may be overloaded as needed.
        """
        shared_key = self.public_key()
        self.csidh(shared_key, pk, sk)
        return _hash(bytes(shared_key), self.pk_size)
    def blind(self, blinding_factor, pk):
        blinded_key = self.public_key()
        self.csidh(blinded_key, pk, blinding_factor)
        return blinded_key
