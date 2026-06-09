#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "User.h"
#include <string>
#include <vector>
using namespace std;

class Customer : public User {
private:
    int customerId;
    string name;
    string phone;
    string email;
    string address;

    static int nextId;

public:
    Customer();
    Customer(string username, string password, string name,
             string phone, string email, string address);
    ~Customer();

    int getCustomerId() const;
    string getName() const;
    string getPhone() const;
    string getEmail() const;
    string getAddress() const;

    
    void setName(const string& name);
    void setPhone(const string& phone);
    void setEmail(const string& email);
    void setAddress(const string& address);

    
    void searchRoom(const string& roomType, const string& checkIn, const string& checkOut, const string& acPref);
    int  bookRoom(int roomNumber, const string& checkIn, const string& checkOut);
    bool cancelReservation(int reservationId);
    void viewBooking(int reservationId);
    void viewBookingHistory();

    void displayInfo() const override;

    
    void saveToFile() const;
    static vector<Customer> loadFromFile();
};

#endif 