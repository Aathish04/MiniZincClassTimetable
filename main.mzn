% Use this editor as a MiniZinc scratch book

include "globals.mzn";
include "alldifferent.mzn";

% Facts
int:days;
set of int: daysPerWeek = 1..days;

int:slots;
set of int: slotsPerDay = 1..slots;

int:num_sections;
set of int:arr_num_sections = 1..num_sections;
int:num_courses_per_section;
set of int:arr_num_courses_per_section = 1..num_courses_per_section;

array[arr_num_sections,arr_num_courses_per_section,1..4] of int: sectiondetails;


set of int: facultyarray;
int: num_faculty;
set of int: facultyindex = 1..num_faculty;
set of int: rooms;
int:num_rooms = length(rooms);
set of int: coursesarray;

int: hours_per_week = 3;

% Final Output Timetable Arrays
array[facultyindex,daysPerWeek,slotsPerDay] of var coursesarray: facultyTimetable;
array[arr_num_sections,daysPerWeek,slotsPerDay] of var coursesarray: sectionTimetable;
array[rooms,daysPerWeek,slotsPerDay] of var coursesarray: roomTimetable;

function int: faculty_id_to_index(int:facultyid) = 
   sum(  [ if facultyid = facultyarray[i] 
              then i
           else 0 endif  
          | i in index_set(facultyarray) ]
   );

% Rules

% Room Limitation Constraint
constraint 
forall(d in daysPerWeek,s in slotsPerDay)
    (
           sum(t in facultyindex)
           (if facultyTimetable[t,d,s]!=0 
               then 1 
               else 0 
           endif) <= num_rooms
    );

% limiting number of hours per day for faculty
constraint 
forall(d in daysPerWeek,t in facultyindex)
    (
        sum(s in slotsPerDay)
           (if facultyTimetable[t,d,s]!=0 
               then 1 
               else 0 
           endif) < 4
    );

% Course-Section-Professor Constraint
constraint
forall(sec in arr_num_sections, d in daysPerWeek, s in slotsPerDay)(
    if sectionTimetable[sec,d,s] > 0
    then
    forall(course in arr_num_courses_per_section)(
        facultyTimetable[faculty_id_to_index(sectiondetails[sec,course,3]),d,s] == sectionTimetable[sec,d,s]
        )
    endif);

% Faculty - FacultySubject Constraint
constraint
forall(t in facultyindex, d in daysPerWeek, s in slotsPerDay)(
    forall(sec in arr_num_sections)(
        forall(course in arr_num_courses_per_section)(
        if sectiondetails[sec,course,3] != facultyarray[t]
        then facultyTimetable[t,d,s] != sectiondetails[sec,course,3]
        endif))
);

% Faculty Occupancy Constraint
constraint 
forall(d in daysPerWeek,t in facultyindex)
    (
        sum(s in slotsPerDay)
           (if facultyTimetable[t,d,s]!=0 
               then 1 
               else 0 
           endif) > 0
    );

% Method of Solving
solve satisfy;
