/*****************************************************************************
* Project Name: Project 2: Real Time Scheduling                              *
* File Name: latex.c                                                         *
* University: Costa Rica Institute of Technology                             *
* Lecture: MP-6117 Real Time Operating Systems                               *
* Students: - David Martinez                                                 *
*           - Jose Martinez                                                  *
* Year: 2021                                                                 *
******************************************************************************/
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/structs.h"
#include "rm.c"
#include "edf.c"
#include "llf.c"

// ------------------------------------------------------------------------- //
// FUNCTIONS
// ------------------------------------------------------------------------- //

// Utility function to find GCD of 'a' and 'b'
unsigned int gcd(unsigned int a, unsigned int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Returns LCM of array elements
unsigned int calculate_lcm(unsigned int arr[], unsigned int n)
{
  // Initialize result
  unsigned int ans = arr[0];

  // ans contains LCM of arr[0], ..arr[i]
  // after i'th iteration,
  for (int i = 1; i < n; i++)
      ans = (((arr[i] * ans)) /
              (gcd(arr[i], ans)));

  return ans;
}

void table_task_create_ppt(FILE* file, configuration config)
{
  // Task description table
  fputs("\\begin{table}[]\n", file);
  fputs("\\begin{tabular}{|c|c|c|}\n", file);
  fputs("\\hline\n", file);
  fputs("\\textbf{Task ID} & \\textbf{Execution Time} & \\textbf{Period}", file);
  fputs("\\\\ \\hline\n", file);
  for(unsigned int i=0; i<config.number_tasks; i++)
  {
    fprintf(file, "\\cellcolor[HTML]{%s}%d &", color[i], i+1);
    fprintf(file, "\\cellcolor[HTML]{%s}%d &", color[i], config.execution_time[i]);
    fprintf(file, "\\cellcolor[HTML]{%s}%d ", color[i], config.period[i]);
    fputs("\\\\ \\hline\n", file);
  }
  fputs("\\end{tabular}\n", file);
  fputs("\\end{table}\n", file);
}

void schedulability_create_ppt(FILE* file, enum algorithm scheduling, configuration config)
{
  float U;
  if (scheduling == RM)
  {
    U = rm_schedulabity(config);
    fputs("\\begin{equation}\n", file);
    fputs("\\prod_{i=0}^{n} \\left( \\frac{E_i}{P_i}+1 \\right) \\leq 2 \\rightarrow", file);
    fprintf(file, "%f \\leq 2\n", U);
    fputs("\\end{equation}\n", file);
    fputs("\\begin{itemize}\n", file);
    if (U>2) fputs("\\item Failed", file);
    else fputs("\\item Passed", file);
    fputs("\\end{itemize}\n", file);
  }

  if (scheduling == EDF)
  {
    U = edf_schedulabity(config);
    fputs("\\begin{equation}\n", file);
    fputs("\\sum_{i=0}^{n} \\left( \\frac{E_i}{P_i} \\right) \\leq 1 \\rightarrow", file);
    fprintf(file, "%f \\leq 1\n", U);
    fputs("\\end{equation}\n", file);
    fputs("\\begin{itemize}\n", file);
    if (U>1) fputs("\\item Failed", file);
    else fputs("\\item Passed", file);
    fputs("\\end{itemize}\n", file);
  }

  if (scheduling == LLF)
  {
    fputs("TODO: Missing equations\n", file);
  }
}

void execution_create_ppt(FILE* file, enum algorithm scheduling, configuration config)
{
  unsigned int schedule_len;
  struct Task tasks[config.number_tasks];
  struct ScheduleTimeUnit timeline[255];
  unsigned int lcm = calculate_lcm(config.period, config.number_tasks);

  for (unsigned int i= 0; i<config.number_tasks; i++)
  {
    tasks[i].task_id = i+1;
    tasks[i].c = config.execution_time[i];
    tasks[i].p = config.period[i];
  }

  if (scheduling == RM) schedule_len = createRateMonotonicSchedule(tasks, config.number_tasks, timeline);
  if (scheduling == EDF) schedule_len = createEarliestDeadlineFirst(tasks, config.number_tasks, timeline);
  if (scheduling == LLF) schedule_len = createLeastLaxitudeFirst(tasks, config.number_tasks, timeline);

  // Execution results table
  fputs("\\begin{table}[]\n", file);
  if(config.multiple_slides) {
    fputs("\\resizebox{\\textwidth}{!}{\n", file);
  } else {
    fputs("\\resizebox{0.8\\textwidth}{0.1\\textheight}{\n", file);
  }
  fputs("\\begin{tabular}{|", file);
  for(unsigned int i=0; i<schedule_len+2; i++) fputs("c|", file);
  fputs("}\n\\hline\n", file);

  // Table Header
  fputs("\\textbf{Task ID} ", file);
  for(unsigned int i=0; i<schedule_len+1; i++) fprintf(file, "& \\textbf{%d} ", i);
  fputs("\\\\ \\hline\n", file);

  // Table Content
  for(unsigned int i=0; i<config.number_tasks; i++)
  {
    fprintf(file, "\\cellcolor[HTML]{%s} \\textbf{%d}", color[i], i+1);
    for(unsigned int j=0; j<schedule_len+1; j++)
    {
      if(j!=0 && j%config.period[i]==0 && (i+1) == timeline[j].task_id)
      {
        fprintf(file, " & \\cellcolor[HTML]{%s} $\\blacktriangleleft$", color[i]);
      }
      else if(j!=0 && j%config.period[i]==0)
      {
        fputs(" & $\\blacktriangleleft$", file);
      } else if ((i+1) == timeline[j].task_id)
      {
        fprintf(file, " & \\cellcolor[HTML]{%s}", color[i]);
      } else
      {
        fputs(" & ", file);
      }
    }
    fputs("\\\\ \\hline\n", file);
  }

  fputs("\\end{tabular}\n", file);
  fputs("}\n", file);
  fputs("\\end{table}\n", file);

  if (schedule_len-1 != lcm)
  {
    fputs("\n \\begin{itemize} \n", file);
    fprintf(file, "\n \\item Error: Deadline missed at %d \n", schedule_len);
    fputs("\n \\end{itemize} \n", file);
  }
}

void single_slide(FILE* file, configuration config)
{
  fputs("\\section{Rate Monotonic, Earliest Deadline Fist, Least Laxity First}\n", file);

  fputs("\\begin{frame}\n", file);
  fputs("\\frametitle{Schedulability Tests}\n", file);
  table_task_create_ppt(file, config);
  fputs("\\end{frame}\n", file);
  fputs("\\begin{frame}\n", file);
  fputs("\\begin{itemize}\n", file);
  if (config.rm)
  {
    fputs("\\item RM\n", file);
    schedulability_create_ppt(file, RM, config);
  }
  if (config.edf)
  {
    fputs("\\item EDF\n", file);
    schedulability_create_ppt(file, EDF, config);
  }
  if (config.llf)
  {
    fputs("\\item LLF\n", file);
    schedulability_create_ppt(file, LLF, config);
  }
  fputs("\\end{itemize}\n", file);
  fputs("\\end{frame}\n", file);

  fputs("\\begin{frame}\n", file);
  fputs("\\frametitle{Execution}\n", file);
  fputs("\\begin{itemize}\n", file);
  if (config.rm)
  {
    fputs("\\item RM\n", file);
    execution_create_ppt(file, RM, config);
  }
  if (config.edf)
  {
    fputs("\\item EDF\n", file);
    execution_create_ppt(file, EDF, config);
  }
  if (config.llf)
  {
    fputs("\\item LLF\n", file);
    execution_create_ppt(file, LLF, config);
  }
  fputs("\\end{itemize}\n", file);
  fputs("\\end{frame}\n", file);
}

void multiple_slides(FILE* file, configuration config)
{
  // Rate Monotonic
  if (config.rm)
  {
    fputs("\\section{Rate Monotonic}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Schedulability Tests}\n", file);
    table_task_create_ppt(file, config);
    schedulability_create_ppt(file, RM, config);
    fputs("\\end{frame}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Execution}\n", file);
    execution_create_ppt(file, RM, config);
    fputs("\\end{frame}\n", file);
  }

  // Earliest Deadline Fisrt
  if (config.edf)
  {
    fputs("\\section{Earliest Deadline First}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Schedulability Tests}\n", file);
    table_task_create_ppt(file, config);
    schedulability_create_ppt(file, EDF, config);
    fputs("\\end{frame}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Execution}\n", file);
    execution_create_ppt(file, EDF, config);
    fputs("\\end{frame}\n", file);
  }

  // Least Laxity First
  if (config.llf)
  {
    fputs("\\section{Least Laxity First}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Schedulability Tests}\n", file);
    table_task_create_ppt(file, config);
    schedulability_create_ppt(file, LLF, config);
    fputs("\\end{frame}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Execution}\n", file);
    execution_create_ppt(file, LLF, config);
    fputs("\\end{frame}\n", file);
  }
}

int generate_pdf(configuration config)
{
   int num;
   FILE *fptr_out;
   FILE *fptr_in;
   char *file_in = "./beamer/base.tex";
   char *file_out = "./beamer/ppt.tex";
   char c;

   // Open input file
   fptr_in = fopen(file_in,"r");
   if (fptr_in == NULL)
   {
       printf("Error: Cannot open file %s \n", file_in);
       return 1;
   }

   // Open output file
   fptr_out = fopen(file_out,"w");
   if (fptr_in == NULL)
   {
       printf("Error: Cannot open file %s \n", file_in);
       return 1;
   }

   // Read contents from template file
   c = fgetc(fptr_in);
   while (c != EOF)
   {
       fputc(c, fptr_out);
       c = fgetc(fptr_in);
   }

   // Generate slides for schedulability tests and execution results
   if (config.multiple_slides) multiple_slides(fptr_out, config);
   else single_slide(fptr_out, config);

   // Close the output file
   fputs("\n", fptr_out);
   fputs("\\end{document}", fptr_out);

   // Close file pointer
   fclose(fptr_in);
   fclose(fptr_out);

   // System calls to generate .pdf and open it
   system("pdflatex ./beamer/ppt.tex -output-directory=./beamer/");
   system("pdflatex ./beamer/ppt.tex -output-directory=./beamer/");
   system("evince ppt.pdf &");

   return 0;
}
