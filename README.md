# MiniZincClassTimetable

Proper Development Installation Instructions:
1. Clone this repository to your system.
2. Create a folder called `lib/` in the top level directory of this repository.
3. Clone [JSMN](https://github.com/zserge/jsmn) into the `lib/` directory.
4. Run `make` in the `lib/jsmn/` directory. Ensure that every test passes.
5. Ensure that `minizinc` is available in your `$PATH`.
6. Compile `main.c`.
7. Run the resulting executable.