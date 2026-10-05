#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tasks.h"

#define NUM_TRAINING 15

// Approach: Rule-Based Decision Tree

typedef struct {
    int         num_tasks;      // number of study tasks              
    int         total_time;     // total required study time (hours)  
    int         available_time; // available study time (hours)       
    double      tpr;            // time pressure ratio = total / avail 
    double      avg_imp;        // average importance                 
    const char *tightness;      // deadline tightness: Tight/Mod/Loose 
    int         imp_var;        // importance variation = max - min    
    const char *label;          // known best strategy           
} TrainingExample;

static const TrainingExample training_data[NUM_TRAINING] = {
    //  n  total avail  tpr   avgI  tightness    var  label
    {  10,  14,  20,  0.70,  5.5, "Moderate",  3, "Sorting-based ranking" },
    {  11,  17,  20,  0.85,  6.0, "Moderate",  4, "Sorting-based ranking" },
    {  12,  23,  25,  0.92,  6.1, "Tight",     5, "Sorting-based ranking" },
    {  11,  19,  20,  0.95,  5.8, "Moderate",  3, "Sorting-based ranking" },
    {  12,  18,  18,  1.00,  6.5, "Loose",     2, "Sorting-based ranking" },
    {  13,  26,  20,  1.30,  6.8, "Tight",     4, "Greedy strategy" },
    {  13,  28,  20,  1.40,  7.0, "Tight",     5, "Greedy strategy" },
    {  14,  27,  18,  1.50,  7.0, "Tight",     6, "Greedy strategy" },
    {  14,  31,  20,  1.55,  6.7, "Tight",     5, "Greedy strategy" },
    {  13,  24,  15,  1.60,  6.5, "Tight",     4, "Greedy strategy" },
    {  14,  24,  12,  2.00,  7.0, "Tight",     4, "Dynamic Programming" },
    {  15,  30,  12,  2.50,  7.2, "Tight",     6, "Dynamic Programming" },
    {  15,  34,  12,  2.83,  7.3, "Tight",     5, "Dynamic Programming" },
    {  15,  29,  20,  1.45,  5.8, "Tight",     9, "Dynamic Programming" },
    {  14,  24,  20,  1.20,  6.0, "Moderate",  8, "Dynamic Programming" }
};

static int is_label(const TrainingExample *e, const char *label)
{
    return strcmp(e->label, label) == 0;
}

// "Training" step: derive the three thresholds from training_data by scanning for the gaps between classes
void derive_thresholds(int show,
                              double *var_to_dp_out,
                              double *tpr_greedy_to_dp_out,
                              double *tpr_sort_to_greedy_out)
{
    // importance_variation threshold
    double max_normal_var = -1;
    for (int i = 0; i < NUM_TRAINING; i++) {
        const TrainingExample *e = &training_data[i];
        if (!is_label(e, "Dynamic Programming") && e->imp_var > max_normal_var)
            max_normal_var = e->imp_var;
    }
    double min_override_var = 1e9;
    for (int i = 0; i < NUM_TRAINING; i++) {
        const TrainingExample *e = &training_data[i];
        if (is_label(e, "Dynamic Programming") &&
            e->imp_var > max_normal_var && e->imp_var < min_override_var)
            min_override_var = e->imp_var;
    }
    double var_to_dp = (max_normal_var + min_override_var) / 2.0;

    // time_pressure_ratio thresholds
    double max_sort_tpr = -1, min_greedy_tpr = 1e9;
    double max_greedy_tpr = -1, min_dp_tpr = 1e9;

    for (int i = 0; i < NUM_TRAINING; i++) {
        const TrainingExample *e = &training_data[i];
        if (e->imp_var >= var_to_dp) continue;

        if (is_label(e, "Sorting-based ranking") && e->tpr > max_sort_tpr)
            max_sort_tpr = e->tpr;
        if (is_label(e, "Greedy strategy")) {
            if (e->tpr < min_greedy_tpr) min_greedy_tpr = e->tpr;
            if (e->tpr > max_greedy_tpr) max_greedy_tpr = e->tpr;
        }
        if (is_label(e, "Dynamic Programming") && e->tpr < min_dp_tpr)
            min_dp_tpr = e->tpr;
    }
    double tpr_sort_to_greedy = max_sort_tpr;
    double tpr_greedy_to_dp   = (max_greedy_tpr + min_dp_tpr) / 2.0;

    if (show) {
        printf("\n  Training Data (15 labelled examples used to set the thresholds)\n");
        printf("  ---------------------------------------------------------------------------\n");
        printf("    %-6s%-7s%-7s%-7s%-8s%-10s%-5s%s\n",
               "tasks", "total", "avail", "tpr", "avgImp", "tight", "var", "label");
        for (int i = 0; i < NUM_TRAINING; i++) {
            const TrainingExample *e = &training_data[i];
            printf("    %-6d%-7d%-7d%-7.2f%-8.1f%-10s%-5d%s\n",
                   e->num_tasks, e->total_time, e->available_time,
                   e->tpr, e->avg_imp, e->tightness, e->imp_var, e->label);
        }
        printf("\n  Thresholds derived from the training data above (gaps between classes):\n");
        printf("    importance_variation: non-DP max %.0f | DP min %.0f  -> %.1f\n",
               max_normal_var, min_override_var, var_to_dp);
        printf("    time_pressure_ratio : Sorting max %.2f | Greedy min %.2f  -> %.1f\n",
               max_sort_tpr, min_greedy_tpr, tpr_sort_to_greedy);
        printf("    time_pressure_ratio : Greedy max %.2f | DP min %.2f  -> %.1f\n",
               max_greedy_tpr, min_dp_tpr, tpr_greedy_to_dp);

        printf("\n  Decision Rules (checked top-down):\n");
        printf("    if   importance_variation >= %.1f -> Dynamic Programming\n", var_to_dp);
        printf("    elif time_pressure_ratio  >= %.1f -> Dynamic Programming\n", tpr_greedy_to_dp);
        printf("    elif time_pressure_ratio  >  %.1f -> Greedy strategy\n", tpr_sort_to_greedy);
        printf("    else                              -> Sorting-based ranking\n");
    }

    *var_to_dp_out          = var_to_dp;
    *tpr_greedy_to_dp_out   = tpr_greedy_to_dp;
    *tpr_sort_to_greedy_out = tpr_sort_to_greedy;
}

void run_aiml(const Scenario *s)
{
    printf("\n");
    printf("==========================================================================================\n");
    printf("  Module 5 - AI/ML Recommendation (Rule-Based Decision Tree)\n");
    printf("==========================================================================================\n");

    double var_to_dp, tpr_greedy_to_dp, tpr_sort_to_greedy;
    derive_thresholds(1, &var_to_dp, &tpr_greedy_to_dp, &tpr_sort_to_greedy);

    /* ===== Execution time measurement START: repeat the computation and average (no printing inside the loop) ===== */
    const long REPEAT = 100000L;

    int total_time = 0, sum_imp = 0, sum_dl = 0;
    int min_imp = s->tasks[0].importance, max_imp = s->tasks[0].importance;
    int min_dl  = s->tasks[0].deadline;
    double num_tasks = 0, avail_time = 0, tpr = 0, avg_imp = 0;
    double avg_deadline = 0, nearest_dl = 0, imp_var = 0;
    const char *tightness = "Loose";
    const char *strategy  = "Sorting-based ranking";
    const char *reason    = "";

    clock_t t0 = clock();
    for (long rep = 0; rep < REPEAT; rep++) {
        // extract the 7 features from the selected scenario
        total_time = 0; sum_imp = 0; sum_dl = 0;
        min_imp = s->tasks[0].importance; max_imp = s->tasks[0].importance;
        min_dl  = s->tasks[0].deadline;
        for (int i = 0; i < s->num_tasks; i++) {
            const Task *t = &s->tasks[i];
            total_time += t->study_time;
            sum_imp    += t->importance;
            sum_dl     += t->deadline;
            if (t->importance < min_imp) min_imp = t->importance;
            if (t->importance > max_imp) max_imp = t->importance;
            if (t->deadline   < min_dl)  min_dl  = t->deadline;
        }
        num_tasks    = s->num_tasks;
        avail_time   = s->available_time;
        tpr          = (double)total_time / s->available_time;
        avg_imp      = (double)sum_imp / s->num_tasks;
        avg_deadline = (double)sum_dl / s->num_tasks;
        nearest_dl   = min_dl;
        imp_var      = (double)(max_imp - min_imp);

        if (avg_deadline <= 4.0)      tightness = "Tight";
        else if (avg_deadline <= 8.0) tightness = "Moderate";
        else                          tightness = "Loose";

        // apply the decision tree (branches on tpr and imp_var)
        if (imp_var >= var_to_dp) {
            strategy = "Dynamic Programming";
            reason   = "Importance scores vary widely across tasks, so optimal\n"
                       "    selection via Dynamic Programming is worth the extra cost.";
        } else if (tpr >= tpr_greedy_to_dp) {
            strategy = "Dynamic Programming";
            reason   = "Time pressure is very high, so an optimal (DP) plan is\n"
                       "    needed to make the best use of limited time.";
        } else if (tpr > tpr_sort_to_greedy) {
            strategy = "Greedy strategy";
            reason   = "Time pressure is moderate and importance is fairly even,\n"
                       "    so a fast greedy plan gives a good result cheaply.";
        } else {
            strategy = "Sorting-based ranking";
            reason   = "Time pressure is low, so simply ranking tasks by\n"
                       "    priority is enough to plan study sessions.";
        }
    }
    clock_t t1 = clock();
    double exec_ms = (1000.0 * (t1 - t0) / CLOCKS_PER_SEC) / (double)REPEAT;
    /* ===== Execution time measurement END ===== */

    // Output: extracted features
    printf("\n  Extracted Features from the scenario\n");
    printf("  ------------------------------------------------------------\n");
    printf("    Number of tasks           : %.0f\n", num_tasks);
    printf("    Total required study time : %d h\n", total_time);
    printf("    Available study time      : %.0f h\n", avail_time);
    printf("    Time pressure ratio       : %.2f\n", tpr);
    printf("    Average importance        : %.2f\n", avg_imp);
    printf("    Deadline tightness        : %s (avg deadline = %.1f day(s), nearest = %.0f day(s))\n",
           tightness, avg_deadline, nearest_dl);
    printf("    Importance variation      : %.0f\n", imp_var);

    // Output: recommendation
    printf("\n  ------------------------------------------------------------\n");
    printf("  Predicted Planning Strategy : %s\n", strategy);
    printf("\n  Reason:\n    %s\n", reason);
    printf("\n  Execution time (algorithm only) : %.6f ms (avg of %ld runs)\n", exec_ms, REPEAT);
}