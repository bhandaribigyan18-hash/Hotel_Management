#include "../include/Room.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;



Room::Room()
    : roomNumber(0), roomType(""), price(0.0),
      isAvailable(true), maxGuests(1), isAC(false) {}

Room::Room(int roomNumber, const string& roomType, double price,
           int maxGuests, bool isAC)
    : roomNumber(roomNumber), roomType(roomType), price(price),
      isAvailable(true), maxGuests(maxGuests), isAC(isAC) {}

Room::~Room() {}



int    Room::getRoomNumber()  const { return roomNumber;  }
string Room::getRoomType()    const { return roomType;    }
double Room::getPrice()       const { return price;       }
bool   Room::getIsAvailable() const { return isAvailable; }
int    Room::getMaxGuests()   const { return maxGuests;   }
bool   Room::getIsAC()        const { return isAC;        }



void Room::setRoomType(const string& t) { roomType  = t; }
void Room::setPrice(double p)           { price     = p; }
void Room::setMaxGuests(int g)          { maxGuests = g; }
void Room::setIsAC(bool ac)             { isAC      = ac; }



bool Room::checkAvailability() const {
    return isAvailable;
}

void Room::updateAvailability(bool available) {
    isAvailable = available;

    // Sync change to file
    vector<Room> rooms = Room::loadFromFile();
    ofstream file("data/rooms.txt", ios::trunc);
    for (Room& r : rooms) {
        if (r.roomNumber == roomNumber)
            r.isAvailable = available;
        file << r.roomNumber  << "|" << r.roomType    << "|"
             << r.price       << "|" << r.isAvailable << "|"
             << r.maxGuests   << "|" << (r.isAC ? "AC" : "NonAC") << "\n";
    }
}

void Room::displayRoom() const {
    cout << "-----------------------------"                              << endl;
    cout << "Room Number  : " << roomNumber                             << endl;
    cout << "Type         : " << roomType                               << endl;
    cout << "AC           : " << (isAC ? "AC" : "Non-AC")              << endl;
    cout << "Price/Night  : $" << price                                 << endl;
    cout << "Max Guests   : " << maxGuests                              << endl;
    cout << "Availability : " << (isAvailable ? "Available" : "Occupied") << endl;
}

// ─── File I/O ─────────────────────────────────────────────────────────────────

void Room::saveToFile() const {
    ofstream file("data/rooms.txt", ios::app);
    if (file.is_open()) {
        file << roomNumber << "|" << roomType    << "|"
             << price      << "|" << isAvailable << "|"
             << maxGuests  << "|" << (isAC ? "AC" : "NonAC") << "\n";
        file.close();
    } else {
        cout << "Error: Unable to open rooms.txt for writing." << endl;
    }
}

vector<Room> Room::loadFromFile() {
    vector<Room> rooms;
    ifstream file("data/rooms.txt");
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() == 6) {
            bool ac = (tokens[5] == "AC" || tokens[5] == "ac" || tokens[5] == "1");
            Room r(stoi(tokens[0]), tokens[1], stod(tokens[2]),
                   stoi(tokens[4]), ac);
            r.isAvailable = (tokens[3] == "1");
            rooms.push_back(r);
        }
    }
    return rooms;
}

void Room::removeFromFile(int roomNumber) {
    vector<Room> rooms = loadFromFile();
    ofstream file("data/rooms.txt", ios::trunc);
    for (const Room& r : rooms)
        if (r.roomNumber != roomNumber)
            file << r.roomNumber  << "|" << r.roomType    << "|"
                 << r.price       << "|" << r.isAvailable << "|"
                 << r.maxGuests   << "|" << (r.isAC ? "AC" : "NonAC") << "\n";
}

// ─── Validation ───────────────────────────────────────────────────────────────

bool Room::isValidRoomNumber(int roomNumber) {
    // Rule 1 & 2: Must be between 100 and 999
    if (roomNumber < 100 || roomNumber > 999) {
        cout << "❌ Invalid room number: " << roomNumber
             << ". Must be between 100 and 999." << endl;
        return false;
    }

    // Rule 3: Floor digit (1st digit) 1-9, room digit (last 2) 01-10
    int floor  = roomNumber / 100;
    int roomNo = roomNumber % 100;

    if (floor < 1 || floor > 9) {
        cout << "❌ Invalid floor digit: " << floor
             << ". Floor must be 1 to 9." << endl;
        return false;
    }
    if (roomNo < 1 || roomNo > 10) {
        cout << "❌ Invalid room digit: " << roomNo
             << ". Room position must be 01 to 10 (e.g. 101, 205, 310)." << endl;
        return false;
    }

    return true;
}

bool Room::isDuplicateRoomNumber(int roomNumber) {
    // Rule 4: No duplicate room numbers
    vector<Room> rooms = loadFromFile();
    for (const Room& r : rooms) {
        if (r.roomNumber == roomNumber) {
            cout << "❌ Duplicate: Room " << roomNumber
                 << " already exists." << endl;
            return true;
        }
    }
    return false;
}