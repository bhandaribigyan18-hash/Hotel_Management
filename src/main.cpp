#include "../include/User.h"
#include "../include/Customer.h"
#include "../include/Admin.h"
#include "../include/Room.h"
#include "../include/Reservation.h"
#include "../include/Payment.h"
#include "../include/BookingHistory.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;



string currentDate() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    ostringstream ss;
    ss << put_time(t, "%Y-%m-%d");
    return ss.str();
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}



void customerMenu(Customer& customer) {
    int choice;
    do {
        cout << "\n===== Customer Menu =====" << endl;
        cout << "1. Search Rooms"           << endl;
        cout << "2. Book a Room"            << endl;
        cout << "3. Cancel Reservation"     << endl;
        cout << "4. View Booking"           << endl;
        cout << "5. View Booking History"   << endl;
        cout << "6. My Profile"             << endl;
        cout << "0. Logout"                 << endl;
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            string type, checkIn, checkOut;
            cout << "Room type (Single/Double/Suite/Any): ";
            getline(cin, type);
            cout << "Check-in  (YYYY-MM-DD): ";
            getline(cin, checkIn);
            cout << "Check-out (YYYY-MM-DD): ";
            getline(cin, checkOut);
            customer.searchRoom(type, checkIn, checkOut);

        } else if (choice == 2) {
            int roomNum;
            string checkIn, checkOut;
            cout << "Room number   : ";
            cin >> roomNum;
            clearInput();
            cout << "Check-in  (YYYY-MM-DD): ";
            getline(cin, checkIn);
            cout << "Check-out (YYYY-MM-DD): ";
            getline(cin, checkOut);
            int resId = customer.bookRoom(roomNum, checkIn, checkOut);
            if (resId != -1) {
                // Fetch price for payment
                vector<Room> rooms = Room::loadFromFile();
                double price = 0;
                for (const Room& r : rooms)
                    if (r.getRoomNumber() == roomNum)
                        price = r.getPrice();

                Reservation res = Reservation::findById(resId);
                double bill = res.calculateBill(price);
                cout << "Total Bill: $" << bill << endl;

                cout << "Payment method (Cash/Credit Card/Online): ";
                string method;
                getline(cin, method);
                Payment pay(resId, bill, method, currentDate());
                pay.processPayment();

                BookingHistory hist(customer.getCustomerId(), resId,
                                    currentDate(), "Confirmed");
                hist.saveBookingHistory();
            }

        } else if (choice == 3) {
            int resId;
            cout << "Reservation ID to cancel: ";
            cin >> resId;
            clearInput();
            customer.cancelReservation(resId);
            BookingHistory hist(customer.getCustomerId(), resId,
                                currentDate(), "Cancelled");
            hist.saveBookingHistory();

        } else if (choice == 4) {
            int resId;
            cout << "Reservation ID: ";
            cin >> resId;
            clearInput();
            customer.viewBooking(resId);

        } else if (choice == 5) {
            customer.viewBookingHistory();

        } else if (choice == 6) {
            customer.displayInfo();
        }

    } while (choice != 0);
    customer.logout();
}



void adminMenu(Admin& admin) {
    int choice;
    do {
        cout << "\n===== Admin Menu =====" << endl;
        cout << "1. Add Room"              << endl;
        cout << "2. Remove Room"           << endl;
        cout << "3. Update Room"           << endl;
        cout << "4. Manage Reservation"    << endl;
        cout << "5. Generate Report"       << endl;
        cout << "6. My Profile"            << endl;
        cout << "0. Logout"               << endl;
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            int roomNum, maxGuests;
            double price;
            string type;
            cout << "Room number : "; cin >> roomNum;    clearInput();
            cout << "Room type   : "; getline(cin, type);
            cout << "Price/night : "; cin >> price;      clearInput();
            cout << "Max guests  : "; cin >> maxGuests;  clearInput();
            admin.addRoom(roomNum, type, price, maxGuests);

        } else if (choice == 2) {
            int roomNum;
            cout << "Room number to remove: ";
            cin >> roomNum;
            clearInput();
            admin.removeRoom(roomNum);

        } else if (choice == 3) {
            int roomNum;
            string field, value;
            cout << "Room number     : "; cin >> roomNum;  clearInput();
            cout << "Field (price/type/maxGuests): "; getline(cin, field);
            cout << "New value       : "; getline(cin, value);
            admin.updateRoom(roomNum, field, value);

        } else if (choice == 4) {
            int resId;
            string action;
            cout << "Reservation ID  : "; cin >> resId;   clearInput();
            cout << "Action (Confirmed/Cancelled/Completed): ";
            getline(cin, action);
            admin.manageReservation(resId, action);

        } else if (choice == 5) {
            admin.generateReport();

        } else if (choice == 6) {
            admin.displayInfo();
        }

    } while (choice != 0);
    admin.logout();
}



int main() {
    cout << "=====================================" << endl;
    cout << "   Hotel Reservation System          " << endl;
    cout << "=====================================" << endl;
   

    int mainChoice;
    do {
        cout << "\n1. Login as Customer"   << endl;
        cout << "2. Register as Customer" << endl;
        cout << "3. Login as Admin"       << endl;
        cout << "0. Exit"                 << endl;
        cout << "Choice: ";
        cin >> mainChoice;
        clearInput();

        if (mainChoice == 1) {
            
            string username, password;
            cout << "Username: "; getline(cin, username);
            cout << "Password: "; getline(cin, password);

            vector<Customer> customers = Customer::loadFromFile();
            bool found = false;
            for (Customer& c : customers) {
                if (c.login(username, password)) {
                    found = true;
                    customerMenu(c);
                    break;
                }
            }
            if (!found)
                cout << "Login failed. Customer not found." << endl;

        } else if (mainChoice == 2) {
           
            string username, password, name, phone, email, address;
            cout << "Username : "; getline(cin, username);
            cout << "Password : "; getline(cin, password);
            cout << "Name     : "; getline(cin, name);
            cout << "Phone    : "; getline(cin, phone);
            cout << "Email    : "; getline(cin, email);
            cout << "Address  : "; getline(cin, address);

            Customer c(username, password, name, phone, email, address);
            c.saveToFile();
            cout << "Registration successful! You can now log in." << endl;

        } else if (mainChoice == 3) {
            
            string username, password;
            cout << "Admin Username: "; getline(cin, username);
            cout << "Admin Password: "; getline(cin, password);

           
            Admin admin("admin", "admin123");
            if (admin.login(username, password)) {
                adminMenu(admin);
            } else {
                Admin loaded = Admin::loadFromFile(username);
                if (loaded.getAdminId() != 0 &&
                    loaded.login(username, password)) {
                    adminMenu(loaded);
                } else {
                    cout << "Admin login failed." << endl;
                }
            }
        }

    } while (mainChoice != 0);

    cout << "Thank you for using Hotel Reservation System. Goodbye!" << endl;
    return 0;
}