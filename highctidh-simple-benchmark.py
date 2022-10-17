#!/usr/bin/env python3

from platform import architecture, machine
from highctidh import ctidh
import timeit

if __name__ == "__main__":
    print("Benchmark for {0}, {1}".format(architecture()[0], machine()))
    sk_gen_511_t = timeit.timeit(
        stmt="ctidh511.csidh_private(alice_private_key)",
        setup="from highctidh import ctidh; ctidh511 = ctidh(511);alice_private_key = ctidh511.private_key()",
        number=1000,
    )

    pk_511_t = timeit.timeit(
        stmt="ctidh511.derive_public_key(alice_private_key)",
        setup="from highctidh import ctidh; ctidh511 = ctidh(511);alice_private_key = ctidh511.private_key(); ctidh511.csidh_private(alice_private_key)",
        number=3,
    )

    dh_511_t = timeit.timeit(
        stmt="ctidh511.dh(alice_private_key, bob_public_key)",
        setup="from highctidh import ctidh; ctidh511 = ctidh(511);alice_private_key = ctidh511.private_key(); ctidh511.csidh_private(alice_private_key); bob_private_key = ctidh511.private_key(); ctidh511.csidh_private(bob_private_key); alice_public_key = ctidh511.derive_public_key(alice_private_key); bob_public_key = ctidh511.derive_public_key(bob_private_key);",
        number=3,
    )

    print("511    Secret key generation time: {0}".format(sk_gen_511_t))
    print("511    Public key generation time: {0}".format(pk_511_t))
    print("511    DH computation time: {0}".format(dh_511_t))

    sk_gen_512_t = timeit.timeit(
        stmt="ctidh512.csidh_private(alice_private_key)",
        setup="from highctidh import ctidh; ctidh512 = ctidh(512);alice_private_key = ctidh512.private_key()",
        number=1000,
    )

    pk_512_t = timeit.timeit(
        stmt="ctidh512.derive_public_key(alice_private_key)",
        setup="from highctidh import ctidh; ctidh512 = ctidh(512);alice_private_key = ctidh512.private_key(); ctidh512.csidh_private(alice_private_key)",
        number=3,
    )

    dh_512_t = timeit.timeit(
        stmt="ctidh512.dh(alice_private_key, bob_public_key)",
        setup="from highctidh import ctidh; ctidh512 = ctidh(512);alice_private_key = ctidh512.private_key(); ctidh512.csidh_private(alice_private_key); bob_private_key = ctidh512.private_key(); ctidh512.csidh_private(bob_private_key); alice_public_key = ctidh512.derive_public_key(alice_private_key); bob_public_key = ctidh512.derive_public_key(bob_private_key);",
        number=3,
    )

    print("512    Secret key generation time: {0}".format(sk_gen_512_t))
    print("512    Public key generation time: {0}".format(pk_512_t))
    print("512    DH computation time: {0}".format(dh_512_t))

    sk_gen_1024_t = timeit.timeit(
        stmt="ctidh1024.csidh_private(alice_private_key)",
        setup="from highctidh import ctidh; ctidh1024 = ctidh(1024);alice_private_key = ctidh1024.private_key()",
        number=1000,
    )

    pk_1024_t = timeit.timeit(
        stmt="ctidh1024.derive_public_key(alice_private_key)",
        setup="from highctidh import ctidh; ctidh1024 = ctidh(1024);alice_private_key = ctidh1024.private_key(); ctidh1024.csidh_private(alice_private_key)",
        number=3,
    )

    dh_1024_t = timeit.timeit(
        stmt="ctidh1024.dh(alice_private_key, bob_public_key)",
        setup="from highctidh import ctidh; ctidh1024 = ctidh(1024);alice_private_key = ctidh1024.private_key(); ctidh1024.csidh_private(alice_private_key); bob_private_key = ctidh1024.private_key(); ctidh1024.csidh_private(bob_private_key); alice_public_key = ctidh1024.derive_public_key(alice_private_key); bob_public_key = ctidh1024.derive_public_key(bob_private_key);",
        number=3,
    )

    print("1024   Secret key generation time: {0}".format(sk_gen_1024_t))
    print("1024   Public key generation time: {0}".format(pk_1024_t))
    print("1024   DH computation time: {0}".format(dh_1024_t))

    sk_gen_2048_t = timeit.timeit(
        stmt="ctidh2048.csidh_private(alice_private_key)",
        setup="from highctidh import ctidh; ctidh2048 = ctidh(2048);alice_private_key = ctidh2048.private_key()",
        number=1000,
    )

    pk_2048_t = timeit.timeit(
        stmt="ctidh2048.derive_public_key(alice_private_key)",
        setup="from highctidh import ctidh; ctidh2048 = ctidh(2048);alice_private_key = ctidh2048.private_key(); ctidh2048.csidh_private(alice_private_key)",
        number=3,
    )

    dh_2048_t = timeit.timeit(
        stmt="ctidh2048.dh(alice_private_key, bob_public_key)",
        setup="from highctidh import ctidh; ctidh2048 = ctidh(2048);alice_private_key = ctidh2048.private_key(); ctidh2048.csidh_private(alice_private_key); bob_private_key = ctidh2048.private_key(); ctidh2048.csidh_private(bob_private_key); alice_public_key = ctidh2048.derive_public_key(alice_private_key); bob_public_key = ctidh2048.derive_public_key(bob_private_key);",
        number=3,
    )

    print("2048   Secret key generation time: {0}".format(sk_gen_2048_t))
    print("2048   Public key generation time: {0}".format(pk_2048_t))
    print("2048   DH computation time: {0}".format(dh_2048_t))
