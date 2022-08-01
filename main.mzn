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

array[faculty_index_arr,classes_per_week_for_single_fac_index_arr,1..6] of int: facultydetails;

int:num_sections;
set of int:sections_index_arr = 1..num_sections;

array[sections_index_arr] of int : sectionids_array;

set of int: rooms;
int:num_rooms = length(rooms);
set of int: coursesarray;

% Final Output Timetable Arrays
array[faculty_index_arr,daysPerWeek,slotsPerDay] of var coursesarray: facultyTimetable;
array[sections_index_arr,daysPerWeek,slotsPerDay] of var coursesarray: sectionTimetable;
array[rooms,daysPerWeek,slotsPerDay] of var array2set(facultyids_array): roomTimetable;

% Helper Functions
function var int: faculty_id_to_index(var int:facultyid) = 
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

% Ensure faculty don't teach for more than half the number of
% slots per day.
constraint 
forall(d in daysPerWeek,t in faculty_index_arr)
    (
        sum(s in slotsPerDay)
           (if facultyTimetable[t,d,s]!=0 
               then 1 
               else 0 
           endif) < (slots/2)+1
    );

% Ensure No Consecutive Same Class for Faculty
constraint
forall(f in faculty_index_arr,d in daysPerWeek)(
    forall(s in slotsPerDay[1..slots-1])(
        facultyTimetable[f,d,s] != facultyTimetable[f,d,s+1]
    )
);

% % Ensure faculty have free slot after every occupied slot
% constraint
% forall(f in faculty_index_arr,d in daysPerWeek)(
%     forall(s in slotsPerDay[1..slots-1])(
%         if facultyTimetable[f,d,s] > 0
%         then facultyTimetable[f,d,s+1] = 0
%         endif
%     )
% );

% Constrain hours per week for course to hours_per_week for that course.
constraint
forall(f in faculty_index_arr,class in classes_per_week_for_single_fac_index_arr)(
    sum(d in daysPerWeek,s in slotsPerDay)(
        if facultyTimetable[f,d,s] == facultydetails[f,class,3]
        then 1
        else 0
        endif
    ) == facultydetails[f,class,4]
);

% Ensure Faculty teach at least one slot every day.
constraint
forall(d in daysPerWeek,t in faculty_index_arr)
    (
        sum(s in slotsPerDay)
           (if facultyTimetable[t,d,s]!=0 
               then 1 
               else 0 
           endif) > 0
    );

% Ensure faculty are only given courses they've been assigned.
% NOTE: THIS NEEDS MORE TESTING!!!
constraint
forall (c in coursesarray)(
    if sum(f in faculty_index_arr, class in classes_per_week_for_single_fac_index_arr)
    (
        if facultydetails[f,class,3] == c
        then 1
        else 0
        endif
    ) < 1 
    then forall(f in faculty_index_arr,d in daysPerWeek, s in slotsPerDay)(
        if facultyTimetable[f,d,s] != 0
        then facultyTimetable[f,d,s] != c
        endif
    )
    endif
);

% Constrain faculty and sections to have a consistent timetable.
constraint
forall(f in faculty_index_arr,class in classes_per_week_for_single_fac_index_arr)(
    forall(sec in sections_index_arr,d in daysPerWeek, s in slotsPerDay)(
        if facultydetails[f,class,2] == sectionids_array[sec]
        then sectionTimetable[sec,d,s] = facultyTimetable[f,d,s]
        endif
    )
);

% If a room is allotted to a faculty, they must not be free then.
constraint
forall(f in faculty_index_arr,d in daysPerWeek, s in slotsPerDay)(
    forall(roomid in rooms)(
        if roomTimetable[roomid,d,s] == facultyids_array[f]
        then facultyTimetable[f,d,s] != 0
        endif
    )
);

% Faculty not given two rooms for same slot.
constraint
forall(f in faculty_index_arr, d in daysPerWeek, s in slotsPerDay)(
    forall(allottedroomid in rooms)(
        if roomTimetable[allottedroomid,d,s] == facultyids_array[f]
        then forall(everyotherroomid in (rooms diff (allottedroomid..allottedroomid)))(
            roomTimetable[everyotherroomid,d,s] != facultyids_array[f]
        )
        endif
    )
);
% Method of Solving
solve satisfy;
