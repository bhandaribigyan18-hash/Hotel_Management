#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include <string>
using namespace std;

class Admin : public User {
private:
    int adminId;
    static int nextId;

public:
    Admin();
    Admin(string username, string password);
    ~Admin();

  
    int getAdminId() const;

    
    void addRoom(int roomNumber, const string& roomType, double price, int maxGuests);
    void removeRoom(int roomNumber);
    void updateRoom(int roomNumber, const string& field, const string& newValue);
    void manageReservation(int reservationId, const string& action);
    void generateReport();

    void displayInfo() const override;

    void saveToFile() const;
    static Admin loadFromFile(const string& username);
};

#endif 