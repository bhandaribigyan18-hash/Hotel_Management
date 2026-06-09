#include "../include/Payment.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <regex>
using namespace std;

int Payment::nextId = 1;



Payment::Payment()
    : paymentId(0), amount(0.0), paymentMethod(""),
      paymentStatus("Pending"), paymentDate(""),
      cardNumber(""), transactionId(""), reservationId(0) {}

Payment::Payment(int reservationId, double amount,
                 const string& method, const string& date)
    : paymentId(nextId++), amount(amount), paymentMethod(method),
      paymentStatus("Pending"), paymentDate(date),
      cardNumber(""), transactionId(""), reservationId(reservationId) {}

Payment::~Payment() {}



int    Payment::getPaymentId()     const { return paymentId;     }
double Payment::getAmount()        const { return amount;        }
string Payment::getPaymentMethod() const { return paymentMethod; }
string Payment::getPaymentStatus() const { return paymentStatus; }
string Payment::getPaymentDate()   const { return paymentDate;   }
string Payment::getCardNumber()    const { return cardNumber;    }
string Payment::getTransactionId() const { return transactionId; }
int    Payment::getReservationId() const { return reservationId; }



void Payment::setPaymentStatus(const string& s) { paymentStatus = s; }



string Payment::generateConfirmationCode() {
    srand(static_cast<unsigned>(time(nullptr)));
    string code = "TXN";
    for (int i = 0; i < 8; i++)
        code += to_string(rand() % 10);
    return code;
}

bool Payment::isValidCardNumber(const string& number) {
    // Must be exactly 16 digits
    if (number.length() != 16) return false;
    for (char c : number)
        if (!isdigit(c)) return false;

    // Luhn algorithm check
    int sum = 0;
    bool alternate = false;
    for (int i = number.length() - 1; i >= 0; i--) {
        int n = number[i] - '0';
        if (alternate) {
            n *= 2;
            if (n > 9) n -= 9;
        }
        sum += n;
        alternate = !alternate;
    }
    return (sum % 10 == 0);
}

string Payment::maskCardNumber(const string& number) {
    // Show only last 4 digits: **** **** **** 1234
    return "**** **** **** " + number.substr(12, 4);
}



bool Payment::processCash() {
    cout << "\n💵 Cash Payment" << endl;
    cout << "-----------------------------" << endl;
    cout << "Amount Due : $" << amount << endl;
    cout << "Please collect $" << amount << " from the customer." << endl;

    string confirm;
    cout << "Confirm cash received? (yes/no): ";
    getline(cin, confirm);

    if (confirm == "yes" || confirm == "Yes" || confirm == "YES") {
        paymentStatus = "Completed";
        cout << "✅ Cash payment confirmed." << endl;
        return true;
    }
    cout << "❌ Cash payment not confirmed. Booking cancelled." << endl;
    return false;
}

bool Payment::processCreditCard() {
    cout << "\n💳 Credit Card Payment" << endl;
    cout << "-----------------------------" << endl;
    cout << "Amount to charge: $" << amount << endl;

    string number;
    int attempts = 0;

    
    while (attempts < 3) {
        cout << "Enter 16-digit card number: ";
        getline(cin, number);

       
        number.erase(remove(number.begin(), number.end(), ' '), number.end());

        if (!isValidCardNumber(number)) {
            attempts++;
            if (attempts < 3)
                cout << "❌ Invalid card number. Please try again. ("
                     << (3 - attempts) << " attempts left)" << endl;
            else {
                cout << "❌ Too many failed attempts. Payment cancelled." << endl;
                return false;
            }
        } else {
            break;
        }
    }

   
    cardNumber = maskCardNumber(number);
    cout << "\nCard     : " << cardNumber << endl;
    cout << "Amount   : $" << amount     << endl;

    
    string expiry, cvv;
    cout << "Expiry (MM/YY): ";
    getline(cin, expiry);
    cout << "CVV           : ";
    getline(cin, cvv);

    
    string confirm;
    cout << "\nConfirm payment of $" << amount
         << " on card " << cardNumber << "? (yes/no): ";
    getline(cin, confirm);

    if (confirm == "yes" || confirm == "Yes" || confirm == "YES") {
        transactionId = generateConfirmationCode();
        paymentStatus = "Completed";
        cout << "✅ Credit card payment approved!" << endl;
        cout << "   Transaction ID : " << transactionId << endl;
        return true;
    }
    cout << "❌ Payment cancelled by user." << endl;
    return false;
}

bool Payment::processOnline() {
    cout << "\n🌐 Online Payment" << endl;
    cout << "-----------------------------" << endl;
    cout << "Amount to pay: $" << amount << endl;

    cout << "\nSelect online payment method:" << endl;
    cout << "1. UPI / Mobile Wallet"         << endl;
    cout << "2. Net Banking"                  << endl;
    cout << "3. QR Code"                      << endl;
    cout << "Choice: ";

    int onlineChoice;
    cin >> onlineChoice;
    cin.ignore();

    string paymentDetail;
    if (onlineChoice == 1) {
        cout << "Enter UPI ID / Wallet number: ";
        getline(cin, paymentDetail);
        cout << "\n📱 Payment request of $" << amount
             << " sent to " << paymentDetail << endl;
    } else if (onlineChoice == 2) {
        cout << "Enter Bank Account / Net Banking ID: ";
        getline(cin, paymentDetail);
        cout << "\n🏦 Net banking transfer of $" << amount
             << " initiated for " << paymentDetail << endl;
    } else {
        cout << "\n📷 Please scan the QR code to pay $" << amount << endl;
        paymentDetail = "QR Code";
    }

   
    transactionId = generateConfirmationCode();
    cout << "\n⏳ Processing payment..." << endl;
    cout << "✅ Payment successful!" << endl;
    cout << "   A confirmation message has been sent to your registered" << endl;
    cout << "   mobile number / email." << endl;
    cout << "   Confirmation Code : " << transactionId << endl;

    string confirm;
    cout << "\nEnter the confirmation code to verify: ";
    getline(cin, confirm);

    if (confirm == transactionId) {
        paymentStatus = "Completed";
        cout << "✅ Online payment verified and completed!" << endl;
        return true;
    }
    cout << "❌ Confirmation code mismatch. Payment not verified." << endl;
    return false;
}



bool Payment::processPayment() {
    if (amount <= 0) {
        cout << "❌ Invalid payment amount." << endl;
        return false;
    }

    bool success = false;

    if (paymentMethod == "Cash" || paymentMethod == "cash")
        success = processCash();
    else if (paymentMethod == "Credit Card" || paymentMethod == "credit card")
        success = processCreditCard();
    else if (paymentMethod == "Online" || paymentMethod == "online")
        success = processOnline();
    else {
        cout << "❌ Unknown payment method: " << paymentMethod << endl;
        return false;
    }

    if (success) {
        saveToFile();
        generateReceipt();
    }
    return success;
}

void Payment::generateReceipt() const {
    cout << "\n========== Payment Receipt ==========" << endl;
    cout << "Payment ID        : " << paymentId    << endl;
    cout << "Reservation ID    : " << reservationId << endl;
    cout << "Amount Paid       : $" << amount       << endl;
    cout << "Payment Method    : " << paymentMethod << endl;
    if (!cardNumber.empty())
        cout << "Card Number       : " << cardNumber  << endl;
    if (!transactionId.empty())
        cout << "Transaction ID    : " << transactionId << endl;
    cout << "Payment Status    : " << paymentStatus << endl;
    cout << "Payment Date      : " << paymentDate   << endl;
    cout << "=====================================" << endl;
}

bool Payment::refundPayment() {
    if (paymentStatus != "Completed") {
        cout << "❌ Cannot refund. Payment is not completed." << endl;
        return false;
    }
    paymentStatus = "Refunded";
    Payment::updateStatusInFile(paymentId, "Refunded");
    cout << "✅ Refund of $" << amount
         << " for Payment ID " << paymentId
         << " processed successfully." << endl;
    return true;
}



void Payment::saveToFile() const {
    ofstream file("data/payments.txt", ios::app);
    if (file.is_open()) {
        file << paymentId     << "|" << reservationId  << "|"
             << amount        << "|" << paymentMethod  << "|"
             << paymentStatus << "|" << paymentDate    << "|"
             << cardNumber    << "|" << transactionId  << "\n";
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

        if (tokens.size() >= 6) {
            Payment p(stoi(tokens[1]), stod(tokens[2]), tokens[3], tokens[5]);
            p.paymentId     = stoi(tokens[0]);
            p.paymentStatus = tokens[4];
            if (tokens.size() >= 7) p.cardNumber    = tokens[6];
            if (tokens.size() >= 8) p.transactionId = tokens[7];
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
        file << p.paymentId     << "|" << p.reservationId  << "|"
             << p.amount        << "|" << p.paymentMethod  << "|"
             << p.paymentStatus << "|" << p.paymentDate    << "|"
             << p.cardNumber    << "|" << p.transactionId  << "\n";
    }
}