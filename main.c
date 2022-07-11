#include <stdio.h>
#include <stdlib.h>

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
    int num_unique_sections, int max_num_courses_for_single_section, int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][3], int num_unique_teachers, int unique_teachers_array[]);
int fill_unique_teachersarray_return_num_unique_teachers(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size], int unique_teachers_array[sectionscsv_numrecords]);

int main()
{
    char CoursesCSVPath[] = "./inputdata/Courses.csv";
    char FacultyCSVPath[] = "./inputdata/Faculty.csv";
    char SectionsCSVPath[] = "./inputdata/Sections.csv";
    char RoomsCSVPath[] = "./inputdata/Rooms.csv";
    char OutputJSONPath[] = "InputData.json";

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

    int unique_teachers_array[sectionscsv_numrecords];
    int num_unique_teachers = fill_unique_teachersarray_return_num_unique_teachers(sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array, unique_teachers_array);
    write_output_json_file(OutputJSONPath, num_unique_sections, max_num_courses_for_single_section, sectiondetailsarray, num_unique_teachers, unique_teachers_array);
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

int fill_unique_teachersarray_return_num_unique_teachers(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size], int unique_teachers_array[sectionscsv_numrecords])
{
    int first_teacher_id = strtol(sectionscsv_raw_array[0][5], NULL, 10);
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        unique_teachers_array[i] = first_teacher_id;
    }
    int unique_sections_array_index = 1;
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        int teacherid_as_int = strtol(sectionscsv_raw_array[i][5], NULL, 10);
        if (!(int_value_in_array(teacherid_as_int, unique_teachers_array, sectionscsv_numrecords)))
        {
            unique_teachers_array[unique_sections_array_index++] = teacherid_as_int;
        }
    }
    return unique_sections_array_index;
}

void write_output_json_file(
    char OutputJSONPath[],
    int num_unique_sections, int max_num_courses_for_single_section, int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][3], int num_unique_teachers, int unique_teachers_array[])
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
    fprintf(outputjsonfilepointer, "\"teacherarray\": [");
    for (int i = 0; i < num_unique_teachers; i++)
    {
        fprintf(outputjsonfilepointer, "%d", unique_teachers_array[i]);
        if (i < num_unique_teachers - 1)
        {
            fprintf(outputjsonfilepointer, ",");
        }
    }
    fprintf(outputjsonfilepointer, "]");
    fprintf(outputjsonfilepointer, "}");
}