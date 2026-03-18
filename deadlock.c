#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

int n, m; // n = number of processes, m = number of resource types

int allocation[MAX_PROCESSES][MAX_RESOURCES];
int request[MAX_PROCESSES][MAX_RESOURCES];
int available[MAX_RESOURCES];

// ---- Wait-For Graph (WFG) ----
int wfg[MAX_PROCESSES][MAX_PROCESSES];
bool visited[MAX_PROCESSES];
bool recStack[MAX_PROCESSES];

// Build Wait-For Graph
void buildWFG() {
    memset(wfg, 0, sizeof(wfg));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (request[i][j] > 0) {
                for (int k = 0; k < n; k++) {
                    if (k != i && allocation[k][j] > 0) {
                        wfg[i][k] = 1; // Pi waits for Pk
                    }
                }
            }
        }
    }
}

// DFS to detect cycle in WFG
bool dfsCycle(int v) {
    visited[v] = true;
    recStack[v] = true;

    for (int u = 0; u < n; u++) {
        if (wfg[v][u]) {
            if (!visited[u]) {
                if (dfsCycle(u))
                    return true;
            } else if (recStack[u]) {
                return true;
            }
        }
    }

    recStack[v] = false;
    return false;
}

// Detect deadlock using WFG
bool detectDeadlockWFG() {
    memset(visited, false, sizeof(visited));
    memset(recStack, false, sizeof(recStack));

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            if (dfsCycle(i))
                return true;
        }
    }
    return false;
}

// ---- Resource Allocation Graph (RAG) Method ----
bool detectDeadlockRAG() {
    int work[MAX_RESOURCES];
    bool finish[MAX_PROCESSES];

    memcpy(work, available, m * sizeof(int));
    memset(finish, false, sizeof(finish));

    bool progress = true;

    while (progress) {
        progress = false;

        for (int i = 0; i < n; i++) {
            if (!finish[i]) {
                bool canRun = true;

                for (int j = 0; j < m; j++) {
                    if (request[i][j] > work[j]) {
                        canRun = false;
                        break;
                    }
                }

                if (canRun) {
                    for (int j = 0; j < m; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    progress = true;
                }
            }
        }
    }

    // Check unfinished processes
    for (int i = 0; i < n; i++) {
        if (!finish[i])
            return true; // Deadlock exists
    }

    return false;
}

// Print deadlocked processes (RAG)
void printDeadlockedProcesses() {
    int work[MAX_RESOURCES];
    bool finish[MAX_PROCESSES];

    memcpy(work, available, m * sizeof(int));
    memset(finish, false, sizeof(finish));

    bool progress = true;

    while (progress) {
        progress = false;

        for (int i = 0; i < n; i++) {
            if (!finish[i]) {
                bool canRun = true;

                for (int j = 0; j < m; j++) {
                    if (request[i][j] > work[j]) {
                        canRun = false;
                        break;
                    }
                }

                if (canRun) {
                    for (int j = 0; j < m; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    progress = true;
                }
            }
        }
    }

    printf("Deadlocked Processes: ");
    for (int i = 0; i < n; i++) {
        if (!finish[i]) {
            printf("P%d ", i);
        }
    }
    printf("\n");
}

// ---- Main Function ----
int main() {
    printf("Enter number of processes and resource types: ");
    scanf("%d %d", &n, &m);

    printf("Enter Allocation Matrix (%d x %d):\n", n, m);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &allocation[i][j]);

    printf("Enter Request Matrix (%d x %d):\n", n, m);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &request[i][j]);

    printf("Enter Available Resources (%d values):\n", m);
    for (int j = 0; j < m; j++)
        scanf("%d", &available[j]);

    printf("\n--- Deadlock Detection Results ---\n");

    // RAG Method
    if (detectDeadlockRAG()) {
        printf("[RAG Method] DEADLOCK DETECTED\n");
        printDeadlockedProcesses();
    } else {
        printf("[RAG Method] No Deadlock. System is in Safe State.\n");
    }

    // WFG Method
    buildWFG();
    if (detectDeadlockWFG()) {
        printf("[WFG Method] DEADLOCK DETECTED (Cycle in Wait-For Graph)\n");
    } else {
        printf("[WFG Method] No Deadlock. No cycle in Wait-For Graph.\n");
    }

    return 0;
}
