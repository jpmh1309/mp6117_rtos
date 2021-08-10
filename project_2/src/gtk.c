/***********************************************************
* Project Name: Project 2: Real Time Scheduling            *
* File Name: gtk.c                                         *
* University: Costa Rica Institute of Technology           *
* Lecture: MP-6117 Real Time Operating Systems             *
* Students: - David Martinez                               *
*           - Jose Martinez                                *
* Year: 2021                                               *
************************************************************/
#include <gtk/gtk.h>
#include <assert.h>
#include "latex.c"

// ------------------------------------------------------------------------- //
// GLOBAL VARIABLES AND STRUCTS
// ------------------------------------------------------------------------- //
/*typedef struct {
  unsigned int execution_time[20];
  unsigned int period[20];
  GtkWidget *window;
  GtkWidget *entry;
  unsigned int number_tasks;
  GtkWidget *c_entry[20];
  GtkWidget *p_entry[20];
  GtkWidget *button_rm;
  GtkWidget *button_edf;
  GtkWidget *button_llf;
  GtkWidget *button_ss;
  GtkWidget *button_ms ;
  unsigned char multiple_slides;
  unsigned char rm;
  unsigned char edf;
  unsigned char llf;
} configuration;*/

static void cb_generate_pdf (GtkWidget *widget,
                             gpointer   user_data)
{
  int status = 0;
  const gchar *entry_data;
  configuration* config = (configuration*) user_data;
  if (config->multiple_slides) g_print ("Multiple Slides\n");
  else g_print ("Single Slides\n");
  g_print ("Generating .tex %d\n", config->multiple_slides);

  if (!config->rm && !config->edf && !config->llf) {
    g_print ("Error: There hasn't been a selected algorithm\n");
    status = 1;
  }

  // Parse entry data
  // TODO: Add assertion to only receive int
  for (int i = 0; i<config->number_tasks; i++) {
    entry_data = gtk_entry_get_text(GTK_ENTRY(config->c_entry[i]));
    config->execution_time[i] = (unsigned int)atoi(entry_data);

    if (config->execution_time[i] <= 0) {
      g_print ("Error: Invalid execution time in Task ID %d\n", i+1);
    }

    entry_data = gtk_entry_get_text(GTK_ENTRY(config->p_entry[i]));
    config->period[i] = (unsigned int)atoi(entry_data);

    if (config->period[i] <= 0) {
      g_print ("Error: Invalid period in Task ID %d\n", i+1);
    }

    if (config->execution_time[i] > config->period[i]) {
      g_print ("Error: Task ID %d has execution time greater than the period\n", i+1);
      status = 1;
    }
  }

  if (config->rm) g_print ("RM Enabled\n");
  if (config->edf) g_print ("EDF Enabled\n");
  if (config->llf) g_print ("LLF Enabled\n");

  if (!status) status = generate_pdf(*config);

  if (status) {
    g_print ("Error: Could generate .tex, please check the errors above\n");
  } else {
    g_print ("Sucess: Generating .tex\n");
  }
}

static void cb_clear_entry (GtkWidget *widget,
                         gpointer  user_data)
{
  gtk_entry_set_text(GTK_ENTRY(widget), "");
}

static void cb_toggle_bit (GtkWidget *widget,
                        gpointer   user_data)
{
  unsigned char* bit = (unsigned char *) user_data;
  g_print ("Toggle Bit\n");
  *bit^=0x01;
}

static void table_content(GtkGrid *grid,
                          gpointer user_data)
{
  gchar *text;
  GtkWidget *button;
  configuration* config = (configuration*) user_data;
  g_print ("table_contect: %d\n", config->number_tasks);

  for (int i = 0; i<config->number_tasks; i++){
    text = g_strdup_printf("%d", i+1);
    button = gtk_button_new_with_label (text);
    gtk_grid_attach (GTK_GRID(grid), button, 0, i+1, 10, 1);

    config->c_entry[i] = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(config->c_entry[i]),"Execution Time");
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(config->c_entry[i]), GTK_ENTRY_ICON_SECONDARY, "edit-clear");
    g_signal_connect (config->c_entry[i], "icon-press", G_CALLBACK (cb_clear_entry), NULL);
    gtk_grid_attach (GTK_GRID (grid), config->c_entry[i], 10, i+1, 10, 1);

    config->p_entry[i] = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(config->p_entry[i]),"Period");
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(config->p_entry[i]), GTK_ENTRY_ICON_SECONDARY, "edit-clear");
    g_signal_connect (config->p_entry[i], "icon-press", G_CALLBACK (cb_clear_entry), NULL);
    gtk_grid_attach (GTK_GRID (grid), config->p_entry[i], 20, i+1, 10, 1);
  }
}

static void setup_window (gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *button, *button_rm, *button_edf, *button_llf, *button_ss, *button_ms;

  // cast the user_data to an array of Points.
  configuration* config = (configuration*) user_data;

  //window = gtk_application_window_new (app);
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (window), "Project 2 - RTOS - Setup Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 30, 10);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();
  gtk_grid_set_column_homogeneous(GTK_GRID (grid), TRUE);
  gtk_grid_set_column_spacing(GTK_GRID (grid), 5);
  gtk_grid_set_row_homogeneous(GTK_GRID (grid), TRUE);
  gtk_grid_set_row_spacing(GTK_GRID (grid), 2);

  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (window), grid);

  button = gtk_button_new_with_label ("Task ID");
  gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 10, 1);

  button = gtk_button_new_with_label ("Execution Time");
  gtk_grid_attach (GTK_GRID (grid), button, 10, 0, 10, 1);

  button = gtk_button_new_with_label ("Period");
  gtk_grid_attach (GTK_GRID (grid), button, 20, 0, 10, 1);

  /* ----------------------------------------------------------------------- */

  g_print("table_contect: %d\n", config->number_tasks);
  table_content(GTK_GRID(grid), config);

  /* ----------------------------------------------------------------------- */

  config->button_rm = gtk_check_button_new_with_label ("RM");
  g_signal_connect (config->button_rm, "toggled", G_CALLBACK (cb_toggle_bit), &config->rm);
  gtk_grid_attach (GTK_GRID (grid), config->button_rm , 0, config->number_tasks+1, 10, 1);

  config->button_edf = gtk_check_button_new_with_label ("EDF");
  g_signal_connect (config->button_edf, "toggled", G_CALLBACK (cb_toggle_bit), &config->edf);
  gtk_grid_attach (GTK_GRID (grid), config->button_edf, 10, config->number_tasks+1, 10, 1);

  config->button_llf = gtk_check_button_new_with_label ("LLF");
  g_signal_connect (config->button_llf, "toggled", G_CALLBACK (cb_toggle_bit), &config->llf);
  gtk_grid_attach (GTK_GRID (grid), config->button_llf, 20, config->number_tasks+1, 10, 1);

  /* ----------------------------------------------------------------------- */

  config->button_ms = gtk_radio_button_new_with_label_from_widget (NULL, "Multiple Slides");
  gtk_grid_attach (GTK_GRID (grid), config->button_ms, 0, config->number_tasks+2, 10, 1);

  config->button_ss = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (config->button_ms), "Single Slide");
  gtk_grid_attach (GTK_GRID (grid), config->button_ss, 15, config->number_tasks+2, 15, 1);

  g_signal_connect (config->button_ms, "clicked", G_CALLBACK (cb_toggle_bit), &config->multiple_slides);

  /* ----------------------------------------------------------------------- */

  button = gtk_button_new_with_label ("Generate .pdf");
  g_signal_connect (button, "clicked", G_CALLBACK (cb_generate_pdf), config);
  gtk_grid_attach (GTK_GRID (grid), button, 0, config->number_tasks+3, 15, 1);

  button = gtk_button_new_with_label ("Quit");
  gtk_grid_attach (GTK_GRID (grid), button, 15, config->number_tasks+3, 15, 1);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), config->window);

  /* Now that we are done packing our widgets, we show them all
   * in one go, by calling gtk_widget_show_all() on the window.
   * This call recursively calls gtk_widget_show() on all widgets
   * that are contained in the window, directly or indirectly.
   */
  gtk_widget_show_all (window);
}

static void open_setup_window (GtkWidget *widget,
                               gpointer   user_data)
{
  configuration* config = (configuration*) user_data;
  const gchar *entry_data;
  GtkWidget *entry = user_data;
  entry_data = gtk_entry_get_text(GTK_ENTRY(config->entry));
  // g_print ("%s\n", entry_data);

  // TODO: Add assertion to only receive int
  config->number_tasks = (unsigned int)atoi(entry_data);

  if (config->number_tasks  <= 0) {
    g_print ("Error: Invalid number of tasks\n");
    return;
  }

  //g_print ("%d\n", config->number_tasks);
  setup_window(user_data);
  gtk_widget_hide(config->window);
}

static void init_window (GtkApplication *app,
                         gpointer        user_data)
{
  GtkWidget *button;
  GtkWidget *grid;
  GtkWidget *label;

  configuration* config = (configuration*) user_data;

  config->window = gtk_application_window_new (app);
  gtk_window_set_title(GTK_WINDOW (config->window), "Project 2 - RTOS - Init Window");
  gtk_window_set_default_size (GTK_WINDOW (config->window), 20, 30);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();
  gtk_grid_set_column_homogeneous(GTK_GRID (grid), TRUE);
  gtk_grid_set_column_spacing(GTK_GRID (grid), 5);
  gtk_grid_set_row_homogeneous(GTK_GRID (grid), TRUE);
  gtk_grid_set_row_spacing(GTK_GRID (grid), 1);

  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (config->window), grid);

  label = gtk_label_new("Project 2: Real Time Scheduling Algorithms");
  gtk_grid_attach (GTK_GRID (grid), label, 0, 9, 6, 1);

  label = gtk_label_new("How many tasks are going to be added?");
  gtk_grid_attach (GTK_GRID (grid), label, 0, 10, 6, 1);

  config->entry = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(config->entry),"Number of tasks");
  gtk_entry_set_icon_from_icon_name(GTK_ENTRY(config->entry), GTK_ENTRY_ICON_SECONDARY, "edit-clear");
  g_signal_connect (config->entry, "icon-press", G_CALLBACK (cb_clear_entry), NULL);
  gtk_grid_attach (GTK_GRID (grid), config->entry, 2, 11, 2, 1);

  button = gtk_button_new_with_label ("Continue");
  g_signal_connect (button, "clicked", G_CALLBACK (open_setup_window), user_data);
  gtk_grid_attach (GTK_GRID (grid), button, 1, 12, 1, 1);

  button = gtk_button_new_with_label ("Quit");
  gtk_grid_attach (GTK_GRID (grid), button, 4, 12, 1, 1);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), config->window);

  /* Now that we are done packing our widgets, we show them all
   * in one go, by calling gtk_widget_show_all() on the window.
   * This call recursively calls gtk_widget_show() on all widgets
   * that are contained in the window, directly or indirectly.
   */
  gtk_widget_show_all (config->window);
}

void initial_conditions(configuration *config)
{
  config->number_tasks = 0;
  config->multiple_slides = 0x01 ;
  config->rm = 0x00 ;
  config->edf = 0x00 ;
  config->llf = 0x00 ;
}

int execute_gtk (int argc, char **argv)
{
  GtkApplication *app;
  configuration config;
  int status;

  initial_conditions(&config);

  app = gtk_application_new ("project2.mp6117", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (init_window), &config);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
