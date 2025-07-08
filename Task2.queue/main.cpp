#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

// Structure to hold print job details
struct PrintJob {
    string pcName;
    int arrivalTime; // Time in minutes since midnight
    int duration;    // Duration in minutes
    int execTime;    // Execution start time (to be calculated)
    int waitTime;    // Waiting time (to be calculated)
};

// Node for linked list
struct Node {
    PrintJob job;
    Node* next;
    Node(PrintJob j) {
        job=j;
        next=nullptr;
    }
};

// Linked List class
class LinkedList {
private:
    Node* head;
    Node* tail;

public:
    LinkedList() {
        head=nullptr;
        tail=nullptr;
    }

    // Insert at the end
    void insert(PrintJob job) {
        Node* newNode = new Node(job);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    // Remove from front
    bool remove(PrintJob& job) {
        if (!head) return false;
        Node* temp = head;
        job = head->job;
        head = head->next;
        if (!head) tail = nullptr;
        delete temp;
        return true;
    }

    // Check if empty
    bool isEmpty() {
        return head == nullptr;
    }

    // Destructor
    ~LinkedList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// Queue class
class Queue {
private:
    LinkedList list;

public:
    void enqueue(PrintJob job) {
        list.insert(job);
    }

    bool dequeue(PrintJob& job) {
        return list.remove(job);
    }

    bool isEmpty() {
        return list.isEmpty();
    }
};

// Convert hh:mm to minutes since midnight
int timeToMinutes(string time) {
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}

// Convert minutes to hh:mm format
string minutesToTime(int minutes) {
    int hours = minutes / 60;
    int mins = minutes % 60;
    return (hours < 10 ? "0" : "") + to_string(hours) + ":" + (mins < 10 ? "0" : "") + to_string(mins);
}

int main() {
    ifstream file("printer.txt");
    if (!file) {
        cout << "Error opening printer.txt" << endl;
        return 1;
    }

    int n;
    file >> n; // Number of jobs
    vector<PrintJob> jobs(n);

    // Read jobs
    for (int i = 0; i < n; i++) {
        string pc, time;
        int duration;
        file >> pc >> time >> duration;
        jobs[i] = {pc, timeToMinutes(time), duration, 0, 0};
    }
    file.close();

    // Sort jobs by arrival time, then by duration
    sort(jobs.begin(), jobs.end(), [](const PrintJob& a, const PrintJob& b) {
        if (a.arrivalTime == b.arrivalTime) return a.duration < b.duration;
        return a.arrivalTime < b.arrivalTime;
    });

    // Enqueue jobs
    Queue printerQueue;
    for (const auto& job : jobs) {
        printerQueue.enqueue(job);
    }

    // Process jobs
    vector<PrintJob> executedJobs;
    int currentTime = 0; // Current time for printer
    int totalOccupied = 0; // Total printer occupied time
    int totalWaitTime = 0; // Total waiting time for average

    while (!printerQueue.isEmpty()) {
        PrintJob job;
        printerQueue.dequeue(job);

        // Execution time is max of arrival time and current time
        job.execTime = max(job.arrivalTime, currentTime);
        job.waitTime = job.execTime - job.arrivalTime;

        // Update times
        currentTime = job.execTime + job.duration;
        totalOccupied += job.duration;
        totalWaitTime += job.waitTime;

        executedJobs.push_back(job);
    }

    // Output results
    cout << "Jobs Executed: " << executedJobs.size() << endl;
    for (const auto& job : executedJobs) {
        cout << job.pcName << " " << minutesToTime(job.arrivalTime) << " " << job.duration
             << " executed at " << minutesToTime(job.execTime) << " waiting time: " << job.waitTime << endl;
    }
    cout << "The printer was occupied for " << totalOccupied << " minutes" << endl;
    cout << fixed << setprecision(2);
    cout << "Average waiting time for all jobs is " << (double)totalWaitTime / executedJobs.size() << " minutes" << endl;

    return 0;
}
