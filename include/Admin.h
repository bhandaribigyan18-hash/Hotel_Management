#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include <string>
#include <vector>
using namespace std;

class Admin : public User {
private:
    int adminId;
    static int nextId;

public:
    Admin();
    Admin(string username, string password);
    ~Admin();

    // Getters
    int getAdminId() const;

    // Room Management
    void addRoom(int roomNumber, const string& roomType, double price, int maxGuests, bool isAC);
    void removeRoom(int roomNumber);
    void updateRoom(int roomNumber, const string& field, const string& newValue);

    // Reservation Management
    void manageReservation(int reservationId, const string& action);

    // User Management  ← NEW
    void viewAllUsers();
    void removeUser(const string& username);
    void searchUser(const string& username);
    void viewUserBookings(const string& username);

    // Report
    void generateReport();

    // File I/O
    void saveToFile() const;
    static Admin loadFromFile(const string& username);
};

#endif // ADMIN_H