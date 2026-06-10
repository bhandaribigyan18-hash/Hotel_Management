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
#include <regex>
using namespace std;

// ─── Helpers ──────────────────────────────────────────────────────────────────

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

// ─── Date Validation ──────────────────────────────────────────────────────────

bool isValidDateFormat(const string& date) {
    // Must match YYYY-MM-DD exactly
    regex pattern("^\\d{4}-\\d{2}-\\d{2}$");
    if (!regex_match(date, pattern)) {
        cout << "❌ Invalid format. Use YYYY-MM-DD (e.g. 2025-06-15)" << endl;
        return false;
    }

    int year  = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day   = stoi(date.substr(8, 2));

    // Month range
    if (month < 1 || month > 12) {
        cout << "❌ Invalid month: " << month << ". Must be 01-12." << endl;
        return false;
    }

    // Day range per month
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    // Leap year check
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (leap) daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1]) {
        cout << "❌ Invalid day: " << day
             << " for month " << month << "." << endl;
        return false;
    }

    // Year range
    if (year < 2024 || year > 2100) {
        cout << "❌ Invalid year: " << year
             << ". Must be between 2024 and 2100." << endl;
        return false;
    }

    return true;
}

bool isCheckOutAfterCheckIn(const string& checkIn, const string& checkOut) {
    if (checkOut <= checkIn) {
        cout << "❌ Check-out date must be after check-in date." << endl;
        return false;
    }
    return true;
}

bool isNotPastDate(const string& date) {
    string today = currentDate();
    if (date < today) {
        cout << "❌ Date " << date << " is in the past. "
             << "Today is " << today << "." << endl;
        return false;
    }
    return true;
}

// Keeps asking until a valid date is entered
string getValidDate(const string& prompt, bool allowPast = false) {
    string date;
    while (true) {
        cout << prompt;
        getline(cin, date);
        if (!isValidDateFormat(date))       continue;
        if (!allowPast && !isNotPastDate(date)) continue;
        break;
    }
    return date;
}

// Keeps asking until check-out is after check-in
string getValidCheckOut(const string& checkIn) {
    string date;
    while (true) {
        cout << "Check-out (YYYY-MM-DD): ";
        getline(cin, date);
        if (!isValidDateFormat(date))              continue;
        if (!isCheckOutAfterCheckIn(checkIn, date)) continue;
        break;
    }
    return date;
}

// ─── Customer Menu ────────────────────────────────────────────────────────────

void customerMenu(Customer& customer) {
    int choice;
    do {
        cout << "\n===== Customer Menu =====" << endl;
        cout << "1. Search Rooms"            << endl;
        cout << "2. Book a Room"             << endl;
        cout << "3. Cancel Reservation"      << endl;
        cout << "4. View Booking"            << endl;
        cout << "5. View Booking History"    << endl;
        cout << "6. My Profile"              << endl;
        cout << "0. Logout"                  << endl;
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            string type, acPref;
            cout << "Room type (Single/Double/Suite): ";
            getline(cin, type);
            cout << "AC preference (AC/NonAC): ";
            getline(cin, acPref);
            string checkIn  = getValidDate("Check-in  (YYYY-MM-DD): ");
            string checkOut = getValidCheckOut(checkIn);
            customer.searchRoom(type, checkIn, checkOut, acPref);

        } else if (choice == 2) {
            int roomNum;
            cout << "Room number: ";
            cin >> roomNum;
            clearInput();

            string checkIn  = getValidDate("Check-in  (YYYY-MM-DD): ");
            string checkOut = getValidCheckOut(checkIn);

            int resId = customer.bookRoom(roomNum, checkIn, checkOut);
            if (resId != -1) {
                vector<Room> rooms = Room::loadFromFile();
                double price = 0;
                for (const Room& r : rooms)
                    if (r.getRoomNumber() == roomNum)
                        price = r.getPrice();

                Reservation res = Reservation::findById(resId);
                double bill = res.calculateBill(price);
                cout << "Total Bill: $" << bill << endl;

                // Payment method selection menu
                cout << "\nSelect Payment Method:" << endl;
                cout << "1. Cash"         << endl;
                cout << "2. Credit Card"  << endl;
                cout << "3. Online"       << endl;
                cout << "Choice: ";
                int payChoice; cin >> payChoice; clearInput();

                string method;
                if      (payChoice == 1) method = "Cash";
                else if (payChoice == 2) method = "Credit Card";
                else if (payChoice == 3) method = "Online";
                else {
                    cout << "❌ Invalid choice. Defaulting to Cash." << endl;
                    method = "Cash";
                }

                Payment pay(resId, bill, method, currentDate());
                bool paySuccess = pay.processPayment();
                if (!paySuccess) {
                    cout << "⚠️  Payment failed. Please visit the front desk to complete payment." << endl;
                }

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

// ─── Admin Menu ───────────────────────────────────────────────────────────────

void adminMenu(Admin& admin) {
    int choice;
    do {
        cout << "\n===== Admin Menu =====" << endl;
        cout << "1. Add Room"             << endl;
        cout << "2. Remove Room"          << endl;
        cout << "3. Update Room"          << endl;
        cout << "4. Manage Reservation"   << endl;
        cout << "5. Manage Users"         << endl;
        cout << "6. Generate Report"      << endl;
        cout << "0. Logout"              << endl;
        cout << "Choice: ";
        cin >> choice;
        clearInput();

        if (choice == 1) {
            int roomNum, maxGuests;
            double price;
            string type, acInput;
            cout << "Room number        : "; cin >> roomNum;   clearInput();
            cout << "Room type          : "; getline(cin, type);
            cout << "AC or Non-AC (AC/NonAC): "; getline(cin, acInput);
            bool isAC = (acInput == "AC" || acInput == "ac" || acInput == "1");
            cout << "Price/night        : "; cin >> price;     clearInput();
            cout << "Max guests         : "; cin >> maxGuests; clearInput();
            admin.addRoom(roomNum, type, price, maxGuests, isAC);

        } else if (choice == 2) {
            int roomNum;
            cout << "Room number to remove: ";
            cin >> roomNum;
            clearInput();
            admin.removeRoom(roomNum);

        } else if (choice == 3) {
            int roomNum;
            cout << "Room number to update: "; cin >> roomNum; clearInput();

            vector<Room> rooms = Room::loadFromFile();
            bool exists = false;
            for (const Room& r : rooms)
                if (r.getRoomNumber() == roomNum) { r.displayRoom(); exists = true; break; }

            if (!exists) {
                cout << "❌ Room " << roomNum << " not found." << endl;
            } else {
                cout << "\nWhat do you want to update?" << endl;
                cout << "1. Price"       << endl;
                cout << "2. Room Type"   << endl;
                cout << "3. Max Guests"  << endl;
                cout << "4. AC / Non-AC" << endl;
                cout << "Choice: ";
                int updateChoice; cin >> updateChoice; clearInput();

                if (updateChoice == 1) {
                    double newPrice;
                    cout << "New price/night: $"; cin >> newPrice; clearInput();
                    admin.updateRoom(roomNum, "price", to_string(newPrice));

                } else if (updateChoice == 2) {
                    string newType;
                    cout << "New room type (Single/Double/Suite): ";
                    getline(cin, newType);
                    admin.updateRoom(roomNum, "type", newType);

                } else if (updateChoice == 3) {
                    int newGuests;
                    cout << "New max guests: "; cin >> newGuests; clearInput();
                    admin.updateRoom(roomNum, "maxGuests", to_string(newGuests));

                } else if (updateChoice == 4) {
                    cout << "1. AC"    << endl;
                    cout << "2. Non-AC" << endl;
                    cout << "Choice: ";
                    int acChoice; cin >> acChoice; clearInput();
                    admin.updateRoom(roomNum, "ac", acChoice == 1 ? "AC" : "NonAC");

                } else {
                    cout << "❌ Invalid choice." << endl;
                }
            }

        } else if (choice == 4) {
            int resId;
            string action;
            cout << "Reservation ID: "; cin >> resId; clearInput();
            cout << "Action (Confirmed/Cancelled/Completed): ";
            getline(cin, action);
            admin.manageReservation(resId, action);

        } else if (choice == 5) {
            // Manage Users submenu
            int userChoice;
            cout << "\n===== Manage Users =====" << endl;
            cout << "1. View All Users"           << endl;
            cout << "2. Search User"              << endl;
            cout << "3. View User Bookings"       << endl;
            cout << "4. Remove User"              << endl;
            cout << "0. Back"                     << endl;
            cout << "Choice: ";
            cin >> userChoice;
            clearInput();

            if (userChoice == 1) {
                admin.viewAllUsers();

            } else if (userChoice == 2) {
                string uname;
                cout << "Enter username to search: ";
                getline(cin, uname);
                admin.searchUser(uname);

            } else if (userChoice == 3) {
                string uname;
                cout << "Enter username to view bookings: ";
                getline(cin, uname);
                admin.viewUserBookings(uname);

            } else if (userChoice == 4) {
                string uname;
                cout << "Enter username to remove: ";
                getline(cin, uname);
                admin.removeUser(uname);

            } else if (userChoice != 0) {
                cout << "❌ Invalid choice." << endl;
            }

        } else if (choice == 6) {
            admin.generateReport();
        }

    } while (choice != 0);
    admin.logout();
}

// ─── Main ─────────────────────────────────────────────────────────────────────

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

    regex usernamePattern("^[A-Za-z0-9_]{3,20}$");
    regex namePattern("^[A-Za-z ]+$");
    regex phonePattern("^[0-9]{7,15}$");
    regex emailPattern(R"(^[\w\.-]+@[\w\.-]+\.[a-zA-Z]{2,}$)");

    // Username
    while (true) {
        cout << "Username : ";
        getline(cin, username);

        if (username.empty()) {
            cout << "❌ Username cannot be empty.\n";
            continue;
        }

        if (!regex_match(username, usernamePattern)) {
            cout << "❌ Username must contain only letters, numbers, or _ (3-20 characters).\n";
            continue;
        }

        break;
    }

    // Password
    while (true) {
        cout << "Password : ";
        getline(cin, password);

        if (password.length() < 6) {
            cout << "❌ Password must be at least 6 characters.\n";
            continue;
        }

        break;
    }

    // Name
    while (true) {
        cout << "Name     : ";
        getline(cin, name);

        if (name.empty()) {
            cout << "❌ Name cannot be empty.\n";
            continue;
        }

        if (!regex_match(name, namePattern)) {
            cout << "❌ Name must contain letters only.\n";
            continue;
        }

        break;
    }

    // Phone Number
    while (true) {
        cout << "Phone    : ";
        getline(cin, phone);

        if (!regex_match(phone, phonePattern)) {
            cout << "❌ Phone number must contain digits only (7-15 digits).\n";
            continue;
        }

        break;
    }

    // Email
    while (true) {
        cout << "Email    : ";
        getline(cin, email);

        if (!regex_match(email, emailPattern)) {
            cout << "❌ Invalid email format.\n";
            continue;
        }

        break;
    }

    // Address
    while (true) {
        cout << "Address  : ";
        getline(cin, address);

        if (address.empty()) {
            cout << "❌ Address cannot be empty.\n";
            continue;
        }

        break;
    }

    // Check duplicate username
    vector<Customer> customers = Customer::loadFromFile();
    bool exists = false;

    for (const Customer& c : customers) {
        if (c.getUsername() == username) {
            exists = true;
            break;
        }
    }

    if (exists) {
        cout << "❌ Username already exists. Please choose another username.\n";
    }
    else {
        Customer c(username, password, name, phone, email, address);
        c.saveToFile();

        cout << "\n✅ Registration successful!" << endl;
        cout << "You can now log in.\n";
    }
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
