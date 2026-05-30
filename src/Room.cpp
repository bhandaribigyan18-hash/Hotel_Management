#include "../include/Room.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;



Room::Room()
    : roomNumber(0), roomType(""), price(0.0),
      isAvailable(true), maxGuests(1) {}

Room::Room(int roomNumber, const string& roomType, double price, int maxGuests)
    : roomNumber(roomNumber), roomType(roomType), price(price),
      isAvailable(true), maxGuests(maxGuests) {}

Room::~Room() {}



int    Room::getRoomNumber()  const { return roomNumber;  }
string Room::getRoomType()    const { return roomType;    }
double Room::getPrice()       const { return price;       }
bool   Room::getIsAvailable() const { return isAvailable; }
int    Room::getMaxGuests()   const { return maxGuests;   }



void Room::setRoomType(const string& t) { roomType  = t; }
void Room::setPrice(double p)           { price     = p; }
void Room::setMaxGuests(int g)          { maxGuests = g; }



bool Room::checkAvailability() const {
    return isAvailable;
}

void Room::updateAvailability(bool available) {
    isAvailable = available;

  
    vector<Room> rooms = Room::loadFromFile();
    ofstream file("data/rooms.txt", ios::trunc);
    for (Room& r : rooms) {
        if (r.roomNumber == roomNumber)
            r.isAvailable = available;
        file << r.roomNumber  << "|" << r.roomType    << "|"
             << r.price       << "|" << r.isAvailable << "|"
             << r.maxGuests   << "\n";
    }
}

void Room::displayRoom() const {
    cout << "-----------------------------" << endl;
    cout << "Room Number  : " << roomNumber             << endl;
    cout << "Type         : " << roomType               << endl;
    cout << "Price/Night  : $" << price                 << endl;
    cout << "Max Guests   : " << maxGuests              << endl;
    cout << "Availability : " << (isAvailable ? "Available" : "Occupied") << endl;
}



void Room::saveToFile() const {
    ofstream file("data/rooms.txt", ios::app);
    if (file.is_open()) {
        file << roomNumber << "|" << roomType   << "|"
             << price      << "|" << isAvailable << "|"
             << maxGuests  << "\n";
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

        if (tokens.size() == 5) {
            Room r(stoi(tokens[0]), tokens[1], stod(tokens[2]), stoi(tokens[4]));
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
            file << r.roomNumber << "|" << r.roomType   << "|"
                 << r.price      << "|" << r.isAvailable << "|"
                 << r.maxGuests  << "\n";
}