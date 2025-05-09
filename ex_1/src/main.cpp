/*
Course: THUEE-OS 
Project-1: 银行柜员服务问题 
Author: Jiahao Huang 
Time: 2025/5/9
*/ 

#include <stdio.h>  
#include <windows.h> 
#include <fstream> 
#include <ctime> 
#include <iostream> 
#include <iomanip> 

#define N                5         // 柜台数量 
#define CUSTOMER_NUM_MAX 200        // 最大顾客数量 

using namespace std;

int current_get_num = 0;       //顾客取号
int service_call_num = 0;       //柜台叫号
int customer_num = 0;               //总顾客数量  
int current_served_num = 0;       //已经被服务好的顾客数量

int customer_info[CUSTOMER_NUM_MAX][6] = {};        //顾客信息: No., arrival time, service time
int customer_wait_list[CUSTOMER_NUM_MAX][2] = {};   //等待队列: 顾客ID, 顾客No.
int customer_call_list[CUSTOMER_NUM_MAX][5] = {};   //叫号队列

HANDLE customer_thread[CUSTOMER_NUM_MAX];   //顾客线程
HANDLE counter_thread[N];                   //柜台线程
HANDLE service_semaphore = CreateSemaphore(NULL, 0, CUSTOMER_NUM_MAX, NULL); // 柜台服务信号量 
HANDLE counter_call_mutex = CreateMutex(NULL, FALSE, NULL);     // 柜台叫号锁
HANDLE customer_get_mutex = CreateMutex(NULL, FALSE, NULL);     // 顾客取号锁

time_t init_time = time(NULL); // Start time

DWORD WINAPI CustomerThread(PVOID param)
{
    long long customer_id = (long long)param;

    // Simulate customer arrival
    Sleep(1000 * customer_info[customer_id][1]); // Arrival time

    // Get a number
    WaitForSingleObject(customer_get_mutex, INFINITE);
    customer_wait_list[current_get_num][0] = customer_id; // Customer ID
    customer_wait_list[current_get_num][1] = customer_info[customer_id][0]; // Customer No. 
    current_get_num++;
    cout << "Customer " << customer_id << " got number " << current_get_num << endl;
    ReleaseMutex(customer_get_mutex);

    // Signal that the customer is ready to be served
    ReleaseSemaphore(service_semaphore, 1, NULL);
    return 0;
}

DWORD WINAPI CounterThread(PVOID param)
{
    int service_time = 0;
    int customer_index = 0;

    while (true) {
        // Wait for a customer to be ready
        WaitForSingleObject(service_semaphore, INFINITE);

        // Call the next customer
        WaitForSingleObject(counter_call_mutex, INFINITE);
        customer_index = service_call_num++; // index of the customer being served
        cout << "Counter " << (long long)param << " is serving customer " << customer_wait_list[customer_index][1] << endl;
        service_time = customer_info[customer_index][2]; // Service time
        customer_info[customer_index][3] = (long long)param; // Counter number
        customer_info[customer_index][4] = time(NULL) - init_time; // Start time being served
        customer_info[customer_index][5] = customer_info[customer_index][4] + service_time; // End time
        ReleaseMutex(counter_call_mutex);

        // Simulate service time
        Sleep(1000 * service_time);
        current_served_num++;
    }
}

int main()
{
    ifstream input_file("test.txt");
    if (!input_file) {
        cout << "Error opening input file!" << endl;
        return 1;
    }

    // Read customer information from file
    int i; 
    while (input_file.good()) {
        input_file >> customer_info[customer_num][0] >> customer_info[customer_num][1] >> customer_info[customer_num][2]; // No., arrival time, service time
        customer_num++;
        if (input_file.eof()) break;
    }
    input_file.close();
    cout << "Total customers: " << customer_num << endl;

    // Create customer threads
    for (i = 0; i < customer_num; i++) {
        customer_thread[i] = CreateThread(NULL, 0, CustomerThread, (PVOID)i, 0, NULL);
        if (customer_thread[i] == NULL) {
            cout << "Error creating customer thread!" << endl;
            return 1;
        }
    }

    // Create counter threads
    for (i = 0; i < N; i++) {
        counter_thread[i] = CreateThread(NULL, 0, CounterThread, (PVOID)i, 0, NULL);
        if (counter_thread[i] == NULL) {
            cout << "Error creating counter thread!" << endl;
            return 1;
        }
    }

    // Wait for all customer threads to finish
    while (current_served_num < customer_num) {
        Sleep(100);
    }
    
    for (i = 0; i < customer_num; i++) {
        CloseHandle(customer_thread[i]);
    }

    for (i = 0; i < N; i++) {
        CloseHandle(counter_thread[i]);
    }

    // Output results
    ofstream output_file("result.txt");
    if (!output_file) {
        cout << "Error opening output file!" << endl;
        return 1;
    }
    output_file << left << setw(15) << "Customer No."
            << setw(15) << "Arrival Time"
            << setw(15) << "Service Time"
            << setw(15) << "Counter No."
            << setw(15) << "Start Time"
            << setw(15) << "End Time" << endl;
    for (i = 0; i < customer_num; i++) {
    output_file << left << setw(15) << customer_info[i][0]
                << setw(15) << customer_info[i][1]
                << setw(15) << customer_info[i][2]
                << setw(15) << customer_info[i][3]
                << setw(15) << customer_info[i][4]
                << setw(15) << customer_info[i][5] << endl;
    }
    output_file.close();
    cout << "Results written to result.txt" << endl;
    cout << "All customers have been served." << endl;
}