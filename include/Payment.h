#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>
#include <vector>
using namespace std;

class Payment {
private:
    int    paymentId;
    double amount;
    string paymentMethod;   // "Cash", "Credit Card", "Online"
    string paymentStatus;   // "Pending", "Completed", "Refunded"
    string paymentDate;
    string cardNumber;      // NEW: last 4 digits of card (masked)
    string transactionId;   // NEW: online transaction confirmation code

    int    reservationId;

    static int nextId;

    
    bool processCash();
    bool processCreditCard();
    bool processOnline();

    static string generateConfirmationCode();
    static bool   isValidCardNumber(const string& number);
    static string maskCardNumber(const string& number);

public:
    Payment();
    Payment(int reservationId, double amount,
            const string& method, const string& date);
    ~Payment();

    
    int    getPaymentId()      const;
    double getAmount()         const;
    string getPaymentMethod()  const;
    string getPaymentStatus()  const;
    string getPaymentDate()    const;
    string getCardNumber()     const;
    string getTransactionId()  const;
    int    getReservationId()  const;

    
    void setPaymentStatus(const string& status);

    
    bool processPayment();
    void generateReceipt() const;
    bool refundPayment();

    
    void saveToFile() const;
    static vector<Payment> loadFromFile();
    static Payment findByReservationId(int reservationId);
    static void updateStatusInFile(int paymentId, const string& status);
};

#endif 