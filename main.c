#include <stdio.h>

void fill_csv_metadata(char csvfilepath[], char col_delimitchar, char row_delimitchar, char commentchar, int *num_records_out_ptr, int *num_cols_out_ptr, int *max_valuelen_out_ptr);
void fill_csv_raw_array(char csvfilepath[], char col_delimitchar, char row_delimitchar, char commentchar, int num_records, int num_cols, int value_str_len, char csv_raw_array[num_records][num_cols][value_str_len]);

int main()
{
    char CoursesCSVPath[] = "./inputdata/Courses.csv";
    char FacultyCSVPath[] = "./inputdata/Faculty.csv";
    char SectionsCSVPath[] = "./inputdata/Sections.csv";
    char RoomsCSVPath[] = "./inputdata/Rooms.csv";

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

    printf("Printing Courses CSV:\n\n");
    for (int i = 0; i < coursescsv_numrecords; i++)
    {
        for (int j = 0; j < coursescsv_numcols; j++)
        {
            printf("%s,", coursescsv_raw_array[i][j]);
        }
        printf("\n");
    }
    printf("\nPrinting Faculty CSV:\n\n");
    for (int i = 0; i < facultycsv_numrecords; i++)
    {
        for (int j = 0; j < facultycsv_numcols; j++)
        {
            printf("%s,", facultycsv_raw_array[i][j]);
        }
        printf("\n");
    }
    printf("\nPrinting Sections CSV:\n\n");
    for (int i = 0; i < sectionscsv_numrecords; i++)
    {
        for (int j = 0; j < sectionscsv_numcols; j++)
        {
            printf("%s,", sectionscsv_raw_array[i][j]);
        }
        printf("\n");
    }
    printf("\nPrinting Rooms CSV:\n\n");
    for (int i = 0; i < roomscsv_numrecords; i++)
    {
        for (int j = 0; j < roomscsv_numcols; j++)
        {
            printf("%s,", roomscsv_raw_array[i][j]);
        }
        printf("\n");
    }
}

void fill_csv_metadata(char csvfilepath[], char col_delimitchar, char row_delimitchar, char commentchar, int *num_records_out_ptr, int *num_cols_out_ptr, int *max_valuelen_out_ptr)
{
    FILE *csvfilepointer;
    csvfilepointer = fopen(csvfilepath, "r");

    char curchar, prevchar = '\0';
    int linenum = 0, commentlinestate = 0;
    int cur_valuelen = 0;
    *max_valuelen_out_ptr = 0,
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