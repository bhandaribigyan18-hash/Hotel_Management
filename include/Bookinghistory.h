#ifndef BOOKINGHISTORY_H
#define BOOKINGHISTORY_H

#include <string>
#include <vector>
using namespace std;

class BookingHistory {
private:
    int    historyId;
    string bookingDate;
    string bookingStatus;

    int    customerId;
    int    reservationId;

    static int nextId;

public:
    BookingHistory();
    BookingHistory(int customerId, int reservationId,
                   const string& bookingDate, const string& bookingStatus);
    ~BookingHistory();

    int    getHistoryId()     const;
    string getBookingDate()   const;
    string getBookingStatus() const;
    int    getCustomerId()    const;
    int    getReservationId() const;

    void saveBookingHistory();
    void displayBookingHistory() const;
    static void searchBookingHistory(int customerId);

    
    static vector<BookingHistory> loadFromFile();
    static vector<BookingHistory> loadByCustomerId(int customerId);
};

#endif 