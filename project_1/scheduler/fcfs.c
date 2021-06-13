#include <setjmp.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void sig_alrm(int);
static sigjmp_buf jmp_buf_stack[255];
static int num_process;
static int curr_process;
static int first_run;


int main(void){
    //add +1 to the specified processes to get +1
    printf("Setting up the system\n");
    num_process = 5 +1 ;
    curr_process = 0;
    first_run = 1;
    printf("Setting up the alarm\n");
    signal(SIGALRM, sig_alrm);
    printf("Activating the alarm now\n");
    alarm(1);
    while(1);
}

void run_process(){
    int i = 0;
    
    for(i = 0; i< 9999999999999999; i++){
        sleep(1);  // Delay for 1 second
        printf("Process taking: %ds\n",i);
    }
}

static void sig_alrm(int signo){
    printf("Timer Expired\n");
    if(!sigsetjmp(jmp_buf_stack[curr_process],1)){
        if(first_run){
            curr_process++;
            if(curr_process == num_process){
                first_run = 0;
            }
            printf("First Run. Running process:%d\n", curr_process);
            alarm(1);
            run_process();
        }
        else{
            //Round robin current process
            //Process #0 might be avoided cause it is the main loop. 
            curr_process = (curr_process == num_process) ? 0 : curr_process + 1;
            printf("Running process:%d\n", curr_process);
            alarm(1);
            siglongjmp(jmp_buf_stack[curr_process],1);

        }
    }
}

