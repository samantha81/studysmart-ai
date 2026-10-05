/*

Compile:
gcc main.c sorting.c greedyPlanning.c dynamicProgramming.c aiml.c comparison.c -o studysmart
      
Run:
studysmart
 
Responsibilities of main.c:
- Display all menus
- Load / task scenario (hardcoded or manual)
- Pass the Scenario to the correct module function (Algorithm or AIML)
 - Each module prints its own results directly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tasks.h"

//Task type options
static const char *TASK_TYPES[] = {
    "Lecture", "Tutorial", "Assignment", "Practice", "Revision"
};
#define NUM_TYPES 5


// UI Functions
static void flush_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void press_enter(void)
{
    printf("\n  Press ENTER to continue...");
    fflush(stdout);
    flush_stdin();
    getchar();
}

static void print_line(char ch, int width)
{
    for (int i = 0; i < width; i++) putchar(ch);
    putchar('\n');
}

static void print_banner(void)
{
    printf("\n");
    print_line('=', 62);
    printf("    _____ _             _       _____                      _\n");
    printf("   / ____| |           | |     / ____|                    | |\n");
    printf("  | (___ | |_ _   _  __| |_   | (___  _ __ ___   __ _ _ __| |_\n");
    printf("   \\___ \\| __| | | |/ _` | | | \\___ \\| '_ ` _ \\ / _` | '__| __|\n");
    printf("   ____) | |_| |_| | (_| | |_| |____) | | | | | | (_| | |  | |_\n");
    printf("  |_____/ \\__|\\__,_|\\__,_|\\__, |_____/|_| |_| |_|\\__,_|_|   \\__|\n");
    printf("                           __/ |           AI Planning System\n");
    printf("                          |___/            CST207 Group Project\n");
    print_line('=', 62);
    printf("\n");
}


// Display task table for a given scenario, including summary statistics
static void print_task_table(const Scenario *s)
{
    int total = 0, sum_imp = 0, min_dl;

    printf("\n  Scenario : %s\n", s->label);
    printf("  Available: %d hours\n\n", s->available_time);

    printf("  %-4s  %-34s  %-6s  %-6s  %-8s  %-4s  %-10s\n",
           "ID", "Task Name", "Time", "Imp.", "Deadline", "Diff", "Type");
    print_line('-', 82);

    for (int i = 0; i < s->num_tasks; i++) {
        const Task *t = &s->tasks[i];
        printf("  %-4s  %-34s  %-6d  %-6d  %-8d  %-4d  %-10s\n",
               t->id, t->name, t->study_time, t->importance,
               t->deadline, t->difficulty, t->type);
        total   += t->study_time;
        sum_imp += t->importance;
    }

    print_line('-', 82);

    min_dl = s->tasks[0].deadline;
    for (int i = 1; i < s->num_tasks; i++)
        if (s->tasks[i].deadline < min_dl) min_dl = s->tasks[i].deadline;

    printf("\n  Summary\n");
    print_line('-', 40);
    printf("  Total tasks         : %d\n",   s->num_tasks);
    printf("  Total required time : %d h\n", total);
    printf("  Available time      : %d h\n", s->available_time);
    printf("  Time-pressure ratio : %.2f\n", (double)total / s->available_time);
    printf("  Average importance  : %.1f\n", (double)sum_imp / s->num_tasks);
    printf("  Nearest deadline    : %d day(s)\n", min_dl);
    print_line('-', 40);
}

// Pre-built scenarios (15 tasks each)
static void make_task(Task *t,
                      const char *id, const char *name,
                      int st, int imp, int dl, int diff,
                      const char *type)
{
    strncpy(t->id,   id,   sizeof(t->id)   - 1); t->id[sizeof(t->id)-1]    = '\0';
    strncpy(t->name, name, sizeof(t->name) - 1); t->name[sizeof(t->name)-1] = '\0';
    strncpy(t->type, type, sizeof(t->type) - 1); t->type[sizeof(t->type)-1] = '\0';
    t->study_time = st;
    t->importance = imp;
    t->deadline   = dl;
    t->difficulty = diff;
}

// Scenario A: Low-Pressure Scenario
static void load_scenario_A(Scenario *s)
{
    s->num_tasks      = 15;
    s->available_time = 24;
    strncpy(s->label, "Scenario A: Low-Pressure (Avail=24h, Req=22h)", sizeof(s->label)-1);

    make_task(&s->tasks[ 0], "T1",  "Revise Sorting Algorithms",    2, 7,  7, 2, "Revision");
    make_task(&s->tasks[ 1], "T2",  "Read Lecture Notes Ch.3",      1, 5,  5, 1, "Lecture");
    make_task(&s->tasks[ 2], "T3",  "Complete Tutorial Sheet 4",    2, 6,  4, 2, "Tutorial");
    make_task(&s->tasks[ 3], "T4",  "Practise Graph Problems",      2, 8,  6, 3, "Practice");
    make_task(&s->tasks[ 4], "T5",  "Review Big-O Notation",        1, 6,  8, 1, "Revision");
    make_task(&s->tasks[ 5], "T6",  "Attempt Past-Year Questions",  2, 7,  9, 3, "Practice");
    make_task(&s->tasks[ 6], "T7",  "Read Tree Chapter",            1, 5, 10, 2, "Lecture");
    make_task(&s->tasks[ 7], "T8",  "Assignment 1 Draft",           2, 9,  3, 4, "Assignment");
    make_task(&s->tasks[ 8], "T9",  "Study Recursion Examples",     1, 6,  6, 2, "Practice");
    make_task(&s->tasks[ 9], "T10", "Revise Linked Lists",          1, 5,  8, 1, "Revision");
    make_task(&s->tasks[10], "T11", "Watch Algorithm Lecture",      1, 4, 11, 1, "Lecture");
    make_task(&s->tasks[11], "T12", "Do Lab Exercise 3",            2, 7,  5, 2, "Tutorial");
    make_task(&s->tasks[12], "T13", "Review Stack and Queue",       1, 6,  7, 2, "Revision");
    make_task(&s->tasks[13], "T14", "Read Hashing Chapter",         2, 5, 12, 2, "Lecture");
    make_task(&s->tasks[14], "T15", "Practise String Problems",     1, 6,  9, 2, "Practice");
}

// Scenario B: High-Pressure Scenario
static void load_scenario_B(Scenario *s)
{
    s->num_tasks      = 15;
    s->available_time = 12;
    strncpy(s->label, "Scenario B: High-Pressure (Avail=12h, Req=34h)", sizeof(s->label)-1);

    make_task(&s->tasks[ 0], "T1",  "Revise Dynamic Programming",   3,  9, 3, 4, "Revision");
    make_task(&s->tasks[ 1], "T2",  "Complete Graph Tutorial",      2,  8, 4, 3, "Tutorial");
    make_task(&s->tasks[ 2], "T3",  "Practise Greedy Problems",     2,  7, 5, 3, "Practice");
    make_task(&s->tasks[ 3], "T4",  "Read Divide and Conquer",      3,  6, 6, 2, "Lecture");
    make_task(&s->tasks[ 4], "T5",  "Assignment 2 Write-Up",        4, 10, 2, 5, "Assignment");
    make_task(&s->tasks[ 5], "T6",  "Study Hash Table Methods",     2,  6, 7, 3, "Lecture");
    make_task(&s->tasks[ 6], "T7",  "Practice Backtracking",        3,  7, 4, 4, "Practice");
    make_task(&s->tasks[ 7], "T8",  "Review Complexity Classes",    2,  8, 3, 2, "Revision");
    make_task(&s->tasks[ 8], "T9",  "Read Heap Chapter",            2,  5, 8, 3, "Lecture");
    make_task(&s->tasks[ 9], "T10", "Mock Test Simulation",         3,  9, 2, 4, "Practice");
    make_task(&s->tasks[10], "T11", "Study AVL Trees",              2,  7, 5, 4, "Revision");
    make_task(&s->tasks[11], "T12", "Implement Dijkstra",           3,  8, 4, 5, "Assignment");
    make_task(&s->tasks[12], "T13", "Review NP Problems",           2,  6, 6, 3, "Lecture");
    make_task(&s->tasks[13], "T14", "Practise DP on Strings",       2,  9, 3, 4, "Practice");
    make_task(&s->tasks[14], "T15", "Read Floyd-Warshall Notes",    1,  5, 9, 2, "Lecture");
}

// Scenario C: Deadline-Focused
static void load_scenario_C(Scenario *s)
{
    s->num_tasks      = 15;
    s->available_time = 18;
    strncpy(s->label, "Scenario C: Deadline-Focused (Avail=18h, Req=27h)", sizeof(s->label)-1);

    make_task(&s->tasks[ 0], "T1",  "Submit Lab Report",            2,  8, 1, 3, "Assignment");
    make_task(&s->tasks[ 1], "T2",  "Quiz Preparation",             1,  7, 1, 2, "Revision");
    make_task(&s->tasks[ 2], "T3",  "Online Test Chapter 5",        2,  9, 2, 3, "Practice");
    make_task(&s->tasks[ 3], "T4",  "Read Exam Briefing Notes",     1,  6, 2, 1, "Lecture");
    make_task(&s->tasks[ 4], "T5",  "Finish Coding Assignment",     3, 10, 2, 5, "Assignment");
    make_task(&s->tasks[ 5], "T6",  "Tutorial Submission",          2,  7, 3, 2, "Tutorial");
    make_task(&s->tasks[ 6], "T7",  "Revise Exam Topics",           2,  8, 3, 3, "Revision");
    make_task(&s->tasks[ 7], "T8",  "Peer Review Task",             1,  5, 4, 1, "Assignment");
    make_task(&s->tasks[ 8], "T9",  "Practice Problems Set B",      2,  6, 5, 3, "Practice");
    make_task(&s->tasks[ 9], "T10", "Read Week 10 Slides",          2,  5, 6, 2, "Lecture");
    make_task(&s->tasks[10], "T11", "Group Report Section",         2,  8, 3, 3, "Assignment");
    make_task(&s->tasks[11], "T12", "Timed Practice Quiz",          1,  7, 2, 2, "Practice");
    make_task(&s->tasks[12], "T13", "Review Past Exam Q3",          2,  6, 4, 2, "Revision");
    make_task(&s->tasks[13], "T14", "Read Feedback on Assignment",  1,  4, 5, 1, "Lecture");
    make_task(&s->tasks[14], "T15", "Prepare Presentation Slides",  3,  9, 3, 4, "Assignment");
}

// Scenario D: Importance-Focused Scenario
static void load_scenario_D(Scenario *s)
{
    s->num_tasks      = 15;
    s->available_time = 20;
    strncpy(s->label, "Scenario D: Importance-Focused (Avail=20h, Req=29h)", sizeof(s->label)-1);

    make_task(&s->tasks[ 0], "T1",  "Final Exam Revision",          3, 10,  5, 4, "Revision");
    make_task(&s->tasks[ 1], "T2",  "Main Project Coding",          4, 10,  4, 5, "Assignment");
    make_task(&s->tasks[ 2], "T3",  "Core Algorithm Study",         3,  9,  6, 4, "Practice");
    make_task(&s->tasks[ 3], "T4",  "Assignment Core Section",      3,  9,  3, 4, "Assignment");
    make_task(&s->tasks[ 4], "T5",  "Mock Interview Practice",      2,  8,  5, 3, "Practice");
    make_task(&s->tasks[ 5], "T6",  "Tutorial Bonus Question",      1,  3,  8, 2, "Tutorial");
    make_task(&s->tasks[ 6], "T7",  "Group Discussion Notes",       1,  4,  7, 1, "Tutorial");
    make_task(&s->tasks[ 7], "T8",  "Supplementary Reading",        2,  2, 10, 1, "Lecture");
    make_task(&s->tasks[ 8], "T9",  "Extra Practice (Low Prio)",    2,  1, 12, 2, "Practice");
    make_task(&s->tasks[ 9], "T10", "Read Optional Chapter",        2,  2, 14, 1, "Lecture");
    make_task(&s->tasks[10], "T11", "Revise Key Theorems",          2,  9,  4, 3, "Revision");
    make_task(&s->tasks[11], "T12", "Attempt Hard Problems",        2,  8,  6, 5, "Practice");
    make_task(&s->tasks[12], "T13", "Skim Bonus Slides",            1,  2, 10, 1, "Lecture");
    make_task(&s->tasks[13], "T14", "Lab Report Extension",         1,  3,  9, 2, "Assignment");
    make_task(&s->tasks[14], "T15", "Study Graph Colouring",        2,  7,  7, 4, "Practice");
}

// Interactive manual input scenario from user
static int input_tasks(Scenario *s)
{
    printf("\n  Manual Task Entry\n");
    print_line('-', 40);

    printf("  Available study time (hours) : ");
    if (scanf("%d", &s->available_time) != 1 || s->available_time <= 0) {
        printf("  [!] Invalid input.\n");
        flush_stdin();
        return 0;
    }

    printf("  Number of tasks (1-%d)        : ", MAX_TASKS);
    if (scanf("%d", &s->num_tasks) != 1 ||
        s->num_tasks < 1 || s->num_tasks > MAX_TASKS) {
        printf("  [!] Must be 1-%d.\n", MAX_TASKS);
        flush_stdin();
        return 0;
    }

    for (int i = 0; i < s->num_tasks; i++) {
        Task *t = &s->tasks[i];
        printf("\n  Task %d of %d\n", i + 1, s->num_tasks);
        print_line('-', 30);
        snprintf(t->id, sizeof(t->id), "T%d", i + 1);

        flush_stdin();
        printf("  Name           : ");
        fgets(t->name, sizeof(t->name), stdin);
        t->name[strcspn(t->name, "\n")] = '\0';
        if (strlen(t->name) == 0)
            snprintf(t->name, sizeof(t->name), "Task %d", i + 1);

        printf("  Study time (h) : ");
        scanf("%d", &t->study_time);
        if (t->study_time < 1) t->study_time = 1;

        printf("  Importance 1-10: ");
        scanf("%d", &t->importance);
        if (t->importance < 1)  t->importance = 1;
        if (t->importance > 10) t->importance = 10;

        printf("  Deadline (days): ");
        scanf("%d", &t->deadline);
        if (t->deadline < 1) t->deadline = 1;

        printf("  Difficulty 1-5 : ");
        scanf("%d", &t->difficulty);
        if (t->difficulty < 1) t->difficulty = 1;
        if (t->difficulty > 5) t->difficulty = 5;

        printf("  Task type:\n");
        for (int k = 0; k < NUM_TYPES; k++)
            printf("    %d. %s\n", k + 1, TASK_TYPES[k]);
        printf("  Select 1-%d     : ", NUM_TYPES);
        int tc;
        scanf("%d", &tc);
        if (tc < 1 || tc > NUM_TYPES) tc = 1;
        strncpy(t->type, TASK_TYPES[tc - 1], sizeof(t->type) - 1);
        t->type[sizeof(t->type) - 1] = '\0';
    }

    snprintf(s->label, sizeof(s->label),
             "Custom Scenario (Avail=%dh, Tasks=%d)",
             s->available_time, s->num_tasks);
    return 1;
}


// Menu to load a scenario (hardcoded or manual)
static int menu_load_scenario(Scenario *s)
{
    int choice;
    
    printf("\n  STEP 1: Choose Task Source\n");
    print_line('-', 40);
    printf("  1. Use a hardcoded scenario\n");
    printf("  2. Enter tasks manually\n");
    printf("  0. Back\n");
    printf("\n  Choice: ");
    if (scanf("%d", &choice) != 1) { flush_stdin(); return 0; }

    if (choice == 1) {
        printf("\n  Select Scenario\n");
        print_line('-', 40);
        printf("  1. Scenario A - Low-Pressure\n");
        printf("     Available=24h, Required=22h (ratio 0.92)\n\n");
        printf("  2. Scenario B - High-Pressure\n");
        printf("     Available=12h, Required=34h (ratio 2.83)\n\n");
        printf("  3. Scenario C - Deadline-Focused\n");
        printf("     Several tasks due in 1-3 days\n\n");
        printf("  4. Scenario D - Importance-Focused\n");
        printf("     Wide spread of importance scores (1-10)\n\n");
        printf("  0. Back\n");
        printf("\n  Choice: ");
        if (scanf("%d", &choice) != 1) { flush_stdin(); return 0; }

        memset(s, 0, sizeof(Scenario));
        switch (choice) {
            case 1: load_scenario_A(s); return 1;
            case 2: load_scenario_B(s); return 1;
            case 3: load_scenario_C(s); return 1;
            case 4: load_scenario_D(s); return 1;
            case 0: return 0;
            default:
                printf("\n  [!] Invalid choice.\n");
                return 0;
        }
    }

    if (choice == 2) {
        memset(s, 0, sizeof(Scenario));
        return input_tasks(s);
    }

    if (choice == 0) return 0;

    printf("\n  [!] Invalid choice.\n");
    return 0;
}

// Menu to select algorithms / AIML recommendation
static void menu_algorithms(const Scenario *s)
{
    int choice;

    for (;;) {
        print_task_table(s);
        printf("\n  STEP 2: Choose Algorithm\n");
        print_line('-', 40);
        printf("  1. Sorting\n");
        printf("  2. Greedy Planning\n");
        printf("  3. 0/1 Knapsack Dynamic Programming\n");
        printf("  4. AI/ML Recommendation\n");
        printf("  5. Run All ( To compare all four)\n");
        printf("  0. Back\n");
        printf("\n  Choice: ");

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }

        switch (choice) {
            case 1:
                run_sorting(s);
                press_enter();
                break;
            case 2:
                run_greedy(s);
                press_enter();
                break;
            case 3:
                run_dp(s);
                press_enter();
                break;
            case 4:
                run_aiml(s);
                press_enter();
                break;
            case 5:
                run_comparison(s);
                press_enter();
                break;
            case 0:
                return;
            default:
                printf("\n  [!] Invalid choice. Enter 0-5.\n");
        }
    }
}

// Main menu
int main(void)
{
    Scenario s;
    int choice;

    print_banner();

    for (;;) {
        printf("  MAIN MENU\n");
        print_line('-', 40);
        printf("  1. Start - Load scenario and run algorithms\n");
        printf("  0. Exit\n");
        printf("\n  Choice: ");

        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }

        if (choice == 0) {
            printf("\n  Goodbye!\n\n");
            break;
        }

        if (choice == 1) {
            if (menu_load_scenario(&s))
                menu_algorithms(&s);
        } else {
            printf("\n  [!] Invalid choice.\n");
        }
    }

    return 0;
}
