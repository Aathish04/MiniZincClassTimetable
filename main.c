#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/jsmn/jsmn.h"

int int_value_in_array(int value, int array[], int arraylen);
void fill_csv_metadata(char csvfilepath[], char col_delimitchar, char row_delimitchar, char commentchar, int *num_records_out_ptr, int *num_cols_out_ptr, int *max_valuelen_out_ptr);
void fill_csv_raw_array(char csvfilepath[], char col_delimitchar, char row_delimitchar, char commentchar, int num_records, int num_cols, int value_str_len, char csv_raw_array[num_records][num_cols][value_str_len]);
int calc_max_num_courses_for_single_sec(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size]);
int calc_num_unique_sections(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size]);
void fill_sectiondetailsarray(
    int num_unique_sections, int max_num_courses_for_single_section, int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][3],
    int sectionscsv_numrecords, int sectionscsv_numcols, int sectionscsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectionscsv_data_size],
    int coursescsv_numrecords, int coursescsv_numcols, int coursescsv_data_size, char coursescsv_raw_array[coursescsv_numrecords][coursescsv_numcols][coursescsv_data_size]);
void write_output_json_file(
    char OutputJSONPath[],
    int num_unique_sections, int max_num_courses_for_single_section, int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][3],
    int num_unique_faculty, int unique_faculty_array[],
    int num_unique_courses, int unique_courses_array[],
    int num_slots_per_day,
    int num_days_per_week,
    int num_courses_per_section,
    int num_sections,
    int num_rooms, int roomsarray[],
    int num_faculty);
int fill_unique_facultyarray_return_num_unique_faculty(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size], int unique_faculty_array[sectionscsv_numrecords]);
int fill_unique_coursesarray_return_num_unique_courses(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size], int unique_courses_array[sectionscsv_numrecords]);

void call_minizinc_and_fill_timetable_arrays(
    char COMMAND[], int num_slots_per_day, int num_days_per_week,
    int num_faculty, int faculty_timetable_array[num_faculty][num_days_per_week][num_slots_per_day],
    int num_sections, int section_timetable_array[num_sections][num_days_per_week][num_slots_per_day],
    int num_rooms, int room_timetable_array[num_rooms][num_days_per_week][num_slots_per_day]);

int main()
{
    char CoursesCSVPath[] = "./inputdata/Courses.csv";
    char FacultyCSVPath[] = "./inputdata/Faculty.csv";
    char SectionsCSVPath[] = "./inputdata/Sections.csv";
    char RoomsCSVPath[] = "./inputdata/Rooms.csv";
    char OutputJSONPath[] = "./InputData.json";
    char COMMAND[] = "minizinc main.mzn InputData.json --solver chuffed --json-stream --output-mode json";

    int num_slots_per_day = 2;
    int num_days_per_week = 2;

    printf("Reading data from CSV Files... ");
    int coursescsv_numrecords, coursescsv_numcols, coursescsv_longestvaluelen;
    fill_csv_metadata(CoursesCSVPath, ',', '\n', '#', &coursescsv_numrecords, &coursescsv_numcols, &coursescsv_longestvaluelen);
    char coursescsv_raw_array[coursescsv_numrecords][coursescsv_numcols][coursescsv_longestvaluelen + 1];
    fill_csv_raw_array(CoursesCSVPath, ',', '\n', '#', coursescsv_numrecords, coursescsv_numcols, coursescsv_longestvaluelen + 1, coursescsv_raw_array);

    int facultycsv_numrecords, facultycsv_numcols, facultycsv_longestvaluelen;
    fill_csv_metadata(FacultyCSVPath, ',', '\n', '#', &facultycsv_numrecords, &facultycsv_numcols, &facultycsv_longestvaluelen);
    char facultycsv_raw_array[facultycsv_numrecords][facultycsv_numcols][facultycsv_longestvaluelen + 1];
    fill_csv_raw_array(FacultyCSVPath, ',', '\n', '#', facultycsv_numrecords, facultycsv_numcols, facultycsv_longestvaluelen + 1, facultycsv_raw_array);

    int sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen;
    fill_csv_metadata(SectionsCSVPath, ',', '\n', '#', &sectionscsv_numrecords, &sectionscsv_numcols, &sectionscsv_longestvaluelen);
    char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectionscsv_longestvaluelen + 1];
    fill_csv_raw_array(SectionsCSVPath, ',', '\n', '#', sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array);

    int roomscsv_numrecords, roomscsv_numcols, roomscsv_longestvaluelen;
    fill_csv_metadata(RoomsCSVPath, ',', '\n', '#', &roomscsv_numrecords, &roomscsv_numcols, &roomscsv_longestvaluelen);
    char roomscsv_raw_array[roomscsv_numrecords][roomscsv_numcols][roomscsv_longestvaluelen + 1];
    fill_csv_raw_array(RoomsCSVPath, ',', '\n', '#', roomscsv_numrecords, roomscsv_numcols, roomscsv_longestvaluelen + 1, roomscsv_raw_array);

    int max_num_courses_for_single_section = calc_max_num_courses_for_single_sec(sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array);
    int num_unique_sections = calc_num_unique_sections(sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array);
    int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][3];

    fill_sectiondetailsarray(
        num_unique_sections, max_num_courses_for_single_section, sectiondetailsarray,
        sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array,
        coursescsv_numrecords, coursescsv_numcols, coursescsv_longestvaluelen + 1, coursescsv_raw_array);

    int unique_faculty_array[sectionscsv_numrecords];
    int num_unique_faculty = fill_unique_facultyarray_return_num_unique_faculty(sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array, unique_faculty_array);

    int unique_courses_array[sectionscsv_numrecords];
    int num_unique_courses = fill_unique_coursesarray_return_num_unique_courses(sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array, unique_courses_array);

    int rooms_array[roomscsv_numrecords];
    for (int i = 0; i < roomscsv_numrecords; i++)
    {
        rooms_array[i] = strtol(roomscsv_raw_array[i][0], NULL, 10);
    }
    printf("Done.\n");

    printf("Writing Data to JSON file... ");
    write_output_json_file(
        OutputJSONPath,
        num_unique_sections, max_num_courses_for_single_section, sectiondetailsarray,
        num_unique_faculty, unique_faculty_array,
        num_unique_courses, unique_courses_array,
        num_slots_per_day, num_days_per_week, max_num_courses_for_single_section, num_unique_sections,
        roomscsv_numrecords, rooms_array, num_unique_faculty);
    printf("Done.\n");

    int faculty_timetable_array[num_unique_faculty][num_days_per_week][num_slots_per_day];

    int section_timetable_array[num_unique_sections][num_days_per_week][num_slots_per_day];

    int room_timetable_array[roomscsv_numrecords][num_days_per_week][num_slots_per_day];
    
    printf("Calling MiniZinc and obtaining Timetables...\n");
    call_minizinc_and_fill_timetable_arrays(
        COMMAND, num_slots_per_day, num_days_per_week,
        num_unique_faculty, faculty_timetable_array,
        num_unique_sections, section_timetable_array,
        roomscsv_numrecords, room_timetable_array);

    for (int i = 0; i < num_unique_faculty; i++)
    {
        printf("Faculty: %d\n", i);
        for (int j = 0; j < num_days_per_week; j++)
        {
            printf("Day %d: \t", j);
            for (int k = 0; k < num_slots_per_day; k++)
            {
                printf("%d ", faculty_timetable_array[i][j][k]);
            }
            printf("\n");
        }
    }

    for (int i = 0; i < num_unique_sections; i++)
    {
        printf("Section: %d\n", i);
        for (int j = 0; j < num_days_per_week; j++)
        {
            printf("Day %d: \t", j);
            for (int k = 0; k < num_slots_per_day; k++)
            {
                printf("%d ", section_timetable_array[i][j][k]);
            }
            printf("\n");
        }
    }

    for (int i = 0; i < roomscsv_numrecords; i++)
    {
        printf("Room: %d\n", i);
        for (int j = 0; j < num_days_per_week; j++)
        {
            printf("Day %d: \t", j);
            for (int k = 0; k < num_slots_per_day; k++)
            {
                printf("%d ", room_timetable_array[i][j][k]);
            }
            printf("\n");
        }
    }
    
    printf("Done.\n");
    return 0;
}

void fill_csv_metadata(char csvfilepath[], char col_delimitchar, char row_delimitchar, char commentchar, int *num_records_out_ptr, int *num_cols_out_ptr, int *max_valuelen_out_ptr)
{
    FILE *csvfilepointer;
    csvfilepointer = fopen(csvfilepath, "r");

    char curchar, prevchar = '\0';
    int linenum = 0, commentlinestate = 0;
    int cur_valuelen = 0;
    *max_valuelen_out_ptr = 0;
    /* My method adds 1 to *num_records_out_ptr at every newline except first (cause headings).
    The last row ends with EOF, not newline, so init *num_records_out_ptr with 1.
    My method counts number of col_delimitchar to get *num_cols_out_ptr,
    and *num_cols_out_ptr will always be one greater than number of col_delimitchar.*/
    *num_records_out_ptr = 1;
    *num_cols_out_ptr = 1;
    while ((curchar = getc(csvfilepointer)) != EOF)
    {
        if (prevchar == row_delimitchar)
        {
            linenum++;
            commentlinestate = (curchar == commentchar ? 1 : 0);
        }
        if (!commentlinestate)
        {
            if (linenum > 0) // For skipping the row with column headings
            {
                if (curchar != col_delimitchar && curchar != row_delimitchar)
                {
                    cur_valuelen++;
                }
                else
                {
                    if (curchar == row_delimitchar)
                    {
                        (*num_records_out_ptr)++;
                    }
                    if (*max_valuelen_out_ptr < cur_valuelen)
                    {
                        *max_valuelen_out_ptr = cur_valuelen;
                    }
                    cur_valuelen = 0;
                }
            }
            else
            {
                if (curchar == col_delimitchar)
                {
                    (*num_cols_out_ptr)++;
                }
            }
        }
        prevchar = curchar;
    }
    fclose(csvfilepointer);
}

void fill_csv_raw_array(char csvfilepath[], char col_delimitchar, char row_delimitchar, char commentchar, int num_records, int num_cols, int value_str_len, char csv_raw_array[num_records][num_cols][value_str_len])
{

    FILE *csvfilepointer;
    csvfilepointer = fopen(csvfilepath, "r");

    char curchar, prevchar = '\0';
    int linenum = 0, commentlinestate = 0;

    int recordnum = 0, colnum = 0, raw_array_charcounter = 0;
    while (((curchar = getc(csvfilepointer)) != EOF))
    {
        if (prevchar == row_delimitchar)
        {
            linenum++;
            commentlinestate = (curchar == commentchar ? 1 : 0);
        }
        if (!commentlinestate)
        {
            if (linenum > 0) // For skipping the row with column headings
            {
                if (curchar != col_delimitchar && curchar != row_delimitchar)
                {
                    csv_raw_array[recordnum][colnum][raw_array_charcounter++] = curchar;
                }
                else
                {
                    csv_raw_array[recordnum][colnum][raw_array_charcounter] = '\0';
                    raw_array_charcounter = 0;
                    if (curchar == col_delimitchar)
                    {
                        colnum++;
                    }
                    else if (curchar == row_delimitchar)
                    {
                        recordnum++;
                        colnum = 0;
                    }
                }
            }
        }
        prevchar = curchar;
    }
    csv_raw_array[recordnum][colnum][raw_array_charcounter] = '\0';
    fclose(csvfilepointer);
}

int calc_max_num_courses_for_single_sec(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size])
{
    int max_num_courses_for_single_sec = 0;
    int num_courses_for_single_class = 0;
    int current_sectionid = -1;

    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        int section_id_as_int = strtol(sectionscsv_raw_array[i][0], NULL, 10);
        if (section_id_as_int != current_sectionid)
        {
            if (max_num_courses_for_single_sec < num_courses_for_single_class)
            {
                max_num_courses_for_single_sec = num_courses_for_single_class;
            }
            current_sectionid = section_id_as_int;
            num_courses_for_single_class = 1;
        }
        else
        {
            num_courses_for_single_class++;
        }
    }
    return max_num_courses_for_single_sec;
}

int calc_num_unique_sections(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size])
{
    int unique_sections_array[sectionscsv_numrecords];
    int first_section_id = strtol(sectionscsv_raw_array[0][0], NULL, 10);
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        unique_sections_array[i] = first_section_id;
    }
    int unique_sections_array_index = 1;
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        int sectionid_as_int = strtol(sectionscsv_raw_array[i][0], NULL, 10);
        if (!(int_value_in_array(sectionid_as_int, unique_sections_array, sectionscsv_numrecords)))
        {
            unique_sections_array[unique_sections_array_index++] = sectionid_as_int;
        }
    }
    return unique_sections_array_index;
}

int int_value_in_array(int value, int array[], int arraylen)
{
    for (int i = 0; i < arraylen; i++)
    {
        if (value == array[i])
        {
            return 1;
        }
    }
    return 0;
}

void fill_sectiondetailsarray(
    int num_unique_sections, int max_num_courses_for_single_section, int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][3],
    int sectionscsv_numrecords, int sectionscsv_numcols, int sectionscsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectionscsv_data_size],
    int coursescsv_numrecords, int coursescsv_numcols, int coursescsv_data_size, char coursescsv_raw_array[coursescsv_numrecords][coursescsv_numcols][coursescsv_data_size])
{
    for (int i = 0; i < num_unique_sections; i++)
    {
        for (int j = 0; j < max_num_courses_for_single_section; j++)
        {
            sectiondetailsarray[i][j][0] = -1;
            sectiondetailsarray[i][j][1] = -1;
            sectiondetailsarray[i][j][2] = -1;
        }
    }

    int current_section_id = -1;
    int sectiondetailsarraysectioncounter = 0;
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        int j;
        int section_id_as_int = strtol(sectionscsv_raw_array[i][0], NULL, 10);
        if (current_section_id != section_id_as_int)
        {
            current_section_id = section_id_as_int;
            sectiondetailsarraysectioncounter = 0;
        }
        for (j = 0; j < coursescsv_numrecords; j++)
        {
            if (strtol(sectionscsv_raw_array[i][4], NULL, 10) == strtol(coursescsv_raw_array[j][0], NULL, 10))
            {
                break;
            }
        }
        sectiondetailsarray[section_id_as_int][sectiondetailsarraysectioncounter][0] = strtol(sectionscsv_raw_array[i][4], NULL, 10);
        sectiondetailsarray[section_id_as_int][sectiondetailsarraysectioncounter][1] = strtol(sectionscsv_raw_array[i][5], NULL, 10);
        sectiondetailsarray[section_id_as_int][sectiondetailsarraysectioncounter++][2] = strtol(coursescsv_raw_array[j][4], NULL, 10);
    }
}

int fill_unique_facultyarray_return_num_unique_faculty(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size], int unique_faculty_array[sectionscsv_numrecords])
{
    int first_faculty_id = strtol(sectionscsv_raw_array[0][5], NULL, 10);
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        unique_faculty_array[i] = first_faculty_id;
    }
    int unique_faculty_array_index = 1;
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        int facultyid_as_int = strtol(sectionscsv_raw_array[i][5], NULL, 10);
        if (!(int_value_in_array(facultyid_as_int, unique_faculty_array, sectionscsv_numrecords)))
        {
            unique_faculty_array[unique_faculty_array_index++] = facultyid_as_int;
        }
    }
    return unique_faculty_array_index;
}

int fill_unique_coursesarray_return_num_unique_courses(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size], int unique_courses_array[sectionscsv_numrecords])
{
    int first_course_id = strtol(sectionscsv_raw_array[0][4], NULL, 10);
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        unique_courses_array[i] = first_course_id;
    }
    int unique_courses_array_index = 1;
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        int courseid_as_int = strtol(sectionscsv_raw_array[i][4], NULL, 10);
        if (!(int_value_in_array(courseid_as_int, unique_courses_array, sectionscsv_numrecords)))
        {
            unique_courses_array[unique_courses_array_index++] = courseid_as_int;
        }
    }
    return unique_courses_array_index;
}

void write_output_json_file(
    char OutputJSONPath[],
    int num_unique_sections, int max_num_courses_for_single_section, int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][3],
    int num_unique_faculty, int unique_faculty_array[],
    int num_unique_courses, int unique_courses_array[],
    int num_slots_per_day,
    int num_days_per_week,
    int num_courses_per_section,
    int num_sections,
    int num_rooms, int roomsarray[],
    int num_faculty)
{
    FILE *outputjsonfilepointer;
    outputjsonfilepointer = fopen(OutputJSONPath, "w");
    fprintf(outputjsonfilepointer, "{");
    fprintf(outputjsonfilepointer, "\"sectiondetails\": [");
    for (int i = 0; i < num_unique_sections; i++)
    {
        fprintf(outputjsonfilepointer, "[");
        for (int j = 0; j < max_num_courses_for_single_section; j++)
        {
            fprintf(outputjsonfilepointer, "[%d, %d, %d]", sectiondetailsarray[i][j][0], sectiondetailsarray[i][j][1], sectiondetailsarray[i][j][2]);
            if (j < max_num_courses_for_single_section - 1)
            {
                fprintf(outputjsonfilepointer, ", ");
            }
        }
        fprintf(outputjsonfilepointer, "]");
        if (i < num_unique_sections - 1)
        {
            fprintf(outputjsonfilepointer, ", ");
        }
    }
    fprintf(outputjsonfilepointer, "],");
    fprintf(outputjsonfilepointer, "\"facultyarray\": [");
    for (int i = 0; i < num_unique_faculty; i++)
    {
        fprintf(outputjsonfilepointer, "%d", unique_faculty_array[i]);
        if (i < num_unique_faculty - 1)
        {
            fprintf(outputjsonfilepointer, ",");
        }
    }
    fprintf(outputjsonfilepointer, "],");
    fprintf(outputjsonfilepointer, "\"coursesarray\": [0,"); // Add course 0 (free period) manually here.
    for (int i = 0; i < num_unique_courses; i++)
    {
        fprintf(outputjsonfilepointer, "%d", unique_courses_array[i]);
        if (i < num_unique_courses - 1)
        {
            fprintf(outputjsonfilepointer, ",");
        }
    }
    fprintf(outputjsonfilepointer, "],");
    fprintf(outputjsonfilepointer, "\"rooms\": [");
    for (int i = 0; i < num_rooms; i++)
    {
        fprintf(outputjsonfilepointer, "%d", roomsarray[i]);
        if (i < num_rooms - 1)
        {
            fprintf(outputjsonfilepointer, ",");
        }
    }
    fprintf(outputjsonfilepointer, "],");
    fprintf(outputjsonfilepointer, "\"slots\":%d,", num_slots_per_day);
    fprintf(outputjsonfilepointer, "\"days\":%d,", num_days_per_week);
    fprintf(outputjsonfilepointer, "\"num_courses_per_section\":%d,", num_courses_per_section);
    fprintf(outputjsonfilepointer, "\"num_sections\":%d,", num_sections);
    fprintf(outputjsonfilepointer, "\"num_faculty\":%d", num_faculty);
    fprintf(outputjsonfilepointer, "}");
    fclose(outputjsonfilepointer);
}

void call_minizinc_and_fill_timetable_arrays(
    char COMMAND[], int num_slots_per_day, int num_days_per_week,
    int num_faculty, int faculty_timetable_array[num_faculty][num_days_per_week][num_slots_per_day],
    int num_sections, int section_timetable_array[num_sections][num_days_per_week][num_slots_per_day],
    int num_rooms, int room_timetable_array[num_rooms][num_days_per_week][num_slots_per_day])
{

    FILE *MiniZincCall = popen(COMMAND, "r");
    char curchar;
    int charcounter = 0;
    int chunksize = 128;
    char *MiniZincCallOutput = malloc(chunksize);
    if (MiniZincCallOutput == NULL)
    {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    while ((curchar = getc(MiniZincCall)) != EOF)
    {
        if (charcounter >= chunksize - 1)
        {
            /* time to make it bigger */
            chunksize += 128;
            MiniZincCallOutput = realloc(MiniZincCallOutput, chunksize);
            if (MiniZincCallOutput == NULL)
            {
                fprintf(stderr, "out of memory\n");
                exit(1);
            }
        }

        MiniZincCallOutput[charcounter++] = curchar;
    }
    MiniZincCallOutput[charcounter] = '\0';
    pclose(MiniZincCall);

    typedef enum
    {
        FACULTY = 0,
        SECTION = 1,
        ROOM = 2,
        NONE = -1
    } timetable_kind;
    int facultyid = 0, facultyday = 0, facultyslot = 0;
    int sectionid = 0, sectionday = 0, sectionslot = 0;
    int roomid = 0, roomday = 0, roomslot = 0;
    jsmn_parser json_parser;
    jsmn_init(&json_parser);
    int num_tokens = jsmn_parse(&json_parser, MiniZincCallOutput, charcounter, NULL, -1);
    jsmntok_t tokens_array[num_tokens];

    jsmn_init(&json_parser);
    jsmn_parse(&json_parser, MiniZincCallOutput, charcounter, tokens_array, num_tokens);
    char last_string_key_read[17];
    timetable_kind current_timetable_kind = NONE;
    for (int i = 0; i < num_tokens; i++)
    {
        jsmntok_t key = tokens_array[i];
        unsigned int length = key.end - key.start;
        char keyString[length + 1];
        memcpy(keyString, &MiniZincCallOutput[key.start], length);
        keyString[length] = '\0';
        if (key.type == JSMN_STRING)
        {
            if (!(strcmp(keyString, "facultyTimetable")))
            {
                current_timetable_kind = FACULTY;
            }
            else if (!(strcmp(keyString, "sectionTimetable")))
            {
                current_timetable_kind = SECTION;
            }
            else if (!(strcmp(keyString, "roomTimetable")))
            {
                current_timetable_kind = ROOM;
            }
            else
            {
                current_timetable_kind = NONE;
            }
        }
        if (key.type == JSMN_PRIMITIVE)
        {
            if (current_timetable_kind == FACULTY)
            {
                faculty_timetable_array[facultyid][facultyday][facultyslot++] = strtol(keyString, NULL, 10);
                if (facultyslot == num_slots_per_day)
                {
                    facultyday++;
                    facultyslot = 0;
                }
                if (facultyday == num_days_per_week)
                {
                    facultyid++;
                    facultyday = 0;
                    facultyslot = 0;
                }
            }
            else if (current_timetable_kind == SECTION)
            {
                section_timetable_array[sectionid][sectionday][sectionslot++] = strtol(keyString, NULL, 10);
                if (sectionslot == num_slots_per_day)
                {
                    sectionday++;
                    sectionslot = 0;
                }
                if (sectionday == num_days_per_week)
                {
                    sectionid++;
                    sectionday = 0;
                    sectionslot = 0;
                }
            }
            else if (current_timetable_kind == ROOM)
            {
                room_timetable_array[roomid][roomday][roomslot++] = strtol(keyString, NULL, 10);
                if (roomslot == num_slots_per_day)
                {
                    roomday++;
                    roomslot = 0;
                }
                if (roomday == num_days_per_week)
                {
                    roomid++;
                    roomday = 0;
                    roomslot = 0;
                }
            }
        }
    }
}