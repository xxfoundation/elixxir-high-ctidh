Run the unit tests:

   python3 -m unittest -v

Build and install the shared library, and then build and install the Python
module:

    ./autogen
    make libhighctidh_511.so libhighctidh_512.so libhighctidh_1024.so libhighctidh_2048.so
    sudo make install
    sudo ldconfig
    python3 -m build
    pip install dist/highctidh-*-py3-none-any.whl

Alternatively, rather than using `python3 -m build` pip may be used:

    pip install . # or sudo pip install .

Use the Python module:

    $ python3
    Python 3.10.4 (main, Jun 29 2022, 12:14:53) [GCC 11.2.0] on linux
    Type "help", "copyright", "credits" or "license" for more information.
    >>> import highctidh
    >>> ctidh = highctidh.ctidh(512) # options are 511, 512, 1024, 2048

See tests/test_highctidh.py for example Python module usage.

To build a Debian package that includes the Python module and the relevant .so
files, run:

    python3 setup.py bdist_deb

To build an x86_64 wheel:

   python setup.py bdist_wheel --plat-name x86_64
