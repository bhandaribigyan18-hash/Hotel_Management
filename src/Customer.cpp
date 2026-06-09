#include "../include/Customer.h"
#include "../include/Room.h"
#include "../include/Reservation.h"
#include "../include/BookingHistory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

int Customer::nextId = 1;

// ─── Constructors / Destructor ────────────────────────────────────────────────

Customer::Customer()
    : User(), customerId(nextId++), name(""), phone(""), email(""), address("") {}

Customer::Customer(string username, string password, string name,
                   string phone, string email, string address)
    : User(username, password, "Customer"),
      customerId(nextId++), name(name), phone(phone),
      email(email), address(address) {}

Customer::~Customer() {}

// ─── Getters ──────────────────────────────────────────────────────────────────

int    Customer::getCustomerId() const { return customerId; }
string Customer::getName()       const { return name; }
string Customer::getPhone()      const { return phone; }
string Customer::getEmail()      const { return email; }
string Customer::getAddress()    const { return address; }

// ─── Setters ──────────────────────────────────────────────────────────────────

void Customer::setName(const string& n)    { name    = n; }
void Customer::setPhone(const string& p)   { phone   = p; }
void Customer::setEmail(const string& e)   { email   = e; }
void Customer::setAddress(const string& a) { address = a; }

// ─── Methods ──────────────────────────────────────────────────────────────────

void Customer::searchRoom(const string& roomType,
                          const string& checkIn,
                          const string& checkOut,
                          const string& acPref) {
    cout << "\n=== Available Rooms ===" << endl;
    cout << "Type: "      << roomType
         << " | AC: "     << acPref
         << " | Check-in: "  << checkIn
         << " | Check-out: " << checkOut << endl;

    vector<Room> rooms = Room::loadFromFile();
    bool found = false;
    for (const Room& r : rooms) {
        bool typeMatch = (roomType == "Any" || r.getRoomType() == roomType);
        bool acMatch   = (acPref   == "Any"
                          || (acPref == "AC"    &&  r.getIsAC())
                          || (acPref == "NonAC" && !r.getIsAC()));
        if (r.checkAvailability() && typeMatch && acMatch) {
            r.displayRoom();
            found = true;
        }
    }
    if (!found)
        cout << "No available rooms found for the given criteria." << endl;
}

int Customer::bookRoom(int roomNumber,
                       const string& checkIn,
                       const string& checkOut) {
    Reservation res(customerId, roomNumber, checkIn, checkOut);
    if (res.createReservation()) {
        res.saveToFile();
        cout << "Room " << roomNumber << " booked successfully! "
             << "Reservation ID: " << res.getReservationId() << endl;
        return res.getReservationId();
    }
    cout << "Booking failed. Room may not be available." << endl;
    return -1;
}

bool Customer::cancelReservation(int reservationId) {
    Reservation res = Reservation::findById(reservationId);
    if (res.getReservationId() == reservationId &&
        res.getCustomerId()    == customerId) {
        if (res.cancelReservation()) {
            Reservation::updateStatusInFile(reservationId, "Cancelled");
            cout << "Reservation " << reservationId
                 << " cancelled successfully." << endl;
            return true;
        }
    }
    cout << "Cancellation failed. Reservation not found or unauthorized." << endl;
    return false;
}

void Customer::viewBooking(int reservationId) {
    Reservation res = Reservation::findById(reservationId);
    if (res.getReservationId() == reservationId)
        res.displayReservation();
    else
        cout << "Reservation not found." << endl;
}

void Customer::viewBookingHistory() {
    cout << "\n=== Booking History for " << name << " ===" << endl;
    BookingHistory::searchBookingHistory(customerId);
}

void Customer::displayInfo() const {
    cout << "=== Customer Info ===" << endl;
    cout << "ID      : " << customerId << endl;
    cout << "Name    : " << name       << endl;
    cout << "Phone   : " << phone      << endl;
    cout << "Email   : " << email      << endl;
    cout << "Address : " << address    << endl;
    cout << "Username: " << getUsername() << endl;
}

// ─── File I/O ─────────────────────────────────────────────────────────────────

void Customer::saveToFile() const {
    ofstream file("data/users.txt", ios::app);
    if (file.is_open()) {
        file << customerId << "|" << getUsername() << "|" << getPassword() << "|"
             << name << "|" << phone << "|" << email << "|" << address
             << "|Customer\n";
        file.close();
    } else {
        cout << "Error: Unable to open users.txt for writing." << endl;
    }
}

vector<Customer> Customer::loadFromFile() {
    vector<Customer> customers;
    ifstream file("data/users.txt");
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() == 8 && tokens[7] == "Customer") {
            Customer c(tokens[1], tokens[2], tokens[3],
                       tokens[4], tokens[5], tokens[6]);
            customers.push_back(c);
        }
    }
    return customers;
}