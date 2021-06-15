#include <gtk/gtk.h>


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



static void activate (GtkApplication *app,
                      gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *progress_bar;

  gdouble fraction = 0.0;

  /*Create a window with a title, and a default size*/
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "ProgressBar Example");
  gtk_window_set_default_size (GTK_WINDOW (window), 220, 20);

  /*Create a progressbar and add it to the window*/
  progress_bar = gtk_progress_bar_new ();
  gtk_container_add (GTK_CONTAINER (window), progress_bar);

  /*Fill in the given fraction of the bar. Has to be between 0.0-1.0 inclusive*/
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);

  /*Use the created fill function every 500 milliseconds*/
  g_timeout_add (500, fill, GTK_PROGRESS_BAR (progress_bar));

  gtk_widget_show_all (window);
}



int
main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
