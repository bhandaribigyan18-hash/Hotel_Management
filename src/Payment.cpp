#include "../include/Payment.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int Payment::nextId = 1;



Payment::Payment()
    : paymentId(0), amount(0.0), paymentMethod(""),
      paymentStatus("Pending"), paymentDate(""), reservationId(0) {}

Payment::Payment(int reservationId, double amount,
                 const string& method, const string& date)
    : paymentId(nextId++), amount(amount), paymentMethod(method),
      paymentStatus("Pending"), paymentDate(date),
      reservationId(reservationId) {}

Payment::~Payment() {}



int    Payment::getPaymentId()     const { return paymentId;     }
double Payment::getAmount()        const { return amount;        }
string Payment::getPaymentMethod() const { return paymentMethod; }
string Payment::getPaymentStatus() const { return paymentStatus; }
string Payment::getPaymentDate()   const { return paymentDate;   }
int    Payment::getReservationId() const { return reservationId; }



void Payment::setPaymentStatus(const string& s) { paymentStatus = s; }



bool Payment::processPayment() {
    if (amount <= 0) {
        cout << "Invalid payment amount." << endl;
        return false;
    }
    paymentStatus = "Completed";
    saveToFile();
    cout << "Payment of $" << amount
         << " processed successfully via " << paymentMethod << "." << endl;
    generateReceipt();
    return true;
}

void Payment::generateReceipt() const {
    cout << "\n========== Payment Receipt ==========" << endl;
    cout << "Payment ID        : " << paymentId     << endl;
    cout << "Reservation ID    : " << reservationId  << endl;
    cout << "Amount Paid       : $" << amount        << endl;
    cout << "Payment Method    : " << paymentMethod  << endl;
    cout << "Payment Status    : " << paymentStatus  << endl;
    cout << "Payment Date      : " << paymentDate    << endl;
    cout << "=====================================" << endl;
}

bool Payment::refundPayment() {
    if (paymentStatus != "Completed") {
        cout << "Cannot refund. Payment is not completed." << endl;
        return false;
    }
    paymentStatus = "Refunded";
    Payment::updateStatusInFile(paymentId, "Refunded");
    cout << "Refund of $" << amount
         << " for Payment ID " << paymentId
         << " processed successfully." << endl;
    return true;
}


void Payment::saveToFile() const {
    ofstream file("data/payments.txt", ios::app);
    if (file.is_open()) {
        file << paymentId      << "|" << reservationId << "|"
             << amount         << "|" << paymentMethod << "|"
             << paymentStatus  << "|" << paymentDate   << "\n";
        file.close();
    } else {
        cout << "Error: Unable to open payments.txt for writing." << endl;
    }
}

vector<Payment> Payment::loadFromFile() {
    vector<Payment> payments;
    ifstream file("data/payments.txt");
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|'))
            tokens.push_back(token);

        if (tokens.size() == 6) {
            Payment p(stoi(tokens[1]), stod(tokens[2]), tokens[3], tokens[5]);
            p.paymentId     = stoi(tokens[0]);
            p.paymentStatus = tokens[4];
            payments.push_back(p);
        }
    }
    return payments;
}

Payment Payment::findByReservationId(int resId) {
    vector<Payment> all = loadFromFile();
    for (const Payment& p : all)
        if (p.reservationId == resId)
            return p;
    return Payment();
}

void Payment::updateStatusInFile(int id, const string& status) {
    vector<Payment> all = loadFromFile();
    ofstream file("data/payments.txt", ios::trunc);
    for (Payment& p : all) {
        if (p.paymentId == id)
            p.paymentStatus = status;
        file << p.paymentId     << "|" << p.reservationId << "|"
             << p.amount        << "|" << p.paymentMethod << "|"
             << p.paymentStatus << "|" << p.paymentDate   << "\n";
    }
}