# StudySmart AI 📚

> A Command-Line Interface study planner written in C that compares different algorithms such as **merge sort**, **greedy selection** and **0/1 knapsack dynamic programming** for building study plans, and uses a **rule-based decision tree** to recommend which one to use.

---

## Overview

Given a set of study tasks and a limited number of available hours, which tasks should a student study, and in what order? StudySmart AI answers this with three classic algorithmic strategies, benchmarks them against each other, and has a lightweight AI/ML module that predicts the best strategy for a given scenario.

### Features

- **Sorting-based ranking (Merge Sort, divide-and-conquer)**: ranks tasks in descending order by a user-chosen criterion: importance, deadline/urgency, difficulty, study time, or importance-to-time ratio. Stable O(n log n) in all cases.
- **Greedy planning**: selects top-ranked tasks until the available time runs out, using four rules: Highest Importance First, Earliest Deadline First, Shortest Study Time First, Highest Importance-to-Time Ratio First.
- **Dynamic programming (0/1 knapsack)**: finds the *optimal* set of tasks that maximises total importance within the time limit.
- **AI/ML recommendation**: a rule-based decision tree (plain C, no external libraries, fully explainable) that summarises a scenario into features and recommends Sorting, Greedy or Dynamic Programming with a short reason. Its thresholds are derived from 15 labelled training examples.
- **Performance comparison**: runs every strategy on the same scenario (averaged over 100,000 runs for fair timing) and compares tasks selected, total study time, total importance and execution time. It also checks whether the AI/ML prediction matches the actual best performer.
- **Pre-built and manual scenarios**: four hard-coded scenarios, or enter up to 15 tasks of your own.

---

## Task Data Model

Each scenario holds a maximum of **15 tasks**, each with 7 fields:

| Field | Description |
|---|---|
| Task ID | Unique identifier |
| Task Name | Name of the task or topic |
| Study Time (hours) | Time needed to complete the task |
| Importance Score | Academic importance / expected benefit (1–10) |
| Deadline (days) | How soon the task must be completed |
| Difficulty Level | Estimated difficulty (1–5) |
| Task Type | Lecture, Tutorial, Assignment, Practice or Revision |

### Pre-built Scenarios

| Scenario | Type | Design |
|---|---|---|
| **A** | Low pressure | 24 h available > 22 h required; baseline where every algorithm should succeed |
| **B** | High pressure | 12 h available << 34 h required; exposes the gap between greedy and DP |
| **C** | Deadline-focused | Many urgent deadlines (avg 3.07 days); punishes algorithms that ignore deadlines |
| **D** | Importance-focused | A few tasks with very high importance (9–10); favours importance-driven selection |

---

## Project Structure

```
studyplan/
├── main.c                 # Menus, scenario loading (pre-built / manual input)
├── tasks.h                # Shared Task / Scenario structs and function declarations
├── sorting.c              # Module: merge sort ranking
├── greedyPlanning.c       # Module: greedy planning (4 rules)
├── dynamicProgramming.c   # Module: 0/1 knapsack DP
├── aiml.c                 # Module: rule-based decision-tree recommendation
└── comparison.c           # Module: performance comparison across all strategies
```

---

## Getting Started

### Prerequisites

- A C compiler (e.g. `gcc`)

### Build

```bash
cd studyplan
gcc main.c sorting.c greedyPlanning.c dynamicProgramming.c aiml.c comparison.c -o studysmart
```

### Run

```bash
./studysmart        # Linux / macOS
studysmart.exe      # Windows
```

Then follow the on-screen menus to load a scenario, choose an algorithm, ask for an AI/ML recommendation, or run the full comparison.

---

## How the AI/ML Recommendation Works

A rule-based decision tree summarises a scenario into seven features but decides using two:

1. **Time-pressure ratio** = total required hours ÷ available hours
2. **Importance variation** = max importance − min importance

| Condition | Recommendation |
|---|---|
| Low time pressure | Sorting-based ranking |
| Moderate time pressure | Greedy strategy |
| Very high time pressure, or widely spread importance | Dynamic Programming |

Thresholds come from the gaps between classes in 15 labelled training examples. The decision tree was chosen over a black-box model because it is lightweight, dependency-free and explainable.

---

## Key Findings

- **Low pressure (A):** all strategies select the same 15 tasks (importance 92); only execution time differs.
- **High pressure (B):** dynamic programming gives the best result (6 tasks, importance 46 in 12 h), beating the best greedy and sorting results (importance 44).
- **Trade-off:** DP is optimal on importance and time, but it ignores deadlines and difficulty, so it may skip near-due tasks or cluster hard ones together.
- **Greedy** is fast and simple but short-sighted: it never revisits earlier choices, so it can miss a better combination.

The full analysis, workflow diagrams and result tables for all four scenarios are in the project report.

---

## License

All rights reserved. Academic coursework.
