#include <gtk/gtk.h>

typedef struct {
  int number_process;
  gchar *algorithm;
  gchar *operation_mode;
} point;


static void print_hello(GtkWidget *widget,
                        gpointer   data)
{
  g_print ("Hello World\n");
}

static gboolean fill (gpointer   user_data)
{
  GtkWidget *progress_bar = user_data;

  /*Get the current progress*/
  gdouble fraction;
  fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar));

  /*Increase the bar by 10% each time this function is called*/
  fraction += 0.1;

  /*Fill in the bar with the new fraction*/
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);

  /*Ensures that the fraction stays below 1.0*/
  if (fraction < 1.0)
    return TRUE;

  return FALSE;
}

static void table_content(GtkGrid *grid,
                          int number_process)
{
  GtkWidget *progress_bar;
  GtkWidget *label;
  gdouble fraction = 0.0;

  for(int i=0; i<number_process; i++)
  {
    int y = 2+i;
    // DISPLAY INFORMATION
    gchar *text = g_strdup_printf("PID %i", i);
    label = gtk_button_new_with_label(text);
    gtk_grid_attach (GTK_GRID (grid), label, 0, y, 2, 1);

    /*Create a progressbar and add it to the window*/
    progress_bar = gtk_progress_bar_new ();
    gtk_grid_attach_next_to (GTK_GRID (grid),
                             progress_bar,
                             label,
                             GTK_POS_RIGHT, 4, 1);

    /*Fill in the given fraction of the bar. Has to be between 0.0-1.0 inclusive*/
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);

    // gtk_progress_set_percentage (GTK_PROGRESS (pdata->progressbar),
    //                              pvalue);

    /*Use the created fill function every 500 milliseconds*/
    g_timeout_add (500, fill, GTK_PROGRESS_BAR (progress_bar));

    label = gtk_button_new_with_label("3.12222");
    gtk_grid_attach (GTK_GRID (grid), label, 6, y, 2, 1);

    label = gtk_button_new_with_label("10%");
    gtk_grid_attach (GTK_GRID (grid), label, 8, y, 2, 1);
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

  // button = gtk_button_new_with_label ("Button 2");
  // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //
  // /* Place the second button in the grid cell (1, 0), and make it fill
  //  * just 1 cell horizontally and vertically (ie no spanning)
  //  */
  // gtk_grid_attach (GTK_GRID (grid), button, 3, 0, 1, 1);
  //
  // button = gtk_button_new_with_label ("STOP");
  // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //
  // gtk_grid_attach (GTK_GRID (grid), button, 4, 0, 1, 1);
  //
  // button = gtk_button_new_with_label ("Button 2");
  // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //
  // gtk_grid_attach (GTK_GRID (grid), button, 5, 0, 1, 1);
  //
  // button = gtk_button_new_with_label ("STOP");
  // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //
  // gtk_grid_attach (GTK_GRID (grid), button, 6, 0, 1, 1);
  //
  // button = gtk_button_new_with_label ("Button 2");
  // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //
  // gtk_grid_attach (GTK_GRID (grid), button, 7, 0, 1, 1);
  //
  // button = gtk_button_new_with_label ("Button 2");
  // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //
  // /* Place the second button in the grid cell (1, 0), and make it fill
  //  * just 1 cell horizontally and vertically (ie no spanning)
  //  */
  // gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

 //  // TABLE
 //
 //  /* Create a 2x2 table */
 // table = gtk_table_new (2, 2, TRUE);
 // /* Put the table in the main window */
 // gtk_grid_attach (GTK_GRID (grid), table, 0, 4, 2, 2);

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

  // box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  // gtk_grid_attach (GTK_GRID (grid), label, 0, 2+data->number_process, 10, 1);



  // QUIT Button
  button = gtk_button_new_with_label ("CURRENT PROCESS:");

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.
   */
  gtk_grid_attach (GTK_GRID (grid), button, 0, 2+data->number_process, 5, 1);

  // QUIT Button
  button = gtk_button_new_with_label ("PID 001");

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.
   */
  gtk_grid_attach (GTK_GRID (grid), button, 5, 2+data->number_process, 5, 1);

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

int init_gtk(int argc, char **argv) {
  int status;
  GtkApplication *app;
  point data = {15, "LOTTERY SCHEDULING", "PREEMPTIVE"};

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), &data);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

int main (int argc, char **argv)
{
  return init_gtk(argc, argv);
}
