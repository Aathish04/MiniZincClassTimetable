% Use this editor as a MiniZinc scratch book

include "globals.mzn";
include "alldifferent.mzn";

% Facts
int:days=5;
set of int: daysPerWeek = 1..days;

int:slots=2;
set of int: slotsPerDay = 1..slots;

int: courses = 2;
int: rooms = 1;
int: sections = 2;
int: teachers = 2;

% Intermediate Variables
set of int: sectionList = 1..sections;
set of int: teacherList = 1..teachers;
set of int: roomList = 1..rooms;
set of int: courseList = 1..courses;
set of int: courseIndex = 0..courses;


% Final Output Timetable Arrays
array[teacherList,daysPerWeek,slotsPerDay] of var courseIndex: teacherRoutine;
array[sectionList,daysPerWeek,slotsPerDay] of var courseIndex: sectionRoutine;
array[roomList,daysPerWeek,slotsPerDay] of var courseIndex: roomRoutine;

% Rules

% Room Limitation Constraint
constraint 
forall(d in daysPerWeek,s in slotsPerDay)
    (
           sum(t in teacherList)
           (if teacherRoutine[t,d,s]!=0 
               then 1 
               else 0 
           endif) <= rooms
    );


constraint    
forall(i in courseList,sec in sectionList)(
  forall(d in daysPerWeek)
       (sum(s in slotsPerDay)
           (if sectionRoutine[sec,d,s] == i
           then 1
           else 0
           endif) <= 1));

constraint    
forall(i in courseList,teach in teacherList)(
  forall(d in daysPerWeek)
       (sum(s in slotsPerDay)
           (if teacherRoutine[teach,d,s] == i
           then 1
           else 0
           endif) <= 1));

% Method of Solving
solve satisfy;
