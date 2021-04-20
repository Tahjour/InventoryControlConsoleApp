#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>

// Using namespace std so we don't have to type "std::" before everything.
using namespace std;

// This is the class for the inventories Items.
class Item {
    // Access Specifier. "public:" makes the attributes of the Item class accessible anywhere.
public:
    string name{ "" }; // The Item's name.
    float price{ 0 };  // The Item's Price.
    int amount{ 0 };   // The Item's Amount.
};

// This map will aid in reading the Inventory CSV file.
// A map is a key and value pair. In this map, the name of the item will be the key and an object of the Item class will be the value.
map<string, Item> Inventory;

//This map was created to minimize the repetition of menu prompts. We will simply create the menus in this map and access them accordingly.
// So far, we just have the Main Menu called "MainMenu" and it only has one prompt inside the vector.
// By the way, a vector is a dynamic array or a way to store a sequence of data and modify it while the program is running.
// In the vector, we're just going to store all the prompts of each menu.
map<string, vector<string>> MenuList{
    { "MainMenu", { "1|> Add an Inventory Item\n", "2|> View Inventory\n", "3|> Search Inventory\n" } },
    { "ViewMenu", { "1|> Edit an Item\n", "2|> Delete an Item\n" } }
};
bool localInventoryUpdated{ false };

// Funtion Declarations
// Helper Functions: Most of these are to help with minimizing code repetition or error checking.
void showInvalidOptionError();
bool is_digits(const string&);
void pauseScreen();
void clearScreen();
void pauseThenClearScreen();
void showMenu(const string&);
void checkNextStepBasedOnMenu(string&, string&);
bool didFileOpen(ifstream&);
bool didFileOpen(ofstream&);
bool isLocalInventoryUpdated();
bool doesItemAlreadyExist(string);
void printRowOfInventory(string, float, int);
void createTableHeaders();

// Main Functionality Functions
void addItemToInventory();
void loadInventoryFromFile();
void viewInventory();
void searchInventory();

int main() {
    string option;
    string thisMenuName{ "MainMenu" };
    do {
        showMenu(thisMenuName);
        cin >> option;
        cin.ignore();

        if (option.length() != 1 || !is_digits(option)) {
            showInvalidOptionError();
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

void checkNextStepBasedOnMenu(string& option, string& menuName) {
    int optionNum = stoi(option);
    if (menuName == "MainMenu") {
        switch (optionNum) {
            case 0:
                return;
            case 1:
                addItemToInventory();
                pauseThenClearScreen();
                break;
            case 2:
                viewInventory();
                pauseThenClearScreen();
                break;
            case 3:
                searchInventory();
                pauseThenClearScreen();
                break;
            default:
                showInvalidOptionError();
                return;
        }
    }
}

void loadInventoryFromFile() {
    if (isLocalInventoryUpdated()) { return; }
    ifstream inventoryFile{ "Inventory.csv" };
    if (!didFileOpen(inventoryFile)) { return; }
    string rowString, colString;
    Item item;
    vector<string> row;
    while (getline(inventoryFile, rowString)) {
        row.clear();
        stringstream str{ rowString };
        while (getline(str, colString, ',')) {
            row.push_back(colString);
        }
        item.name = row[0];         // first column from csv file
        item.price = stof(row[1]);  // second column
        item.amount = stoi(row[2]); // third column

        // Adding a new {key, value} pair to the Inventory map.
        // The key is the item's name and the value is an instance of an Item Object.
        Inventory[item.name].name = item.name;
        Inventory[item.name].price = item.price;   // add price if duplicate item name is found
        Inventory[item.name].amount = item.amount; // add amount if duplicate item name is found
    }
    inventoryFile.close();
    localInventoryUpdated = true;
}

void addItemToInventory() {
    loadInventoryFromFile();
    ofstream inventoryFile{ "Inventory.csv", ios_base::app };
    if (!didFileOpen(inventoryFile)) { return; }
    Item item;
    cout << "\nEnter the Item's Name: ";
    getline(cin, item.name);
    if (doesItemAlreadyExist(item.name)) { return; }
    cout << "Enter the Item's Price: ";
    cin >> item.price;
    cout << "Enter the Item's Amount: ";
    cin >> item.amount;
    inventoryFile << item.name << "," << item.price << "," << item.amount << endl;
    inventoryFile.close();
    localInventoryUpdated = false;
}

void viewInventory() {
    loadInventoryFromFile();
    // If the Inventory map still isn't updated after loading it, just cancel.
    if (!localInventoryUpdated) { return; }
    clearScreen();
    createTableHeaders();
    for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
        // C-Style console printing to specify spacing for table display.
        printRowOfInventory((*it).second.name, (*it).second.price, (*it).second.amount);
    }
}

void searchInventory() {
    viewInventory();
    if (!localInventoryUpdated) { return; }
    string query{ "" };
    cout << "\n\nEnter the Name of Item to Search: ";
    getline(cin, query);
    if (Inventory.find(query) != Inventory.end()) {
        clearScreen();
        createTableHeaders();
        printRowOfInventory(Inventory[query].name, Inventory[query].price, Inventory[query].amount);
    }

    // todo: implement this part
}

void createTableHeaders() {
    int colWidth{ 60 };
    cout << setfill('*') << setw(colWidth) << "*" << endl;
    printf("| %-20s | %-20s | %s\n", "Item Name", "Item Price", "Item Amount");
    cout << setfill('*') << setw(colWidth) << "*" << endl;
}

void printRowOfInventory(string name, float price, int amount) {
    printf("| %-20s | $%-20.2f | %d\n", name.c_str(), price, amount);
}

bool is_digits(const string& str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

void showInvalidOptionError() {
    cout << "Invalid, Try again\n";
    pauseThenClearScreen();
}

void pauseScreen() {
    cout << "\n\nPress any key to continue...\n";
    cin.get();
}

void pauseThenClearScreen() {
    pauseScreen();
    clearScreen();
}

bool didFileOpen(ifstream& inventoryFile) {
    if (!inventoryFile.is_open()) {
        cout << "Couldn't open file...\n";
        return false;
    }
    return true;
}
bool didFileOpen(ofstream& inventoryFile) {
    if (!inventoryFile.is_open()) {
        cout << "Couldn't open file...\n";
        return false;
    }
    return true;
}

bool doesItemAlreadyExist(string itemName) {
    if (Inventory.find(itemName) != Inventory.end()) {
        cout << "This Item already exists. Going back main menu..\n";
        return true;
    }
    return false;
}

bool isLocalInventoryUpdated() {
    if (localInventoryUpdated) { return true; }
    return false;
}

void clearScreen() {
// These are macros.
// Unfortunately, depending on the Operating System, the command is different to clear the console screen.
// So, I'm checking to see if the user is using (linux or unix or mac) or (32-bit or 64-bit Windows)
// This is just to make sure the code is more portable. If I just used the system("cls") command, it would only work on Windows computers.
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    system("clear");
#endif

#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#endif
}