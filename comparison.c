#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tasks.h"

// user choose one of the four scenarios in main menu
// if they choose to run all (compare all strategy), this file will be runned
// each row shows: tasks selected, total study time used, total importance gained and execution time
// AI/ML only predicts label so the first three columns show "-"
 
// from sorting.c
// avoids duplication
extern int  compare_tasks(Task t1, Task t2, int choice);
extern void mergeSort(Task arr[], int left, int right, int choice);

// from greedyPlanning.c
// avoids duplication
extern void sort_by_importance(Task arr[], Task temp[], int left, int right);
extern void sort_by_deadline(Task arr[], Task temp[], int left, int right);
extern void sort_by_time(Task arr[], Task temp[], int left, int right);
extern void sort_by_ratio(Task arr[], Task temp[], int left, int right);
 extern void derive_thresholds(int show, double *var_to_dp_out, double *tpr_greedy_to_dp_out, double *tpr_sort_to_greedy_out);
#define REPEAT 100000L

// result strcture for every row 
typedef struct {
    const char *strategy;   
    const char *detail;                             
    int    has_selection;                           
    int    selected_count;
    int    time_used;
    int    importance_gained;
    double exec_ms;
} RowResult;
 

// sorting subrows 
static const char *SORT_CRITERIA_NAMES[5] = {
    "Importance", "Deadline", "Difficulty", "Study Time", "Imp/Time Ratio"
};
 
static RowResult run_sorting_criterion_silent(const Scenario *s, int choice)
{
    RowResult r = {"Sorting", SORT_CRITERIA_NAMES[choice - 1], 1, 0, 0, 0, 0.0};
    int n = s->num_tasks;
    Task sorted[MAX_TASKS];
    memcpy(sorted, s->tasks, n * sizeof(Task));
    mergeSort(sorted, 0, n - 1, choice);
 
    int time_used = 0, imp_gained = 0, count = 0;
    for (int i = 0; i < n; i++) {
        if (time_used + sorted[i].study_time <= s->available_time) {
            time_used  += sorted[i].study_time;
            imp_gained += sorted[i].importance;
            count++;
        }
    }
    r.selected_count = count;
    r.time_used = time_used;
    r.importance_gained = imp_gained;
 
    Task bench[MAX_TASKS];
    clock_t t0 = clock();
    for (long rep = 0; rep < REPEAT; rep++) {
        memcpy(bench, s->tasks, n * sizeof(Task));
        mergeSort(bench, 0, n - 1, choice);
        int bt = 0;
        for (int i = 0; i < n; i++)
            if (bt + bench[i].study_time <= s->available_time) bt += bench[i].study_time;
    }
    clock_t t1 = clock();
    r.exec_ms = (1000.0 * (t1 - t0) / CLOCKS_PER_SEC) / (double)REPEAT;
    return r;
}
 
// greedy subrows
typedef void (*GreedySortFn)(Task[], Task[], int, int);
 
static const char *GREEDY_RULE_NAMES[4] = {
    "Importance", "Deadline", "Study Time", "Imp/Time Ratio"
};
static GreedySortFn GREEDY_SORT_FNS[4] = {
    sort_by_importance, sort_by_deadline, sort_by_time, sort_by_ratio
};
 
static RowResult run_greedy_rule_silent(const Scenario *s, int rule_index)
{
    RowResult r = {"Greedy", GREEDY_RULE_NAMES[rule_index], 1, 0, 0, 0, 0.0};
    int n = s->num_tasks;
    Task sorted[MAX_TASKS], temp[MAX_TASKS];
    memcpy(sorted, s->tasks, n * sizeof(Task));
    GREEDY_SORT_FNS[rule_index](sorted, temp, 0, n - 1);
 
    int time_used = 0, imp_gained = 0, count = 0;
    for (int i = 0; i < n; i++) {
        if (time_used + sorted[i].study_time <= s->available_time) {
            time_used  += sorted[i].study_time;
            imp_gained += sorted[i].importance;
            count++;
        }
    }
    r.selected_count = count;
    r.time_used = time_used;
    r.importance_gained = imp_gained;
 
    Task bench[MAX_TASKS], btemp[MAX_TASKS];
    clock_t t0 = clock();
    for (long rep = 0; rep < REPEAT; rep++) {
        memcpy(bench, s->tasks, n * sizeof(Task));
        GREEDY_SORT_FNS[rule_index](bench, btemp, 0, n - 1);
        int bt = 0;
        for (int i = 0; i < n; i++)
            if (bt + bench[i].study_time <= s->available_time) bt += bench[i].study_time;
    }
    clock_t t1 = clock();
    r.exec_ms = (1000.0 * (t1 - t0) / CLOCKS_PER_SEC) / (double)REPEAT;
    return r;
}
 
// dynamic programming 
static int max_int(int a, int b) { return (a > b) ? a : b; }
 
static RowResult run_dp_silent(const Scenario *s)
{
    RowResult r = {"Dynamic Programming", "", 1, 0, 0, 0, 0.0};
    int n = s->num_tasks;
    int c = s->available_time;
 
    int v[MAX_TASKS + 1], wt[MAX_TASKS + 1];
    v[0] = 0; wt[0] = 0;
    for (int i = 1; i <= n; i++) {
        v[i]  = s->tasks[i - 1].importance;
        wt[i] = s->tasks[i - 1].study_time;
    }
 
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) dp[i] = (int *)malloc((c + 1) * sizeof(int));
 
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= c; j++) {
            if (i == 0 || j == 0) dp[i][j] = 0;
            else if (wt[i] <= j) dp[i][j] = max_int(v[i] + dp[i - 1][j - wt[i]], dp[i - 1][j]);
            else dp[i][j] = dp[i - 1][j];
        }
    }
 
    int *selected = (int *)calloc((n + 1), sizeof(int));
    int i = n, j = c;
    while (i > 0 && j > 0) {
        if (dp[i][j] == dp[i - 1][j]) { selected[i] = 0; i--; }
        else { selected[i] = 1; j -= wt[i]; i--; }
    }
 
    int time_used = 0, imp_gained = 0, count = 0;
    for (int k = 1; k <= n; k++) {
        if (selected[k]) {
            time_used  += wt[k];
            imp_gained += v[k];
            count++;
        }
    }
    r.selected_count = count;
    r.time_used = time_used;
    r.importance_gained = imp_gained;
 
    int *bench_sel = (int *)calloc((n + 1), sizeof(int));
    clock_t t0 = clock();
    for (long rep = 0; rep < REPEAT; rep++) {
        for (int a = 0; a <= n; a++) {
            for (int b = 0; b <= c; b++) {
                if (a == 0 || b == 0) dp[a][b] = 0;
                else if (wt[a] <= b) dp[a][b] = max_int(v[a] + dp[a - 1][b - wt[a]], dp[a - 1][b]);
                else dp[a][b] = dp[a - 1][b];
            }
        }
        int a = n, b = c;
        while (a > 0 && b > 0) {
            if (dp[a][b] == dp[a - 1][b]) { 
                bench_sel[a] = 0; 
                a--; 
            }
            else {
                bench_sel[a] = 1; 
                b = b - wt[a]; 
                a--; 
            }
        }
    }
    clock_t t1 = clock();
    r.exec_ms = (1000.0 * (t1 - t0) / CLOCKS_PER_SEC) / (double)REPEAT;
 
    free(bench_sel);
    free(selected);
    for (int k = 0; k <= n; k++) free(dp[k]);
    free(dp);
    return r;
}
 
// aiml 
static RowResult run_aiml_silent(const Scenario *s, char *predicted_label, size_t label_size)
{
    RowResult r = {"AI/ML Recommended", "", 0, 0, 0, 0, 0.0};
 
    /* Fixed thresholds reproduced from aiml.c's derive_thresholds() on the
       same 15-row training_data table (the training data is hardcoded and
       scenario-independent, so these constants match what aiml.c computes
       at runtime: var_to_dp = (6+8)/2 = 7.0,
                   tpr_sort_to_greedy = max Sorting tpr = 1.00,
                   tpr_greedy_to_dp   = (1.60+2.00)/2 = 1.80). */

    double var_to_dp, tpr_greedy_to_dp, tpr_sort_to_greedy;
    derive_thresholds(0, &var_to_dp, &tpr_greedy_to_dp, &tpr_sort_to_greedy);
 
    int total_time = 0, sum_imp = 0;
    int min_imp = s->tasks[0].importance, max_imp = s->tasks[0].importance;
 
    clock_t t0 = clock();
    for (long rep = 0; rep < REPEAT; rep++) {
        total_time = 0; sum_imp = 0;
        min_imp = s->tasks[0].importance; max_imp = s->tasks[0].importance;
        for (int i = 0; i < s->num_tasks; i++) {
            const Task *t = &s->tasks[i];
            total_time += t->study_time;
            sum_imp += t->importance;
            if (t->importance < min_imp) min_imp = t->importance;
            if (t->importance > max_imp) max_imp = t->importance;
        }
    }
    clock_t t1 = clock();
    r.exec_ms = (1000.0 * (t1 - t0) / CLOCKS_PER_SEC) / (double)REPEAT;
 
    double tpr     = (double)total_time / s->available_time;
    double imp_var = (double)(max_imp - min_imp);
    (void)sum_imp;
 
    if (imp_var >= var_to_dp) strncpy(predicted_label, "Dynamic Programming", label_size - 1);
    else if (tpr >= tpr_greedy_to_dp) strncpy(predicted_label, "Dynamic Programming", label_size - 1);
    else if (tpr > tpr_sort_to_greedy) strncpy(predicted_label, "Greedy strategy", label_size - 1);
    else strncpy(predicted_label, "Sorting-based ranking", label_size - 1);
    predicted_label[label_size - 1] = '\0';

    return r;
}

// best row comparison
// first compare importance gained (higher one wins)
// if there is a tie, then lower execution time wins
static int is_better(const RowResult *candidate, const RowResult *current)
{
    if (candidate->importance_gained != current->importance_gained)
        return candidate->importance_gained > current->importance_gained;
 
    if (candidate->selected_count == current->selected_count && candidate->time_used      == current->time_used)
        return candidate->exec_ms < current->exec_ms;

    return 0;
}
 
// print table 
#define COL_STRATEGY   21
#define COL_DETAIL     16
#define COL_SELECTED    9
#define COL_TIME       11
#define COL_IMPORTANCE 11
#define COL_EXEC       14
 
static void print_divider(void)
{
    printf("  ");
    for (int i = 0; i < COL_STRATEGY;   i++) { putchar('-'); } putchar('+');
    for (int i = 0; i < COL_DETAIL;     i++) { putchar('-'); } putchar('+');
    for (int i = 0; i < COL_SELECTED;   i++) { putchar('-'); } putchar('+');
    for (int i = 0; i < COL_TIME;       i++) { putchar('-'); } putchar('+');
    for (int i = 0; i < COL_IMPORTANCE; i++) { putchar('-'); } putchar('+');
    for (int i = 0; i < COL_EXEC;       i++) { putchar('-'); }
    putchar('\n');
}
 
static void print_header(void)
{
    printf("  %-*s|%-*s|%*s|%*s|%*s|%*s\n",
           COL_STRATEGY,   "Strategy",
           COL_DETAIL,     " Detail",
           COL_SELECTED,   "Selected ",
           COL_TIME,       "Time used ",
           COL_IMPORTANCE, "Importance ",
           COL_EXEC,       "Exec time ");
}
 
static void print_row(const RowResult *r)
{
    char sel_buf[COL_SELECTED + 1];
    char time_buf[COL_TIME + 1];
    char imp_buf[COL_IMPORTANCE + 1];
    char exec_buf[COL_EXEC + 1];
    char detail_buf[COL_DETAIL + 1];
 
    if (r->has_selection) {
        snprintf(sel_buf,  sizeof(sel_buf),  "%d ",   r->selected_count);
        snprintf(time_buf, sizeof(time_buf), "%d h ", r->time_used);
        snprintf(imp_buf,  sizeof(imp_buf),  "%d ",   r->importance_gained);
    } 
    else {
        snprintf(sel_buf,  sizeof(sel_buf),  "- ");
        snprintf(time_buf, sizeof(time_buf), "- ");
        snprintf(imp_buf,  sizeof(imp_buf),  "- ");
    }
    snprintf(exec_buf, sizeof(exec_buf), "%.4f ms", r->exec_ms);
    snprintf(detail_buf, sizeof(detail_buf), " %s", r->detail);
 
    printf("  %-*s|%-*s|%*s|%*s|%*s|%*s\n", COL_STRATEGY, r->strategy, COL_DETAIL, detail_buf, COL_SELECTED, sel_buf, COL_TIME, time_buf, COL_IMPORTANCE, imp_buf, COL_EXEC, exec_buf);
}
 
void run_comparison(const Scenario *s)
{
    printf("\n");
    printf("==========================================================================================\n");
    printf("  Module 6 - Performance Comparison\n");
    printf("  (Sorting: all 5 criteria | Greedy: all 4 rules | Dynamic Programming | AI/ML)\n");
    printf("==========================================================================================\n");

    printf("\n  %s\n", s->label);
    print_divider();
    print_header();
    print_divider();
 
    RowResult sort_rows[5];
    for (int c = 1; c <= 5; c++) {
        sort_rows[c - 1] = run_sorting_criterion_silent(s, c);
        print_row(&sort_rows[c - 1]);
    }
    print_divider();
 
    RowResult greedy_rows[4];
    for (int rule = 0; rule < 4; rule++) {
        greedy_rows[rule] = run_greedy_rule_silent(s, rule);
        print_row(&greedy_rows[rule]);
    }
    print_divider();
 
    RowResult dp_row = run_dp_silent(s);
    print_row(&dp_row);
 
    char aiml_label[32];
    RowResult aiml_row = run_aiml_silent(s, aiml_label, sizeof(aiml_label));
    print_row(&aiml_row);
    print_divider();

    const RowResult *best = &sort_rows[0];
    for (int c = 1; c < 5; c++)
        if (is_better(&sort_rows[c], best)) best = &sort_rows[c];
    for (int rule = 0; rule < 4; rule++)
        if (is_better(&greedy_rows[rule], best)) best = &greedy_rows[rule];
    if (is_better(&dp_row, best)) best = &dp_row;
 
    if (best->detail[0] != '\0')
        printf("  Best: %s (%s) - Importance = %d | AI/ML predicted: %s\n", best->strategy, best->detail, best->importance_gained, aiml_label);
    else
        printf("  Best: %s - Importance = %d | AI/ML predicted: %s\n", best->strategy, best->importance_gained, aiml_label);
}