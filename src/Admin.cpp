#include "../include/Admin.h"
#include "../include/Room.h"
#include "../include/Reservation.h"
#include "../include/Payment.h"
#include "../include/Customer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

int Admin::nextId = 1;

Admin::Admin()
    : User(), adminId(nextId++) {}

Admin::Admin(string username, string password)
    : User(username, password, "Admin"), adminId(nextId++) {}

Admin::~Admin() {}

int Admin::getAdminId() const { return adminId; }

void Admin::addRoom(int roomNumber, const string& roomType,
                    double price, int maxGuests, bool isAC) {
    if (!Room::isValidRoomNumber(roomNumber))
        return;
    if (roomNumber <= 0) {
        cout << "❌ Room number must be a positive integer." << endl;
        return;
    }
    if (Room::isDuplicateRoomNumber(roomNumber))
        return;

    Room room(roomNumber, roomType, price, maxGuests, isAC);
    room.saveToFile();
    cout << "✅ Room " << roomNumber << " (" << roomType << ", "
         << (isAC ? "AC" : "Non-AC")
         << ") added successfully at $" << price << "/night." << endl;
}

void Admin::removeRoom(int roomNumber) {
    vector<Room> rooms = Room::loadFromFile();
    bool found = false;
    for (const Room& r : rooms)
        if (r.getRoomNumber() == roomNumber) { found = true; break; }

    if (!found) {
        cout << "❌ Room " << roomNumber << " not found. Cannot remove." << endl;
        return;
    }
    Room::removeFromFile(roomNumber);
    cout << "✅ Room " << roomNumber << " removed successfully." << endl;
}

void Admin::updateRoom(int roomNumber,
                       const string& field,
                       const string& newValue) {
    vector<Room> rooms = Room::loadFromFile();
    bool found = false;
    ofstream file("data/rooms.txt", ios::trunc);

    for (Room& r : rooms) {
        if (r.getRoomNumber() == roomNumber) {
            found = true;
            if      (field == "price")     r.setPrice(stod(newValue));
            else if (field == "type")      r.setRoomType(newValue);
            else if (field == "maxGuests") r.setMaxGuests(stoi(newValue));
            else if (field == "ac")        r.setIsAC(newValue == "AC" || newValue == "ac" || newValue == "1");
        }
        file << r.getRoomNumber()  << "|" << r.getRoomType()    << "|"
             << r.getPrice()       << "|" << r.getIsAvailable() << "|"
             << r.getMaxGuests()   << "|" << (r.getIsAC() ? "AC" : "NonAC") << "\n";
    }

    if (found)
        cout << "✅ Room " << roomNumber << " updated successfully." << endl;
    else
        cout << "❌ Room " << roomNumber << " not found." << endl;
}

void Admin::manageReservation(int reservationId, const string& action) {
    Reservation::updateStatusInFile(reservationId, action);
    cout << "✅ Reservation " << reservationId
         << " status updated to: " << action << endl;
}

void Admin::generateReport() {
    cout << "\n========== Hotel Report ==========" << endl;

    vector<Room> rooms = Room::loadFromFile();
    int available = 0, occupied = 0, acCount = 0, nonAcCount = 0;
    for (const Room& r : rooms) {
        r.checkAvailability() ? available++ : occupied++;
        r.getIsAC() ? acCount++ : nonAcCount++;
    }
    cout << "Total Rooms  : " << rooms.size() << endl;
    cout << "Available    : " << available     << endl;
    cout << "Occupied     : " << occupied      << endl;
    cout << "AC Rooms     : " << acCount       << endl;
    cout << "Non-AC Rooms : " << nonAcCount    << endl;

    vector<Reservation> reservations = Reservation::loadFromFile();
    int confirmed = 0, cancelled = 0, completed = 0;
    for (const Reservation& r : reservations) {
        if      (r.getReservationStatus() == "Confirmed")  confirmed++;
        else if (r.getReservationStatus() == "Cancelled")  cancelled++;
        else if (r.getReservationStatus() == "Completed")  completed++;
    }
    cout << "\nTotal Reservations : " << reservations.size() << endl;
    cout << "Confirmed          : " << confirmed << endl;
    cout << "Cancelled          : " << cancelled << endl;
    cout << "Completed          : " << completed << endl;

    vector<Payment> payments = Payment::loadFromFile();
    double totalRevenue = 0;
    for (const Payment& p : payments)
        if (p.getPaymentStatus() == "Completed")
            totalRevenue += p.getAmount();

    cout << "\nTotal Revenue: $" << totalRevenue << endl;
    cout << "==================================\n" << endl;
}

void Admin::displayInfo() const {
    cout << "=== Admin Info ===" << endl;
    cout << "Admin ID : " << adminId       << endl;
    cout << "Username : " << getUsername() << endl;
}

void Admin::saveToFile() const {
    ofstream file("data/users.txt", ios::app);
    if (file.is_open()) {
        file << adminId << "|" << getUsername() << "|" << getPassword()
             << "||||" << "|Admin\n";
        file.close();
    }
}

Admin Admin::loadFromFile(const string& username) {
    ifstream file("data/users.txt");
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() >= 8 && tokens[7] == "Admin" && tokens[1] == username) {
            Admin a(tokens[1], tokens[2]);
            return a;
        }
    }
    return Admin();
}