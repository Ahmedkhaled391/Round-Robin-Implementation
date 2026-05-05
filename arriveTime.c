/*  C Program to implement Round Robin Process Scheduling Algorithm.
 *  Arrival time of the processes is considered (user enters arrival time).
 *  This program draws the Gantt Chart for the list of processes.
 *  Number of processes is limited to 10.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX 10
#define MAX_GANTT 200

typedef struct
{
    int process; // Process index; -1 means CPU is idle.
    int start;
    int end;
} GanttEntry;

void roundRobin(int burst_time[], int arrival_time[], int no_of_processes, int quantum);
void drawGanttChart(GanttEntry gantt[], int gantt_size, int total_time);

int main(void)
{
    int no_of_processes, bt[MAX], at[MAX], quantum;

    printf("Enter the number of processes (Max 10): ");
    scanf("%d", &no_of_processes);

    printf("\nEnter the burst time and arrival time of each process:\n");
    for (int i = 0; i < no_of_processes; ++i)
    {
        printf("Enter burst time of process %d: ", i + 1);
        scanf("%d", &bt[i]);
        printf("Enter arrival time of process %d: ", i + 1);
        scanf("%d", &at[i]);
    }

    printf("\nEnter the time quantum: ");
    scanf("%d", &quantum);

    printf("\n");

    roundRobin(bt, at, no_of_processes, quantum);

    printf("\n\n");

    return 0;
}

void drawGanttChart(GanttEntry gantt[], int gantt_size, int total_time)
{
    printf("\nGantt Chart:\n\n");

    // Print process / idle labels.
    for (int i = 0; i < gantt_size; ++i)
    {
        if (gantt[i].process == -1)
            printf("|IDLE");
        else
            printf("|P%d  ", gantt[i].process + 1);
    }
    printf("|\n");

    // Print the timeline (start of each entry, then the final end time).
    for (int i = 0; i < gantt_size; ++i)
    {
        printf("%d\t", gantt[i].start);
    }
    printf("%d\n\n", total_time);
}

// Round Robin scheduling with dynamic arrival times.

void roundRobin(int burst_time[], int arrival_time[], int no_of_processes, int quantum)
{
    int remaining_time[MAX];
    int completion_time[MAX];
    int in_queue[MAX];
    int completed[MAX];

    int queue[MAX_GANTT]; // Ready queue storing process indices.
    int front = 0, rear = 0;
    int done = 0;
    int time = 0;
    int total_waiting = 0, total_turnaround = 0;

    GanttEntry gantt[MAX_GANTT];
    int gantt_size = 0;

    for (int i = 0; i < no_of_processes; ++i)
    {
        remaining_time[i] = burst_time[i];
        in_queue[i] = 0;
        completed[i] = 0;
    }

    // Enqueue all processes that arrive at time 0.
    for (int i = 0; i < no_of_processes; ++i)
    {
        if (arrival_time[i] == 0)
        {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }

    printf("Process No.\t\tWaiting Time\tTurnaround Time\n");

    while (done < no_of_processes)
    {
        // If the ready queue is empty the CPU sits idle until the next arrival.
        if (front == rear)
        {
            int next_arrival = INT_MAX;
            for (int i = 0; i < no_of_processes; ++i)
            {
                if (!completed[i] && !in_queue[i] && arrival_time[i] < next_arrival)
                    next_arrival = arrival_time[i];
            }

            // Record idle period in Gantt chart.
            if (gantt_size > 0 && gantt[gantt_size - 1].process == -1)
            {
                gantt[gantt_size - 1].end = next_arrival; // Extend existing idle block.
            }
            else
            {
                gantt[gantt_size].process = -1;
                gantt[gantt_size].start = time;
                gantt[gantt_size].end = next_arrival;
                ++gantt_size;
            }

            time = next_arrival;

            // Enqueue processes that have now arrived.
            for (int i = 0; i < no_of_processes; ++i)
            {
                if (!completed[i] && !in_queue[i] && arrival_time[i] <= time)
                {
                    queue[rear++] = i;
                    in_queue[i] = 1;
                }
            }
            continue;
        }

        // Dequeue next process.
        int process = queue[front++];
        in_queue[process] = 0;

        // Execute for at most one quantum.
        int exec_time = (remaining_time[process] < quantum)
                            ? remaining_time[process]
                            : quantum;

        // Record Gantt entry.
        gantt[gantt_size].process = process;
        gantt[gantt_size].start = time;
        gantt[gantt_size].end = time + exec_time;
        ++gantt_size;

        time += exec_time;
        remaining_time[process] -= exec_time;

        // Enqueue any processes that arrived during this execution slice
        // (add them before re-enqueuing the current process so they go ahead of it).
        for (int i = 0; i < no_of_processes; ++i)
        {
            if (!completed[i] && !in_queue[i] && arrival_time[i] <= time && i != process)
            {
                queue[rear++] = i;
                in_queue[i] = 1;
            }
        }

        if (remaining_time[process] == 0)
        {
            // Process finished.
            completion_time[process] = time;
            completed[process] = 1;
            ++done;
        }
        else
        {
            // Process still has work left; put it at the end of the queue.
            queue[rear++] = process;
            in_queue[process] = 1;
        }
    }

    int total_time = time;

    for (int i = 0; i < no_of_processes; ++i)
    {
        int tat = completion_time[i] - arrival_time[i];
        int wt = tat - burst_time[i];
        printf("Process[%d]\t\t\t%d\t\t\t\t%d\n", i + 1, wt, tat);
        total_waiting += wt;
        total_turnaround += tat;
    }

    drawGanttChart(gantt, gantt_size, total_time);

    printf("Average turnaround time: %.2f\n", (float)total_turnaround / no_of_processes);
    printf("Average waiting time: %.2f", (float)total_waiting / no_of_processes);
}
