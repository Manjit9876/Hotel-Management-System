#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

class Room {
public:
    int roomNumber;
    string type;
    int capacity;
    bool available;

    Room(int roomNumber, string type, int capacity) : roomNumber(roomNumber), type(type), capacity(capacity), available(true) {}
};

class Customer {
public:
    string name;
    string contact;
    string idType;

    Customer(string name, string contact, string idType) : name(name), contact(contact), idType(idType) {}
};

class Reservation {
public:
    Room room;
    Customer customer;
    Reservation(Room room, Customer customer) : room(room), customer(customer) {}

    // Convert reservation data to string for file storage
    string toString() const { 
        return customer.name + "," + customer.contact + "," + customer.idType + "," + to_string(room.roomNumber) + "," + room.type;
    }       
    // Parse reservation data from string loaded from file
    static Reservation fromString(const string& data) { // Add const qualifier
        vector<string> tokens;
        stringstream ss(data);
        string token;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() != 5) {
            throw runtime_error("Invalid reservation data format");
        }
        Customer customer(tokens[0], tokens[1], tokens[2]);
        Room room(stoi(tokens[3]), tokens[4], 0); // Capacity is not stored in file, so set to 0
        return Reservation(room, customer);
    }
};

// Hotel class to manage rooms and reservations
class Hotel {
private:
    vector<Room> rooms;
    vector<Reservation> reservations;
    const vector<string> idTypes = {"Aadhar", "Passport", "Driver's License"};
    const string reservationFileName = "reservations.txt";

public:
    Hotel() {
        loadReservations();
        // Add some example rooms
        addRooms(101, "Single", 1, 5); // Add 5 single rooms
        addRooms(201, "Double", 2, 5); // Add 5 double rooms
        addRooms(301, "Suite", 4, 3);  // Add 3 suite rooms
    }

    void addRooms(int startRoomNumber, string type, int capacity, int count) {
        for (int i = 0; i < count; ++i) {
            rooms.push_back(Room(startRoomNumber + i, type, capacity));
        }
    }

    void loadReservations() {

        ifstream file(reservationFileName);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) { // read the all lines from the file
                try {                       
                    reservations.push_back(Reservation::fromString(line));
                } catch (const exception& e) {
                    cerr << "Error loading reservation: " << e.what() << endl;  // to detect the error 
                }
            }
            file.close();
        } else {
            cerr << "Unable to open reservation file" << endl;
        }
    }

    // Save reservations to file
    void saveReservations() {
        ofstream file(reservationFileName);
        if (file.is_open()) {
            for (const Reservation& reservation : reservations) {
                file << reservation.toString() << endl;
            }
            file.close();
        } else {
            cerr << "Unable to open reservation file for writing" << endl;
        }
    }

    void displayAvailableRooms() {

        // It's useful for providing information to hotel staff 
        cout << "Available Rooms:" << endl;
        for (const Room& room : rooms) {
            cout << "Room Number: " << room.roomNumber << ", Type: " << room.type << ", Capacity: " << room.capacity;
            if (room.available) {
                cout << " (Available)" << endl;
            } else {
                cout << " (Occupied)" << endl;
            }
        }
    }

    bool reserveRoom(int roomNumber, string customerName, string customerContact, string idType) {
        // Validate contact number
        if (customerContact.length() != 10) {
            cout << "Invalid contact number. Contact number must be 10 digits." << endl;
            return false;
        }

        // Validate ID type
        if (find(idTypes.begin(), idTypes.end(), idType) == idTypes.end()) {
            cout << "Invalid ID type. Available options are Aadhar, Passport, and Driver's License." << endl;
            return false;
        }

        for (Room& room : rooms) {
            if (room.roomNumber == roomNumber && room.available) {
                room.available = false;
                Customer customer(customerName, customerContact, idType);
                reservations.push_back(Reservation(room, customer));
                cout << "Room " << roomNumber << " reserved successfully." << endl;
                cout << "Customer Name: " << customerName << endl;
                cout << "Customer Contact: " << customerContact << endl;
                cout << "ID Type: " << idType << endl;
                cout << "Room Type: " << room.type << endl;
                return true;
            }
        }
        cout << "Sorry, room " << roomNumber << " is not available or does not exist." << endl;
        return false;
    }

    bool checkoutRoom(int roomNumber) {
        for (Room& room : rooms) {
            if (room.roomNumber == roomNumber && !room.available) {
                room.available = true;
                // Find and remove the reservation for this room
                auto it = remove_if(reservations.begin(), reservations.end(), [roomNumber](const Reservation& reservation) {
                    return reservation.room.roomNumber == roomNumber;
                });
                reservations.erase(it, reservations.end());
                cout << "Room " << roomNumber << " has been checked out and is now available." << endl;
                return true;
            }
        }
        cout << "Sorry, room " << roomNumber << " is already available or does not exist." << endl;
        return false;
    }
};

int main() {
    Hotel hotel;

    char choice;
    do {
        cout << "-------------------------------------------" << endl;
        cout << "Welcome to the Hotel Reservation System!" << endl;
        cout << "-------------------------------------------" << endl;
        cout << "1. Display available rooms" << endl;
        cout << "2. Reserve a room" << endl;
        cout << "3. Check out from a room" << endl;
        cout << "4. Quit" << endl;
        cout << "-------------------------------------------" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case '1': {
                hotel.displayAvailableRooms();
                break;
            }
            case '2': {
                int roomNumber;
                string customerName, customerContact, idType;
                cout << "Enter room number: ";
                cin >> roomNumber;
                cout << "Enter customer name: ";
                cin.ignore(); // Clear the input buffer
                getline(cin, customerName);
                cout << "Enter customer contact: ";
                cin >> customerContact;
                cout << "Enter ID type (Aadhar/Passport/Driver's License): ";
                cin >> idType;
                hotel.reserveRoom(roomNumber, customerName, customerContact, idType);
                break;
            }
            case '3': {
                int roomNumber;
                cout << "Enter room number to check out: ";
                cin >> roomNumber;
                hotel.checkoutRoom(roomNumber);
                break;
            }
            case '4': {
                cout << "Exiting program. Saving reservations to file..." << endl;
                hotel.saveReservations();
                break;
            }
            default: {
                cout << "Invalid choice. Please enter a number from 1 to 4." << endl;
                break;
            }
        }
    } while (choice != '4');

    return 0;
}

