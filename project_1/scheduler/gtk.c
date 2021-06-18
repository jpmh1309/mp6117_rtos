#include <gtk/gtk.h>
#include <setjmp.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define STACK_SIZE 4096

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7


struct scheduler_config{
    char* algorithm;
    char* operation_mode;
    int num_threads;
    int quantum;
    int workload;
    int bills;
    int arrival_time;
    int ticket_am;
};

enum thread_state{
  MAIN,
  READY,
  WAITING,
  RUNNING,
  FINISHED
};

struct thread{
    int pid;
    sigjmp_buf env;
    long double pi;
    int current_workload;
    //Lottery schedule
    int bill;
    int i;
    enum thread_state state;
    char stack[STACK_SIZE];
    void *(function);
    int tickets[255];
    int ticket_am;
};



void sig_alrm(int);
void run_process_fcfs();
void config_get(struct scheduler_config *config);
void update_current();

struct scheduler_config config;
static int curr_process;
static int first_run;
struct thread * processes;
sigjmp_buf scheduler_buf;

int ticket_am;
void create_process(void * function, int p){


        address_t sp, pc;
        processes[p].current_workload = 0;
        processes[p].state = MAIN;
}
void update_tickets(int process, int lottery_ticket){
    printf("Ticket_copy_length: %d\n", process);
    int tickets_copy[processes[process].ticket_am];
    printf("CHECK A\n");
    int i =0;
    printf("CHECK B\n");
    memcpy(tickets_copy, processes[process].tickets, processes[process].ticket_am * sizeof(int));
    printf("CHECK C\n");
    for(int j = 0; j< processes[process].ticket_am; j++){
        if(tickets_copy[j] != lottery_ticket){
            processes[process].tickets[i] = tickets_copy[j];
            printf("Saved tickets: %d\n", processes[process].tickets[i]);
            i++;
        }
        else{
            printf("Deleted ticket: %d\n", processes[process].tickets[i]);
        }
    }
    printf("CHECK D\n");
    processes[process].ticket_am = i;
    //Need to implement the creation of new tickets when we are out of tickets
    if(processes[process].ticket_am == 0){
        //Create new 5 tickets.
        printf("update_tickets()::ticket_am: %d\n", ticket_am);
        for(int i = ticket_am; i< ticket_am + config.ticket_am; i++){
            processes[process].tickets[i- ticket_am ] = i;
            printf("update_tickets()::processes[%d].tickets[%d]: %d\n", process, i - ticket_am, processes[process].tickets[i- ticket_am]);
        }
        ticket_am += config.ticket_am;
        processes[process].ticket_am = config.ticket_am;
    }

    printf("CHECK E\n");
}
void create_all_tickets(){
    int ticket_amm = config.num_threads*config.ticket_am;
    int tickets[255];
    //Create tickets by acending numbers to make them unique.
    for(int i = 0; i< ticket_amm; i++) tickets[i] = i;
    //Distribute the tickets to each process.
    for(int i = 0; i< config.num_threads; i++){
        printf("Generating tickets from: %d to %d.\n", tickets[config.ticket_am*i], tickets[config.ticket_am*i + config.ticket_am -1]);
        memcpy(processes[i].tickets,&tickets[config.ticket_am*i], sizeof(int)*config.ticket_am);
        processes[i].ticket_am = config.ticket_am;
    }
    ticket_am = ticket_amm;
}

int get_tickets(int * all_tickets){
    int ticket_quantity = 0;
    for(int i =0; i< config.num_threads; i++){
        for(int j =0; j< processes[i].ticket_am; j++){
            printf("Thread: %d has ticket: %d\n", i, processes[i].tickets[j]);
            all_tickets[ticket_quantity] = processes[i].tickets[j];
            ticket_quantity++;
        }
    }
    for(int i =0; i < ticket_quantity; i++){
        printf("get_tickets::all_tickets[%d]: %d\n",i,all_tickets[i]);
    }
    printf("ticket_quantity: %d\n", ticket_quantity);
    return ticket_quantity;
}
int get_ticket_owner(int ticket){
    int ticket_owner;
    for(int i =0; i<config.num_threads; i++){
        for(int j = 0; j< processes[i].ticket_am; j++){
            if(processes[i].tickets[j] == ticket){
                ticket_owner = i;
                break;
            }
        }
    }
    return ticket_owner;
}
void* main_thread(){
    printf("Setting up the system\n");
    config_get(&config);
    processes = malloc(sizeof(processes)*config.num_threads);
    int i;
    printf("Selected algorithm: %s\n",config.algorithm);
    for(i = 0; i < config.num_threads; i++ ){
        create_process(run_process_fcfs,i);
    }
    if(!strcmp(config.algorithm,"FCFS")){
        first_run = 1;
        //printf("Setting up the alarm\n");
        signal(SIGALRM, sig_alrm);

        curr_process = 0;
        update_current();
        //Save the state of the main.
        sigsetjmp(scheduler_buf, 1);


        curr_process = (curr_process == config.num_threads - 1) ? 0 : curr_process + 1;
        update_current();
        if(processes[curr_process].current_workload < config.workload){
            //printf("Running process: %d\n", curr_process);
            if(!strcmp(config.operation_mode,"PREEMPTIVE")) {
                //printf("Preemtive mode.\n");
                ualarm(config.quantum,0);
            }
            //siglongjmp(processes[curr_process].env,1);
            run_process_fcfs();
        }
        else{
            ualarm(config.quantum,0);
            siglongjmp(scheduler_buf,1);
        }
    }
    else if(!strcmp(config.algorithm,"LS")){
        printf("LS mode\n");
        int lottery_tickets[255];
        int ticket_amount;
        int random_ticket_entry;
        int winner_process;
        srand(time(NULL));

        create_all_tickets();

        signal(SIGALRM, sig_alrm);

        sigsetjmp(scheduler_buf, 1);
        printf("Check 1\n");
        ticket_amount = get_tickets(lottery_tickets);
        printf("Check 2\n");
        //Select a random ticket as a winner!
        if(ticket_amount != 0 ){
            random_ticket_entry = rand() % ticket_amount;
            //Get the ticket owner.
            printf("Check 3\n");
            winner_process = get_ticket_owner(lottery_tickets[random_ticket_entry]);
            printf("Process: %d. Lottery_ticket: %d. Random Ticket Entry: %d. Ticket amount: %d\n", winner_process, lottery_tickets[random_ticket_entry], random_ticket_entry, ticket_amount);
            //Remove the ticket from the ticket queue.
            update_tickets(winner_process,lottery_tickets[random_ticket_entry]);
            printf("Check 6\n");
            curr_process = winner_process;
            update_current();

            ualarm(config.quantum,0);
            printf("Process Selected: %d\n", curr_process);
            printf("Check\n");
            run_process_fcfs();
        }
        //Initialice with Tickets.



    }
    while(1);
}

void run_process_ls(){


}
void run_process_fcfs(){

    processes[curr_process].state = RUNNING;
    while(1){
        processes[curr_process].pi += 2*( 2*pow(-1, processes[curr_process].current_workload ) / (1+2*processes[curr_process].current_workload));
        //printf("current_workload: %d\n",processes[curr_process].current_workload );
        if(processes[curr_process].current_workload >= config.workload){
            printf("Process finished!!!\n");
            printf("Pi calculation: %Lf\n", processes[curr_process].pi);
            processes[curr_process].state = FINISHED;
            processes[curr_process].ticket_am =0;
            siglongjmp(scheduler_buf,1);
        }
        processes[curr_process].state = RUNNING;
        if(!strcmp(config.operation_mode, "NON-PREEMPTIVE")){
            //printf("Setup is non-preemptive\n");
            if(processes[curr_process].current_workload % config.quantum == 0){
                //printf("Saving status for process\n");
                    processes[curr_process].current_workload ++;
                    processes[curr_process].state = WAITING;
                    siglongjmp(scheduler_buf,1);
            }

        }
        processes[curr_process].current_workload ++;
    }
}

void sig_alrm(int signo){
    processes[curr_process].state = WAITING;
    //printf("Timer Expired\n");
    update_current();
    siglongjmp(scheduler_buf,1);
}


void config_get(struct scheduler_config *config)
{
  char * algorithm = malloc(sizeof(char)*255);
  char * operation_mode = malloc(sizeof(char)*255);
  int process_num;
  int arrival_time;
  int workload;
  int bills;
  int quantum;
  int ticket_am;

  FILE *fp = fopen("config.txt", "r");

  int i = 0;
  fscanf (fp, "%s", algorithm);
  fscanf (fp, "%s", operation_mode);
  fscanf (fp, "%d", &process_num);
  fscanf (fp, "%d", &arrival_time);
  fscanf (fp, "%d", &workload);
  fscanf (fp, "%d", &bills);
  fscanf (fp, "%d", &quantum);
  fscanf (fp, "%d", &ticket_am);
  fclose (fp);


  config->algorithm = algorithm;
  config->operation_mode = operation_mode;
  config->num_threads= process_num;
  config->arrival_time = arrival_time;
  config->workload = workload;
  config->bills = bills;
  config->ticket_am = ticket_am;

  // Main Thread + Number of Threads
  config->quantum = quantum;

  printf("config->algorithm: %s\n", config->algorithm);
  printf("config->operation_mode: %s\n", config->operation_mode);
  printf("config->num_threads: %d\n", config->num_threads);
  printf("config->arrival_time: %d\n", config->arrival_time);
  printf("config->workload: %d\n", config->workload);
  printf("config->bills: %d\n", config->bills);
  printf("config->quantum: %d\n", config->quantum);
}

int current_thread;

GtkWidget *button_current_thread;

typedef struct {
GtkWidget *progress_bar;
GtkWidget *pi;
GtkWidget *error;
} process_info;

typedef struct {
int number_process;
gchar algorithm[1000];
gchar operation_mode[1000];
} point;


// process_info process_info_table[15];
process_info *process_info_table;

static void print_hello(GtkWidget *widget,
                      gpointer   data)
{
g_print ("Hello World\n");
}

void update_current()
{
gchar *text = g_strdup_printf("PID %d", curr_process);
printf("CURRENT PROCESS %d\n", curr_process);
gtk_button_set_label(GTK_BUTTON (button_current_thread), text);
// g_timeout_add (5, update_current, GTK_BUTTON (button_current_thread));
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
g_signal_connect (button, "clicked", G_CALLBACK (main_thread), NULL);

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
// if(!strcmp(config.algorithm,"FCFS")) strcpy(data->algorithm, "FCFS");
// else strcpy(data->algorithm, "LOTERRY SCHEDULING");
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
// if(!strcmp(config.operation_mode,"PREEMPTIVE")) strcpy(data->operation_mode, "PREEMPTIVE");
// else strcpy(data->operation_mode, "NON-PREEMPTIVE");
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

void init_gtk() {
int status;
GtkApplication *app;
point data = {15, "LOTTERY SCHEDULING", "PREEMPTIVE"};
process_info_table = malloc(data.number_process * sizeof(process_info));

current_thread = 0;
app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
g_signal_connect (app, "activate", G_CALLBACK (activate), &data);
status = g_application_run (G_APPLICATION (app), 0, NULL);

g_thread_new("", &main_thread, NULL);

g_object_unref (app);

// return status;
}

int main (int argc, char **argv)
{

init_gtk();

}
