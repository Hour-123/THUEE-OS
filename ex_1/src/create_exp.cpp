#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    ofstream output_file("test.txt");
    if (!output_file) {
        cout << "Error creating test.txt!" << endl;
        return 1;
    }

    int num_customers = 8; // Number of customers to generate
    int max_arrival_time = 20; // Maximum arrival time
    int max_service_time = 10; // Maximum service time

    srand((unsigned)time(NULL)); // Seed for random number generation

    for (int i = 1; i <= num_customers; i++) {
        int arrival_time = rand() % max_arrival_time + 1; // Random arrival time (1 to max_arrival_time)
        int service_time = rand() % max_service_time + 1; // Random service time (1 to max_service_time)
        
        // Write data without a trailing newline for the last customer
        if (i == num_customers) {
            output_file << i << " " << arrival_time << " " << service_time;
        } else {
            output_file << i << " " << arrival_time << " " << service_time << endl;
        }
    }

    output_file.close();
    cout << "test.txt generated successfully!" << endl;

    return 0;
}