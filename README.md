odbc-cli-c
============
[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI for Linux, Windows, macOS](https://github.com/SamuelMarks/odbc-cli-c/workflows/CI%20for%20Linux,%20Windows,%20macOS/badge.svg)](https://github.com/SamuelMarks/odbc-cli-c/actions)
[![CI for FreeBSD](https://api.cirrus-ci.com/github/SamuelMarks/odbc-cli-c.svg)](https://cirrus-ci.com/github/SamuelMarks/odbc-cli-c)
[![C89](https://img.shields.io/badge/C-89-blue)](https://en.wikipedia.org/wiki/C89_(C_version))

ODBC CLI. Intended for simple use-cases: execution from file or literals; with proper sanitisation.

## `--help`

    odbc-cli-c: ODBC CLI
    
    Usage:
    odbc-cli-c...
    odbc-cli-c --output=<o>...
    odbc-cli-c --command=<c>...
    odbc-cli-c --command-file=<f>...
    odbc-cli-c -c=<c> --params=<PARAMS>...
    odbc-cli-c --help
    odbc-cli-c --version
    
    Options:
    -h --help                     Show this screen.
    --version                     Show version.
    -c=<c>, --command=<c>         Query to execute
    -f=<c>, --command-file=<c>    Alternative query to execute from file or stdin
    -p=<p>, --params=<p>          Parameters to provide sanitarily to SQL query
    -o=<o>, --output=<o>          Output file. If not specified: will use `stdout`. If specified: will overwrite file.

## Build dependencies

- [CMake](https://cmake.org) (3.19 or later)
- C compiler (any that work with CMake, and were released within the last 30 years)

### Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Docker

```bash
docker build . -t odbc-cli-c:latest
```

### CLI interface

Generated with `docopt`, use `python -m pip install https://api.github.com/repos/offscale/docopt.c/zipball/pyth
on-3-13#egg=docopt_c` then:

```bash
$ python -m docopt_c '.docopt' -o 'odbc_cli_c/odbc_cli_c'
```

---

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or <https://www.apache.org/licenses/LICENSE-2.0>)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or <https://opensource.org/licenses/MIT>)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.
