#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iomanip>

// Using namespace std so we don't have to type "std::" before everything.
using namespace std;

// This is the class for the inventories Items.
class Item {
    // Access Specifier. "public:" makes the attributes of the Item class accessible anywhere.
public:
    string itemName{ "" }; // The Item's name.
    float itemPrice{ 0 };  // The Item's Price.
    int itemAmount{ 0 };   // The Item's Amount.
};

// This map will aid in reading the Inventory CSV file.
// A map is a key and value pair. In this map, the name of the item will be the key and an object of the Item class will be the value.
map<string, Item> Inventory;

//This map was created to minimize the repetition of menu prompts. We will simply create the menus in this map and access them accordingly.
// So far, we just have the Main Menu called "MainMenu" and it only has one prompt inside the vector.
// By the way, a vector is a dynamic array or a way to store a sequence of data and modify it while the program is running.
// In the vector, we're just going to store all the prompts of each menu.
map<string, vector<string>> MenuList{
    { "MainMenu", { "1|> Add an Inventory Item\n", "2|> View Inventory\n" } }
};

// Funtion Declarations
// Most of these are to help with minimizing code repetition.
void showInvalidOptionError();
bool is_digits(const string&);
void pauseScreen();
void clearScreen();
void showMenu(const string&);
void checkNextStepBasedOnMenu(string&, string&);

void addItemToInventory();
void viewInventory();

int main() {
    string option;
    string thisMenuName = "MainMenu";
    do {
        showMenu(thisMenuName);
        cin >> option;

        if (option.length() != 1 || !is_digits(option)) {
            showInvalidOptionError();
            pauseScreen();
            clearScreen();
            continue;
        } else {
            checkNextStepBasedOnMenu(option, thisMenuName);
        }
    } while (option != "0");

    cout << "Thanks for using this program. Bye!" << endl;
    return 0;
}

void showMenu(const string& menuNameKey) {
    for (int x{ 0 }; x < MenuList[menuNameKey].size(); x++) {
        cout << MenuList[menuNameKey].at(x);
    }
    cout << "\n";
    cout << "Enter Option Number(0 to Exit): ";
}

bool is_digits(const string& str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

void showInvalidOptionError() {
    cout << "Invalid, Try again\n";
}

void pauseScreen() {
    cout << "Press any key to continue...\n";
    cin.ignore();
    cin.get();
}

void clearScreen() {
// These are macros.
// Unfortunately, depending on the Operating System, the command is different to clear the console screen.
// So, I'm checking to see if the user is using (linux, unix, mac PC) or (32-bit or 64-bit Windows PC)
// This is just to make sure the code is more portable. If I just used the system("cls") command, it would only work on Windows computers.
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    system("clear");
#endif

#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#endif
}

void checkNextStepBasedOnMenu(string& option, string& menuName) {
    int optionNum = stoi(option);
    if (menuName == "MainMenu") {
        switch (optionNum) {
            case 0:
                return;
            case 1:
                addItemToInventory();
                break;
            case 2:
                viewInventory();
                break;
            default:
                showInvalidOptionError();
                pauseScreen();
                clearScreen();
                return;
        }
    }
}

void addItemToInventory() {
    ofstream inventoryFile{ "Inventory.csv", ios_base::app };
    if (!inventoryFile.is_open()) {
        cout << "Couldn't open file...\n";
        return;
    }
    Item item;
    cout << "\n";
    cout << "Enter the Item's Name: ";
    cin.ignore();
    getline(cin, item.itemName);
    cout << "Enter the Item's Price: ";
    cin >> item.itemPrice;
    cout << "Enter the Item's Amount: ";
    cin >> item.itemAmount;
    inventoryFile << item.itemName << "," << item.itemPrice << "," << item.itemAmount << endl;
    inventoryFile.close();
    pauseScreen();
    clearScreen();
}

void viewInventory() {
    ifstream inventoryFile{ "Inventory.csv" };
    if (!inventoryFile.is_open()) {
        cout << "Couldn't open file...\n";
        return;
    }
    string rowString, colString;
    Item item;
    vector<string> row;
    while (getline(inventoryFile, rowString)) {
        row.clear();
        stringstream str{ rowString };
        while (getline(str, colString, ',')) {
            row.push_back(colString);
        }
        item.itemName = row[0];
        item.itemPrice = stof(row[1]);
        item.itemAmount = stoi(row[2]);
        Inventory[item.itemName].itemName = item.itemName;
        Inventory[item.itemName].itemPrice += item.itemPrice;
        Inventory[item.itemName].itemAmount += item.itemAmount;
    }
    clearScreen();
    // Table Headers
    int colwidth{ 60 };
    cout << setfill('*') << setw(colwidth) << "*" << endl;
    printf("| %-20s | %-20s | %s\n", "Item Name", "Item Price", "Item Amount");
    cout << setfill('*') << setw(colwidth) << "*" << endl;

    for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
        // C-Style printing to console to specify spacing for table display.
        printf("| %-20s | $%-20.2f | %d\n", (*it).second.itemName.c_str(), (*it).second.itemPrice, (*it).second.itemAmount);
    }
    inventoryFile.close();
    pauseScreen();
    clearScreen();
}
