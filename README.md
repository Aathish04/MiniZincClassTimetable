# MiniZincClassTimetable

Proper Development Installation Instructions:
1. Clone this repository to your system.
2. Create a folder called `lib/ext` in the top level directory of this repository.
3. Clone [JSMN](https://github.com/zserge/jsmn) into the `lib/ext` directory.
4. Run `make` in the `lib/ext/jsmn/` directory. Ensure that every test passes.
5. Ensure that `minizinc` is available in your `$PATH`.
6. Compile `gui.c` by running `make gui.out`. It depends on `gtk4`, so ensure you have that installed.
7. Run the resulting executable. From there it should be self-explanatory.

# TODOS:
## Hard Constraints:
- [x] A classroom is assigned to just one lecture at a time.​
- [x] Staff/Student clash checks - Faculty and Students are not allotted different classes at the same time.​
- [x] The first hour must be allotted to an accredited course.
- [ ] A faculty member may have a maximum of two theory hours and a maximum of one lab session per day.
- [x] Electives are scheduled for the same session across departments.
- [x] Faculty are not allotted Consecutive teaching hours.

## Soft Constraints:
Implement it as follows:
1. Take each parameter bounded by a soft constraint and use `constrain` to set it to the number of times it is violated or passed depending on the type of soft constraing.
2. Use `constrain` to set the value of a `objective_function_return` variable to some combination of the parameters described in point 1.
3. Maximise or minimise `objective_function_return` depending on how exactly things are implemented.