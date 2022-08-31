from setuptools import setup, Extension


try:
    from stdeb.command.sdist_dsc import sdist_dsc
    from stdeb.command.bdist_deb import bdist_deb
except ImportError:
    bdist_deb = None
    sdist_dsc = None

requirements = []

data_files = [
    (
        "lib/",
        [
            "libhighctidh_511.so",
            "libhighctidh_512.so",
            "libhighctidh_1024.so",
            "libhighctidh_2048.so",
        ],
    )
]

if __name__ == "__main__":
    setup(
        name="highctidh",
        version="2022.31.08",
        author="Jacob Appelbaum",
        zip_safe=False,
        author_email="jacob@appelbaum.net",
        data_files=data_files,
        include_package_data=True,
        install_requires=requirements,
        cmdclass=dict(bdist_deb=bdist_deb, sdist_dsc=sdist_dsc),
    )
