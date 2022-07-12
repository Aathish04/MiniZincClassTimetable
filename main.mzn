% Use this editor as a MiniZinc scratch book

include "globals.mzn";
include "alldifferent.mzn";

% Facts
int:days;
set of int: daysPerWeek = 1..days;

int:slots;
set of int: slotsPerDay = 1..slots;

int:num_sections;
set of int:arr_num_sections = 0..num_sections-1;
int:num_courses_per_section;
set of int:arr_num_courses_per_section = 0..num_courses_per_section-1;

array[arr_num_sections,arr_num_courses_per_section,0..2] of int: sectiondetails;


set of int: teachersarray;
int: num_teachers;
set of int: teachersindex = 1..num_teachers-1;
set of int: rooms;
set of int: coursesarray;
% array[classList,properties] of int: classDetails;

% Final Output Timetable Arrays
array[teachersindex,daysPerWeek,slotsPerDay] of var coursesarray: teacherRoutine;
array[arr_num_sections,daysPerWeek,slotsPerDay] of var coursesarray: sectionRoutine;
array[rooms,daysPerWeek,slotsPerDay] of var coursesarray: roomRoutine;

% Rules

% % Room Limitation Constraint
% constraint 
% forall(d in daysPerWeek,s in slotsPerDay)
%     (
%            sum(t in teacherList)
%            (if teacherRoutine[t,d,s]!=0 
%                then 1 
%                else 0 
%            endif) <= rooms
%     );


% constraint    
% forall(i in courseList,sec in sectionList)(
%   forall(d in daysPerWeek)
%        (sum(s in slotsPerDay)
%            (if sectionRoutine[sec,d,s] == i
%            then 1
%            else 0
%            endif) <= 1));

% constraint    
% forall(i in courseList,teach in teacherList)(
%   forall(d in daysPerWeek)
%        (sum(s in slotsPerDay)
%            (if teacherRoutine[teach,d,s] == i
%            then 1
%            else 0
%            endif) <= 1));

% Method of Solving
solve satisfy;
