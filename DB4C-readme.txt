Project Title: Timetable Management System for an Academic Institution
Team Name: DB4C
Team Members: Aathish Sivasubrahmanian, Ayshwarya Badhrinath, Harshida Sujatha Palaniraj

This project consists of a front-end application built in C that interfaces
with a constraint solver written in MiniZinc, a constraint solver interfacer that is
used to solve Constraint Logic Programming Problems.


Compilation
-----------
The C Front-End Application is built with the help certain external Libraries.
Before compiling, one must ensure all dependencies are satisfied.
These dependencies include:
    - The JSON Parsing Library JSMN which must be installed into the lib/ext
    directory in the source code.
    - The GUI library GTK4, which must be installed globally on the system.
Once JSMN has been installed to the appropriate directory, and GTK4 has been installed,
one can then compile the the file gui.c into the executable `gui.out` or `gui.exe`.

Running the Executable
----------------------
To Generate a timetable, there needs to exist at least 4 different files:
    -   One File with the schema [The Courses CSV File]
            courseid,coursecode,coursename,hoursperweek,numcredits,islab
    -   One File with the schema [The Faculty CSV File]
            facultyid,departmentname,name,sectionid,courseid
    -   One File with the schema [The Room CSV File]
            roomid,department,islab
    -   One File with the schema [The Sections CSV File]
        sectionid,semester,department,sectionname
After running the gui.exe/gui.out file, the user will be presented with an interface
that allows them to select each of the 4 required data CSV files.
After selecting these files, the end user must then select the number of slots/hours
every working day has [with a limit of 1 to 20 slots/hours], the number of working days
in the week [with a limit of 1 to 7 days], and the break slot
[for lunches/separating the afternoon and forenoon sessions]

If no breakslot is required, it can be set to 0.

Once the parameters have been set, the Solve For TimeTable Button must be clicked.
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