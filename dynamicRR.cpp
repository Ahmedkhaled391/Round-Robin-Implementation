#include <stdio.h>
#include <stdlib.h>
#define MAX 10

void roundRobin(int burst_time[], int arrival_time[], int no_of_processes, int quantum);
void drawGanttChart(int burst_time[], int arrival_time[], int no_of_processes, int quantum);

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

void drawGanttChart(int burst_time[], int arrival_time[], int no_of_processes, int quantum)
{
    int remaining_time[MAX];
    int total = 0;
    int remain = no_of_processes;
    int process = 0;
    int time = 0;

    printf("\nGantt Chart:\n\n");

    for (int i = 0; i < no_of_processes; ++i)
    {
        remaining_time[i] = burst_time[i];
    }

    while (remain != 0)
    {
        int executed = 0;

        // Run this process only if it has already arrived and still needs CPU time.
        if (arrival_time[process] <= time && remaining_time[process] > 0)
        {
            printf("|P%d ", process + 1);

            if (remaining_time[process] <= quantum)
            {
                time += remaining_time[process];
                remaining_time[process] = 0;
                --remain;
            }
            else
            {
                remaining_time[process] -= quantum;
                time += quantum;
            }

            executed = 1;
        }

        if (!executed)
        {
            int found = 0;
            int next_arrival = 1000000000;

            // Check whether any not-finished process is ready at current time.
            // If none are ready, track the earliest future arrival.
            for (int i = 0; i < no_of_processes; ++i)
            {
                if (remaining_time[i] > 0)
                {
                    if (arrival_time[i] <= time)
                    {
                        found = 1;
                    }
                    else if (arrival_time[i] < next_arrival)
                    {

                        next_arrival = arrival_time[i];
                    }
                }
            }

            if (!found && next_arrival != 1000000000)
            {
                // CPU is idle until the next process arrives.
                printf("|IDLE ");
                time = next_arrival;
            }
        }

        if (process == no_of_processes - 1)
        {
            process = 0;
        }
        else
        {
            ++process;
        }
    }

    printf("|\n");

    total = time;
    int print_time = 0;
    while (print_time <= total)
    {
        printf("%d\t", print_time);
        print_time += quantum;
    }

    if ((print_time - quantum) != total)
    {
        printf("%d", total);
    }

    printf("\n\n");
}

void roundRobin(int burst_time[], int arrival_time[], int no_of_processes, int quantum)
{
    int remaining_time[MAX];
    int waiting_time = 0;
    int turnaround_time = 0;
    int completion_time[MAX] = {0};

    for (int i = 0; i < no_of_processes; ++i)
    {
        remaining_time[i] = burst_time[i];
    }

    int process = 0;
    int time = 0;
    int remain = no_of_processes;

    printf("Process No.\t\tWaiting Time\tTurnaround Time\n");

    while (remain != 0)
    {
        int flag = 0;

        // A process can execute only after its arrival time.
        if (arrival_time[process] <= time && remaining_time[process] > 0)
        {
            if (remaining_time[process] <= quantum)
            {
                time += remaining_time[process];
                remaining_time[process] = 0;
                flag = 1;
            }
            else
            {
                time += quantum;
                remaining_time[process] -= quantum;
            }
        }
        else
        {
            int found = 0;
            int next_arrival = 1000000000;

            // If no process is ready now, jump the clock to the nearest arrival.
            for (int i = 0; i < no_of_processes; ++i)
            {
                if (remaining_time[i] > 0)
                {
                    if (arrival_time[i] <= time)
                    {
                        found = 1;
                    }
                    else if (arrival_time[i] < next_arrival)
                    {
                        next_arrival = arrival_time[i];
                        // Keep the earliest upcoming arrival among unfinished processes.
                    }
                }
            }

            if (!found && next_arrival != 1000000000)
            {
                // This avoids getting stuck when all remaining processes arrive later.
                time = next_arrival;
            }
        }

        if (flag == 1)
        {
            int tat = time - arrival_time[process];
            int wt = tat - burst_time[process];

            completion_time[process] = time;
            --remain;

            printf("Process[%d]\t\t\t%d\t\t\t\t%d\n", process + 1, wt, tat);

            waiting_time += wt;
            turnaround_time += tat;
        }

        if (process == no_of_processes - 1)
        {
            process = 0;
        }
        else
        {
            ++process;
        }
    }

    drawGanttChart(burst_time, arrival_time, no_of_processes, quantum);

    printf("Average turnaround time: %.2f\n", (float)turnaround_time / no_of_processes);
    printf("Average waiting time: %.2f", (float)waiting_time / no_of_processes);
}