#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
using namespace std;

class Room {
private:
    int    roomNumber;
    string roomType;
    double price;
    bool   isAvailable;
    int    maxGuests;
    bool   isAC;           

public:
    Room();
    Room(int roomNumber, const string& roomType, double price,
         int maxGuests, bool isAC);
    ~Room();

    
    int    getRoomNumber()  const;
    string getRoomType()    const;
    double getPrice()       const;
    bool   getIsAvailable() const;
    int    getMaxGuests()   const;
    bool   getIsAC()        const;   

    
    void setRoomType(const string& roomType);
    void setPrice(double price);
    void setMaxGuests(int maxGuests);
    void setIsAC(bool isAC);         

    
    bool checkAvailability() const;
    void updateAvailability(bool available);
    void displayRoom() const;

    
    static bool isValidRoomNumber(int roomNumber);
    static bool isDuplicateRoomNumber(int roomNumber);

   
    void saveToFile() const;
    static vector<Room> loadFromFile();
    static void removeFromFile(int roomNumber);
};

#endif 