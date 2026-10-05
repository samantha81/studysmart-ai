#ifndef TASKS_H
#define TASKS_H

// tasks.h -- Shared data structures and function declarations

#define MAX_TASKS    15
#define MAX_NAME_LEN 60
#define MAX_TYPE_LEN 20

typedef struct {
    char id[8];
    char name[MAX_NAME_LEN];
    int  study_time;   // unit: hours        
    int  importance;   // range: 1-10
    int  deadline;     // unit: days 
    int  difficulty;   // range: 1-5
    char type[MAX_TYPE_LEN];
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    int  num_tasks;
    int  available_time;
    char label[64];
} Scenario;

// sorting.c
void run_sorting(const Scenario *s);

// greedyPlanning.c
void run_greedy(const Scenario *s);

// dynamicProgramming.c
void run_dp(const Scenario *s);

// aiml.c
void run_aiml(const Scenario *s);

// comparison.c
void run_comparison(const Scenario *s);

#endif /* TASKS_H */
