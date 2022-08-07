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

int : num_info_in_facultydetails;
array[faculty_index_arr,classes_per_week_for_single_fac_index_arr,1..num_info_in_facultydetails] of int: facultydetails;

int:num_sections;
set of int:sections_index_arr = 1..num_sections;

array[sections_index_arr] of int : sectionids_array;

int: num_rooms;
set of int: rooms = 1..num_rooms;
array[rooms,1..2] of int: roomsdetails;
set of int: coursesarray;

int: break_slot;

% Final Output Timetable Arrays
array[faculty_index_arr,daysPerWeek,slotsPerDay] of var coursesarray: facultyTimetable;
array[sections_index_arr,daysPerWeek,slotsPerDay] of var coursesarray: sectionTimetable;
array[rooms,daysPerWeek,slotsPerDay] of var array2set(facultyids_array++(0..0)): roomTimetable;

var int : num_times_first_hour_not_credited;
var int : num_times_first_hour_not_dept_faculty;
var int : num_times_last_hour_credited;
var int : num_times_sessions_not_alternate_bw_credited_and_not_credited;
var int : soft_constraint_minimization_function_return;

% Helper Functions
function var bool: courseid_is_lab(var int:courseid) = 
    sum(facultyindex in faculty_index_arr, class in classes_per_week_for_single_fac_index_arr)(
        if facultydetails[facultyindex,class,3] == courseid /\ facultydetails[facultyindex,class,6] == 1
        then 1
        else 0
        endif
    ) > 0;

function var bool: courseid_is_credited(var int:courseid) = 
    sum(facultyindex in faculty_index_arr, class in classes_per_week_for_single_fac_index_arr)(
        if facultydetails[facultyindex,class,3] == courseid /\ facultydetails[facultyindex,class,5] > 0
        then 1
        else 0
        endif
    ) > 0;

function var bool: courseid_secid_handled_by_dept_faculty(var int:courseid, var int: sectionid) = 
    sum(facultyindex in faculty_index_arr, class in classes_per_week_for_single_fac_index_arr)(
        if facultydetails[facultyindex,class,3] == courseid /\ facultydetails[facultyindex,class,2] == sectionid /\ facultydetails[facultyindex,class,7]==1
        then 1
        else 0
        endif
    ) > 0;
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

% Add a break slot if one is given
constraint
if break_slot > 0
then
    forall(day in daysPerWeek)(
        forall(sectionindex in sections_index_arr)(
            sectionTimetable[sectionindex,day,break_slot] == 0
        ) /\
        forall(facultyindex in faculty_index_arr)(
            facultyTimetable[facultyindex,day,break_slot] == 0
        ) /\
        forall(roomid in rooms)(
            roomTimetable[roomid,day,break_slot] == 0
        )
    )
endif;
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

% Ensure sections are only given courses they've been assigned.
constraint
forall(sectionindex in sections_index_arr, day in daysPerWeek, slot in slotsPerDay)(
    if sectionTimetable[sectionindex, day, slot] > 0
    then sum(facultyindex in faculty_index_arr, class in classes_per_week_for_single_fac_index_arr)(
        if facultydetails[facultyindex, class, 3] == sectionTimetable[sectionindex, day, slot] /\
            facultydetails[facultyindex, class, 2] == sectionids_array[sectionindex] /\
            facultyTimetable[facultyindex, day, slot] == sectionTimetable[sectionindex, day, slot]
        then 1
        else 0
        endif
    ) > 0
    endif
    
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
constraint
forall(facultyindex in faculty_index_arr,day in daysPerWeek)(
    % Ensure faculty have free slot after every occupied slot unless
    % it is for a lab session.
    forall(slot in slotsPerDay)(
        if slot < slots
        then
            if facultyTimetable[facultyindex,day,slot] > 0 /\ not courseid_is_lab(facultyTimetable[facultyindex,day,slot])
            then facultyTimetable[facultyindex,day,slot+1] = 0
            endif
        endif /\
        if slot > 1
        then
            if facultyTimetable[facultyindex,day,slot] > 0 /\ not courseid_is_lab(facultyTimetable[facultyindex,day,slot])
            then facultyTimetable[facultyindex,day,slot-1] = 0
            endif
        endif
    )
);

% % Ensure faculty have a maximum of two theory hours per day unless there is a lab session.
% constraint 
% forall(day in daysPerWeek,facultyindex in faculty_index_arr)
%     (
%         sum(slot in slotsPerDay)
%            (if facultyTimetable[facultyindex,day,slot]!=0 /\ not courseid_is_lab(facultyTimetable[facultyindex,day,slot])
%                then 1 
%                else 0 
%            endif) < 2
%     );

% Ensure Lab sessions are always continous.
constraint
forall(facultyindex in faculty_index_arr,class in classes_per_week_for_single_fac_index_arr)(
    if facultydetails[facultyindex,class,6] == 1
    then forall(day in daysPerWeek, slot in slotsPerDay)(
        if facultyTimetable[facultyindex,day,slot] == facultydetails[facultyindex,class,3]
        then 
            if slot == 1
            then facultyTimetable[facultyindex,day,slot+1] == facultydetails[facultyindex,class,3]
            endif /\
            if slot == slots
            then facultyTimetable[facultyindex,day,slot-1] == facultydetails[facultyindex,class,3]
            endif /\
            if (slot > 1 /\ slot < slots) /\ facultyTimetable[facultyindex,day,slot-1] != facultydetails[facultyindex,class,3]
            then
                facultyTimetable[facultyindex,day,slot+1] == facultydetails[facultyindex,class,3]
            endif
        endif
    )
    endif
);

% Ensure lab sessions are held at the same slot and same day for all classes.
constraint
forall(facultyindex in faculty_index_arr,class in classes_per_week_for_single_fac_index_arr)(
    if facultydetails[facultyindex,class,8] == 1
    then forall(day in daysPerWeek, slot in slotsPerDay)(
        if facultyTimetable[facultyindex, day, slot] == facultydetails[facultyindex,class,3]
        then forall(facultyindex2 in faculty_index_arr)(
            if facultydetails[facultyindex,class,3] == facultydetails[facultyindex2,class,3]
            then facultyTimetable[facultyindex2,day,slot] == facultyTimetable[facultyindex, day, slot]
            endif
        )
        endif
    )
    endif
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

% Ensure lab classes are allotted lab classrooms.
constraint
forall(facultyindex in faculty_index_arr,day in daysPerWeek, slot in slotsPerDay)(
    forall(roomid in rooms)(
        if courseid_is_lab(facultyTimetable[facultyindex,day,slot])
        then
            if roomsdetails[roomid,2] != 1
            then roomTimetable[roomid,day,slot] != facultyids_array[facultyindex]
            endif
        endif
    )
);

% Ensure continous periods occur in the same room.
constraint
forall(facultyindex in faculty_index_arr,day in daysPerWeek, slot in slotsPerDay)(
    forall(roomid in rooms)(
        if slot > 1
        then
            if facultyTimetable[facultyindex,day,slot-1] == facultyTimetable[facultyindex,day,slot] /\ roomTimetable[roomid,day,slot] == facultyids_array[facultyindex]
            then roomTimetable[roomid,day,slot-1] == facultyids_array[facultyindex]
            endif
        endif /\
        if slot < slots
        then
            if facultyTimetable[facultyindex,day,slot+1] == facultyTimetable[facultyindex,day,slot] /\ roomTimetable[roomid,day,slot] == facultyids_array[facultyindex]
            then roomTimetable[roomid,day,slot+1] == facultyids_array[facultyindex]
            endif
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

% Soft Constraints
constraint
num_times_first_hour_not_credited = sum(sectionsindex in sections_index_arr, day in daysPerWeek)(
    if not courseid_is_credited(sectionTimetable[sectionsindex,day,1])
    then 1
    else 0
    endif
);

constraint
num_times_first_hour_not_dept_faculty = sum(sectionsindex in sections_index_arr, day in daysPerWeek)(
    if not courseid_secid_handled_by_dept_faculty(sectionTimetable[sectionsindex,day,1],sectionids_array[sectionsindex])
    then 1
    else 0
    endif
);

constraint
num_times_last_hour_credited = sum(sectionsindex in sections_index_arr, day in daysPerWeek)(
    if courseid_is_credited(sectionTimetable[sectionsindex,day,slots])
    then 1
    else 0
    endif
);

% Might need to refactor this.
% Try to make courses alternate between credited and noncredited for students.
constraint
num_times_sessions_not_alternate_bw_credited_and_not_credited = sum(sectionsindex in sections_index_arr, day in daysPerWeek, slot in slotsPerDay)(
    if courseid_is_credited(sectionTimetable[sectionsindex,day,slot])
    then
        if slot != slots
        then
            if courseid_is_credited(sectionTimetable[sectionsindex,day,slot+1])
            then 1
            else 0
            endif
        else
            if courseid_is_credited(sectionTimetable[sectionsindex,day,slot-1])
            then 1
            else 0
            endif
        endif
    else 0
    endif
);
constraint
soft_constraint_minimization_function_return = num_times_first_hour_not_credited + num_times_first_hour_not_dept_faculty + num_times_last_hour_credited + num_times_sessions_not_alternate_bw_credited_and_not_credited; 

% Method of Solving
solve minimize soft_constraint_minimization_function_return
