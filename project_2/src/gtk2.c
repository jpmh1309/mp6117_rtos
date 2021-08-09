#include <gtk/gtk.h>
#include <math.h>

int current_thread;

GtkWidget *button_current_thread;

typedef struct {
GtkWidget *progress_bar;
GtkWidget *pi;
GtkWidget *error;
} process_info;

typedef struct {
int number_process;
gchar *algorithm;
gchar *operation_mode;
} point;


// process_info process_info_table[15];
process_info *process_info_table;

static void print_hello(GtkWidget *widget,
                      gpointer   data)
{
g_print ("Hello World\n");
}

static void update_current(int current_thread)
{
gchar *text = g_strdup_printf("PID %d", current_thread);
gtk_button_set_label(GTK_BUTTON (button_current_thread), text);
}

static void update_process(int current_thread, int workload, long double pi)
{

gdouble fraction = (gdouble) workload/15;
gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (process_info_table[current_thread].progress_bar), fraction);
/*Fill in the given fraction of the bar. Has to be between 0.0-1.0 inclusive*/

gchar *text = g_strdup_printf("%.5LF", pi);
gtk_button_set_label(GTK_BUTTON (process_info_table[current_thread].pi), text);

text = g_strdup_printf("%.2LF %%", 100*(1-pi/M_PI));
gtk_button_set_label(GTK_BUTTON (process_info_table[current_thread].error), text);

}

static void table_content(GtkGrid *grid,
                        int number_process)
{
// GtkWidget *progress_bar;
GtkWidget *label;
gdouble fraction = 0.2;

for(int i=0; i<number_process; i++)
{
  int y = 2+i;
  // DISPLAY INFORMATION
  gchar *text = g_strdup_printf("PID %i", i);
  label = gtk_button_new_with_label(text);
  gtk_grid_attach (GTK_GRID (grid), label, 0, y, 2, 1);

  /*Create a progressbar and add it to the window*/
  process_info_table[i].progress_bar = gtk_progress_bar_new ();
  gtk_grid_attach_next_to (GTK_GRID (grid),
                           process_info_table[i].progress_bar,
                           label,
                           GTK_POS_RIGHT, 4, 1);

  /*Fill in the given fraction of the bar. Has to be between 0.0-1.0 inclusive*/
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (process_info_table[i].progress_bar), fraction);

  process_info_table[i].pi = gtk_button_new_with_label("3.12222");
  gtk_grid_attach (GTK_GRID (grid), process_info_table[i].pi, 6, y, 2, 1);

  process_info_table[i].error = gtk_button_new_with_label("10%");
  gtk_grid_attach (GTK_GRID (grid), process_info_table[i].error, 8, y, 2, 1);
}
}

static void activate(GtkApplication *app,
                   gpointer        user_data)
{
GtkWidget *window;
GtkWidget *grid;
GtkWidget *label;
GtkWidget *button;
GdkColor color;
// GtkWidget *box;

// cast the user_data to an array of Points.
point* data = (point*) user_data;

/* create a new window, and set its title */
window = gtk_application_window_new (app);
gtk_window_set_title (GTK_WINDOW (window), "Project 1 - RTOS");
gtk_container_set_border_width (GTK_CONTAINER (window), 20);

/* Here we construct the container that is going pack our buttons */
grid = gtk_grid_new ();

/* Pack the container in the window */
gtk_container_add (GTK_CONTAINER (window), grid);

button = gtk_button_new_with_label ("START");
g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

/* Place the first button in the grid cell (0, 0), and make it fill
 * just 1 cell horizontally and vertically (ie no spanning)
 */
gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 5, 1);

button = gtk_button_new_with_label ("STOP");
g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

/* Place the second button in the grid cell (1, 0), and make it fill
 * just 1 cell horizontally and vertically (ie no spanning)
 */
gtk_grid_attach (GTK_GRID (grid), button, 5, 0, 5, 1);

// TABLE HEADER
label = gtk_button_new_with_label("PROCESS ID");
gtk_grid_attach (GTK_GRID (grid), label, 0, 1, 2, 1);

label = gtk_button_new_with_label("PROGRESS");
gtk_grid_attach (GTK_GRID (grid), label, 2, 1, 4, 1);

label = gtk_button_new_with_label("VALUE");
gtk_grid_attach (GTK_GRID (grid), label, 6, 1, 2, 1);

label = gtk_button_new_with_label("ERROR(%)");
gtk_grid_attach (GTK_GRID (grid), label, 8, 1, 2, 1);

// TABLE CONTENT
table_content(GTK_GRID (grid), data->number_process);

// QUIT Button
button = gtk_button_new_with_label ("CURRENT PROCESS:");

/* Place the Quit button in the grid cell (0, 1), and make it
 * span 2 columns.
 */
gtk_grid_attach (GTK_GRID (grid), button, 0, 2+data->number_process, 5, 1);

// QUIT Button
gchar *text = g_strdup_printf("PID %d", current_thread);
button_current_thread = gtk_button_new_with_label(text);

/* Place the Quit button in the grid cell (0, 1), and make it
 * span 2 columns.
 */
gtk_grid_attach (GTK_GRID (grid), button_current_thread, 5, 2+data->number_process, 5, 1);

button = gtk_button_new_with_label ("ALGORITHM:");

/* Place the Quit button in the grid cell (0, 1), and make it
 * span 2 columns.
 */
gtk_grid_attach (GTK_GRID (grid), button, 0, 3+data->number_process, 5, 1);

// QUIT Button
button = gtk_button_new_with_label (data->algorithm);

/* Place the Quit button in the grid cell (0, 1), and make it
 * span 2 columns.
 */
gtk_grid_attach (GTK_GRID (grid), button, 5, 3+data->number_process, 5, 1);

button = gtk_button_new_with_label ("OPERATION MODE:");

/* Place the Quit button in the grid cell (0, 1), and make it
 * span 2 columns.
 */
gtk_grid_attach (GTK_GRID (grid), button, 0, 4+data->number_process, 5, 1);

// QUIT Button
button = gtk_button_new_with_label (data->operation_mode);

/* Place the Quit button in the grid cell (0, 1), and make it
 * span 2 columns.
 */
gtk_grid_attach (GTK_GRID (grid), button, 5, 4+data->number_process, 5, 1);

update_process(5, 12, 3.10);

// QUIT Button
button = gtk_button_new_with_label ("QUIT");
g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);

/* Place the Quit button in the grid cell (0, 1), and make it
 * span 2 columns.
 */
gtk_grid_attach (GTK_GRID (grid), button, 4, 5+data->number_process, 2, 1);

/* Now that we are done packing our widgets, we show them all
 * in one go, by calling gtk_widget_show_all() on the window.
 * This call recursively calls gtk_widget_show() on all widgets
 * that are contained in the window, directly or indirectly.
 */
gtk_widget_show_all (window);

}

int main (int argc, char **argv)
{
  int status;
  GtkApplication *app;

  current_thread = 0;
  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), &data);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
