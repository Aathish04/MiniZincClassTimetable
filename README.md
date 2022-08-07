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
1. [x] A classroom is assigned to just one lecture at a time.​
2. [x] Staff/Student clash checks - Faculty and Students are not allotted different classes at the same time.​
3. [x] The first hour must be allotted to an accredited course.[Now Soft]
4. [x] A break between forenoon and afternoon session must exist. [Implemented, but I don't really like it because the break occupies one of the slots.]
5. [x] A faculty member may have a maximum of two theory hours ~~and a maximum of one lab session per day.~~ [Too restrictive. Maximum of one lab session needs some thought. Present in code, but commented out entirely.]
6. [x] Electives are scheduled for the same session across departments.
7. [x] Faculty are not allotted Consecutive teaching hours.

## Soft Constraints:
1. [x] Allotting Core subjects in the last time slot should be avoided.​
2. [x] ~~Co-curricular~~ Non-credited subjects are allotted to the afternoon sessions.
3. [x] Sessions alternate between Core and non-Core subjects. [Check this for consistency.]
4. [x] First hour session is allotted to department faculty.​
5. [x] First hour allotted to an accredited course.
