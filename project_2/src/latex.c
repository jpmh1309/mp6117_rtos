/***********************************************************
* Project Name: Project 2: Real Time Scheduling            *
* File Name: latex.c                                       *
* University: Costa Rica Institute of Technology           *
* Lecture: MP-6117 Real Time Operating Systems             *
* Students: - David Martinez                               *
*           - Jose Martinez                                *
* Year: 2021                                               *
************************************************************/
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ------------------------------------------------------------------------- //
// GLOBAL VARIABLES AND STRUCTS
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

char color[20][7] = {
    "ECF6CE", "A4A4A4", "F781D8", "8181F7", "80FF00", "F4FA58", "A9D0F5",
    "0080FF", "F5A9F2", "04B45F", "FF0040", "088A85", "FFCC67", "FFC702",
    "6200C9", "303498", "FD6864", "F5A9F2", "FE642E", "DF7401"};

enum algorithm{RM, EDF, LLF};

// ------------------------------------------------------------------------- //
// FUNCTIONS
// ------------------------------------------------------------------------- //
void table_task_create_ppt(FILE* file, configuration config)
{
  int deadline[4] = {2, 3, 4, 6};
  int computation[4] = {1, 1, 1, 1};
  int execution[12] = {1, 2, 3, 4, 0, 0, 1, 2, 3, 4, 0, 0};
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

void schedulability_create_ppt(FILE* file, unsigned int lcm, enum algorithm scheduling, configuration config)
{
  // TODO: Missing schedulability test
}

void execution_create_ppt(FILE* file, unsigned int lcm, enum algorithm scheduling, configuration config)
{
  int deadline[4] = {2, 3, 4, 6};
  int computation[4] = {1, 1, 1, 1};
  int execution[12] = {1, 2, 3, 4, 0, 0, 1, 2, 3, 4, 0, 0};

  // Execution results table
  fputs("\\begin{table}[]\n", file);
  if(config.multiple_slides) {
    fputs("\\resizebox{\\textwidth}{!}{\n", file);
  } else {
    fputs("\\resizebox{0.8\\textwidth}{0.1\\textheight}{\n", file);
  }
  fputs("\\begin{tabular}{|", file);
  for(unsigned int i=0; i<lcm+2; i++) fputs("c|", file);
  fputs("}\n\\hline\n", file);

  // Table Header
  fputs("\\textbf{Task ID} ", file);
  for(unsigned int i=0; i<lcm+1; i++) fprintf(file, "& \\textbf{%d} ", i);
  fputs("\\\\ \\hline\n", file);

  // Table Content
  for(unsigned int i=0; i<config.number_tasks; i++)
  {
    fprintf(file, "\\cellcolor[HTML]{%s} \\textbf{%d}", color[i], i+1);
    for(unsigned int j=0; j<lcm+1; j++)
    {
      if(j!=0 && j%config.period[i]==0)
      {
        fputs(" & X", file);
      } else if ((i+1) == execution[j])
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
}

void single_slide(FILE* file, configuration config)
{
  fputs("\\section{Rate Monotonic, Earliest Deadline Fist, Least Laxity First}\n", file);

  fputs("\\begin{frame}\n", file);
  fputs("\\frametitle{Schedulability Tests}\n", file);
  table_task_create_ppt(file, config);
  fputs("\\begin{itemize}\n", file);
  if (config.rm)
  {
    fputs("\\item RM\n", file);
    schedulability_create_ppt(file, 12, RM, config);
  }
  if (config.edf)
  {
    fputs("\\item EDF\n", file);
    schedulability_create_ppt(file, 12, EDF, config);
  }
  if (config.llf)
  {
    fputs("\\item LLF\n", file);
    schedulability_create_ppt(file, 12, LLF, config);
  }
  fputs("\\end{itemize}\n", file);
  fputs("\\end{frame}\n", file);

  fputs("\\begin{frame}\n", file);
  fputs("\\frametitle{Execution}\n", file);
  fputs("\\begin{itemize}\n", file);
  if (config.rm)
  {
    fputs("\\item RM\n", file);
    execution_create_ppt(file, 12, RM, config);
  }
  if (config.edf)
  {
    fputs("\\item EDF\n", file);
    execution_create_ppt(file, 12, EDF, config);
  }
  if (config.llf)
  {
    fputs("\\item LLF\n", file);
    execution_create_ppt(file, 12, LLF, config);
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
    fputs("\\end{frame}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Execution}\n", file);
    execution_create_ppt(file, 12, RM, config);
    schedulability_create_ppt(file, 12, RM, config);
    fputs("\\end{frame}\n", file);
  }

  // Earliest Deadline Fisrt
  if (config.edf)
  {
    fputs("\\section{Earliest Deadline First}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Schedulability Tests}\n", file);
    table_task_create_ppt(file, config);
    fputs("\\end{frame}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Execution}\n", file);
    execution_create_ppt(file, 12, EDF, config);
    schedulability_create_ppt(file, 12, EDF, config);
    fputs("\\end{frame}\n", file);
  }

  // Least Laxity First
  if (config.llf)
  {
    fputs("\\section{Least Laxity First}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Schedulability Tests}\n", file);
    table_task_create_ppt(file, config);
    fputs("\\end{frame}\n", file);

    fputs("\\begin{frame}\n", file);
    fputs("\\frametitle{Execution}\n", file);
    execution_create_ppt(file, 12, LLF, config);
    schedulability_create_ppt(file, 12, LLF, config);
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
