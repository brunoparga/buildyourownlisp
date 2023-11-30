# Lye

Code based on [buildyourownlisp.com]. I've decided to name my language Lye. The course uses "lispy", so I had chosen the `.ly` file extension; but apparently that is already taken by a sheet music program. So yeah, I'm just adding the -e at the end and making the name of my language kinda confusing.

## Running Lye code

You can enter a REPL with just the command:

```bash
$ build/lye

Lye Version 0.0.0.11
Press Ctrl+c to Exit

lye>
```

You can also pass it an argument, the name of a file to be executed. It will print the value of the last expression evaluated:

```bash
$ build/lye test/number/number.lye
Error: S-expression must start with a function, found type Number.
```

The command line option `-s` allows you to pass in Lye code as a single string argument, to be evaluated:

```bash
$ build/lye -s "max 10 3 17 31 42 0 -5"
42
```

## Running language tests

Tests are run by the `build/test` executable, which is run automatically by `make`. Without an argument, `build/test` runs every file in the `test` directory.

```bash
$ build/test
Ran 105 tests, all passed. Hooray!
```

It is also possible to run only certain test files, by passing each of them as an argument to the test runner:

```bash
$ build/test test/number/number.lye test/term/add.lye
Ran 8 tests, all passed. Hooray!
```