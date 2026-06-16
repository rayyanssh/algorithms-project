#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>       
#include <climits>     
#include <algorithm>   
#include <limits>
#include <map>
using namespace std;


// Study Task Data Structure
struct StudyTask {
    string id;
    string name;
    double estimated_time;   // in hours
    int importance_score;  // Scale 1 - 5
    double deadline_days;    // in days
    int difficulty_level;  // Scale 1 - 5
    string task_type; 
};

// Algorithm Result Structure
// Every module (Sorting, Greedy, DP, AI/ML) must fill and return one of these.
// Module 6 reads the global moduleResults vector to build the comparison table.
struct AlgorithmResult {
    string strategyName;          // e.g. "Greedy Strategy"
    vector<string> selectedTaskIDs; // IDs of tasks selected/ranked, e.g. {"T1","T3"}
    double totalTime;             // sum of estimated_time of selected tasks
    int totalImportance;          // sum of importance_score of selected tasks
    double executionTimeMs;       // measured using clock(), in milliseconds
};

// Global variables 
vector<StudyTask> taskDataset;
double availableStudyTime = 0.0; 
string activeScenarioName = "None Loaded";

// Stores results from each module run; used by Module 6 for comparison.
// Index: 0 = Sorting, 1 = Greedy, 2 = DP, 3 = AI/ML
// Initialised with empty placeholders so Module 6 can detect unrun modules.
vector<AlgorithmResult> moduleResults(4, {"Not Run", {}, 0.0, 0, 0.0});


// Input helper functions for integers 
int readInt(const string& prompt, int low, int high) {
    int val;
    while (true) {
        if (!prompt.empty()) cout << prompt;
        if (cin >> val && val >= low && val <= high) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "[!] Enter a whole number between " << low << " and " << high << ".\n";
    }
}

// Input helper functions for doubles 
double readDouble(const string& prompt, double low, double high) {
    double val;
    while (true) {
        if (!prompt.empty()) cout << prompt;
        if (cin >> val && val >= low && val <= high) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "[!] Enter a number between " << low << " and " << high << ".\n";
    }
}


// Data Printing
void displayDataset() {
    if (taskDataset.empty()) {
        cout << "\n[!] No dataset loaded. Please choose Option 1 or 2 first.\n";
        return;
    }

    cout << "\n========================================================================================\n";
    cout << " CURRENT SCENARIO: " << activeScenarioName << " | Available Study Time: " << availableStudyTime << " hours\n";
    cout << "========================================================================================\n";
    
    // to display decimals cleanly like: 2.5 and 3.0
    cout << fixed << setprecision(1); 
    
    cout << left 
         << setw(6)  << "ID" 
         << setw(28) << "Task Name" 
         << setw(12) << "Time (hrs)" 
         << setw(12) << "Importance" 
         << setw(10) << "Deadline" 
         << setw(12) << "Difficulty" 
         << setw(12) << "Type" << "\n";
    cout << "----------------------------------------------------------------------------------------\n";

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
    cout << "----------------------------------------------------------------------------------------\n";
    cout << "Total Tasks: " << taskDataset.size() 
         << " | Total Required Time: " << totalRequiredTime << " hours\n";
    cout << "========================================================================================\n";
}


// Built-in Scenario Generation
void loadBuiltInScenarios() {
    taskDataset.clear();
    
    cout << "\nSelect a scenario:\n";
    cout << "1. Low-Pressure Scenario (Time budget is high)\n";
    cout << "2. Deadline-Focused Scenario (Several tasks are urgent)\n";
    cout << "3. High-Pressure Scenario (Time budget is extremely low)\n";
    
    int choice = readInt("Enter choice (1-3): ", 1, 3);

    vector<StudyTask> baseTasks = {
        {"T1", "Revise Dynamic Programming", 3.5, 5, 7.0, 4, "Revision"},
        {"T2", "Complete Java Tutorial",     2.0, 4, 5.0, 3, "Tutorial"},
        {"T3", "Practise Greedy Problems",   2.0, 3, 6.0, 2, "Practice"},
        {"T4", "Do NLP Assignment",          4.0, 5, 8.0, 4, "Assignment"},
        {"T5", "Study CNN Lecture",          2.5, 2, 9.0, 1, "Lecture"}, 
        {"T6", "Practice OS Lab Exercise3", 2.0, 3, 4.0, 3, "Practice"},
        {"T7", "Complete OS Lab Report",     2.0, 4, 10.0, 3, "Assignment"},
        {"T8", "Study Dynamic Programming",  1.0, 1, 12.0, 1, "Lecture"},
        {"T9", "Deep Learning Quiz Review",  1.0, 3, 5.0, 2, "Revision"},
        {"T10","Review Computer Vision Notes",1.5, 4, 14.0, 3, "Revision"}
    };

    switch (choice) {
        case 1:
            taskDataset = baseTasks;
            availableStudyTime = 30.0; 
            activeScenarioName = "Low-Pressure Scenario";
            cout << "\n[+] Low-pressure scenario loaded successfully.\n";
            break;
        case 2:
            taskDataset = baseTasks;
            taskDataset[0].deadline_days = 1.5; 
            taskDataset[3].deadline_days = 2.0; 
            taskDataset[5].deadline_days = 1.0; 
            availableStudyTime = 15.0; 
            activeScenarioName = "Deadline-Focused Scenario";
            cout << "\n[+] Deadline-focused scenario loaded Successfully.\n";
            break;
        case 3:
            taskDataset = baseTasks;
            availableStudyTime = 8.0;  
            activeScenarioName = "High-Pressure Scenario";
            cout << "\n[+] High-pressure scenario loaded Successfully.\n";
            break;
        default:
            cout << "\nInvalid choice! Returning to main menu.\n";
            break;
    }
}


// Manual User Input 
void handleUserInputDataset() {
    taskDataset.clear();
    activeScenarioName = "Custom User-Defined Scenario";

    cout << "\n====================================\n";
    cout << "     MANUAL DATASET INPUT MODE\n";
    cout << "====================================\n";
    
    availableStudyTime = readDouble("Enter total Available Study Time for this scenario (in hours): ", 0.5, 999.0);

    int numTasks = readInt("Enter the number of tasks you have, (must be between 8 and 15): ", 8, 15);

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
        getline(cin, task.task_type);

        taskDataset.push_back(task);
    }
    cout << "\n[+] Custom dataset of " << numTasks << " tasks successfully loaded.\n";
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

    cout << "\n  [DP Table — each cell = max importance achievable]\n";
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

        // Print every column — getVal returns 0 if that coordinate
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
    cout << "   MODULE 4: DYNAMIC PROGRAMMING — 0/1 KNAPSACK\n";
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
   clock_t startTime = clock();

    
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
                // reach the same time unit — we keep the best one.
                if (getVal(dp, i, newW) < newImp)
                    dp[i][newW] = newImp;
            }
        }
    }

    clock_t endTime = clock();
    double elapsedMs = (double)(endTime - startTime) / CLOCKS_PER_SEC * 1000.0;

    // --------------------------------------------------------
    // Print DP table (only if small enough to be readable)
    // --------------------------------------------------------
if (capUnits <= 20) {
        printDPTable(dp, tasks, capUnits);
    } else {
        cout << "\n  [DP Table has " << (capUnits + 1)
             << " columns — too wide to display fully.]\n";
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
    ector<StudyTask> selected;
    int w = bestW;
    for (int i = n; i >= 1; i--) {
        if (getVal(dp, i, w) != getVal(dp, i-1, w)) {
            // Task i was included
            selected.push_back(tasks[i-1]);
            w -= (int)(tasks[i-1].estimated_time * DP_SCALE);
        }
        // If equal, task i was skipped — just move up a row
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
    cout << "\n  [DP guarantees the OPTIMAL selection — no other combination\n";
    cout << "   of these tasks gives higher importance within the time limit.]\n";
    cout << "========================================================\n";

   
    return {"Dynamic Programming", selectedIDs, totalTime, totalImp, elapsedMs};
}



// ==========================================
// MAIN PROGRAM
// ==========================================
int main() {
    int choice = 0;
    
    cout << "\n==================================================\n";
    cout << "         WELCOME TO THE STUDYSMART AI SYSTEM      \n";
    cout << "==================================================\n";

    while (choice != 9) {
        cout << "\n==================================================\n";
        cout << "                 MAIN APPLICATION MENU            \n";
        cout << "==================================================\n";
        cout << "1. Load Built-In Scenarios (Generated Data)\n";
        cout << "2. Input Custom Scenario Manually (User Input)\n";
        cout << "3. View Loaded Task Data Table\n";
        cout << "4. Run Sorting using Divide-and-Conquer\n"; // the menu choices' description will be changed after all is done
        cout << "5. Run Greedy Planning Module\n";
        cout << "6. Run Dynamic Programming Module \n";
        cout << "7. AI/ML Recommendation Module\n";
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
                // Member 2: implement runSortingModule() and return an AlgorithmResult.
                // Uncomment the line below once your function is ready:
                // moduleResults[0] = runSortingModule(taskDataset, availableStudyTime);
                cout << "[!] Sorting module not yet connected. Member 2: implement runSortingModule().\n";//delete this once done
                break;
            case 5:
                cout << "\n[-->] Executing Module 3 (Greedy Planning)... \n";
                // Member 3: implement runGreedyModule() and return an AlgorithmResult.
                // Uncomment the line below once your function is ready:
                // moduleResults[1] = runGreedyModule(taskDataset, availableStudyTime);
                cout << "[!] Greedy module not yet connected. Member 3: implement runGreedyModule().\n";//delete this once done
                break;
            case 6:
                cout << "\n[-->] Executing Module 4 (Dynamic Programming / 0-1 Knapsack)... \n";
                // Member 4: implement runDPModule() and return an AlgorithmResult.
                // Uncomment the line below once your function is ready:
                moduleResults[2] = runDPModule(taskDataset, availableStudyTime);
                
                break;
            case 7:
                cout << "\n[-->] Executing Module 5 (AI/ML Recommendation)...\n";
                // Member 5: implement runAIModule() and return an AlgorithmResult.
                // Features to extract from taskDataset and availableStudyTime:
                //   - taskDataset.size()                    -> number of tasks
                //   - sum of estimated_time                 -> total required time
                //   - availableStudyTime                    -> available time
                //   - totalRequired / availableStudyTime    -> time pressure ratio
                //   - sum of importance_score / n           -> average importance
                //   - count of tasks where deadline_days <= 3 -> deadline tightness
                //   - max importance - min importance       -> importance variation
                // Uncomment the line below once your function is ready:
                // moduleResults[3] = runAIModule(taskDataset, availableStudyTime);
                cout << "[!] AI/ML module not yet connected. Member 5: implement runAIModule().\n";//delete this once done
                break;
            case 8:
                cout << "\n[-->] Executing Module 6 (Performance Comparison)...\n";
                // Member 5: implement runComparisonModule() which reads moduleResults[]
                // and prints the comparison table. Check for "Not Run" entries and skip them.
                // Uncomment the line below once your function is ready:
                // runComparisonModule(moduleResults, activeScenarioName);
                cout << "[!] Comparison module not yet connected. Member 5: implement runComparisonModule().\n";//delete this once done
                //you may delete the line below or keep it depending on your implementation. you may check the condition in your runComparisonModule().
                cout << "    Note: Run options 4, 5, 6, and 7 before running this.\n"; 
                break;
            case 9:
                cout << "\nExiting StudySmart AI Application safely. Good luck with submission!\n";
                break;
            default:
                cout << "\n[!] Invalid input choice. Please select a valid option from the menu.\n";
                break;
        }
    }
    return 0;
}
