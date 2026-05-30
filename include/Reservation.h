#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
#include <vector>
using namespace std;

class Reservation {
private:
    int    reservationId;
    string checkInDate;
    string checkOutDate;
    string reservationStatus;   // "Confirmed", "Cancelled", "Completed"
    int    totalDays;

    int    customerId;
    int    roomNumber;

    static int nextId;

public:
    Reservation();
    Reservation(int customerId, int roomNumber,
                const string& checkIn, const string& checkOut);
    ~Reservation();

    
    
    int    getReservationId()     const;
    string getCheckInDate()       const;
    string getCheckOutDate()      const;
    string getReservationStatus() const;
    int    getTotalDays()         const;
    int    getCustomerId()        const;
    int    getRoomNumber()        const;

    
    
    void setReservationStatus(const string& status);

    bool createReservation();
    bool cancelReservation();
    double calculateBill(double pricePerNight) const;
    void displayReservation() const;

    
    void saveToFile() const;
    static vector<Reservation> loadFromFile();
    static Reservation findById(int reservationId);
    static void updateStatusInFile(int reservationId, const string& status);

private:
    int calculateTotalDays(const string& checkIn, const string& checkOut) const;
};

#endif 