#include "FibonacciHeap.h"
#include <iostream>
#include <string>
#include <limits>

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void displayMenu() {
    cout << "\n=== TaskTrack - Task Management System ===\n";
    cout << "1. Add New Task\n";
    cout << "2. View Next Task\n";
    cout << "3. Mark Next Task as Done\n";
    cout << "4. View All Tasks\n";
    cout << "5. Update Task Priority\n";
    cout << "6. Exit\n";
    cout << "Choose an option (1-6): ";

}

string getValidDate() {
    while (true) {
        string date;
        cout << "Enter deadline (MM/DD/YYYY): ";
        getline(cin, date);

        // Basic date format validation
        if (date.length() == 10 &&
            date[2] == '/' &&
            date[5] == '/' &&
            isdigit(date[0]) && isdigit(date[1]) &&
            isdigit(date[3]) && isdigit(date[4]) &&
            isdigit(date[6]) && isdigit(date[7]) &&
            isdigit(date[8]) && isdigit(date[9])) {
            return date;
        }
        cout << "Invalid date format. Please use MM/DD/YYYY format.\n";
    }
}

int main() {
    FibonacciHeap<string, int> taskHeap;

    while (true) {
        system("cls"); // Clear screen (use "clear" for Unix-based systems)


        displayMenu();

        int choice;
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
        case 1: { // Add New Task
            cout << "Enter task name: ";
            string taskName;
            getline(cin, taskName);

            string deadline = getValidDate();

            taskHeap.insert(taskName, deadline);
            cout << "Task added successfully!\n";
            break;
        }

        case 2: { // View Next Task
            if (taskHeap.isEmpty()) {
                cout << "No tasks available.\n";
            }
            else {
                auto nextTask = taskHeap.peekNextTask();
                cout << "\nNext task: " << nextTask.first << "\n";
                cout << "Deadline: " << nextTask.second << "\n";
            }
            break;
        }

        case 3: { // Mark Next Task as Done
            if (taskHeap.isEmpty()) {
                cout << "No tasks available.\n";
            }
            else {
                taskHeap.extractMin();
                cout << "Task marked as done!\n";
            }
            break;
        }

        case 4: { // View All Tasks
            if (taskHeap.isEmpty()) {
                cout << "No tasks available.\n";
            }
            else {
                cout << "\nAll Tasks:\n";
                cout << "==========\n";
                taskHeap.display();
            }
            break;
        }

        case 5: { // Update Task Priority
            if (taskHeap.isEmpty()) {
                cout << "No tasks available.\n";
                break;
            }

            cout << "Enter task name to update: ";
            string taskName;
            getline(cin, taskName);

            cout << "Enter new deadline (earlier date = higher priority): ";
            string newDeadline = getValidDate();

            // Calculate new priority based on deadline
            tm timeinfo = {};
            istringstream ss(newDeadline);
            ss >> get_time(&timeinfo, "%m/%d/%Y");
            auto now = chrono::system_clock::now();
            auto currentTime = chrono::system_clock::to_time_t(now);
            time_t deadlineTime = mktime(&timeinfo);
            int newPriority = static_cast<int>(-difftime(deadlineTime, currentTime) / (60 * 60 * 24));

            taskHeap.decreaseKey(taskName, newPriority);

            break;
        }

        case 6: { // Exit
            cout << "Thank you for using TaskTrack!\n";
            return 0;
        }

        default:
            cout << "Invalid option. Please try again.\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }

    return 0;
}