#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tasks.h"

static void print_line(char ch, int width)
{
    for (int i = 0; i < width; i++)
        putchar(ch);
    putchar('\n');
}

/**
 * Evaluation Engine: Compares two tasks dynamically based on the chosen criterion.
 * Returns 1 (true) if 't1' has a higher sorting priority than 't2', forcing 't1' 
 * to move ahead in the sorted sequence.
 * * Includes primary evaluation and stable tie-breaking logic.
 */
int compare_tasks(Task t1, Task t2, int choice)
{
    switch(choice)
    {
        case 1: // Criterion: Importance (Descending - Highest First)
            if (t1.importance != t2.importance)
                return t1.importance > t2.importance;
            // Tie-breaker: If values match, select the task with an earlier deadline
            return t1.deadline < t2.deadline; 
            
        case 2: // Criterion: Deadline / Urgency (Ascending - Earliest First)
            if (t1.deadline != t2.deadline)
                return t1.deadline < t2.deadline;
            // Tie-breaker: If deadlines match, select the more important task
            return t1.importance > t2.importance;
            
        case 3: // Criterion: Difficulty (Descending - Hardest First)
            if (t1.difficulty != t2.difficulty)
                return t1.difficulty > t2.difficulty;
            // Tie-breaker: If difficulties match, select the more important task
            return t1.importance > t2.importance;
            
        case 4: // Criterion: Study Time (Ascending - Shortest First)
            if (t1.study_time != t2.study_time)
                return t1.study_time < t2.study_time;
            // Tie-breaker: If times match, select the more important task
            return t1.importance > t2.importance;
            
        case 5: // Criterion: Importance-to-Time Efficiency Ratio (Descending)
        default:
            {
                // Dynamic floating-point calculations for priority dense weight maps
                double ratio1 = (double)t1.importance / t1.study_time;
                double ratio2 = (double)t2.importance / t2.study_time;
                if (ratio1 != ratio2)
                    return ratio1 > ratio2;
                // Tie-breaker: If value return densities match, prioritize sooner due date
                return t1.deadline < t2.deadline;
            }
    }
}

/**
 * Conquering Stage: Combines two sorted contiguous sub-arrays into a single sorted range.
 * Reference:
 * The merge procedure in this implementation follows the recursive Merge Sort
 * pseudocode presented in:
 * [1] A. Tyagi and A. K. Ahlawat, "A New Optimized Version of Merge Sort," 
 * in Proceedings of the 2023 11th International Conference on Emerging Trends 
 * in Engineering & Technology – Signal and Information Processing (ICETET-SIP), 
 * Nagpur, India, 2023, pp. 1–5, doi: 10.1109/ICETET-SIP58143.2023.10151579.
 */
void merge(Task arr[], int left, int mid, int right, int choice)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Task L[MAX_TASKS];
    Task R[MAX_TASKS];

    int i, j, k;

    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];

    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0;    // Starting pointer index for the Left structural partition
    j = 0;    // Starting pointer index for the Right structural partition
    k = left; // Target assignment head location in main destination segment array

    while (i < n1 && j < n2)
    {
        if (compare_tasks(L[i], R[j], choice))
        {
            arr[k++] = L[i++]; // Left element satisfies higher priority ranking
        }
        else
        {
            arr[k++] = R[j++]; // Right element satisfies higher priority ranking
        }
    }

    while (i < n1)
        arr[k++] = L[i++];

    while (j < n2)
        arr[k++] = R[j++];
}

/**
 * Dividing Stage: Recursive controller implementing the Divide-and-Conquer strategy.
 * Recursively splits individual array elements until length drops down to single items, 
 * then triggers cascading merge actions upward.
 */
void mergeSort(Task arr[], int left, int right, int choice)
{
    // Structural termination condition: processing range must span more than 1 item
    if (left < right)
    {
        // Calculate mid-point without risking overflow boundary thresholds
        int mid = left + (right - left) / 2;

        // Recursive partition path: Process complete Left Half range split down
        mergeSort(arr, left, mid, choice);
        
        // Recursive partition path: Process complete Right Half range split down
        mergeSort(arr, mid + 1, right, choice);

        // Reconstruct the halves together via local element merge adjustments
        merge(arr, left, mid, right, choice);
    }
}

/**
 * Interface Execution Module: Acts as the primary entry point to load data arrays,
 * accept tracking configuration selections, trigger sorting, and show output tables.
 */
void run_sorting(const Scenario *s)
{
    int choice = 1;
    printf("\n==================================================\n");
    printf(" Choose Sorting Criterion:\n");
    printf(" 1. Highest First Importance\n");
    printf(" 2. Earliest First Deadline/Urgency\n");
    printf(" 3. Highest First Difficulty\n");
    printf(" 4. Shortest First Study Time\n");
    printf(" 5. Highest First Importance-to-Time Ratio\n");
    printf(" Enter choice (1-5): ");
    
    if (scanf("%d", &choice) != 1) {
        choice = 1; 
    }
    
    if(choice < 1 || choice > 5) choice = 1; 

    printf("\n");
    print_line('=', 90);
    printf("  Module 2 - Sorting (Merge Sort)\n");
    print_line('=', 90);

    const char* criteria[] = {"", "Importance", "Deadline", "Difficulty", "Study Time", "Importance-to-Time Ratio"};
    printf("\n  Sorting Criterion    : %s\n", criteria[choice]);
    printf("  Total number of tasks: %d\n", s->num_tasks);
    printf("  Available Study Time : %d hours\n", s->available_time);

    Task sorted[MAX_TASKS];
    int n = s->num_tasks;
    int total_possible_importance = 0;

    for (int i = 0; i < n; i++) {
        sorted[i] = s->tasks[i];
        total_possible_importance += s->tasks[i].importance; 
    }

    // Execute the full Merge Sort workflow
    mergeSort(sorted, 0, n - 1, choice);

    /* Output Table After Sorted*/
    printf("\n  Ranked Task List:\n");
    printf("  %-5s %-6s %-30s %5s %5s %9s %5s %10s\n",
       "Rank","ID","Task Name","Time","Imp.","Deadline","Diff","Ratio");
    print_line('-', 90);

    for (int i = 0; i < n; i++)
    {
        const Task *t = &sorted[i];
        double ratio = (double)t->importance / t->study_time;
        printf("  %-5d %-6s %-30s %5d %5d %9d %5d %10.2f\n",
            i + 1, t->id, t->name, t->study_time, t->importance, t->deadline, t->difficulty, ratio);
    }

    /* Select tasks that fit inside available time */
    int total_time_used = 0;
    int total_importance_gained = 0;
    int selected_count = 0;

    for (int i = 0; i < n; i++)
    {
        if (total_time_used + sorted[i].study_time <= s->available_time)
        {
            total_time_used += sorted[i].study_time;
            total_importance_gained += sorted[i].importance;
            selected_count++;
        }
    }

    /* ===== Execution time measurement START: repeat the computation and average (no printing inside the loop) ===== */
    Task bench_arr[MAX_TASKS];
    int bench_used = 0;
    const long REPEAT = 100000L;
    clock_t bt0 = clock();
    for (long r = 0; r < REPEAT; r++) {
        memcpy(bench_arr, s->tasks, n * sizeof(Task));
        mergeSort(bench_arr, 0, n - 1, choice);
        bench_used = 0;
        for (int i = 0; i < n; i++) {
            if (bench_used + bench_arr[i].study_time <= s->available_time)
                bench_used += bench_arr[i].study_time;
        }
    }
    clock_t bt1 = clock();
    double exec_ms = (1000.0 * (bt1 - bt0) / CLOCKS_PER_SEC) / (double)REPEAT;
    /* ===== Execution time measurement END ===== */

    /* Summary Block */
    printf("\n  Summary:\n");
    
    switch(choice) {
        case 1: // Importance Metrics Presentation
            printf("  - Highest Priority  : %s (Importance: %d)\n", sorted[0].name, sorted[0].importance);
            printf("  - Lowest Priority   : %s (Importance: %d)\n", sorted[n - 1].name, sorted[n - 1].importance);
            break;
        case 2: // Deadline Metrics Presentation
            printf("  - Highest Priority  : %s (Deadline: %d day(s))\n", sorted[0].name, sorted[0].deadline);
            printf("  - Lowest Priority   : %s (Deadline: %d day(s))\n", sorted[n - 1].name, sorted[n - 1].deadline);
            break;
        case 3: // Difficulty Metrics Presentation
            printf("  - Highest Priority  : %s (Difficulty: %d/5)\n", sorted[0].name, sorted[0].difficulty);
            printf("  - Lowest Priority   : %s (Difficulty: %d/5)\n", sorted[n - 1].name, sorted[n - 1].difficulty);
            break;
        case 4: // Time Metrics Presentation
            printf("  - Highest Priority  : %s (Study Time: %d hours)\n", sorted[0].name, sorted[0].study_time);
            printf("  - Lowest Priority   : %s (Study Time: %d hours)\n", sorted[n - 1].name, sorted[n - 1].study_time);
            break;
        case 5: // Efficiency Ratio Metrics Presentation
        default:
            printf("  - Highest Priority  : %s (Ratio: %.2f)\n", sorted[0].name, (double)sorted[0].importance / sorted[0].study_time);
            printf("  - Lowest Priority   : %s (Ratio: %.2f)\n", sorted[n - 1].name, (double)sorted[n - 1].importance / sorted[n - 1].study_time);
            break;
    }

    printf("  - Study tasks selected: %d out of %d\n", selected_count, n);
    printf("  - Total study time    : %d hours (out of %d available)\n", total_time_used, s->available_time);
    printf("  - Remaining study time: %d hours\n", s->available_time - total_time_used);
    printf("  - Total importance score : %d out of %d\n", total_importance_gained, total_possible_importance);
    printf("  - Execution time (algorithm only) : %.6f ms (avg of %ld runs)\n", exec_ms, REPEAT);
}