#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H
#include "MyVector.h"

#include <iostream>
#include <string>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace std;

// Task class to represent each task with name and priority
class Task {
private:
    string name;  // Name of the task
    int priority; // Priority of the task (higher number indicates higher priority)
    string deadline;

public:
    // Constructor
    Task(const string& name, const string& deadline)
        : name(name), deadline(deadline) {
        priority = calculatePriority(deadline);
    }



    // Getter and Setter for task name
    string getName() const { return name; }
    void setName(const string& newName) { name = newName; }

    // Getter and Setter for priority
    int getPriority() const { return priority; }
    void setPriority(int newPriority) { priority = newPriority; }

    string getDeadline() const { return deadline; }

    // Display task details
    void display() const {
        cout << "Task: " << name << ", Priority: " << priority << endl;
    }


private:
    int calculatePriority(const string& deadlineStr) {
        tm timeinfo = {};
        istringstream ss(deadlineStr);
        ss >> get_time(&timeinfo, "%m/%d/%Y");

        auto now = chrono::system_clock::now();
        auto currentTime = chrono::system_clock::to_time_t(now);
        time_t deadlineTime = mktime(&timeinfo);

        double daysUntil = difftime(currentTime, deadlineTime) / (60 * 60 * 24);
        // Return positive value for future dates, negative for past dates
        return static_cast<int>(daysUntil);
    }

};

template <typename T, typename P>
// Node structure for Fibonacci Heap
struct Node {
    T name;           // Name of the task
    P priority;       // Priority of the task
    string deadline;
    Node<T, P>* parent;     // Parent pointer
    Node<T, P>* child;      // Child pointer
    Node<T, P>* left;       // Pointer to the node on the left
    Node<T, P>* right;      // Pointer to the node on the right
    int degree;       // Degree of the node
    bool mark;        // Black or white mark of the node

    // Node constructor
    Node(T name, string deadlineStr)
        : name(name), deadline(deadlineStr), parent(nullptr), child(nullptr),
        left(nullptr), right(nullptr), degree(0), mark(false) {
        priority = calculatePriority(deadlineStr);
    }

    // Copy constructor for Node
    Node(const Node<T, P>& other)
        : name(other.name), deadline(other.deadline), priority(other.priority),
        parent(nullptr), child(nullptr), left(nullptr), right(nullptr), degree(other.degree), mark(other.mark) {
        // Copy child nodes recursively
        if (other.child) {
            child = new Node<T, P>(*other.child);  // Recursive copy of child nodes
        }
    }
private:
    P calculatePriority(const string& deadlineStr) {
        tm timeinfo = {};
        istringstream ss(deadlineStr);
        ss >> get_time(&timeinfo, "%m/%d/%Y");

        auto now = chrono::system_clock::now();
        auto currentTime = chrono::system_clock::to_time_t(now);
        time_t deadlineTime = mktime(&timeinfo);

        double daysUntil = difftime(deadlineTime, currentTime) / (60 * 60 * 24);
        // Return positive value for future dates, negative for past dates
        return static_cast<P>(daysUntil);
    }
};

template <typename T, typename P>
// Fibonacci Heap Class
class FibonacciHeap {
private:
    Node<T, P>* mini;            // Pointer to the minimum node
    int no_of_nodes;             // Number of nodes in the heap

    void addToRootList(Node<T, P>* node);
    void removeFromRootList(Node<T, P>* node);
    void addToChildList(Node<T, P>* parent, Node<T, P>* child);
    void removeFromChildList(Node<T, P>* parent, Node<T, P>* node);
    void cascadingCut(Node<T, P>* node);
    Node<T, P>* findNode(Node<T, P>* node, const T& name);
    void displayChildren(Node<T, P>* child) const;
    void consolidate();

public:
    // Constructor
    FibonacciHeap() : mini(nullptr), no_of_nodes(0) {}

    // Copy constructor for FibonacciHeap
    FibonacciHeap(const FibonacciHeap<T, P>& other)
        : mini(nullptr), no_of_nodes(0) {
        // Copy the root nodes
        Node<T, P>* current = other.mini;
        if (current) {
            do {
                insert(current->name, current->deadline);  // Insert nodes in the new heap
                current = current->right;
            } while (current != other.mini);
        }
    }


    void insert(const T& name, const string& deadline);
    void getMin() const;
    void extractMin();
    void decreaseKey(const T& name, P newPriority);
    void cut(Node<T, P>* node, Node<T, P>* parent);
    void swap(Node<T, P>* x, Node<T, P>* z);
    pair<string, string> peekNextTask() const;
    bool isEmpty() const { return mini == nullptr; }
    void display() const;
};

template <typename T, typename P>
void FibonacciHeap<T, P>::addToRootList(Node<T, P>* node) {
    if (!mini) {
        mini = node;
        node->left = node->right = node;
    }
    else {
        node->left = mini;
        node->right = mini->right;
        mini->right->left = node;
        mini->right = node;
        if (node->priority < mini->priority) mini = node;
    }
}

template <typename T, typename P>
void FibonacciHeap<T, P>::removeFromRootList(Node<T, P>* node) {
    if (node->right == node) {
        mini = nullptr;
    }
    else {
        node->left->right = node->right;
        node->right->left = node->left;
        if (mini == node) mini = node->right;
    }
    node->left = node->right = nullptr;
}

template <typename T, typename P>
void FibonacciHeap<T, P>::addToChildList(Node<T, P>* parent, Node<T, P>* child) {
    child->left = child->right = child;
    child->parent = parent;

    if (!parent->child) {
        parent->child = child;
    }
    else {
        child->left = parent->child;
        child->right = parent->child->right;
        parent->child->right->left = child;
        parent->child->right = child;
    }

    parent->degree++;
}

template <typename T, typename P>
void FibonacciHeap<T, P>::removeFromChildList(Node<T, P>* parent, Node<T, P>* node) {
    if (parent->child == node) {
        if (node->right == node) {
            parent->child = nullptr;
        }
        else {
            parent->child = node->right;
        }
    }

    node->left->right = node->right;
    node->right->left = node->left;
    node->left = node->right = nullptr;
}

template <typename T, typename P>
Node<T, P>* FibonacciHeap<T, P>::findNode(Node<T, P>* startNode, const T& name) {
    if (!startNode) return nullptr;

    Node<T, P>* current = startNode;
    do {
        if (current->name == name) {
            return current;
        }

        if (current->child) {
            Node<T, P>* childResult = findNode(current->child, name);
            if (childResult) return childResult;
        }

        current = current->right;
    } while (current != startNode);

    return nullptr;
}

template <typename T, typename P>
void FibonacciHeap<T, P>::insert(const T& name, const string& deadline) {
    Node<T, P>* newNode = new Node<T, P>(name, deadline);
    addToRootList(newNode);
    ++no_of_nodes;
}

template <typename T, typename P>
void FibonacciHeap<T, P>::getMin() const {
    if (!mini) {
        cout << "The heap is empty!" << endl;
    }
    else {
        cout << "The minimum task is: " << mini->name << endl;
    }
}

template <typename T, typename P>
pair<string, string> FibonacciHeap<T, P>::peekNextTask() const {
    if (!mini) {
        return make_pair("No tasks", "");
    }
    return make_pair(mini->name, mini->deadline);
}

template <typename T, typename P>
void FibonacciHeap<T, P>::consolidate() {
    int maxDegree = static_cast<int>(ceil(log2(no_of_nodes))) + 1;
    MyVector<Node<T, P>*> degreeTable(maxDegree, nullptr); // Use MyVector instead of std::vector

    MyVector<Node<T, P>*> rootList; // Use MyVector for rootList
    Node<T, P>* current = mini;

    if (!current) return;

    do {
        rootList.push_back(current);  // Using push_back to add elements
        current = current->right;
    } while (current != mini);

    for (Node<T, P>* node : rootList) {
        Node<T, P>* x = node;
        int d = x->degree;

        while (degreeTable[d]) {
            Node<T, P>* y = degreeTable[d];
            degreeTable[d] = nullptr;

            if (y->priority < x->priority) swap(x, y);

            addToChildList(x, y);
            d++;
        }

        degreeTable[d] = x;
    } 
   
    // Rebuild the root list and find the new minimum node
    mini = nullptr;

    for (Node<T, P>* node : degreeTable) {
        if (node) {
            if (!mini) {
                // First node in the new root list
                node->left = node->right = node;
                mini = node;
            }
            else {
                // Add node to the root list
                node->left = mini;
                node->right = mini->right;
                mini->right->left = node;
                mini->right = node;

                // Update the minimum node
                if (node->priority < mini->priority) {
                    mini = node;
                }
            }
        }
    }
}



template <typename T, typename P>
void FibonacciHeap<T, P>::extractMin() {
    if (!mini) {
        cout << "The heap is empty!" << endl;
        return;
    }

    Node<T, P>* temp = mini;

    // If minimum node has children, add them to the root list
    if (temp->child) {
        Node<T, P>* x = temp->child;
        do {
            Node<T, P>* nextChild = x->right;
            x->right = mini;
            x->left = mini->left;
            mini->left->right = x;
            mini->left = x;
            x->parent = nullptr;
            x = nextChild;
        } while (x != temp->child);
        temp->child = nullptr;
    }

    // Remove minimum node from root list
    temp->left->right = temp->right;
    temp->right->left = temp->left;

    if (temp == temp->right && !temp->child) {
        mini = nullptr;
    }
    else {
        mini = temp->right;
        consolidate();
    }

    --no_of_nodes;
    cout << "The extracted minimum task is: " << temp->name << endl;
    delete temp;
}

template <typename T, typename P>
void FibonacciHeap<T, P>::cut(Node<T, P>* node, Node<T, P>* parent) {
    if (!node || !parent) {
        throw std::invalid_argument("Node or parent is null.");
    }
    removeFromChildList(parent, node);
    addToRootList(node);
    parent->degree--;
    node->parent = nullptr;
    node->mark = false;

    if (parent->mark && parent->parent) {
        cascadingCut(parent);
    }
    else if (parent->parent) {
        parent->mark = true;
    }
}

template <typename T, typename P>
void FibonacciHeap<T, P>::cascadingCut(Node<T, P>* node) {
    Node<T, P>* parent = node->parent;
    if (parent) {
        if (!node->mark) {
            node->mark = true;
        }
        else {
            cut(node, parent);
            cascadingCut(parent);
        }
    }
}

template <typename T, typename P>
void FibonacciHeap<T, P>::decreaseKey(const T& taskName, P newPriority) {
    if (!mini) {
        cout << "Heap is empty!" << endl;
        return;
    }

    Node<T, P>* node = findNode(mini, taskName);

    if (node == nullptr) {
        cout << "Task '" << taskName << "' not found in the heap!" << endl;
        return;
    }

    if (newPriority >= node->priority) {
        cout << "New priority (" << newPriority << ") must be less than current priority ("
            << node->priority << ")!" << endl;
        return;
    }

    node->priority = newPriority;

    if (node->parent && node->priority < node->parent->priority) {
        cut(node, node->parent);
    }

    if (node->priority < mini->priority) {
        mini = node;
    }
}

template <typename T, typename P>
void FibonacciHeap<T, P>::swap(Node<T, P>* x, Node<T, P>* z) {
    T tempName = x->name;
    P tempPriority = x->priority;
    string tempDeadline = x->deadline;

    x->name = z->name;
    x->priority = z->priority;
    x->deadline = z->deadline;

    z->name = tempName;
    z->priority = tempPriority;
    z->deadline = tempDeadline;
}

template <typename T, typename P>
void FibonacciHeap<T, P>::display() const {
    if (!mini) {
        cout << "Heap is empty!" << endl;
        return;
    }
    displayChildren(mini);
}

template <typename T, typename P>
void FibonacciHeap<T, P>::displayChildren(Node<T, P>* child) const {
    if (!child) return;

    Node<T, P>* current = child;
    do {
        cout << "Task: " << current->name
            << ", Priority: " << current->priority << endl;
          
            
        displayChildren(current->child);
        current = current->right;
    } while (current != child);
}

#endif