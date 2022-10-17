#!/bin/sh -x

export LD_LIBRARY_PATH=.
export PYTHONPATH=$PYTHONPATH:`pwd`
make libhighctidh.so HIGHCTIDH_PORTABLE=1
make testrandom test511 test512 test1024 test2048
python3 tests/test_highctidh.py
./test511
./test512
./test1024
./test2048
./testrandom
python3 highctidh-simple-benchmark.py
