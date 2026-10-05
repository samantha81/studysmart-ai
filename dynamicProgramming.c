#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tasks.h"

int max(int a, int b)
{
    return (a > b) ? a : b;
}

void run_dp(const Scenario *s)
{
    printf("\n");
    printf("==========================================================================================\n");
    printf("  Module 4 - Dynamic Programming (0/1 Knapsack)\n");
    printf("==========================================================================================\n\n");

    int n = s->num_tasks;
    int c = s->available_time;

    printf("  Total number of tasks : %d\n", n);
    printf("  Available study time : %d hours\n\n", c);
    printf("  Available Tasks:\n");
    printf("  %-6s %-30s %10s %10s\n", "ID", "Task Name", "Study Time", "Importance Score");
    printf("  %-6s %-30s %10s %10s\n", "------", "------------------------------", "-----------", "----------------");
    for (int i = 0; i < n; i++) {
        printf("  %-6s %-30s %8d %10d\n",
               s->tasks[i].id,
               s->tasks[i].name,
               s->tasks[i].study_time,
               s->tasks[i].importance);
    }
    printf("\n");

    int *v = (int *)malloc((n + 1) * sizeof(int));   //Value (importance)
    int *wt = (int *)malloc((n + 1) * sizeof(int));  //weight (study time)

    //Base case (set to 0)
    v[0] = 0;
    wt[0] = 0;
    for (int i = 1; i <= n; i++) {
        v[i] = s->tasks[i - 1].importance;
        wt[i] = s->tasks[i - 1].study_time;
    }

    //Build DP table dp[n+1][c+1]
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)malloc((c + 1) * sizeof(int));
    }

    // Fill DP table
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= c; j++) {

            if (i == 0 || j == 0)
                dp[i][j] = 0;

            else if (wt[i] <= j)
                dp[i][j] = max(
                    v[i] + dp[i - 1][j - wt[i]],
                    dp[i - 1][j]
                );

            else
                dp[i][j] = dp[i - 1][j];
        }
    }

    // Display DP table
    printf("  DP Table:\n");
    printf("    j:  ");
    for (int j = 0; j <= c; j++) {
        printf("%2d  ", j);
    }
    printf("\n");
    printf("  -------");
    for (int j = 0; j <= c; j++) {
        printf("----");
    }
    printf("\n");
    for (int i = 0; i <= n; i++) {
        printf("  i=%d:  ", i);
        for (int j = 0; j <= c; j++) {
            printf("%2d  ", dp[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("  Maximum Importance Score = %d\n\n", dp[n][c]);

    // Backtracking to select tasks
    int *selected = (int *)calloc((n + 1), sizeof(int));

    int i = n;
    int j = c;

    while (i > 0 && j > 0) {
        if (dp[i][j] == dp[i - 1][j]) {
            selected[i] = 0;
            i--;
        }
        else {
            selected[i] = 1;
            j = j - wt[i];
            i--;
        }
    }

    // Output
    printf("  Selected Tasks:\n");
    printf("  %-6s %-30s %10s %10s\n", "ID", "Task Name", "Study Time", "Importance Score");
    printf("  %-6s %-30s %10s %10s\n", "------", "------------------------------", "-----------", "----------------");

    int total_time = 0;
    int total_importance = 0;
    int selected_count = 0;

    for (int i = 1; i <= n; i++) {
        if (selected[i] == 1) {
            printf("  %-6s %-30s %8d %10d\n",
                   s->tasks[i - 1].id,
                   s->tasks[i - 1].name,
                   wt[i],
                   v[i]);
            total_time += wt[i];
            total_importance += v[i];
            selected_count++;
        }
    }

    printf("  %-6s %-30s %10s %10s\n", "------", "------------------------------", "-----------", "----------------");
    printf("  %-6s %-30s %8d %10d\n", "", "TOTAL", total_time, total_importance);
    printf("\n");

    // ===== Execution time measurement START: repeat the computation and average (no printing inside the loop) =====
    const long REPEAT = 100000L;
    int *bench_sel = (int *)calloc((n + 1), sizeof(int));
    clock_t bt0 = clock();
    for (long r = 0; r < REPEAT; r++) {
        for (int a = 0; a <= n; a++) {
            for (int b = 0; b <= c; b++) {
                if (a == 0 || b == 0) dp[a][b] = 0;
                else if (wt[a] <= b) dp[a][b] = max(v[a] + dp[a - 1][b - wt[a]], dp[a - 1][b]);
                else dp[a][b] = dp[a - 1][b];
            }
        }
        int a = n, b = c;
        while (a > 0 && b > 0) {
            if (dp[a][b] == dp[a - 1][b]) { bench_sel[a] = 0; a--; }
            else { bench_sel[a] = 1; b = b - wt[a]; a--; }
        }
    }
    clock_t bt1 = clock();
    double exec_ms = (1000.0 * (bt1 - bt0) / CLOCKS_PER_SEC) / (double)REPEAT;
    free(bench_sel);
    // ===== Execution time measurement END =====

    printf("  Summary:\n");
    printf("  - Tasks selected: %d out of %d\n", selected_count, n);
    printf("  - Total study time: %d hours (out of %d available)\n", total_time, c);
    printf("  - Remaining study time: %d hours\n", c - total_time);
    printf("  - Total importance score: %d (optimal)\n", total_importance);
    printf("  - Execution time (algorithm only): %.6f ms (avg of %ld runs)\n", exec_ms, REPEAT);
    printf("\n");

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);
    free(v);
    free(wt);
    free(selected);
}
