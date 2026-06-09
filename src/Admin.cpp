#include "../include/Admin.h"
#include "../include/Room.h"
#include "../include/Reservation.h"
#include "../include/Payment.h"
#include "../include/Customer.h"
#include "../include/BookingHistory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

int Admin::nextId = 1;

// ─── Constructors / Destructor ────────────────────────────────────────────────

Admin::Admin()
    : User(), adminId(nextId++) {}

Admin::Admin(string username, string password)
    : User(username, password, "Admin"), adminId(nextId++) {}

Admin::~Admin() {}

// ─── Getters ──────────────────────────────────────────────────────────────────

int Admin::getAdminId() const { return adminId; }

// ─── Room Management ──────────────────────────────────────────────────────────

void Admin::addRoom(int roomNumber, const string& roomType,
                    double price, int maxGuests, bool isAC) {
    if (!Room::isValidRoomNumber(roomNumber)) return;
    if (roomNumber <= 0) {
        cout << "❌ Room number must be a positive integer." << endl;
        return;
    }
    if (Room::isDuplicateRoomNumber(roomNumber)) return;

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
        cout << "❌ Room " << roomNumber << " not found." << endl;
        return;
    }
    Room::removeFromFile(roomNumber);
    cout << "✅ Room " << roomNumber << " removed successfully." << endl;
}

void Admin::updateRoom(int roomNumber, const string& field, const string& newValue) {
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

// ─── Reservation Management ───────────────────────────────────────────────────

void Admin::manageReservation(int reservationId, const string& action) {
    Reservation::updateStatusInFile(reservationId, action);
    cout << "✅ Reservation " << reservationId
         << " status updated to: " << action << endl;
}

// ─── User Management ──────────────────────────────────────────────────────────

void Admin::viewAllUsers() {
    ifstream file("data/users.txt");
    string line;
    int count = 0;

    cout << "\n========== All Registered Users ==========" << endl;
    cout << left
         << setw(5)  << "No."
         << setw(15) << "Username"
         << setw(20) << "Name"
         << setw(15) << "Phone"
         << setw(25) << "Email"
         << setw(10) << "Role"
         << endl;
    cout << string(90, '-') << endl;

    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() >= 8 && tokens[7] == "Customer") {
            count++;
            cout << left
                 << setw(5)  << count
                 << setw(15) << tokens[1]   // username
                 << setw(20) << tokens[3]   // name
                 << setw(15) << tokens[4]   // phone
                 << setw(25) << tokens[5]   // email
                 << setw(10) << tokens[7]   // role
                 << endl;
        }
    }

    if (count == 0)
        cout << "No registered customers found." << endl;
    else
        cout << string(90, '-') << endl;
    cout << "Total Customers: " << count << endl;
}

void Admin::removeUser(const string& username) {
    // Cannot remove admin accounts
    ifstream checkFile("data/users.txt");
    string line;
    bool found = false;
    bool isAdmin = false;

    while (getline(checkFile, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() >= 8 && tokens[1] == username) {
            found = true;
            if (tokens[7] == "Admin") isAdmin = true;
            break;
        }
    }
    checkFile.close();

    if (!found) {
        cout << "❌ User '" << username << "' not found." << endl;
        return;
    }
    if (isAdmin) {
        cout << "❌ Cannot remove admin accounts." << endl;
        return;
    }

    // Confirm before deleting
    string confirm;
    cout << "⚠️  Are you sure you want to remove user '" << username << "'? (yes/no): ";
    getline(cin, confirm);
    if (confirm != "yes" && confirm != "Yes" && confirm != "YES") {
        cout << "❌ User removal cancelled." << endl;
        return;
    }

    // Rewrite file without that user
    ifstream inFile("data/users.txt");
    ofstream outFile("data/users_temp.txt");
    while (getline(inFile, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() >= 2 && tokens[1] == username)
            continue;   // skip this user
        outFile << line << "\n";
    }
    inFile.close();
    outFile.close();

    remove("data/users.txt");
    rename("data/users_temp.txt", "data/users.txt");

    cout << "✅ User '" << username << "' removed successfully." << endl;
}

void Admin::searchUser(const string& username) {
    ifstream file("data/users.txt");
    string line;
    bool found = false;

    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() >= 8 && tokens[1] == username && tokens[7] == "Customer") {
            found = true;
            cout << "\n=== User Found ===" << endl;
            cout << "Username : " << tokens[1] << endl;
            cout << "Name     : " << tokens[3] << endl;
            cout << "Phone    : " << tokens[4] << endl;
            cout << "Email    : " << tokens[5] << endl;
            cout << "Address  : " << tokens[6] << endl;
            cout << "Role     : " << tokens[7] << endl;
            break;
        }
    }

    if (!found)
        cout << "❌ Customer '" << username << "' not found." << endl;
}

void Admin::viewUserBookings(const string& username) {
    // Find customer ID from users.txt
    ifstream file("data/users.txt");
    string line;
    int customerId = -1;
    string customerName;

    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() >= 8 && tokens[1] == username && tokens[7] == "Customer") {
            customerId   = stoi(tokens[0]);
            customerName = tokens[3];
            break;
        }
    }
    file.close();

    if (customerId == -1) {
        cout << "❌ Customer '" << username << "' not found." << endl;
        return;
    }

    cout << "\n=== Bookings for " << customerName
         << " (@" << username << ") ===" << endl;

    // Show reservations for this customer
    vector<Reservation> reservations = Reservation::loadFromFile();
    bool found = false;
    for (const Reservation& r : reservations) {
        if (r.getCustomerId() == customerId) {
            r.displayReservation();
            found = true;
        }
    }
    if (!found)
        cout << "No bookings found for this customer." << endl;
}

// ─── Report ───────────────────────────────────────────────────────────────────

void Admin::generateReport() {
    cout << "\n========== Hotel Report ==========" << endl;

    // Rooms
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

    // Users
    ifstream uFile("data/users.txt");
    string line;
    int customerCount = 0;
    while (getline(uFile, line)) {
        if (line.find("|Customer") != string::npos) customerCount++;
    }
    cout << "\nTotal Customers : " << customerCount << endl;

    // Reservations
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

    // Revenue
    vector<Payment> payments = Payment::loadFromFile();
    double totalRevenue = 0;
    for (const Payment& p : payments)
        if (p.getPaymentStatus() == "Completed")
            totalRevenue += p.getAmount();

    cout << "\nTotal Revenue: $" << totalRevenue << endl;
    cout << "==================================\n" << endl;
}

// ─── File I/O ─────────────────────────────────────────────────────────────────

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