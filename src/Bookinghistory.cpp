#include "../include/BookingHistory.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int BookingHistory::nextId = 1;



BookingHistory::BookingHistory()
    : historyId(0), bookingDate(""), bookingStatus(""),
      customerId(0), reservationId(0) {}

BookingHistory::BookingHistory(int customerId, int reservationId,
                               const string& bookingDate,
                               const string& bookingStatus)
    : historyId(nextId++), bookingDate(bookingDate),
      bookingStatus(bookingStatus),
      customerId(customerId), reservationId(reservationId) {}

BookingHistory::~BookingHistory() {}



int    BookingHistory::getHistoryId()     const { return historyId;     }
string BookingHistory::getBookingDate()   const { return bookingDate;   }
string BookingHistory::getBookingStatus() const { return bookingStatus; }
int    BookingHistory::getCustomerId()    const { return customerId;    }
int    BookingHistory::getReservationId() const { return reservationId; }



void BookingHistory::saveBookingHistory() {
    ofstream file("data/booking_history.txt", ios::app);
    if (file.is_open()) {
        file << historyId     << "|" << customerId    << "|"
             << reservationId << "|" << bookingDate   << "|"
             << bookingStatus << "\n";
        file.close();
        cout << "Booking history saved successfully." << endl;
    } else {
        cout << "Error: Unable to open booking_history.txt for writing." << endl;
    }
}

void BookingHistory::displayBookingHistory() const {
    cout << "-----------------------------" << endl;
    cout << "History ID     : " << historyId     << endl;
    cout << "Customer ID    : " << customerId    << endl;
    cout << "Reservation ID : " << reservationId << endl;
    cout << "Booking Date   : " << bookingDate   << endl;
    cout << "Status         : " << bookingStatus << endl;
}

void BookingHistory::searchBookingHistory(int custId) {
    vector<BookingHistory> history = loadByCustomerId(custId);
    if (history.empty()) {
        cout << "No booking history found for Customer ID: " << custId << endl;
        return;
    }
    cout << "\n=== Booking History (Customer ID: " << custId << ") ===" << endl;
    for (const BookingHistory& h : history)
        h.displayBookingHistory();
}



vector<BookingHistory> BookingHistory::loadFromFile() {
    vector<BookingHistory> history;
    ifstream file("data/booking_history.txt");
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() == 5) {
            BookingHistory h(stoi(tokens[1]), stoi(tokens[2]),
                             tokens[3], tokens[4]);
            h.historyId = stoi(tokens[0]);
            history.push_back(h);
        }
    }
    return history;
}

vector<BookingHistory> BookingHistory::loadByCustomerId(int custId) {
    vector<BookingHistory> all = loadFromFile();
    vector<BookingHistory> filtered;
    for (const BookingHistory& h : all)
        if (h.customerId == custId)
            filtered.push_back(h);
    return filtered;
}