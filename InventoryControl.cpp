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
    { "MainMenu", { "1|> Add an Inventory Item\n", "2|> View Inventory\n" } },
    { "ViewInventoryMenu", { "1|> Edit an Item\n", "2|> Delete an Item\n" } }
};
bool localInventoryIsUpdated{ false };

// Funtion Declarations
// Helper Functions: Most of these are to help with minimizing code repetition or error checking.
void showInvalidOptionError();
bool is_digits(const string&);
void pauseScreen();
void clearScreen();
void pauseThenClearScreen();
void showMenuAndCheckOption(const string&, string&);
void checkNextStepBasedOnMenu(string&, const string&);
bool didFileOpen(ifstream&);
bool didFileOpen(ofstream&);
bool doesItemAlreadyExist(string&);
void printRowOfInventory(string&, float&, int&);
void createTableHeaders();
void flushInputBuffer();
void searchForEditing(string&, vector<string>&);

//Menu functions
int main();
void viewInventoryMenu();

// Main Functionality Functions
void addItemToInventory();
void loadInventoryFromFile();
void viewInventory();
void editInventoryItem();
void deleteInventoryItem();

int main() {
    string option{ "" };
    string thisMenuName{ "MainMenu" };
    showMenuAndCheckOption(thisMenuName, option);

    cout << "Thanks for using the program! See Ya!" << endl;
    return 0;
}

void viewInventoryMenu() {
    string option{ "" };
    string thisMenuName{ "ViewInventoryMenu" };
    showMenuAndCheckOption(thisMenuName, option);
}

void showMenuAndCheckOption(const string& menuNameKey, string& option) {
    do {
        if (menuNameKey == "ViewInventoryMenu") {
            viewInventory();
            cout << "\n\n";
        }
        for (int x{ 0 }; x < MenuList[menuNameKey].size(); x++) {
            cout << MenuList[menuNameKey].at(x);
        }
        cout << "\n";
        cout << "Enter Option Number(0 to Exit): ";
        cin >> option;
        cin.ignore();

        if (option.length() != 1 || !is_digits(option)) {
            showInvalidOptionError();
            pauseThenClearScreen();
        } else {
            checkNextStepBasedOnMenu(option, menuNameKey);
        }
    } while (option != "0");
}

void checkNextStepBasedOnMenu(string& option, const string& menuName) {
    int optionNum = stoi(option);
    if (menuName == "MainMenu") {
        switch (optionNum) {
            case 0:
                return;
            case 1:
                addItemToInventory();
                flushInputBuffer();
                pauseThenClearScreen();
                break;
            case 2:
                viewInventoryMenu();
                pauseThenClearScreen();
                break;
            default:
                showInvalidOptionError();
                pauseThenClearScreen();
                return;
        }
    } else if (menuName == "ViewInventoryMenu") {
        switch (optionNum) {
            case 0:
                return;
            case 1:
                editInventoryItem();
                pauseThenClearScreen();
                break;
            case 2: //todo: Delete an Item
                break;
            default:
                showInvalidOptionError();
                pauseThenClearScreen();
                return;
        }
    }
}

void loadInventoryFromFile() {
    if (localInventoryIsUpdated) { return; }
    ifstream inventoryFile{ "Inventory.csv" };
    if (!didFileOpen(inventoryFile)) { return; }
    string rowString, colString;
    Item item;
    vector<string> row;
    while (getline(inventoryFile, rowString)) {
        row.clear();                       // empty the vector so it doesn't keep on appending passed index 2
        stringstream strTemp{ rowString }; // This is to help with the getline fucntion, since it expects an istream
        while (getline(strTemp, colString, ',')) {
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
    localInventoryIsUpdated = true;
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
    localInventoryIsUpdated = false;
}

void viewInventory() {
    loadInventoryFromFile();
    // If the Inventory map still isn't updated after loading it, just cancel.
    if (!localInventoryIsUpdated) { return; }
    clearScreen();
    createTableHeaders();
    for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
        // C-Style console printing to specify spacing for table display.
        printRowOfInventory((*it).second.name, (*it).second.price, (*it).second.amount);
    }
}

void editInventoryItem() {
    viewInventory();
    if (!localInventoryIsUpdated) { return; }
    string query{ "" };
    Item item;
    vector<string> itemNameResults;

    cout << "\n\nEnter Item Name to Select: ";
    getline(cin, query);

    searchForEditing(query, itemNameResults);
    if (itemNameResults.size() > 1) {
        do {
            cout << "\nPlease Specify the Item's Name: ";
            getline(cin, query);
            searchForEditing(query, itemNameResults);
        } while (itemNameResults.size() != 1);
    }
    if (itemNameResults.size() == 1) {
        cout << "\nEdit the Item's Name: ";
        getline(cin, item.name);
        cout << "Edit the Item's Price: ";
        cin >> item.price;
        cout << "Edit the Item's Amount: ";
        cin >> item.amount;
        map<string, Item>::iterator itemBeingEdited = Inventory.find(itemNameResults[0]);
        Inventory.erase(itemBeingEdited);
        Inventory.insert(make_pair(item.name, item));
        ofstream updateInventoryFile{ "Inventory.csv" };
        if (!didFileOpen(updateInventoryFile)) { return; }
        for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
            updateInventoryFile << it->second.name << "," << it->second.price << "," << it->second.amount << endl;
        }
        localInventoryIsUpdated = false;
        updateInventoryFile.close();
    }
}

void searchForEditing(string& query, vector<string>& itemNameResults) {
    itemNameResults.clear();
    if (Inventory.find(query) != Inventory.end()) {
        printRowOfInventory(Inventory[query].name, Inventory[query].price, Inventory[query].amount);
        itemNameResults.push_back(Inventory[query].name);
    } else {
        for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
            if ((it->first.find(query) != string::npos)) {
                printRowOfInventory(it->second.name, it->second.price, it->second.amount); // Print each row of matches found
                itemNameResults.push_back(it->second.name);                                // Push each match to the results vector
            }
        }
        if (itemNameResults.size() == 0) {
            cout << "No Matches...\n";
        }
    }
}

void createTableHeaders() {
    int colWidth{ 60 };
    cout << setfill('*') << setw(colWidth) << "*" << endl;
    printf("| %-20s | %-20s | %s\n", "Item Name", "Item Price", "Item Amount");
    cout << setfill('*') << setw(colWidth) << "*" << endl;
}

void printRowOfInventory(string& name, float& price, int& amount) {
    printf("| %-20s | $%-20.2f | %d\n", name.c_str(), price, amount);
}

bool is_digits(const string& str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

void showInvalidOptionError() {
    cout << "Invalid, Try again\n";
}

void pauseScreen() {
    cout << "\n\nPress any key to continue...\n";
    cin.get();
}

void pauseThenClearScreen() {
    pauseScreen();
    clearScreen();
}

void flushInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// This defined twice to perform an overload.
// C++ will automatically detect which one we're trying to use based on the parameters.
bool didFileOpen(ifstream& inventoryFile) {
    if (inventoryFile.is_open()) {
        return true;
    }
    cout << "Couldn't open file...\n";
    return false;
}
bool didFileOpen(ofstream& inventoryFile) {
    if (inventoryFile.is_open()) {
        return true;
    }
    cout << "Couldn't open file...\n";
    return false;
}

bool doesItemAlreadyExist(string& itemName) {
    if (Inventory.find(itemName) != Inventory.end()) {
        cout << "This Item already exists. Going back main menu..\n";
        return true;
    }
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
