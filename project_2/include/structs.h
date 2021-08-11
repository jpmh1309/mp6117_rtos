/*****************************************************************************
* Project Name: Project 2: Real Time Scheduling                              *
* File Name: structs.h                                                           *
* University: Costa Rica Institute of Technology                             *
* Lecture: MP-6117 Real Time Operating Systems                               *
* Students: - David Martinez                                                 *
*           - Jose Martinez                                                  *
* Year: 2021                                                                 *
******************************************************************************/
#ifndef STRUCTS_H_
#define STRUCTS_H_

// ------------------------------------------------------------------------- //
// ENUMS
// ------------------------------------------------------------------------- //
enum algorithm{RM, EDF, LLF};

enum task_status{WAITING, RUNNING, COMPLETED};

// ------------------------------------------------------------------------- //
// STRUCTS
// ------------------------------------------------------------------------- //
typedef struct {
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
} configuration;

struct Task{
    unsigned int task_id;
    unsigned int c;
    unsigned int p;
    unsigned int progress;
    enum task_status status;

};

struct ScheduleTimeUnit{
    unsigned int task_id;
    size_t num_task_arrow;
    unsigned int arrow_task_ids[256];
};

struct Deadlines{
    unsigned int task_id;
    unsigned int deadline;
};

// ------------------------------------------------------------------------- //
// ARRAYS
// ------------------------------------------------------------------------- //
char color[20][7] = {
    "ECF6CE", "A4A4A4", "F781D8", "8181F7", "80FF00", "F4FA58", "A9D0F5",
    "0080FF", "F5A9F2", "04B45F", "FF0040", "088A85", "FFCC67", "FFC702",
    "6200C9", "303498", "FD6864", "F5A9F2", "FE642E", "DF7401"};

#endif // STRUCTS_H_
