#include "../include/Reservation.h"
#include "../include/Room.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <stdexcept>
using namespace std;

int Reservation::nextId = 1;



Reservation::Reservation()
    : reservationId(0), checkInDate(""), checkOutDate(""),
      reservationStatus("Pending"), totalDays(0),
      customerId(0), roomNumber(0) {}

Reservation::Reservation(int customerId, int roomNumber,
                         const string& checkIn, const string& checkOut)
    : reservationId(nextId++), checkInDate(checkIn), checkOutDate(checkOut),
      reservationStatus("Confirmed"),
      totalDays(calculateTotalDays(checkIn, checkOut)),
      customerId(customerId), roomNumber(roomNumber) {}

Reservation::~Reservation() {}



int    Reservation::getReservationId()     const { return reservationId;     }
string Reservation::getCheckInDate()       const { return checkInDate;       }
string Reservation::getCheckOutDate()      const { return checkOutDate;      }
string Reservation::getReservationStatus() const { return reservationStatus; }
int    Reservation::getTotalDays()         const { return totalDays;         }
int    Reservation::getCustomerId()        const { return customerId;         }
int    Reservation::getRoomNumber()        const { return roomNumber;         }


void Reservation::setReservationStatus(const string& s) { reservationStatus = s; }



bool Reservation::createReservation() {
   
    vector<Room> rooms = Room::loadFromFile();
    for (Room& r : rooms) {
        if (r.getRoomNumber() == roomNumber) {
            if (!r.checkAvailability()) {
                cout << "Room " << roomNumber << " is not available." << endl;
                return false;
            }
            r.updateAvailability(false);   // Mark as occupied
            reservationStatus = "Confirmed";
            cout << "Reservation created successfully." << endl;
            return true;
        }
    }
    cout << "Room " << roomNumber << " not found." << endl;
    return false;
}

bool Reservation::cancelReservation() {
    if (reservationStatus == "Cancelled") {
        cout << "Reservation is already cancelled." << endl;
        return false;
    }
    reservationStatus = "Cancelled";

    
    vector<Room> rooms = Room::loadFromFile();
    for (Room& r : rooms)
        if (r.getRoomNumber() == roomNumber)
            r.updateAvailability(true);

    cout << "Reservation cancelled successfully." << endl;
    return true;
}

double Reservation::calculateBill(double pricePerNight) const {
    return pricePerNight * totalDays;
}

void Reservation::displayReservation() const {
    cout << "=== Reservation Details ===" << endl;
    cout << "Reservation ID : " << reservationId     << endl;
    cout << "Customer ID    : " << customerId         << endl;
    cout << "Room Number    : " << roomNumber         << endl;
    cout << "Check-in       : " << checkInDate        << endl;
    cout << "Check-out      : " << checkOutDate       << endl;
    cout << "Total Days     : " << totalDays          << endl;
    cout << "Status         : " << reservationStatus  << endl;
}



void Reservation::saveToFile() const {
    ofstream file("data/reservations.txt", ios::app);
    if (file.is_open()) {
        file << reservationId    << "|" << customerId        << "|"
             << roomNumber       << "|" << checkInDate       << "|"
             << checkOutDate     << "|" << reservationStatus << "|"
             << totalDays        << "\n";
        file.close();
    } else {
        cout << "Error: Unable to open reservations.txt for writing." << endl;
    }
}

vector<Reservation> Reservation::loadFromFile() {
    vector<Reservation> reservations;
    ifstream file("data/reservations.txt");
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() == 7) {
            Reservation r(stoi(tokens[1]), stoi(tokens[2]), tokens[3], tokens[4]);
            r.reservationId     = stoi(tokens[0]);
            r.reservationStatus = tokens[5];
            r.totalDays         = stoi(tokens[6]);
            reservations.push_back(r);
        }
    }
    return reservations;
}

Reservation Reservation::findById(int id) {
    vector<Reservation> all = loadFromFile();
    for (const Reservation& r : all)
        if (r.reservationId == id)
            return r;
    return Reservation();
}

void Reservation::updateStatusInFile(int id, const string& status) {
    vector<Reservation> all = loadFromFile();
    ofstream file("data/reservations.txt", ios::trunc);
    for (Reservation& r : all) {
        if (r.reservationId == id)
            r.reservationStatus = status;
        file << r.reservationId    << "|" << r.customerId        << "|"
             << r.roomNumber       << "|" << r.checkInDate       << "|"
             << r.checkOutDate     << "|" << r.reservationStatus << "|"
             << r.totalDays        << "\n";
    }
}


int Reservation::calculateTotalDays(const string& checkIn,
                                    const string& checkOut) const {
    // Expects format YYYY-MM-DD
    auto toTm = [](const string& date) -> tm {
        tm t = {};
        istringstream ss(date);
        ss >> get_time(&t, "%Y-%m-%d");
        return t;
    };
    tm in  = toTm(checkIn);
    tm out = toTm(checkOut);
    time_t t1 = mktime(&in);
    time_t t2 = mktime(&out);
    double diff = difftime(t2, t1);
    int days = static_cast<int>(diff / 86400);
    return (days > 0) ? days : 1;
}