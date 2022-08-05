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

array[faculty_index_arr,classes_per_week_for_single_fac_index_arr,1..7] of int: facultydetails;

int:num_sections;
set of int:sections_index_arr = 1..num_sections;

array[sections_index_arr] of int : sectionids_array;

set of int: rooms;
int:num_rooms = length(rooms);
set of int: coursesarray;

% Final Output Timetable Arrays
array[faculty_index_arr,daysPerWeek,slotsPerDay] of var coursesarray: facultyTimetable;
array[sections_index_arr,daysPerWeek,slotsPerDay] of var coursesarray: sectionTimetable;
array[rooms,daysPerWeek,slotsPerDay] of var array2set(facultyids_array++(0..0)): roomTimetable;

% Helper Functions
function var int: faculty_id_to_index(var int:facultyid) =
   sum(  [ if facultyid = facultyids_array[i]
              then i
           else 0 endif
          | i in index_set(facultyids_array) ]
   );

% Rules

% Constrain faculty and sections to have a consistent timetable.
constraint
forall(facultyindex in faculty_index_arr,class in classes_per_week_for_single_fac_index_arr)(
    forall(sec in sections_index_arr,day in daysPerWeek, slot in slotsPerDay)(
        if facultydetails[facultyindex,class,2] == sectionids_array[sec] /\ facultyTimetable[facultyindex,day,slot]!=0
        then sectionTimetable[sec,day,slot] = facultyTimetable[facultyindex,day,slot]
        endif
    )
);

% Ensure faculty are only given courses they've been assigned.
% NOTE: THIS NEEDS MORE TESTING!!!
constraint
forall (courseid in coursesarray)(
    forall(facultyindex in faculty_index_arr)(
        if sum(class in classes_per_week_for_single_fac_index_arr)
        (
            if facultydetails[facultyindex,class,3] == courseid
            then 1
            else 0
            endif
        ) < 1
        then forall(day in daysPerWeek, slot in slotsPerDay)(
            if facultyTimetable[facultyindex,day,slot] != 0
            then facultyTimetable[facultyindex,day,slot] != courseid
            endif
        )
        endif
    )
);

% Constrain hours per week for course to hours_per_week for that course.
constraint
forall(facultyindex in faculty_index_arr,class in classes_per_week_for_single_fac_index_arr)(
    sum(day in daysPerWeek, slot in slotsPerDay)(
        if facultyTimetable[facultyindex,day,slot] == facultydetails[facultyindex,class,3]
        then 1
        else 0
        endif
    ) == facultydetails[facultyindex,class,4]
);

% Ensure first hour goes to an accredited course for every section.
constraint
forall(section_index in sections_index_arr, day in daysPerWeek)(
    if sectionTimetable[section_index,day,1] > 0
    then sum(
        facultyindex in faculty_index_arr,
        class in classes_per_week_for_single_fac_index_arr)(
            if facultydetails[facultyindex,class,3]==sectionTimetable[section_index,day,1] /\ facultydetails[facultyindex,class,5] > 0
            then 1
            else 0
            endif
        ) > 0
    endif
);

% Ensure faculty do not have consecutive teaching hours unless
% it is for a lab session.
% Ensure faculty have free slot after every occupied slot unless
% it is for a lab session.
constraint
forall(f in faculty_index_arr,d in daysPerWeek)(
    forall(s in slotsPerDay[1..slots-1])(
        if facultyTimetable[f,d,s] > 0 /\ sum(class in classes_per_week_for_single_fac_index_arr)(
            if facultydetails[f,class,3] == facultyTimetable[f,d,s] /\ (facultydetails[f,class,6] == 1)
            then 1
            else 0
            endif
        ) == 0
        then facultyTimetable[f,d,s+1] = 0
        endif
    )
);
% Ensure faculty have free slot before every occupied slot unless
% it is for a lab session.
constraint
forall(f in faculty_index_arr,d in daysPerWeek)(
    forall(s in slotsPerDay[2..slots])(
        if facultyTimetable[f,d,s] > 0 /\ sum(class in classes_per_week_for_single_fac_index_arr)(
            if facultydetails[f,class,3] == facultyTimetable[f,d,s] /\ (facultydetails[f,class,6] == 1)
            then 1
            else 0
            endif
        ) == 0
        then facultyTimetable[f,d,s-1] = 0
        endif
    )
);

% Room Related Constraints
% Room Limitation Constraint
constraint
forall(day in daysPerWeek, slot in slotsPerDay)(
    sum(facultyindex in faculty_index_arr)(
        if facultyTimetable[facultyindex,day,slot]!=0
        then 1
        else 0
        endif
    ) <= num_rooms
);

% If a room is allotted to a faculty, they must not be free then.
constraint
forall(facultyindex in faculty_index_arr,day in daysPerWeek, slot in slotsPerDay)(
    forall(roomid in rooms)(
        if facultyTimetable[facultyindex,day,slot] == 0
        then roomTimetable[roomid,day,slot] != facultyids_array[facultyindex]
        endif
    )
);

% Limit rooms allotted to faculty for a particular slot to 1.
constraint
forall(facultyindex in faculty_index_arr, day in daysPerWeek, slot in slotsPerDay)( % For every faculty, day, slot combination
    if facultyTimetable[facultyindex,day,slot]>0 % If the faculty is not free in that slot
    then sum(roomid in rooms)
        (if roomTimetable[roomid,day,slot]==facultyids_array[facultyindex]
            then 1
            else 0
        endif) == 1 % Then the number of rooms allotted to that faculty will be limited to 1.
    endif
);
% Method of Solving
solve satisfy;
