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

array[arr_num_sections,arr_num_courses_per_section,1..3] of int: sectiondetails;


set of int: teachersarray;
int: num_teachers;
set of int: teachersindex = 1..num_teachers;
set of int: rooms;
int:num_rooms = length(rooms);
set of int: coursesarray;

int: hours_per_week = 3;

% Final Output Timetable Arrays
array[teachersindex,daysPerWeek,slotsPerDay] of var coursesarray: teacherRoutine;
array[arr_num_sections,daysPerWeek,slotsPerDay] of var coursesarray: sectionRoutine;
array[rooms,daysPerWeek,slotsPerDay] of var coursesarray: roomRoutine;

function int: teacher_id_to_index(int:teacherid) = 
   sum(  [ if teacherid = teachersarray[i] 
              then i
           else 0 endif  
          | i in index_set(teachersarray) ]
   );

% Rules

% Room Limitation Constraint
constraint 
forall(d in daysPerWeek,s in slotsPerDay)
    (
           sum(t in teachersindex)
           (if teacherRoutine[t,d,s]!=0 
               then 1 
               else 0 
           endif) <= num_rooms
    );

% Course-Section-Professor Constraint
constraint
forall(sec in arr_num_sections, d in daysPerWeek, s in slotsPerDay)(
    if sectionRoutine[sec,d,s] > 0
    then
    forall(course in arr_num_courses_per_section)(
        teacherRoutine[teacher_id_to_index(sectiondetails[sec,course,2]),d,s] == sectionRoutine[sec,d,s]
        )
    endif);

% Method of Solving
solve satisfy;
