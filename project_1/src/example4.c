#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>

sigjmp_buf context[3];
int crt = 0;

void parent()
{
while(1)
fprintf(stderr, "Parent process\n");
}

void child(int *i)
{
while(1)
fprintf(stderr, "Child process %d\n", *i);
}

/* create child context and return to parent */
void create(int i)
{
if (sigsetjmp(context[i], 1)) {
child(&crt);
} else {
char stack[1024];
siglongjmp(context[0], -1);
}
}

/* context switch */
void run_thread()
{
int next = (crt + 1) % 3;

if (!sigsetjmp(context[crt], 1)) {
crt = next;
siglongjmp(context[crt], -1);
}
}

void handler(int sig)
{
signal(SIGALRM, handler);
alarm(1);

run_thread();
}

void main()
{
signal(SIGALRM, handler);
alarm(1);

if (!sigsetjmp(context[0], 1)) {
create(1);
}
if (!sigsetjmp(context[0], 1)) {
create(2);
}
parent();

}
