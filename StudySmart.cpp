#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>       
#include <limits>     
#include <algorithm>   
#include <map>

using namespace std;
using namespace std::chrono;


struct StudyTask {
    string id;
    string name;
    double estimated_time;   // in hours
    int importance_score;  // Scale 1 - 5
    double deadline_days;    
    int difficulty_level;  // Scale 1 - 5
    string task_type; 
};

struct AlgorithmResult {
    string strategy;                // e.g. "Greedy Strategy"
    vector<string> selectedTaskIDs; // IDs of tasks selected/ranked, e.g. {"T1","T3"}
    double totalTime;            
    int totalImportance;          
    double executionTime;       // measured using clock() in milliseconds
};

// Global variables 
vector<StudyTask> taskDataset;
string taskTypes[] = {"Lecture", "Tutorial", "Assignment", "Practice", "Revision"};
double availableStudyTime = 0.0; 
string activeScenario = "None";

// Stores results from each module run; used by Module 6 for comparison.
// Index: 0 = Sorting, 1 = Greedy, 2 = DP, 3 = AI/ML
// Initialised with empty placeholders so Module 6 can detect unrun modules.
vector<AlgorithmResult> moduleResults(4, {"Not Run", {}, 0.0, 0, 0.0});

// Helper functions

int readInt(const string& prompt, int low, int high) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= low && val <= high) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
        }
        cin.clear(); // if input failed, reset buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input or the entered number is out or range. Try again.\n";
    }
}
double readDouble(const string& prompt, double low, double high) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= low && val <= high) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
        }
        cin.clear(); // if input failed, reset buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input or the entered number is out or range. Try again.\n";
    }
}
void printBar(char c = '-', int length = 50, bool startwithnewline = false) { 
    if (startwithnewline) cout << "\n";
    for (int i = 0; i < length; ++i) cout << c; 
    cout << "\n";
}

void displayDataset() {
    if (taskDataset.empty()) {
        cout << "\nNo dataset loaded. Please choose Option 1 or 2 first.\n";
        return;
    }

    printBar();
    cout << " CURRENT SCENARIO: " << activeScenario << " | Available Study Time: " << availableStudyTime << " hours\n";
    printBar();
    
    cout << fixed << setprecision(1); 
    
    cout << left 
         << setw(6)  << "ID" 
         << setw(28) << "Task Name" 
         << setw(12) << "Time (hrs)" 
         << setw(12) << "Importance" 
         << setw(10) << "Deadline" 
         << setw(12) << "Difficulty" 
         << setw(12) << "Type" << "\n";
    printBar();
    double totalRequiredTime = 0.0; 
    for (const auto& task : taskDataset) {
        cout << left 
             << setw(6)  << task.id 
             << setw(28) << task.name 
             << setw(12) << task.estimated_time 
             << setw(12) << task.importance_score 
             << setw(10) << task.deadline_days 
             << setw(12) << task.difficulty_level 
             << setw(12) << task.task_type << "\n";
        totalRequiredTime += task.estimated_time;
    }
    printBar();
    cout << "Total Tasks: " << taskDataset.size() 
         << " | Total Required Time: " << totalRequiredTime << " hours\n";
    printBar('=');
}


void loadBuiltInScenarios() {
    taskDataset.clear();
    
    cout << "\nSelect a scenario:\n";
    cout << "1. Low-Pressure Scenario (Time budget is high)\n";
    cout << "2. Deadline-Focused Scenario (Several tasks are urgent)\n";
    cout << "3. High-Pressure Scenario (Time budget is low)\n";
    
    int choice = readInt("Enter choice (1-3): ", 1, 3);

    vector<StudyTask> baseTasks = {
        {"T1", "Revise Dynamic Programming",  3.5, 5, 7.0,  4, "Revision"},
        {"T2", "Complete Java Tutorial",      2.0, 4, 5.0,  3, "Tutorial"},
        {"T3", "Practise Greedy Problems",    2.0, 3, 6.0,  2, "Practice"},
        {"T4", "Do NLP Assignment",           4.0, 5, 8.0,  4, "Assignment"},
        {"T5", "Study CNN Lecture",           2.5, 2, 9.0,  1, "Lecture"}, 
        {"T6", "Practice OS Lab Exercise3",   2.0, 3, 4.0,  3, "Practice"},
        {"T7", "Complete OS Lab Report",      2.0, 4, 10.0, 3, "Assignment"},
        {"T8", "Study Dynamic Programming",   1.0, 1, 12.0, 1, "Lecture"},
        {"T9", "Deep Learning Quiz Review",   1.0, 3, 5.0,  2, "Revision"},
        {"T10","Review Computer Vision Notes",1.5, 4, 14.0, 3, "Revision"}
    };
    taskDataset = baseTasks; 
    switch (choice) {
        case 1:
            availableStudyTime = 30.0; 
            activeScenario = "Low-Pressure Scenario";
            cout << "\nLow-pressure scenario loaded.\n";
            break;
        case 2:
            taskDataset[0].deadline_days = 1.5; 
            taskDataset[3].deadline_days = 2.0; 
            taskDataset[5].deadline_days = 1.0; 
            availableStudyTime = 15.0; 
            activeScenario = "Deadline-Focused Scenario";
            cout << "\nDeadline-focused scenario loaded.\n";
            break;
        case 3:
            availableStudyTime = 8.0;  
            activeScenario = "High-Pressure Scenario";
            cout << "\nHigh-pressure scenario loaded.\n";
            break; 
    }
}


// Manual User Input 
void handleUserInputDataset() {
    taskDataset.clear();
    activeScenario = "Custom User-Defined Scenario";

    printBar('=', 38, true);
    cout << "     MANUAL DATASET INPUT MODE\n";
    printBar('=', 38);
    
    availableStudyTime = readDouble("Enter total Available Study Time for this scenario (in hours): ", 0.5, 999.0);

    int numTasks = readInt("Enter the number of tasks you have: ", 1, 100);

    for (int i = 0; i < numTasks; i++) {
        StudyTask task;
        task.id = "U" + to_string(i + 1); 

        cout << "\n--- Task " << (i + 1) << " of " << numTasks << " ---\n";
        cout << "Enter task name/topic: ";
        getline(cin, task.name);

        task.estimated_time  = readDouble("Enter estimated study time in hours (e.g., 2.5, 3): ", 0.5, 20.0);
        task.importance_score = readInt  ("Enter importance of the task from 1-5 : ",             1,   5);
        task.deadline_days   = readDouble("Enter days remaining (deadline urgency, e.g., 3.5, 4): ", 0.5, 365.0);
        task.difficulty_level = readInt  ("Enter difficulty level (1-5): ",                        1,   5);

        cout << "Enter task type (Lecture, Tutorial, Assignment, Practice, Revision): ";
        while (true) {
            getline(cin, task.task_type);
            // if entered task matches the existing task types
            if (find(begin(taskTypes), end(taskTypes), task.task_type) != end(taskTypes)) {
                break;
            }
            cout << "\nPlease enter a valid task type (Lecture, Tutorial, Assignment, Practice, Revision):\n";
        }
        taskDataset.push_back(task);
    }
    cout << "\n[+] Custom dataset of " << numTasks << " tasks succesfully loaded.\n";
}


// MODULE 2: SORTING, DIVIDE-AND-CONQUER (QUICK SORT)

// Median-of-Three Pre-Processor (selecting the pivot)
void applyMedianOfThree(vector<StudyTask>& tasks, int low, int high) {
    int mid = low + (high - low) / 2;

    double ratioLow = tasks[low].importance_score / tasks[low].estimated_time;
    double ratioMid = tasks[mid].importance_score / tasks[mid].estimated_time;
    double ratioHigh = tasks[high].importance_score / tasks[high].estimated_time;

    // Sort low, mid, and high elements in descending order
    if (ratioLow < ratioMid) {
        swap(tasks[low], tasks[mid]);
        swap(ratioLow, ratioMid); 
    }
    if (ratioLow < ratioHigh) {
        swap(tasks[low], tasks[high]);
        swap(ratioLow, ratioHigh);
    }
    if (ratioMid < ratioHigh) {
        swap(tasks[mid], tasks[high]);
        swap(ratioMid, ratioHigh);
    }

    // Move the median value to the 'high' position to be the pivot
    swap(tasks[mid], tasks[high]);
}

// Partition Logic
int partitionTasks(vector<StudyTask>& tasks, int low, int high) {
    applyMedianOfThree(tasks, low, high); // Inject Optimization

    double pivotRatio = tasks[high].importance_score / tasks[high].estimated_time;
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        double currentRatio = tasks[j].importance_score / tasks[j].estimated_time;
        if (currentRatio > pivotRatio) { // Descending Order
            i++;
            swap(tasks[i], tasks[j]);
        }
    }
    swap(tasks[i + 1], tasks[high]);
    return i + 1;
}

// Recursive Quick Sort
void quickSortTasks(vector<StudyTask>& tasks, int low, int high) {
    if (low < high) {
        int pi = partitionTasks(tasks, low, high);
        quickSortTasks(tasks, low, pi - 1);
        quickSortTasks(tasks, pi + 1, high);
    }
}


// 2. MAIN MODULE WRAPPER
AlgorithmResult runSortingModule(vector<StudyTask> tasks, double timeLimit) {
    (void)timeLimit; // Safely ignore unused parameter warning

    AlgorithmResult result;
    result.strategy = "Sorting-based ranking";
    result.totalTime = 0.0;
    result.totalImportance = 0;

    auto start = high_resolution_clock::now();
    
    if (!tasks.empty()) {
        quickSortTasks(tasks, 0, tasks.size() - 1);
    }
    
    auto end = high_resolution_clock::now();
    duration<double, milli> precise_duration = end - start;
    result.executionTime = precise_duration.count();

    // Populate Shared Result Schema Fields 
    for (const auto& task : tasks) {
        result.selectedTaskIDs.push_back(task.id);
        result.totalTime += task.estimated_time;
        result.totalImportance += task.importance_score;
    }

    // Console Output 
    cout << "\n  ----------------------------------------------------------------------\n";
    cout << "           RANKED STUDY TASKS (Quick Sort)\n";
    cout << "  ----------------------------------------------------------------------\n";
    
    cout << "  " << left << setw(6)  << "Rank"
                         << setw(8)  << "ID"
                         << setw(35) << "Task Name"
                         << setw(15) << "Ratio (Imp/Hr)" << "\n";
    cout << "  " << string(70, '-') << "\n";

    if (tasks.empty()) {
        cout << "  [No tasks available to sort.]\n";
    } else {
        int rank = 1;
        for (const auto& t : tasks) {
            // Calculate ratio for display
            double ratio = t.importance_score / t.estimated_time;
            
            // Print the formatted row
            cout << "  " << left << setw(6)  << rank++
                                 << setw(8)  << t.id
                                 << setw(35) << t.name
                                 << fixed << setprecision(2) << setw(15) << ratio << "\n";
        }
    }

    cout << "  " << string(70, '-') << "\n";
    cout << "  [+] Quick Sort complete. Execution Time: " << fixed << setprecision(4) << result.executionTime << " ms\n";
    cout << "========================================================================\n";

    return result;
} 


 
// MODULE 3: GREEDY PLANNING MODULE (Member 3)
// ============================================================
// Selected greedy rule: HIGHEST IMPORTANCE-TO-TIME RATIO FIRST
//
// Justification of the rule:
//   The ratio (importance / study time) measures how much academic
//   value the student gains for every hour of study. Picking tasks
//   with the highest ratio first fills the limited time budget with
//   the most "profitable" tasks. This balances two factors at once:
//   importance alone ignores task length, and shortest-time-first
//   ignores academic value.
//
// Tie-breaking rule:
//   If two tasks have the same ratio, the task with the earlier
//   deadline is placed first, because it is more urgent.
//
// Algorithm steps:
//   1. Compute ratio = importance_score / estimated_time for each task.
//   2. Sort tasks by ratio in descending order  -> O(n log n)
//   3. Scan the sorted list once. Take a task if it still fits into
//      the remaining study time, otherwise skip it.  -> O(n)
//   Total time complexity: O(n log n). Space complexity: O(n).
//
// Note on optimality:
//   This greedy strategy is fast but NOT always optimal for the
//   0/1 task-selection problem (see counterexample in the report).
//   The optimal result is produced by the DP module (Module 4).
// ============================================================
AlgorithmResult runGreedyModule(const vector<StudyTask>& tasks, double availableTime) {
    AlgorithmResult result;
    result.strategy    = "Greedy";
    result.totalTime       = 0.0;
    result.totalImportance = 0;
    result.executionTime = 0.0;

    if (tasks.empty()) {
        cout << "\n[!] No dataset loaded. Please choose Option 1 or 2 first.\n";
        result.strategy = "Not Run";
        return result;
    }

    // ---------- TIMED SECTION START (algorithm only, no printing) ----------
    clock_t startTime = clock();

    // Step 1 + 2: copy tasks and sort by importance-to-time ratio (descending).
    // A copy is used so the original dataset order is not changed for other modules.
    vector<StudyTask> rankedTasks = tasks;
    sort(rankedTasks.begin(), rankedTasks.end(),
         [](const StudyTask& a, const StudyTask& b) {
             double ratioA = a.importance_score / a.estimated_time;
             double ratioB = b.importance_score / b.estimated_time;
             if (ratioA != ratioB) return ratioA > ratioB; // higher ratio first
             return a.deadline_days < b.deadline_days;     // tie-break: earlier deadline first
         });

    // Step 3: greedy selection. Take every task that still fits into the
    // remaining time budget. Each task is either taken fully or skipped (0/1 rule).
    double remainingTime = availableTime;
    vector<StudyTask> selectedTasks;
    vector<StudyTask> skippedTasks;

    for (const auto& task : rankedTasks) {
        if (task.estimated_time <= remainingTime) {
            selectedTasks.push_back(task);
            remainingTime          -= task.estimated_time;
            result.totalTime       += task.estimated_time;
            result.totalImportance += task.importance_score;
            result.selectedTaskIDs.push_back(task.id);
        } else {
            skippedTasks.push_back(task);
        }
    }

    clock_t endTime = clock();
    result.executionTime = 1000.0 * (double)(endTime - startTime) / CLOCKS_PER_SEC;
    // ---------- TIMED SECTION END ----------

    // ---------- OUTPUT (not timed, so I/O does not distort the measurement) ----------
    cout << fixed << setprecision(2);
    cout << "\n==========================================================================\n";
    cout << " MODULE 3: GREEDY PLANNING  |  Rule: Highest Importance-to-Time Ratio\n";
    cout << " Scenario: " << activeScenario
         << "  |  Available Time: " << availableTime << " hrs\n";
    cout << "==========================================================================\n";

    cout << "\nStep 1: Tasks ranked by ratio (importance / time), descending:\n";
    cout << left << setw(6) << "Rank" << setw(6) << "ID"
         << setw(30) << "Task Name"
         << setw(12) << "Time(hrs)" << setw(12) << "Importance"
         << setw(10) << "Ratio" << setw(10) << "Deadline" << "\n";
    cout << "--------------------------------------------------------------------------\n";
    for (size_t i = 0; i < rankedTasks.size(); i++) {
        const auto& t = rankedTasks[i];
        cout << left << setw(6) << (i + 1) << setw(6) << t.id
             << setw(30) << t.name
             << setw(12) << t.estimated_time
             << setw(12) << t.importance_score
             << setw(10) << (t.importance_score / t.estimated_time)
             << setw(10) << t.deadline_days << "\n";
    }

    cout << "\nStep 2: Greedy selection result (tasks taken in ranked order if they fit):\n";
    cout << "--------------------------------------------------------------------------\n";
    if (selectedTasks.empty()) {
        cout << "  No task fits into the available study time.\n";
    } else {
        for (const auto& t : selectedTasks) {
            cout << "  [SELECTED] " << left << setw(6) << t.id << setw(30) << t.name
                 << " (" << t.estimated_time << " hrs, importance " << t.importance_score << ")\n";
        }
    }
    if (!skippedTasks.empty()) {
        cout << "--------------------------------------------------------------------------\n";
        for (const auto& t : skippedTasks) {
            cout << "  [SKIPPED ] " << left << setw(6) << t.id << setw(30) << t.name
                 << " (needs " << t.estimated_time << " hrs, does not fit)\n";
        }
    }

    cout << "--------------------------------------------------------------------------\n";
    cout << "Total Study Time Used : " << result.totalTime << " / " << availableTime << " hrs\n";
    cout << "Unused Time Left      : " << remainingTime << " hrs\n";
    cout << "Total Importance Score: " << result.totalImportance << "\n";
    cout << fixed << setprecision(4);
    cout << "Execution Time        : " << result.executionTime << " ms\n";
    cout << fixed << setprecision(2);
    cout << "==========================================================================\n";

    return result;
}

// ============================================================
//  MODULE 4: Dynamic Programming - 0/1 Knapsack
// ============================================================


static const int DP_SCALE = 2; // 1 unit = 0.5 hours

static int getVal(const vector<map<int,int>>& dp, int row, int col) {
    auto it = dp[row].find(col);
    return (it != dp[row].end()) ? it->second : 0;
}

// ---- Print the DP table (only when capacity <= 20 units) ----
static void printDPTable(const vector<map<int,int>>& dp,
                         const vector<StudyTask>& tasks,
                         int capUnits) {
    int n = (int)tasks.size();

    cout << "\n  [DP Table - each cell = max importance achievable]\n";
    cout << "  Rows = tasks added one by one | Columns = time units (1 unit = 0.5 hr)\n\n";

    // Header row
    cout << "  " << left << setw(6) << "Task" << " |";
    for (int w = 0; w <= capUnits; w++)
        cout << setw(3) << w;
    cout << "\n  ------+-" << string(capUnits * 3 + 3, '-') << "\n";

    // Data rows
    for (int i = 0; i <= n; i++) {
        if (i == 0)
            cout << "  " << left << setw(6) << "(none)" << " |";
        else
            cout << "  " << left << setw(6) << tasks[i-1].id << " |";

        // Print every column - getVal returns 0 if that coordinate
        // was never stored in the map (meaning no improvement there)
        for (int w = 0; w <= capUnits; w++)
            cout << setw(3) << getVal(dp, i, w);
        cout << "\n";
    }
}

// ---- Main DP module function ----
AlgorithmResult runDPModule(const vector<StudyTask>& tasks, double availTime) {

    // Guard: need a loaded dataset
    if (tasks.empty()) {
        cout << "\n[!] No dataset loaded. Please load a scenario first (Option 1 or 2).\n";
        return {"Dynamic Programming", {}, 0.0, 0, 0.0};
    }

    cout << "\n========================================================\n";
    cout << "   MODULE 4: DYNAMIC PROGRAMMING - 0/1 KNAPSACK\n";
    cout << "========================================================\n";
    cout << fixed << setprecision(1);
    cout << "  Available Study Time : " << availTime << " hours\n";
    cout << "  Number of Tasks      : " << tasks.size() << "\n";

    int n        = (int)tasks.size();
    int capUnits = (int)(availTime * DP_SCALE); // convert hours -> integer units

    //declare the map-based
    vector<map<int,int>> dp(n + 1);

// Row 0 starts with one entry: 0 time used = 0 importance
    dp[0][0] = 0;

    //start clock
    auto startTime = high_resolution_clock::now();

    
for (int i = 1; i <= n; i++) {
    int wt  = (int)(tasks[i-1].estimated_time * DP_SCALE);
    int val = tasks[i-1].importance_score;

    // First carry forward everything from the row above
    // (same as "skip this task" option)
    dp[i] = dp[i-1];

    // Then try adding this task to each existing coordinate
    for (auto& [w, imp] : dp[i-1]) {
            int newW   = w + wt;       // time after adding this task
            int newImp = imp + val;    // importance after adding this task

            // Only proceed if the new time fits within the budget
            if (newW <= capUnits) {
                // Only store if this is better than what is already
                // recorded at this coordinate in the current row.
                // This handles the case where two different paths
                // reach the same time unit - we keep the best one.
                if (getVal(dp, i, newW) < newImp)
                    dp[i][newW] = newImp;
            }
        }
    }

    auto endTime = high_resolution_clock::now();
    duration<double, milli> precise_duration = endTime - startTime;
    double elapsedMs = precise_duration.count();

    // --------------------------------------------------------
    // Print DP table (only if small enough to be readable)
    // --------------------------------------------------------
if (capUnits <= 20) {
        printDPTable(dp, tasks, capUnits);
    } else {
        cout << "\n  [DP Table has " << (capUnits + 1)
             << " columns - too wide to display fully.]\n";
    }
//read final answer
int bestImp = 0;
    int bestW   = 0;
    for (auto& [w, imp] : dp[n]) {
        if (w <= capUnits && imp > bestImp) {
            bestImp = imp;
            bestW   = w;
        }
    }

    // --------------------------------------------------------
    // Traceback: determine which tasks were selected
    // --------------------------------------------------------
    vector<StudyTask> selected;
    int w = bestW;
    for (int i = n; i >= 1; i--) {
        if (getVal(dp, i, w) != getVal(dp, i-1, w)) {
            // Task i was included
            selected.push_back(tasks[i-1]);
            w -= (int)(tasks[i-1].estimated_time * DP_SCALE);
        }
        // If equal, task i was skipped - just move up a row
    }
    reverse(selected.begin(), selected.end()); // restore original order

    // --------------------------------------------------------
    // Calculate totals
    // --------------------------------------------------------
    double totalTime = 0.0;
    int    totalImp  = 0;
    vector<string> selectedIDs;
    for (const auto& t : selected) {
        totalTime += t.estimated_time;
        totalImp  += t.importance_score;
        selectedIDs.push_back(t.id);
    }

    // --------------------------------------------------------
    // STEP 10: Print results table
    // --------------------------------------------------------
    cout << "\n  -------------------------------------------------------\n";
    cout << "   SELECTED TASKS (Optimal Combination by DP)\n";
    cout << "  -------------------------------------------------------\n";
    cout << "  " << left << setw(5)  << "ID"
                         << setw(28) << "Task Name"
                         << setw(10) << "Time(hrs)"
                         << setw(12) << "Importance" << "\n";
    cout << "  " << string(55, '-') << "\n";

    if (selected.empty()) {
        cout << "  [No tasks fit within the available time budget.]\n";
    } else {
        for (const auto& t : selected) {
            cout << "  " << left << setw(5)  << t.id
                                 << setw(28) << t.name
                                 << setw(10) << t.estimated_time
                                 << setw(12) << t.importance_score << "\n";
        }
    }

    cout << "  " << string(55, '-') << "\n";
    cout << "  " << left << setw(33) << "TOTAL"
                         << setw(10) << totalTime
                         << setw(12) << totalImp  << "\n";
    cout << "  " << string(55, '-') << "\n";
    cout << "\n  Execution Time  : " << fixed << setprecision(4)
         << elapsedMs << " ms\n";
    cout << "  Time Used       : " << fixed << setprecision(1)
         << totalTime << " / " << availTime << " hours\n";
    cout << "  Time Remaining  : " << (availTime - totalTime) << " hours\n";
    cout << "\n  [DP guarantees the OPTIMAL selection - no other combination\n";
    cout << "   of these tasks gives higher importance within the time limit.]\n";
    cout << "========================================================\n";

   
    return {"Dynamic Programming", selectedIDs, totalTime, totalImp, elapsedMs};
}


/*
Feature Description
Number of tasks Total number of study tasks in the scenario
Total required study time Sum of all task durations
Available study time Total study time available to the student
Time pressure ratio Total required time divided by available time
Average importance Average importance score of all tasks
Deadline tightness How close the deadlines are
Importance variation Whether task importance scores are similar or very different
*/
// ============================================================
// MODULE 5: AI RECOMMENDATION MODULE
// ============================================================
AlgorithmResult runAIModule(const vector<StudyTask>& tasks, vector<AlgorithmResult>& moduleResults, double availableTime) {
    AlgorithmResult result;

    if (tasks.empty() || availableTime <= 0) {
        cout << "\nError: No dataset loaded or invalid available time.\n";
        result.strategy = "Not Run";
        return result;
    }

    auto start = high_resolution_clock::now();


    // Feature Extraction
    // These are the extracted features for the AI model to make a recommendation.
    int numTasks = tasks.size();
    double totalRequiredTime = 0.0;
    double totalImportanceScore = 0.0;
    double timePressureRatio;
    double averageImportance;
    double deadlineTightness; 
    int importanceVariation;

    int maxImportance = 0;
    int minImportance = 5; 
    int tightDeadlineCount = 0;

    for (const auto& t : tasks) {
        totalRequiredTime += t.estimated_time;
        totalImportanceScore += t.importance_score;
        
        maxImportance = max(maxImportance, t.importance_score);
        minImportance = min(minImportance, t.importance_score);
        
        if (t.deadline_days <= 3.0) tightDeadlineCount++;
    }

    timePressureRatio   = totalRequiredTime / availableTime;
    averageImportance   = totalImportanceScore / numTasks;
    deadlineTightness   = (double)tightDeadlineCount / numTasks; 
    importanceVariation = maxImportance - minImportance;

    
    // Rule-Based Decision Tree. It uses a simple heuristic
    string recommendedStrategy = "";

    // TODO: Adjust these thresholds based on the 8-15 examples in your report
    if (timePressureRatio > 1.2) {
        // High time pressure
        if (importanceVariation >= 2) {
            recommendedStrategy = "Dynamic Programming"; 
        } else {
            recommendedStrategy = "Greedy"; 
        }
    } else if (deadlineTightness > 0.4) {
        // Many urgent tasks, prioritization is the main goal
        recommendedStrategy = "Sorting-based ranking"; 
    } else {
        // Low time pressure, default simple plan
        recommendedStrategy = "Sorting-based ranking"; 
    }

    auto end = high_resolution_clock::now();
    duration<double, milli> elapsed = end - start;
    result.executionTime = elapsed.count();
 

    cout << fixed << setprecision(2);
    cout << " MODULE 5: AI RECOMMENDATION (Rule-Based Decision Tree)\n";
    cout << " Extracted Scenario Features:\n";
    cout << "  - Number of tasks       : " << numTasks << "\n";
    cout << "  - Required study time   : " << totalRequiredTime << " hrs\n";
    cout << "  - Available study time  : " << availableTime << " hrs\n";
    cout << "  - Time pressure ratio   : " << timePressureRatio << "\n";
    cout << "  - Average importance    : " << averageImportance << "\n";
    cout << "  - Deadline tightness    : " << deadlineTightness * 100 << "% of tasks\n";
    cout << "  - Importance variation  : " << importanceVariation << "\n";
    printBar();
    cout << " >>> Recommended Strategy: " << recommendedStrategy << " <<<\n";
    printBar();

    // Set the final result string to pass to Module 6
    result.strategy = "AI Prediction: " + recommendedStrategy;
    for (const auto& r : moduleResults) {
        if (r.strategy.find(recommendedStrategy) != string::npos) {
            result.selectedTaskIDs = r.selectedTaskIDs;
            result.totalTime = r.totalTime;
            result.totalImportance = r.totalImportance;
            break;
        }
}
    return result;
}


// ============================================================
// MODULE 6: PERFORMANCE MEASUREMENT AND COMPARISON
// ============================================================
void runComparisonModule(const vector<AlgorithmResult>& results, const string& scenario) {
    cout << fixed << setprecision(2);
    cout << " MODULE 6: PERFORMANCE COMPARISON SUMMARY\n";
    cout << " Active Scenario: " << scenario << "\n";
    
    // Print Table Header matching Table 7 requirement
    cout << left 
         << setw(38) << "Strategy" 
         << setw(15) << "Selected Tasks" 
         << setw(15) << "Total Time" 
         << setw(18) << "Total Importance" 
         << setw(18) << "Execution Time" 
         << "Comment\n";
    cout << "\n";

    for (const auto& res : results) {
        if (res.strategy == "Not Run") {
            cout << left << setw(38) << "[Module Missing]" 
                 << setw(15) << "N/A" 
                 << setw(15) << "N/A" 
                 << setw(18) << "N/A" 
                 << setw(18) << "N/A" 
                 << "Execute this module from menu first.\n";
            continue;
        }

        // Format the selected task count or sequence
        int taskCount = res.selectedTaskIDs.size();
        
        // Generate analytical commentary based on performance metrics
        string comment = "";
        if (res.strategy.find("Sorting") != string::npos) {
            comment = "Prioritizes all tasks.";
        } else if (res.strategy.find("Greedy") != string::npos) {
            comment = "Fast evaluation; approximate local optima resolution.";
        } else if (res.strategy.find("Dynamic Programming") != string::npos) {
            comment = "Guaranteed optimal value configuration.";
        } else if (res.strategy.find("AI Prediction") != string::npos) {
            comment = "Evaluated contextual scenario metadata.";
        }
        cout << fixed << setprecision(2);
        cout << left 
             << setw(38) << res.strategy 
             << setw(15) << taskCount
             << setw(15) << res.totalTime
             << setw(18) << res.totalImportance;
        
        cout << fixed << setprecision(4) 
             << setw(18) << res.executionTime 
             << comment << "\n";
    }
}


// ==========================================
// MAIN PROGRAM
// ==========================================
int main() {
    int choice = 0;
    
    printBar('=');
    cout << "         WELCOME TO THE STUDYSMART AI SYSTEM      \n";
    printBar('=');

    while (choice != 9) {
        printBar();
        cout << "                 MAIN APPLICATION MENU            \n";
        printBar();
        cout << "1. Load Built-In Scenarios (Generated Data)\n";
        cout << "2. Input Custom Scenario Manually (User Input)\n";
        cout << "3. View Loaded Task Data Table\n";
        cout << "4. Run Sorting using Quick Sort.\n";
        cout << "5. Run Greedy Planning Module\n";
        cout << "6. Run Dynamic Programming Module \n";
        cout << "7. AI Recommendation Module\n";
        cout << "8. Performance Comparison Module\n";
        cout << "9. Exit Application\n";
        cout << "--------------------------------------------------\n";
        
        choice = readInt("Select option (1-9): ", 1, 9);

        switch (choice) {
            case 1:
                loadBuiltInScenarios();
                moduleResults.assign(4, {"Not Run", {}, 0.0, 0, 0.0}); // reset results for new scenario
                displayDataset();
                break;
            case 2:
                handleUserInputDataset();
                moduleResults.assign(4, {"Not Run", {}, 0.0, 0, 0.0}); // reset results for new scenario
                displayDataset();
                break;
            case 3:
                displayDataset();
                break;
            case 4:
                cout << "\n[-->] Executing Module 2 (Sorting / Divide-and-Conquer)... \n";
                moduleResults[0] = runSortingModule(taskDataset, availableStudyTime);
                break;
            case 5:
                cout << "\n[-->] Executing Module 3 (Greedy Planning)... \n";
                moduleResults[1] = runGreedyModule(taskDataset, availableStudyTime);
                break;
            case 6:
                cout << "\n[-->] Executing Module 4 (Dynamic Programming / 0-1 Knapsack)... \n";
                moduleResults[2] = runDPModule(taskDataset, availableStudyTime);
                
                break;
            case 7:
                cout << "\n[-->] Executing Module 5 (AI Recommendation)...\n";
                moduleResults[3] = runAIModule(taskDataset, moduleResults, availableStudyTime);
                break;
            case 8:
                cout << "\n[-->] Executing Module 6 (Performance Comparison)...\n";
                // Member 5: implement runComparisonModule() which reads moduleResults[]
                // and prints the comparison table. Check for "Not Run" entries and skip them.
                runComparisonModule(moduleResults, activeScenario);
                cout << "    Note: Run options 4, 5, 6, and 7 before running this.\n"; 
                break;
            case 9:
                cout << "\nExiting StudySmart AI Application safely. \n";
                break;
            default:
                cout << "\n[!] Invalid input choice. Please select a valid option from the menu.\n";
                break;
        }
    }
    return 0;
}