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
//#include "latex.c"
#include "gtk.c"

int main (int argc, char **argv)
{
  int status;

  status = execute_gtk(argc, argv);

  return status;
}
