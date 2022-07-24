#include <gtk/gtk.h>

const char coursescsvpath[] = "inputdata/Courses.csv";
const char facultycsvpath[] = "inputdata/Faculty.csv";
const char roomscsvpath[] = "inputdata/Rooms.csv";
const char sectionscsvpath[] = "inputdata/Sections.csv";

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

static void activate(GtkApplication *app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;
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

  pathtextentry = gtk_entry_new();
  gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(pathtextentry)), coursescsvpath, -1);
  gtk_entry_set_placeholder_text(GTK_ENTRY(pathtextentry), "Courses CSV Path");
  gtk_editable_set_editable(GTK_EDITABLE(pathtextentry), 0);
  gtk_grid_attach(GTK_GRID(grid), pathtextentry, 0, 0, 1, 1);

  button = gtk_button_new_with_label("Set File");
  g_object_set_data(G_OBJECT(button), "pathtextentry", pathtextentry);
  g_signal_connect(button, "clicked", G_CALLBACK(open_selector_dialog), fileselectordialog);
  gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);

  button = gtk_button_new_with_label("Quit");
  g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);

  gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 1, 1);
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