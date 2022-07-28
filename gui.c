#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/csv.h"
#include "lib/ext/jsmn/jsmn.h"

int int_value_in_array(int value, int array[], int arraylen);
int calc_max_num_courses_for_single_sec(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size]);
int calc_num_unique_sections(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size]);
char *coursename_from_courseid(int courseid, int coursescsv_numrecords, int coursescsv_numcols, int coursescsv_data_size, char coursescsv_raw_array[coursescsv_numrecords][coursescsv_numcols][coursescsv_data_size]);
char *facultyname_from_facultyid(int facultyid, int facultycsv_numrecords, int facultycsv_numcols, int facultycsv_data_size, char facultycsv_raw_array[facultycsv_numrecords][facultycsv_numcols][facultycsv_data_size]);
char *sectionname_from_sectionid(int sectionid, int sectioncsv_numrecords, int sectioncsv_numcols, int sectioncsv_data_size, char sectioncsv_raw_array[sectioncsv_numrecords][sectioncsv_numcols][sectioncsv_data_size]);
void fill_sectiondetailsarray(
    int num_unique_sections, int max_num_courses_for_single_section, int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][4],
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
int fill_unique_facultyarray_return_num_unique_faculty(int facultycsv_numrecords, int facultycsv_numcols, int facultycsv_data_size, char facultycsv_raw_array[facultycsv_numrecords][facultycsv_numcols][facultycsv_data_size], int unique_faculty_array[facultycsv_numrecords]);
int fill_unique_coursesarray_return_num_unique_courses(int sectionscsv_numrecords, int sectionscsv_numcols, int sectioncsv_data_size, char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectioncsv_data_size], int unique_courses_array[sectionscsv_numrecords]);

void call_minizinc_and_fill_timetable_arrays(
    char COMMAND[], int num_slots_per_day, int num_days_per_week,
    int num_faculty, int faculty_timetable_array[num_faculty][num_days_per_week][num_slots_per_day],
    int num_sections, int section_timetable_array[num_sections][num_days_per_week][num_slots_per_day],
    int num_rooms, int room_timetable_array[num_rooms][num_days_per_week][num_slots_per_day]);

const char defaultcoursescsvpath[] = "inputdata/Courses.csv";
const char defaultfacultycsvpath[] = "inputdata/Faculty.csv";
const char defaultroomscsvpath[] = "inputdata/Rooms.csv";
const char defaultsectionscsvpath[] = "inputdata/Sections.csv";

static void on_filechooser_response(GtkNativeDialog *native, int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        GtkWidget *pathtextentry = g_object_get_data(G_OBJECT(native), "pathtextentry");
        GFile *file = gtk_file_chooser_get_file(chooser);
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(pathtextentry)), g_file_get_path(file), -1);
        gtk_entry_set_placeholder_text(GTK_ENTRY(pathtextentry), gtk_entry_get_placeholder_text(GTK_ENTRY(pathtextentry)));
        gtk_native_dialog_hide(native);
        g_object_unref(file);
    }
    gtk_native_dialog_hide(native);
}

static void open_selector_dialog(GtkButton *button, gpointer data)
{
    GtkFileChooserNative *dialog = data;
    g_object_set_data(G_OBJECT(dialog), "pathtextentry", g_object_get_data(G_OBJECT(button), "pathtextentry"));
    g_signal_connect(dialog, "response", G_CALLBACK(on_filechooser_response), NULL);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(dialog));
}

static void set_entity_model_for_treeview(GtkComboBoxText *timetableentityselectorcombobox, gpointer data)
{
    g_print("Entering set_entity_model_for_treeview\n");
    GtkComboBoxText *timetabletypeselectorcombobox = data;
    int timetabletype = gtk_combo_box_get_active(GTK_COMBO_BOX(timetabletypeselectorcombobox));
    int entityindex = gtk_combo_box_get_active(GTK_COMBO_BOX(timetableentityselectorcombobox));
    GtkTreeView *tree = g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "tree");
    GPtrArray *entityliststorearray;
    if (timetabletype == 0)
    {
        entityliststorearray = g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "facu_timetable_store_array");
    }
    else if (timetabletype == 1)
    {
        entityliststorearray = g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "sect_timetable_store_array");
    }
    else
    {
        entityliststorearray = g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "room_timetable_store_array");
    }
    GtkListStore *store = g_ptr_array_index(entityliststorearray, entityindex);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(store));
    g_print("Leaving set_entity_model_for_treeview\n\n");
}
static void set_entityselector_entities(GtkComboBoxText *timetabletypeselectorcombobox, gpointer data)
{
    g_print("Entering Set EntitySelectorEntities.\n");
    GtkComboBoxText *timetableentityselectorcombobox = data;
    // This function (self) changes the value of timetableentityselectorcombobox.
    // To avoid triggering a callback to set_entity_model_for_treeview before we set the entities
    // and causing a segfault by reading memory that isn't there we disconnect the signal handlers 
    // to set_entity_model_for_treeview for timetableentityselectorcombobox and reconnect them at the end of this function.

    g_signal_handlers_disconnect_by_func(timetableentityselectorcombobox, G_CALLBACK(set_entity_model_for_treeview), timetabletypeselectorcombobox);

    int num_unique_faculty = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "num_unique_faculty"));
    int facultycsv_numrecords = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "facultycsv_numrecords"));
    int facultycsv_numcols = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "facultycsv_numcols"));
    int facultycsv_longestvaluelen = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "facultycsv_longestvaluelen"));
    int num_unique_sections = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "num_unique_sections"));
    int sectionscsv_numrecords = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "sectionscsv_numrecords"));
    int sectionscsv_numcols = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "sectionscsv_numcols"));
    int sectionscsv_longestvaluelen = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "sectionscsv_longestvaluelen"));
    int roomscsv_numrecords = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "roomscsv_numrecords"));
    const char *FacultyCSVPath = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "facultycsvpathentry"))));
    const char *SectionsCSVPath = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(g_object_get_data(G_OBJECT(timetableentityselectorcombobox), "sectionscsvpathentry"))));
    int unique_faculty_array[num_unique_faculty];
    char facultycsv_raw_array[facultycsv_numrecords][facultycsv_numcols][facultycsv_longestvaluelen + 1];
    char sectionscsv_raw_array[sectionscsv_numrecords][sectionscsv_numcols][sectionscsv_longestvaluelen + 1];

    fill_csv_raw_array(FacultyCSVPath, ',', '\n', '#', facultycsv_numrecords, facultycsv_numcols, facultycsv_longestvaluelen + 1, facultycsv_raw_array);
    fill_csv_raw_array(SectionsCSVPath, ',', '\n', '#', sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array);
    fill_unique_facultyarray_return_num_unique_faculty(facultycsv_numrecords, facultycsv_numcols, facultycsv_longestvaluelen + 1, facultycsv_raw_array, unique_faculty_array);
    // TODO: GET RID OF THIS WHEN YOU START USING FACULTY FOR EVERYTHING
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
    // END TODO
    int timetabletype = gtk_combo_box_get_active(GTK_COMBO_BOX(timetabletypeselectorcombobox));

    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(timetableentityselectorcombobox));

    if (timetabletype == 0)
    {
        for (int i = 0; i < num_unique_faculty; i++)
        {
            gtk_combo_box_text_append_text(
                GTK_COMBO_BOX_TEXT(timetableentityselectorcombobox),
                facultyname_from_facultyid(unique_faculty_array[i], facultycsv_numrecords, facultycsv_numcols, facultycsv_longestvaluelen + 1, facultycsv_raw_array));
        }
    }
    else if (timetabletype == 1)
    {
        for (int i = 0; i < num_unique_sections; i++)
        {
            gtk_combo_box_text_append_text(
                GTK_COMBO_BOX_TEXT(timetableentityselectorcombobox),
                sectionname_from_sectionid(unique_sections_array[i], sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array));
        }
    }
    else if (timetabletype == 2)
    {
        for (int i = 0; i < roomscsv_numrecords; i++)
        {
            gtk_combo_box_text_append_text(
                GTK_COMBO_BOX_TEXT(timetableentityselectorcombobox),
                g_strdup_printf("RoomID %i", i));
        }
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(timetableentityselectorcombobox), 0);
    g_signal_connect(timetableentityselectorcombobox, "realize", G_CALLBACK(set_entity_model_for_treeview), timetabletypeselectorcombobox);
    g_signal_connect(timetableentityselectorcombobox, "changed", G_CALLBACK(set_entity_model_for_treeview), timetabletypeselectorcombobox);
    g_print("Leaving Set EntitySelectorEntities.\n\n");
}

static void solve_for_timetable(GtkButton *button, gpointer data)
{
    const char *CoursesCSVPath = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(g_object_get_data(G_OBJECT(button), "coursescsvpathentry"))));
    const char *FacultyCSVPath = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(g_object_get_data(G_OBJECT(button), "facultycsvpathentry"))));
    const char *SectionsCSVPath = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(g_object_get_data(G_OBJECT(button), "sectionscsvpathentry"))));
    const char *RoomsCSVPath = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(g_object_get_data(G_OBJECT(button), "roomscsvpathentry"))));
    char OutputJSONPath[] = "./InputData.json";
    char COMMAND[] = "minizinc main.mzn InputData.json --solver chuffed --json-stream --output-mode json";

    int num_slots_per_day = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(g_object_get_data(G_OBJECT(button), "slotsperdayspinbutton")));
    int num_days_per_week = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(g_object_get_data(G_OBJECT(button), "daysperweekspinbutton")));

    g_print("Reading data from CSV Files... ");
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

    int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][4];

    fill_sectiondetailsarray(
        num_unique_sections, max_num_courses_for_single_section, sectiondetailsarray,
        sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array,
        coursescsv_numrecords, coursescsv_numcols, coursescsv_longestvaluelen + 1, coursescsv_raw_array);

    int unique_faculty_array[sectionscsv_numrecords];
    int num_unique_faculty = fill_unique_facultyarray_return_num_unique_faculty(facultycsv_numrecords, facultycsv_numcols, facultycsv_longestvaluelen + 1, facultycsv_raw_array, unique_faculty_array);

    int unique_courses_array[sectionscsv_numrecords];
    int num_unique_courses = fill_unique_coursesarray_return_num_unique_courses(sectionscsv_numrecords, sectionscsv_numcols, sectionscsv_longestvaluelen + 1, sectionscsv_raw_array, unique_courses_array);

    int rooms_array[roomscsv_numrecords];
    for (int i = 0; i < roomscsv_numrecords; i++)
    {
        rooms_array[i] = strtol(roomscsv_raw_array[i][0], NULL, 10);
    }
    g_print("Done.\n");

    g_print("Writing Data to JSON file... ");
    // write_output_json_file(
    //     OutputJSONPath,
    //     num_unique_sections, max_num_courses_for_single_section, sectiondetailsarray,
    //     num_unique_faculty, unique_faculty_array,
    //     num_unique_courses, unique_courses_array,
    //     num_slots_per_day, num_days_per_week, max_num_courses_for_single_section, num_unique_sections,
    //     roomscsv_numrecords, rooms_array, num_unique_faculty);
    g_print("Done.\n");

    int faculty_timetable_array[num_unique_faculty][num_days_per_week][num_slots_per_day];

    int section_timetable_array[num_unique_sections][num_days_per_week][num_slots_per_day];

    int room_timetable_array[roomscsv_numrecords][num_days_per_week][num_slots_per_day];

    g_print("Calling MiniZinc and obtaining Timetables...\n");
    call_minizinc_and_fill_timetable_arrays(
        COMMAND, num_slots_per_day, num_days_per_week,
        num_unique_faculty, faculty_timetable_array,
        num_unique_sections, section_timetable_array,
        roomscsv_numrecords, room_timetable_array);

    for (int i = 0; i < num_unique_faculty; i++)
    {
        g_print("Faculty: %d\n", i);
        for (int j = 0; j < num_days_per_week; j++)
        {
            g_print("Day %d: \t", j);
            for (int k = 0; k < num_slots_per_day; k++)
            {
                g_print("%d ", faculty_timetable_array[i][j][k]);
            }
            g_print("\n");
        }
    }

    for (int i = 0; i < num_unique_sections; i++)
    {
        g_print("Section: %d\n", i);
        for (int j = 0; j < num_days_per_week; j++)
        {
            g_print("Day %d: \t", j);
            for (int k = 0; k < num_slots_per_day; k++)
            {
                g_print("%d ", section_timetable_array[i][j][k]);
            }
            g_print("\n");
        }
    }

    for (int i = 0; i < roomscsv_numrecords; i++)
    {
        g_print("Room: %d\n", i);
        for (int j = 0; j < num_days_per_week; j++)
        {
            g_print("Day %d: \t", j);
            for (int k = 0; k < num_slots_per_day; k++)
            {
                g_print("%d ", room_timetable_array[i][j][k]);
            }
            g_print("\n");
        }
    }

    GtkWidget *outputwindow;
    GtkWidget *outputwindowgrid;
    GtkWidget *timetabletypeselectorcombobox;
    GtkWidget *timetableentityselectorcombobox;

    outputwindowgrid = gtk_grid_new();
    char *DaysOfWeek[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    int entityindex = 0;

    timetabletypeselectorcombobox = gtk_combo_box_text_new();

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(timetabletypeselectorcombobox), "Faculty");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(timetabletypeselectorcombobox), "Section");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(timetabletypeselectorcombobox), "Room");

    gtk_combo_box_set_active(GTK_COMBO_BOX(timetabletypeselectorcombobox), 0);

    timetableentityselectorcombobox = gtk_combo_box_text_new();

    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "num_unique_faculty", GINT_TO_POINTER(num_unique_faculty));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "facultycsv_numrecords", GINT_TO_POINTER(facultycsv_numrecords));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "facultycsv_numcols", GINT_TO_POINTER(facultycsv_numcols));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "facultycsv_longestvaluelen", GINT_TO_POINTER(facultycsv_longestvaluelen));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "num_unique_sections", GINT_TO_POINTER(num_unique_sections));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "sectionscsv_numrecords", GINT_TO_POINTER(sectionscsv_numrecords));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "sectionscsv_numcols", GINT_TO_POINTER(sectionscsv_numcols));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "sectionscsv_longestvaluelen", GINT_TO_POINTER(sectionscsv_longestvaluelen));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "roomscsv_numrecords", GINT_TO_POINTER(roomscsv_numrecords));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "facultycsvpathentry", g_object_get_data(G_OBJECT(button), "facultycsvpathentry"));
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "sectionscsvpathentry", g_object_get_data(G_OBJECT(button), "sectionscsvpathentry"));

    g_signal_connect(timetabletypeselectorcombobox, "realize", G_CALLBACK(set_entityselector_entities), timetableentityselectorcombobox);
    g_signal_connect(timetabletypeselectorcombobox, "changed", G_CALLBACK(set_entityselector_entities), timetableentityselectorcombobox);

    int treeviewnum_cols = num_slots_per_day + 1;

    GType col_datatypes[treeviewnum_cols];
    for (int i = 0; i < treeviewnum_cols; i++)
    {
        col_datatypes[i] = G_TYPE_STRING;
    }

    GPtrArray *facu_timetable_store_array = g_ptr_array_new();
    GPtrArray *sect_timetable_store_array = g_ptr_array_new();
    GPtrArray *room_timetable_store_array = g_ptr_array_new();
    for (int timetabletype = 0; timetabletype < 3; timetabletype++)
    {
        int num_entities;
        if (timetabletype == 0)
        {
            num_entities = num_unique_faculty;
        }
        else if (timetabletype == 1)
        {
            num_entities = num_unique_sections;
        }
        else
        {
            num_entities = roomscsv_numrecords;
        }

        for (int entityindex = 0; entityindex < num_entities; entityindex++)
        {
            GtkListStore *store = gtk_list_store_newv(treeviewnum_cols, col_datatypes);
            GtkTreeIter iter;
            for (int dayofweek = 0; dayofweek < num_days_per_week; dayofweek++)
            {
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter, 0, DaysOfWeek[dayofweek], -1);
                for (int treeviewcolnum = 1; treeviewcolnum < treeviewnum_cols; treeviewcolnum++)
                {
                    if (timetabletype == 0)
                    {
                        gtk_list_store_set(
                            store, &iter, treeviewcolnum,
                            coursename_from_courseid(
                                faculty_timetable_array[entityindex][dayofweek][treeviewcolnum - 1],
                                coursescsv_numrecords,
                                coursescsv_numcols,
                                coursescsv_longestvaluelen + 1,
                                coursescsv_raw_array),
                            -1);
                    }
                    else if (timetabletype == 1)
                    {
                        gtk_list_store_set(
                            store, &iter, treeviewcolnum,
                            coursename_from_courseid(
                                section_timetable_array[entityindex][dayofweek][treeviewcolnum - 1],
                                coursescsv_numrecords,
                                coursescsv_numcols,
                                coursescsv_longestvaluelen + 1,
                                coursescsv_raw_array),
                            -1);
                    }
                    else
                    {
                        gtk_list_store_set(
                            store, &iter, treeviewcolnum,
                            coursename_from_courseid(
                                room_timetable_array[entityindex][dayofweek][treeviewcolnum - 1],
                                coursescsv_numrecords,
                                coursescsv_numcols,
                                coursescsv_longestvaluelen + 1,
                                coursescsv_raw_array),
                            -1);
                    }
                }
            }
            if (timetabletype == 0)
            {
                g_ptr_array_add(facu_timetable_store_array, store);
            }
            else if (timetabletype == 1)
            {
                g_ptr_array_add(sect_timetable_store_array, store);
            }
            else
            {
                g_ptr_array_add(room_timetable_store_array, store);
            }
        }
    }

    GtkWidget *tree;
    tree = gtk_tree_view_new();

    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "facu_timetable_store_array", facu_timetable_store_array);
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "sect_timetable_store_array", sect_timetable_store_array);
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "room_timetable_store_array", room_timetable_store_array);
    g_object_set_data(G_OBJECT(timetableentityselectorcombobox), "tree", tree);

    g_signal_connect(timetableentityselectorcombobox, "realize", G_CALLBACK(set_entity_model_for_treeview), timetabletypeselectorcombobox);
    g_signal_connect(timetableentityselectorcombobox, "changed", G_CALLBACK(set_entity_model_for_treeview), timetabletypeselectorcombobox);

    g_signal_connect_swapped(timetabletypeselectorcombobox, "changed", G_CALLBACK(set_entity_model_for_treeview), timetableentityselectorcombobox);
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    renderer = gtk_cell_renderer_text_new();

    column = gtk_tree_view_column_new_with_attributes("Day\\Slot", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    for (int treeviewcolnum = 1; treeviewcolnum < treeviewnum_cols; treeviewcolnum++)
    {
        gchar *columnname = g_strdup_printf("Slot %i", treeviewcolnum);
        column = gtk_tree_view_column_new_with_attributes(columnname, renderer, "text", treeviewcolnum, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    }
    outputwindow = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(outputwindow), "TimeTable Output");
    gtk_window_set_child(GTK_WINDOW(outputwindow), outputwindowgrid);
    gtk_grid_attach(GTK_GRID(outputwindowgrid), tree, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(outputwindowgrid), timetabletypeselectorcombobox, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(outputwindowgrid), timetableentityselectorcombobox, 1, 1, 1, 1);
    gtk_widget_show(outputwindow);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    GtkWidget *solvebutton;
    GtkWidget *label;
    GtkWidget *pathtextentry;

    GtkFileChooserNative *fileselectordialog;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MiniZinc TimeTable Generator");

    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    fileselectordialog = gtk_file_chooser_native_new(
        "Open File",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "Open", "Cancel");

    solvebutton = gtk_button_new_with_label("Solve for Timetable");

    pathtextentry = gtk_entry_new();
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(pathtextentry)), defaultcoursescsvpath, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(pathtextentry), "Courses CSV Path");
    gtk_editable_set_editable(GTK_EDITABLE(pathtextentry), 0);
    gtk_grid_attach(GTK_GRID(grid), pathtextentry, 0, 0, 1, 1);

    button = gtk_button_new_with_label("Set Courses CSV File");
    g_object_set_data(G_OBJECT(button), "pathtextentry", pathtextentry);
    g_signal_connect(button, "clicked", G_CALLBACK(open_selector_dialog), fileselectordialog);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);
    g_object_set_data(G_OBJECT(solvebutton), "coursescsvpathentry", pathtextentry);

    pathtextentry = gtk_entry_new();
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(pathtextentry)), defaultfacultycsvpath, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(pathtextentry), "Faculty CSV Path");
    gtk_editable_set_editable(GTK_EDITABLE(pathtextentry), 0);
    gtk_grid_attach(GTK_GRID(grid), pathtextentry, 0, 1, 1, 1);
    g_object_set_data(G_OBJECT(solvebutton), "facultycsvpathentry", pathtextentry);

    button = gtk_button_new_with_label("Set Faculty CSV File");
    g_object_set_data(G_OBJECT(button), "pathtextentry", pathtextentry);
    g_signal_connect(button, "clicked", G_CALLBACK(open_selector_dialog), fileselectordialog);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);

    pathtextentry = gtk_entry_new();
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(pathtextentry)), defaultsectionscsvpath, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(pathtextentry), "Sections CSV Path");
    gtk_editable_set_editable(GTK_EDITABLE(pathtextentry), 0);
    gtk_grid_attach(GTK_GRID(grid), pathtextentry, 0, 2, 1, 1);
    g_object_set_data(G_OBJECT(solvebutton), "sectionscsvpathentry", pathtextentry);

    button = gtk_button_new_with_label("Set Sections CSV File");
    g_object_set_data(G_OBJECT(button), "pathtextentry", pathtextentry);
    g_signal_connect(button, "clicked", G_CALLBACK(open_selector_dialog), fileselectordialog);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 2, 1, 1);

    pathtextentry = gtk_entry_new();
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(pathtextentry)), defaultroomscsvpath, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(pathtextentry), "Rooms CSV Path");
    gtk_editable_set_editable(GTK_EDITABLE(pathtextentry), 0);
    gtk_grid_attach(GTK_GRID(grid), pathtextentry, 0, 3, 1, 1);
    g_object_set_data(G_OBJECT(solvebutton), "roomscsvpathentry", pathtextentry);

    button = gtk_button_new_with_label("Set Rooms CSV File");
    g_object_set_data(G_OBJECT(button), "pathtextentry", pathtextentry);
    g_signal_connect(button, "clicked", G_CALLBACK(open_selector_dialog), fileselectordialog);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 3, 1, 1);

    label = gtk_label_new("Days Per Week: ");
    gtk_grid_attach(
        GTK_GRID(grid),
        label,
        0, 4, 1, 1);
    button = gtk_spin_button_new(
        gtk_adjustment_new(2.0, 1.0, 7.0, 1.0, 1.0, 0.0),
        1.0, 0);
    g_object_set_data(G_OBJECT(solvebutton), "daysperweekspinbutton", button);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 4, 1, 1);

    label = gtk_label_new("Slots Per Day: ");
    gtk_grid_attach(
        GTK_GRID(grid),
        label,
        0, 5, 1, 1);
    button = gtk_spin_button_new(gtk_adjustment_new(6.0, 2.0, 20.0, 1.0, 1.0, 0.0), 1.0, 0);
    g_object_set_data(G_OBJECT(solvebutton), "slotsperdayspinbutton", button);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 5, 1, 1);

    g_signal_connect(solvebutton, "clicked", G_CALLBACK(solve_for_timetable), NULL);
    gtk_grid_attach(GTK_GRID(grid), solvebutton, 0, 6, 2, 1);

    gtk_widget_show(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
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
    int num_unique_sections, int max_num_courses_for_single_section, int sectiondetailsarray[num_unique_sections][max_num_courses_for_single_section][4],
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
            sectiondetailsarray[i][j][3] = -1;
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
        sectiondetailsarray[section_id_as_int][sectiondetailsarraysectioncounter][0] = current_section_id;
        sectiondetailsarray[section_id_as_int][sectiondetailsarraysectioncounter][1] = strtol(sectionscsv_raw_array[i][4], NULL, 10);
        sectiondetailsarray[section_id_as_int][sectiondetailsarraysectioncounter][2] = strtol(sectionscsv_raw_array[i][5], NULL, 10);
        sectiondetailsarray[section_id_as_int][sectiondetailsarraysectioncounter++][3] = strtol(coursescsv_raw_array[j][4], NULL, 10);
    }
}

int fill_unique_facultyarray_return_num_unique_faculty(int facultycsv_numrecords, int facultycsv_numcols, int facultycsv_data_size, char facultycsv_raw_array[facultycsv_numrecords][facultycsv_numcols][facultycsv_data_size], int unique_faculty_array[facultycsv_numrecords])
{
    int first_faculty_id = strtol(facultycsv_raw_array[0][0], NULL, 10);
    for (int i = 0; i < facultycsv_numrecords; i++)
    {
        unique_faculty_array[i] = first_faculty_id;
    }
    int unique_faculty_array_index = 1;
    for (int i = 0; i < facultycsv_numrecords; i++)
    {
        int facultyid_as_int = strtol(facultycsv_raw_array[i][0], NULL, 10);
        if (!(int_value_in_array(facultyid_as_int, unique_faculty_array, facultycsv_numrecords)))
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

char *coursename_from_courseid(int courseid, int coursescsv_numrecords, int coursescsv_numcols, int coursescsv_data_size, char coursescsv_raw_array[coursescsv_numrecords][coursescsv_numcols][coursescsv_data_size])
{
    for (int i = 0; i < coursescsv_numrecords; i++)
    {
        if (courseid == strtol(coursescsv_raw_array[i][0], NULL, 10))
        {
            return coursescsv_raw_array[i][1];
        }
    }
    return coursescsv_raw_array[0][1];
}

char *facultyname_from_facultyid(int facultyid, int facultycsv_numrecords, int facultycsv_numcols, int facultycsv_data_size, char facultycsv_raw_array[facultycsv_numrecords][facultycsv_numcols][facultycsv_data_size])
{
    for (int i = 0; i < facultycsv_numrecords; i++)
    {
        if (facultyid == strtol(facultycsv_raw_array[i][0], NULL, 10))
        {
            return facultycsv_raw_array[i][2];
        }
    }
    return "Unknown Professor";
}

char *sectionname_from_sectionid(int sectionid, int sectioncsv_numrecords, int sectioncsv_numcols, int sectioncsv_data_size, char sectioncsv_raw_array[sectioncsv_numrecords][sectioncsv_numcols][sectioncsv_data_size])
{
    for (int i = 0; i < sectioncsv_numrecords; i++)
    {
        if (sectionid == strtol(sectioncsv_raw_array[i][0], NULL, 10))
        {
            return g_strdup_printf("%s %s Sem %s", sectioncsv_raw_array[i][2], sectioncsv_raw_array[i][3], sectioncsv_raw_array[i][1]);
        }
    }
    return "Unknown Section";
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