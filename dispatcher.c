#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MEMORY_SIZE 1024       // Total memory available (1024 MB)
#define REAL_TIME_MEMORY 64    // Memory reserved for real-time processes (64 MB)
#define TIME_QUANTUM 1         // Time quantum for user processes (1 second)

// Process structure
typedef struct {
    int arrival_time;
    int priority;
    int processor_time;
    int memory;
    int printers;
    int scanners;
    int modems;
    int CDs;
} Process;

// Node structure for queues
typedef struct Node {
    Process process;
    struct Node* next;
} Node;

// Global queues for real-time and user processes
Node* realTimeQueue = NULL;
Node* userQueue1 = NULL;
Node* userQueue2 = NULL;
Node* userQueue3 = NULL;

// Global resource counters
int availablePrinters = 2;
int availableScanners = 1;
int availableModems = 1;
int availableCDs = 2;

// Memory management
int memory[MEMORY_SIZE] = {0}; // 0 = free, 1 = allocated

// Function to add a process to the appropriate queue
void addProcessToQueue(Process p) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->process = p;
    newNode->next = NULL;

    if (p.priority == 0) {
        // Add to Real-Time queue
        if (realTimeQueue == NULL) {
            realTimeQueue = newNode;
        } else {
            Node* temp = realTimeQueue;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    } else if (p.priority == 1) {
        // Add to User Queue 1
        if (userQueue1 == NULL) {
            userQueue1 = newNode;
        } else {
            Node* temp = userQueue1;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    } else if (p.priority == 2) {
        // Add to User Queue 2
        if (userQueue2 == NULL) {
            userQueue2 = newNode;
        } else {
            Node* temp = userQueue2;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    } else if (p.priority == 3) {
        // Add to User Queue 3
        if (userQueue3 == NULL) {
            userQueue3 = newNode;
        } else {
            Node* temp = userQueue3;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }
}

// Function to allocate memory using Best Fit algorithm
bool allocateMemory(Process p) {
    int bestFitStart = -1;
    int bestFitSize = MEMORY_SIZE + 1;

    for (int i = 0; i <= MEMORY_SIZE - p.memory; i++) {
        bool isFree = true;
        for (int j = i; j < i + p.memory; j++) {
            if (memory[j] != 0) {
                isFree = false;
                break;
            }
        }
        if (isFree && (i + p.memory <= MEMORY_SIZE - REAL_TIME_MEMORY)) {
            int blockSize = p.memory;
            if (blockSize < bestFitSize) {
                bestFitSize = blockSize;
                bestFitStart = i;
            }
        }
    }

    if (bestFitStart != -1) {
        for (int i = bestFitStart; i < bestFitStart + p.memory; i++) {
            memory[i] = 1; // Mark memory as allocated
        }
        return true;
    }
    return false; // Memory allocation failed
}

// Function to release memory
void releaseMemory(Process p) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i] == 1) {
            memory[i] = 0; // Mark memory as free
        }
    }
}

// Function to check resource availability
bool checkResources(Process p) {
    if (p.printers > availablePrinters || p.scanners > availableScanners ||
        p.modems > availableModems || p.CDs > availableCDs) {
        return false; // Resources not available
    }
    return true;
}

// Function to allocate resources
void allocateResources(Process p) {
    availablePrinters -= p.printers;
    availableScanners -= p.scanners;
    availableModems -= p.modems;
    availableCDs -= p.CDs;
}

// Function to release resources
void releaseResources(Process p) {
    availablePrinters += p.printers;
    availableScanners += p.scanners;
    availableModems += p.modems;
    availableCDs += p.CDs;
}

// Function to select the next process to run
Process selectNextProcess() {
    Process p = {-1, -1, -1, -1, -1, -1, -1, -1}; // Default invalid process

    // Check Real-Time queue first
    if (realTimeQueue != NULL) {
        p = realTimeQueue->process;
        realTimeQueue = realTimeQueue->next;
        return p;
    }

    // Check User Queue 1
    if (userQueue1 != NULL) {
        p = userQueue1->process;
        userQueue1 = userQueue1->next;
        return p;
    }

    // Check User Queue 2
    if (userQueue2 != NULL) {
        p = userQueue2->process;
        userQueue2 = userQueue2->next;
        return p;
    }

    // Check User Queue 3
    if (userQueue3 != NULL) {
        p = userQueue3->process;
        userQueue3 = userQueue3->next;
        return p;
    }

    return p; // No processes available
}

// Function to execute a process
void executeProcess(Process p) {
    printf("Executing Process: Arrival Time = %d, Priority = %d, CPU Time = %d, Memory = %d MB\n",
           p.arrival_time, p.priority, p.processor_time, p.memory);

    if (p.priority == 0) {
        // Real-Time process: run to completion
        printf("Real-Time Process: Running to completion.\n");
    } else {
        // User process: run for time quantum
        p.processor_time -= TIME_QUANTUM;
        if (p.processor_time > 0) {
            // Move to lower priority queue
            p.priority++;
            addProcessToQueue(p);
            printf("User Process: Moved to lower priority queue.\n");
        } else {
            printf("User Process: Completed.\n");
        }
    }
}

// Main function
int main() {
    char filename[100];
    printf("Enter the dispatch list file: ");
    scanf("%s", filename);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read and parse the dispatch list file
    while (!feof(file)) {
        Process p;
        fscanf(file, "%d, %d, %d, %d, %d, %d, %d, %d",
               &p.arrival_time, &p.priority, &p.processor_time, &p.memory,
               &p.printers, &p.scanners, &p.modems, &p.CDs);
        addProcessToQueue(p);
    }

    fclose(file);

    // Main dispatcher loop
    while (1) {
        Process nextProcess = selectNextProcess();
        if (nextProcess.arrival_time == -1) break; // No more processes

        if (checkResources(nextProcess) && allocateMemory(nextProcess)) {
            allocateResources(nextProcess);
            executeProcess(nextProcess);
            releaseResources(nextProcess);
            releaseMemory(nextProcess);
        } else {
            printf("Process could not be executed due to resource or memory constraints.\n");
        }
    }

    printf("All processes completed.\n");
    return 0;
}
