% Use this editor as a MiniZinc scratch book

include "globals.mzn";
include "alldifferent.mzn";

% Facts
int:days;
set of int: daysPerWeek = 1..days;

int:slots;
set of int: slotsPerDay = 1..slots;

int: num_faculty;
set of int:faculty_index_arr = 1..num_faculty;
array[faculty_index_arr] of int : facultyids_array;

int : max_num_diff_classes_per_week_for_single_fac;
set of int:classes_per_week_for_single_fac_index_arr = 1..max_num_diff_classes_per_week_for_single_fac;

array[faculty_index_arr,classes_per_week_for_single_fac_index_arr,1..5] of int: facultydetails;

int:num_sections;
set of int:sections_index_arr = 1..num_sections;

set of int: rooms;
int:num_rooms = length(rooms);
set of int: coursesarray;

% Final Output Timetable Arrays
array[faculty_index_arr,daysPerWeek,slotsPerDay] of var coursesarray: facultyTimetable;
array[sections_index_arr,daysPerWeek,slotsPerDay] of var coursesarray: sectionTimetable;
array[rooms,daysPerWeek,slotsPerDay] of var coursesarray: roomTimetable;

function int: faculty_id_to_index(int:facultyid) = 
   sum(  [ if facultyid = facultyids_array[i] 
              then i
           else 0 endif  
          | i in index_set(facultyids_array) ]
   );

% Rules

% Room Limitation Constraint
constraint 
forall(d in daysPerWeek,s in slotsPerDay)
    (
           sum(t in faculty_index_arr)
           (if facultyTimetable[t,d,s]!=0 
               then 1 
               else 0 
           endif) <= num_rooms
    );

% limiting number of hours per day for faculty
constraint 
forall(d in daysPerWeek,t in faculty_index_arr)
    (
        sum(s in slotsPerDay)
           (if facultyTimetable[t,d,s]!=0 
               then 1 
               else 0 
           endif) < 4
    );

% Faculty Occupancy Constraint
constraint 
forall(d in daysPerWeek,t in faculty_index_arr)
    (
        sum(s in slotsPerDay)
           (if facultyTimetable[t,d,s]!=0 
               then 1 
               else 0 
           endif) > 0
    );

% Method of Solving
solve satisfy;
