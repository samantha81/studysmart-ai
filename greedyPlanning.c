#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "tasks.h"

// Helper Functions
// print separator lines
static void print_line(char ch, int width) {
    for (int i = 0; i < width; i++) putchar(ch);
    putchar('\n');
}

// clears anything left over in the input buffer after scanf
static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Sorting functions for each rule: Highest Importance, Earlier Deadline, Shortest Study Time, Highest Importance-to-Time Ratio
// Sorting using merge sort
// Split array into half repeatedly until each part has only 1 task left 
// Then merge them back into sorted order, until the whole array is sorted
// Reason using merge sort: O(n log n)

// Rule 1: Highest Importance First
// If two tasks have same importance, then task with earlier deadline will be given priority
// Because task that due earlier is more urgent to complete
// combines two already sorted parts of array into one sorted part
static void merge_by_importance(Task arr[], Task temp[], int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        int take_left;
        if (arr[i].importance > arr[j].importance) take_left = 1;
        else if (arr[i].importance < arr[j].importance) take_left = 0;
        else if (arr[i].deadline <= arr[j].deadline) take_left = 1;
        else take_left = 0;

        if (take_left) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }

    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int x = left; x <= right; x++) arr[x] = temp[x];
}

// splits array into two half, sorts each half, then merge the two sorted half
void sort_by_importance(Task arr[], Task temp[], int left, int right) {
    if (left >= right) return;  // base case where 1 or 0 tasks left (already sorted)
    int mid = (left + right) / 2;
    sort_by_importance(arr, temp, left, mid);  // sort left half
    sort_by_importance(arr, temp, mid + 1, right);  // sort right half
    merge_by_importance(arr, temp, left, mid, right);  // merge both half
}

// Rule 2: Earlier Deadline First
// If two tasks have same deadline, then task with higher importance will be given priority
// Because when there are two tasks due on same day, it is more practical to complete task that matters more
static void merge_by_deadline(Task arr[], Task temp[], int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;
    
    while (i <= mid && j <= right) {
        int take_left;
        if (arr[i].deadline <arr[j].deadline) take_left = 1;
        else if (arr[i].deadline > arr[j].deadline) take_left = 0;
        else if (arr[i].importance >= arr[j].importance) take_left = 1;
        else take_left = 0;

        if (take_left) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int x = left; x <= right; x++) arr[x] = temp[x];
}

void sort_by_deadline(Task arr[], Task temp[], int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    sort_by_deadline(arr, temp, left, mid);
    sort_by_deadline(arr, temp, mid + 1, right);
    merge_by_deadline(arr, temp, left, mid, right);
}

// Rule 3: Shortest Study Time First
// If two tasks have same amount of study time, then task with higher importance will be given priority
// Because the time cost is equal, the more important task gives greater value
static void merge_by_time(Task arr[], Task temp[], int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        int take_left;
        if (arr[i].study_time < arr[j].study_time) take_left = 1;
        else if (arr[i].study_time > arr[j].study_time) take_left = 0;
        else if (arr[i].importance >= arr[j].importance) take_left = 1;
        else take_left = 0;

        if (take_left) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int x = left; x <= right; x++) arr[x] = temp[x];
}

void sort_by_time(Task arr[], Task temp[], int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    sort_by_time(arr, temp, left, mid);
    sort_by_time(arr, temp, mid + 1, right);
    merge_by_time(arr, temp, left, mid, right);
}

// Rule 4: Highest Importance-to-Time Ratio First
// If two tasks have same ratio, then task with earlier deadline will be given priority
// Because deadline is the only factor not in the ratio
static void merge_by_ratio(Task arr[], Task temp[], int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        double ratio_i = (double)arr[i].importance / arr[i].study_time;
        double ratio_j = (double)arr[j].importance / arr[j].study_time;
        int take_left;

        if (ratio_i > ratio_j) take_left = 1;
        else if (ratio_i < ratio_j) take_left = 0;
        else if (arr[i].deadline <= arr[j].deadline) take_left = 1;
        else take_left = 0;

        if (take_left) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int x = left; x <= right; x++) arr[x] = temp[x];
}

void sort_by_ratio(Task arr[], Task temp[], int left, int right) {
    if (left >= right) return;
    int mid = (left + right) /2;
    sort_by_ratio(arr, temp, left, mid);
    sort_by_ratio(arr, temp, mid + 1, right);
    merge_by_ratio(arr, temp, left, mid, right);
}

// Text labels for each rule (used when printing)
static const char *RULE_NAMES[] = {"Highest Importance First", "Earliest Deadline First", "Shortest Study Time First", "Highest Importance-to-Time Ratio First"};
// Text labels for each rule when there is a tie (used when printing)
static const char *RULE_TIEBREAK[] = {"Earlier deadline wins tie", "Higher importance wins tie", "Higher importance wins tie", "Earlier deadline wins tie"};

// Main Function
void run_greedy(const Scenario *s)
{
    // Selection Menu: asks user the greedy rule they want to use
    printf("\n");
    print_line('=', 90);
    printf("  Module 3 - Greedy Planning\n");
    print_line('=', 90);
    printf("\n  Select a Greedy Rule:\n");
    print_line('-', 52);
    printf("  1. Highest Importance First\n");
    printf("  2. Earliest Deadline First\n");
    printf("  3. Shortest Study Time First\n");
    printf("  4. Highest Importance-to-Time Ratio First\n");
    print_line('-', 52);

    int rule;
    int valid_input = 0;
    while (!valid_input) {
        printf("  Choice: ");
        fflush(stdout);
        // when input not a number
        if (scanf("%d", &rule) != 1) {
            flush_stdin();
            printf("\n  Invalid input. Please enter a number between 1 and 4.\n");
        }
        // when input not 1, 2, 3, 4
        else if (rule < 1 || rule > 4) {
            flush_stdin();
            printf("\n  Invalid choice. Please enter a number between 1 and 4.\n");
        }
        else {
            valid_input = 1;
        }
    }

    // Show the chosen rule
    printf("\n");
    print_line('=', 90);
    printf("  Module 3 - Greedy Planning\n");
    printf("  Rule: %s\n", RULE_NAMES[rule - 1]);
    print_line('=', 90);
    printf("\n");
    printf("  Available study time : %d hours\n", s->available_time);
    printf("  Number of tasks : %d\n", s->num_tasks);
    printf("  Greedy rule : %s\n", RULE_NAMES[rule - 1]);
    printf("  Tie-break   : %s\n", RULE_TIEBREAK[rule - 1]);

    // Copy the task list then sort the copy
    Task sorted[MAX_TASKS];
    // Used by merge sort to hold merged results
    Task temp[MAX_TASKS];
    int n = s->num_tasks;
    memcpy(sorted, s->tasks, n * sizeof(Task));
    if (rule == 1) sort_by_importance(sorted, temp, 0, n - 1);
    else if (rule == 2) sort_by_deadline(sorted, temp, 0, n - 1);
    else if (rule == 3) sort_by_time(sorted, temp, 0, n - 1);
    else sort_by_ratio(sorted, temp, 0, n - 1);

    // Print the sorted list
    printf("\n  Available Tasks (sorted by %s):\n", RULE_NAMES[rule - 1]);
    printf("\n");
    printf("  %-6s %-30s %12s %12s %12s %12s\n", "ID", "Task Name", "Time", "Importance", "Deadline", "Ratio");
    printf("  "); print_line('-', 92);
    for (int i = 0; i < n; i++) {
        Task t = sorted[i];
        printf("  %-6s %-30s %12d %12d %12d %12.3f\n", t.id, t.name, t.study_time, t.importance, t.deadline, (double)t.importance / t.study_time);
    }

    // Go through the sorted list and implement greedy algorithm when selecting tasks
    // For each task, check whether it fits the remaining available study time 
    // If fits, select it and add its time and importance to time_used and total_imp
    // If doesn't fit, skip it and move on to the next task
    // Will not reconsider or backtrack previous task once the decision (select/skip) is made
    int selected[MAX_TASKS], sel_count  = 0;
    int skipped [MAX_TASKS], skip_count = 0;
    int time_used = 0, total_imp = 0;
    printf("\n  Greedy Selection Steps:\n");
    printf("  %-6s %-30s %12s %12s %12s %12s %15s %12s\n", "Step", "Task", "Time", "Importance", "Ratio", "Deadline", "Cumulative Time", "Action");
    printf("  "); print_line('-', 125);

    for (int i = 0; i < n; i++) {
        Task t = sorted[i];
        // Check whether this task fits in the remaining available time
        int fits = (time_used + t.study_time <= s->available_time);
        // If fits, select the task, then add its time and importance to the total
        if (fits) {
            time_used += t.study_time;
            total_imp += t.importance;
            selected[sel_count] = i;
            sel_count++;
        }
        // If not enough time left for the task, skip it
        else {
            skipped[skip_count] = i;
            skip_count++;
        }

        const char *action;
        if (fits) action = "SELECT";
        else action = "SKIP";
        printf("  %-6d %-30s %12d %12d %12.3f %12d %15d %12s\n", i + 1, t.name, t.study_time, t.importance, (double)t.importance / t.study_time, t.deadline, time_used, action);
    }

    // Print the final selected tasks
    int total_imp_all = 0;
    for (int i = 0; i < n; i++) total_imp_all += s->tasks[i].importance;
    printf("\n  Selected Tasks:\n");
    printf("  %-6s %-30s %15s %15s\n", "ID", "Task Name", "Time", "Importance");
    printf("  "); 
    print_line('-', 75);
    for (int i = 0; i < sel_count; i++) {
        Task t = sorted[selected[i]];
        printf("  %-6s %-30s %15d %15d\n", t.id, t.name, t.study_time, t.importance);
    }
    printf("  "); 
    print_line('-', 75);
    printf("  %-37s %15d %15d\n", "TOTAL", time_used, total_imp);

    // ===== Execution time measurement START: repeat the computation and average (no printing inside the loop) =====
    Task bench_arr[MAX_TASKS], bench_tmp[MAX_TASKS];
    int bench_used = 0;
    const long REPEAT = 100000L;
    clock_t bt0 = clock();
    for (long r = 0; r < REPEAT; r++) {
        memcpy(bench_arr, s->tasks, n * sizeof(Task));
        if (rule == 1) sort_by_importance(bench_arr, bench_tmp, 0, n - 1);
        else if (rule == 2) sort_by_deadline(bench_arr, bench_tmp, 0, n - 1);
        else if (rule == 3) sort_by_time(bench_arr, bench_tmp, 0, n - 1);
        else sort_by_ratio(bench_arr, bench_tmp, 0, n - 1);
        bench_used = 0;
        for (int i = 0; i < n; i++) {
            if (bench_used + bench_arr[i].study_time <= s->available_time)
                bench_used += bench_arr[i].study_time;
        }
    }
    clock_t bt1 = clock();
    double exec_ms = (1000.0 * (bt1 - bt0) / CLOCKS_PER_SEC) / (double)REPEAT;
    // ===== Execution time measurement END =====

    // Print Summary
    printf("\n  Summary:\n");
    printf("  Study tasks selected : %d out of %d\n",  sel_count, n);
    printf("  Total study time : %d hours (out of %d available)\n", time_used, s->available_time);
    printf("  Remaining study time : %d hours\n", s->available_time - time_used);
    printf("  Total importance score : %d out of %d\n", total_imp, total_imp_all);
    printf("  Importance achieved : %.1f%%\n", 100.0 * total_imp / total_imp_all);
    printf("  Execution time (algorithm only) : %.6f ms (avg of %ld runs)\n", exec_ms, REPEAT);

    // Print skipped tasks (only printed for scenario BCD)
    if (skip_count > 0) {
        printf("\n  Skipped Tasks (insufficient time remaining):\n");
        printf("  %-6s %-30s %12s %12s %12s\n", "ID", "Task Name", "Time", "Importance", "Deadline");
        printf("  "); 
        print_line('-', 85);
        for (int i = 0; i < skip_count; i++) {
            Task t = sorted[skipped[i]];
            printf("  %-6s %-30s %12d %12d %12d\n", t.id, t.name, t.study_time, t.importance, t.deadline);
        }
    }
}