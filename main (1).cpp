#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <map>
#include <cstdlib>
#include <ctime>

using namespace std;

// thread synchronization
mutex elevatorMutex;

// this function is used to format floor labels (convert floor numbers to string)
string formatFloor(int floor) {
    if (floor == 0)
        return "Ground Floor";
    else if (floor == 1)
        return "1st Floor";
    else if (floor == 2)
        return "2nd Floor";
    else if (floor == 3)
        return "3rd Floor";
    else if (floor == 4)
        return "4th Floor";
    else if (floor == 5)
        return "5th Floor";
    else if (floor == 6)
        return "6th Floor";
    else if (floor == 7)
        return "7th Floor";
    else if (floor == 8)
        return "8th Floor";
    else if (floor == 9)
        return "9th Floor";
    else
        return to_string(floor) + "th Floor";
}

// this will manage the elevator with current floor of the elevator, capacity, and passengers
struct Elevator {
    int currentFloor;                      // tracks the floor where the elev is located 
    int targetFloor;                       // tracks the elev when moving toward the desired floor 
    int currentPassengers;                 // keeps track of how many passengers inside the elevator
    const int maxCapacity;                 // constant value of max capacity. 8
    map<int, int> passengerDestinations;   // Destination floors of passengers

    Elevator() : currentFloor(0), targetFloor(0), currentPassengers(0), maxCapacity(8) {} // Ground floor is 0, maxCapacity is 8

    void moveToFloor(int floor) {
        scoped_lock lock(elevatorMutex);
        targetFloor = floor;

        cout << "\nElevator is moving from " << formatFloor(currentFloor)
             << " to " << formatFloor(targetFloor) << ".\n";

        // Simulate movement between floors
        while (currentFloor != targetFloor) {
            this_thread::sleep_for(chrono::milliseconds(500)); // Simulate time between floors
            if (currentFloor < targetFloor)
                currentFloor++;
            else
                currentFloor--;

            // Print intermediate floor messages
            if (currentFloor != targetFloor) {
                cout << "Elevator is now at " << formatFloor(currentFloor) << ".\n";
            }

            // Check if passengers need to exit at the current floor
            if (passengerDestinations.count(currentFloor)) {
                int exitingPassengers = passengerDestinations[currentFloor];
                currentPassengers -= exitingPassengers;
                cout << exitingPassengers << " passengers exited the elevator at " << formatFloor(currentFloor) << ".\n";
                passengerDestinations.erase(currentFloor);
            }
        }

        cout << "Elevator has arrived at " << formatFloor(targetFloor) << ".\n";
    }

    bool loadPassengers(int passengers, int destination) {
        scoped_lock lock(elevatorMutex);
        if (currentPassengers + passengers <= maxCapacity && destination != currentFloor) {
            currentPassengers += passengers;
            passengerDestinations[destination] += passengers;
            cout << passengers << " passengers entered the elevator heading to " 
                 << formatFloor(destination) << ". Current passengers: " << currentPassengers << "/" << maxCapacity << ".\n";
            return true;
        } else {
            cout << "Elevator cannot take " << passengers << " more passengers or invalid destination. Current passengers: " 
                 << currentPassengers << "/" << maxCapacity << ".\n";
            return false;
        }
    }
};

// Function for elevator thread
void elevatorOperation(Elevator &elevator, const vector<pair<int, int>> &requests) {
    for (auto [floor, destination] : requests) {
        // Simulate passengers waiting at the floor
        int newPassengers = rand() % 5 + 1; // 1 to 5 passengers
        cout << "\n" << newPassengers << " passenger/s are waiting at " << formatFloor(floor) << ".\n";

        elevator.moveToFloor(floor);

        if (!elevator.loadPassengers(newPassengers, destination)) {
            cout << "Some passengers could not enter due to full capacity or invalid destination.\n";
        }
    }

    // Additional 5 passengers at 6th Floor going to 9th Floor
    int floor = 6;
    int destination = 9;
    int newPassengers = 5;

    cout << "\n" << newPassengers << " passenger/s are waiting at " << formatFloor(floor) << ".\n";
    elevator.moveToFloor(floor);

    if (!elevator.loadPassengers(newPassengers, destination)) {
        cout << "Some passengers could not enter due to full capacity.\n";
    }

    elevator.moveToFloor(destination); // Move to the 9th Floor so passengers can exit

    cout << "\nElevator has completed all requests.\n";
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Seed for random number generation

    cout << "      Elevator System Simulation \n";
    cout << "=========================================\n";

    // Create the elevator
    Elevator elevator;

    // passengers request for the elevators: (entry floor, destination floor)
    vector<pair<int, int>> requests = {
        {2, 5}, // Passengers at 2nd Floor going to 5th Floor
        {3, 6}, // Passengers at 3rd Floor going to 6th Floor
    };

    // thread for the elevator
    thread t(elevatorOperation, ref(elevator), requests);

    t.join();

    cout << "\nElevator simulation has ended.\n";
    return 0;
}
