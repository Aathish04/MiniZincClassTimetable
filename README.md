# MiniZincClassTimetable

This repository contains the source code, some input data, and some documentation that was written for an undergraduate course Project (which, frankly is a little rough around the edges), the details of which can be found below:

- Project Title: Timetable Management System for an Academic Institution
- Team Name: DB4C
- Team Members: [Aathish Sivasubrahmanian](https://github.com/Aathish04/), [Ayshwarya Badhrinath](https://github.com/Ayshu05), [Harshida Sujatha Palaniraj](https://github.com/spharshida)

This project consists of a front-end application built in C that interfaces with a constraint-satisfaction/minimisation model written in MiniZinc, which is a constraint solver interfacer that is used to solve Constraint Logic Programming Problems.

## Compilation
The C Front-End Application is built with the help of certain external Libraries.
Before compiling, one must ensure all these build-dependencies are satisfied.
These dependencies include:
- The JSON Parsing Library [`JSMN`](https://github.com/zserge/jsmn) which must be installed into the `lib/ext` directory in the source code.
- The GUI Toolkit library [`GTK4`](https://gtk.org/), which must be installed globally on the system.

Once `JSMN` has been installed to the appropriate directory, and `GTK4` has been installed, one can then compile the the file `gui.c` into the executable `gui.out`.

In addition to these build-dependencies, the application also requires the presence of the [`minizinc`](https://github.com/MiniZinc/libminizinc) binary/executable and the [`chuffed`](https://github.com/chuffed/chuffed) solver to work properly.

## Running the Executable
To Generate a timetable, there needs to exist at least 4 different files:
- One File with the schema: (The Courses CSV File)
    - `courseid`,`coursecode`,`coursename`,`hoursperweek,numcredits`,`islab`
-   One File with the schema: (The Faculty CSV File)
    - `facultyid`,`departmentname`,`name`,`sectionid`,`courseid`
-   One File with the schema: (The Room CSV File)
    - `roomid`,`department`,`islab`
-   One File with the schema: (The Sections CSV File)
    - `sectionid`,`semester`,`department`,`sectionname`

After running the `gui.exe`/`gui.out` file, the user will be presented with an interface
that allows them to select each of the 4 required data CSV files.
After selecting these files, the end user must then select the number of slots/hours
every working day has [with a limit of 1 to 20 slots/hours], the number of working days
in the week [with a limit of 1 to 7 days], and the `breakslot`
[for lunches/separating the afternoon and forenoon sessions]

If no `breakslot` is required, it can be set to 0.

Once the parameters have been set, the `Solve For TimeTable` Button must be clicked.

The Application will then attempt to generate a timetable conforming to the required
parameters.
If no valid timetable can be generated with those specific parameters, the application
will alert the user, informing them that their constraints could not be satisfied.
If a valid timetable can be generated, the one satisfying the highest number of soft
constraints the highest number of times will be displayed to the end user in a tabular
form.

Pre-existing input data corresponding to testing for certain real-life scenarios (Test-Cases)
can be found in the folders enclosed within the `inputdata` folder. 4 Test cases have been included
that cover a variety of real-life scenarios.


# Constraints Implemented:
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
