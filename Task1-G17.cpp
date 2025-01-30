#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <map>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

// Thread synchronization
mutex elevatorMutex;

// Function to format floor labels
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

// Elevator structure to manage its operations
struct Elevator {
    int currentFloor;
    bool goingUp;
    int currentPassengers;
    const int maxCapacity;
    map<int, int> passengerDestinations;

    Elevator() : currentFloor(0), goingUp(true), currentPassengers(0), maxCapacity(8) {}

    void moveToFloor(int floor) {
        scoped_lock lock(elevatorMutex);

        cout<<"\nElevator is moving from "<<formatFloor(currentFloor)
             <<" to "<<formatFloor(floor)<<".\n";

        // Simulate movement between floors
        while (currentFloor != floor) {
            this_thread::sleep_for(chrono::milliseconds(500));
            if (currentFloor < floor)
                currentFloor++;
            else
                currentFloor--;

            cout<<"Elevator is now at " << formatFloor(currentFloor)<<".\n";

            // Check if passengers need to exit
            if (passengerDestinations.count(currentFloor)) {
                int exitingPassengers = passengerDestinations[currentFloor];
                currentPassengers -= exitingPassengers;
                passengerDestinations.erase(currentFloor);
                cout<<exitingPassengers<<" passengers exited the elevator at "
                     << formatFloor(currentFloor)<<".\n";
            }

            // Handle 9th floor logic
            if (currentFloor == 9) {
                goingUp = false;
                cout<<"Elevator is at the top floor (9th Floor). It will now go down.\n";
                break;
            }
        }

        cout<<"Elevator has arrived at "<<formatFloor(floor)<<".\n";
    }

    bool loadPassengers(int passengers, int destination) {
        scoped_lock lock(elevatorMutex);
        if (currentPassengers + passengers <= maxCapacity && destination != currentFloor) {
            currentPassengers += passengers;
            passengerDestinations[destination] += passengers;
            cout<<passengers<<" passengers entered the elevator heading to "
                 <<formatFloor(destination)<<". Current passengers: "
                 <<currentPassengers<<"/"<<maxCapacity<< ".\n";
            return true;
        } else {
            cout<<"Cannot load "<< passengers<<" passengers. Current: "
                 <<currentPassengers<<"/"<<maxCapacity<<".\n";
            return false;
        }
    }
};

void elevatorOperation(Elevator &elevator) {
    while (true) {
        cout<<"\nEnter floor requests (format: current_floor,destination_floor separated by spaces), or type 'exit' to quit:\n";
        string input;
        getline(cin, input);

        if (input == "exit") break;

        istringstream iss(input);
        string token;

        while (getline(iss, token, ' ')) {
            size_t comma = token.find(',');
            if (comma == string::npos) {
                cout<<"Invalid format. Please use: current_floor,destination_floor.\n";
                continue;
            }

            int currentFloor, destinationFloor;
            try {
                currentFloor = stoi(token.substr(0, comma));
                destinationFloor = stoi(token.substr(comma + 1));

                if (currentFloor < 0 || currentFloor > 9 || destinationFloor < 0 || destinationFloor > 9 || currentFloor == destinationFloor) {
                    throw invalid_argument("Invalid floor numbers");
                }
            } catch (...) {
                cout<<"Invalid input. Please use the format: current_floor,destination_floor (e.g., 2,5).\n";
                continue;
            }

            int passengers = rand() % 5 + 1; // Random number of passengers (1 to 5)
            cout<<"\n"<<passengers<<" passengers are waiting at "<<formatFloor(currentFloor)<<".\n";

            // Move to current floor
            elevator.moveToFloor(currentFloor);

            // Load passengers and move to destination
            if (elevator.loadPassengers(passengers, destinationFloor)) {
                elevator.moveToFloor(destinationFloor);
            } else {
                cout<<"Some passengers could not enter due to full capacity.\n";
            }
        }
    }

    cout<<"\nElevator operation has ended.\n";
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Seed for random number generation

    cout<<"\t\t\t\t\t=================================================\n";
    cout<<"\t\t\t\t\t                ELEVATOR SYSTEM           \n";
    cout<<"\t\t\t\t\t                    DESIGN                \n";
    cout<<"\t\t\t\t\t=================================================\n";
    
    Elevator elevator;
    elevatorOperation(elevator);

    return 0;
}
