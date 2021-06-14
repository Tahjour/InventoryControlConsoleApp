#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <ctime>

using namespace std;

class Item {
public:
    string name{ "" };
    float price{ 0 };
    int amount{ 0 };
};

map<string, Item> Inventory;

map<string, vector<string>> MenuList{
    { "MainMenu", { "Add Inventory Item\n", "View Inventory\n" } },
    { "ViewInventoryMenu", { "Edit an Item\n", "Delete an Item\n", "Manage Purchase\n" } },
};
bool localInventoryIsUpdated{ false };
const string MainInventoryFileName{ "Inventory.csv" };
clock_t timerStart;
clock_t timerEnd;

// Funtion Declarations
// Helper Functions: Most of these are to help with minimizing code repetition or error checking.
void showInvalidOptionError();
void pauseScreen();
void clearScreen();
void pauseThenClearScreen();
bool is_digits(const string& inputString);
void showMenuAndCheckOption(const string& menuNameKey, string& option);
void checkNextStepBasedOnMenu(string& option, const string& menuName);
bool doesFileExist(fstream& file);
bool doesItemAlreadyExist(string& itemName);
void printRowOfInventory(Item& item);
void createTableHeaders();
void flushInputBuffer();
void createLogEntry(Item& item, string& operation);

//Menu functions
int main();
void viewInventoryMenu();

// Main Functionality Functions
void addItemToInventory();
void loadInventoryFromFile();
void viewInventory();
void searchForInventoryItem(string&, vector<string>&);
void editDeletePurchaseInventoryItem(string&);
void editDeletePurchaseItemFound(vector<string>&, Item&, string&, fstream&);

int main() {
    string option{ "" };
    string thisMenuName{ "MainMenu" };
    showMenuAndCheckOption(thisMenuName, option);
    cout << "Thanks for using the program! See Ya!" << endl;
    pauseScreen();
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
            if (!localInventoryIsUpdated) { return; }
            cout << "\n\n";
        }

        for (int x{ 0 }; x < MenuList[menuNameKey].size(); x++) {
            cout << x + 1 << "|-> " << MenuList[menuNameKey].at(x);
        }
        cout << "\n";
        cout << "Enter Option Number(0 to Exit): ";
        cin >> option;

        if (option.length() != 1 || !is_digits(option)) {
            showInvalidOptionError();
            flushInputBuffer();
            pauseThenClearScreen();
        } else {
            flushInputBuffer();
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
        string editCommand{ "edit" };
        string deleteCommand{ "delete" };
        string purchaseCommand{ "purchase" };
        switch (optionNum) {
            case 0:
                return;
            case 1:
                editDeletePurchaseInventoryItem(editCommand);
                pauseThenClearScreen();
                break;
            case 2:
                editDeletePurchaseInventoryItem(deleteCommand);
                pauseThenClearScreen();
                break;
            case 3:
                editDeletePurchaseInventoryItem(purchaseCommand);
                pauseThenClearScreen();
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
    fstream inventoryFile{ MainInventoryFileName, ios_base::in };
    if (!doesFileExist(inventoryFile)) { return; }
    string rowString, colString;
    Item item;
    vector<string> row;
    while (getline(inventoryFile, rowString)) {
        row.clear();
        stringstream strTemp{ rowString };
        while (getline(strTemp, colString, ',')) {
            row.push_back(colString);
        }
        item.name = row[0];
        item.price = stof(row[1]);
        item.amount = stoi(row[2]);
        Inventory[item.name].name = item.name;
        Inventory[item.name].price = item.price;
        Inventory[item.name].amount = item.amount;
    }
    localInventoryIsUpdated = true;
}

void addItemToInventory() {
    fstream inventoryFile{ MainInventoryFileName, ios_base::app };
    loadInventoryFromFile();
    if (!doesFileExist(inventoryFile)) { return; }
    Item item;
    cout << "\nEnter the Item's Name: ";
    getline(cin, item.name);
    if (doesItemAlreadyExist(item.name)) { return; }
    cout << "Enter the Item's Price: ";
    cin >> item.price;
    cout << "Enter the Item's Amount: ";
    cin >> item.amount;
    inventoryFile << item.name << "," << item.price << "," << item.amount << endl;
    string operation = "add";
    createLogEntry(item, operation);
    localInventoryIsUpdated = false;
}

void viewInventory() {
    timerStart = clock();
    loadInventoryFromFile();
    timerEnd = clock();
    if (!localInventoryIsUpdated) { return; }
    clearScreen();
    double timeSpent = (double)(timerEnd - timerStart) / CLOCKS_PER_SEC;
    cout << "Loading Time = " << timeSpent << "s" << endl;
    createTableHeaders();
    for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
        printRowOfInventory(it->second);
    }
}

void editDeletePurchaseInventoryItem(string& operation) {
    viewInventory();
    if (!localInventoryIsUpdated) { return; }
    string query{ "" };
    Item item;
    vector<string> itemNameResults;

    cout << "\n\nEnter Item Name to Select: ";
    getline(cin, query);

    searchForInventoryItem(query, itemNameResults);
    if (itemNameResults.size() > 1) {
        do {
            cout << "\nPlease Specify the Item's Name: ";
            getline(cin, query);
            searchForInventoryItem(query, itemNameResults);
        } while (itemNameResults.size() != 1);
    }
    if (itemNameResults.size() == 1 && operation == "edit") {
        cout << "\nEdit the Item's Name: ";
        getline(cin, item.name);
        cout << "Edit the Item's Price: ";
        cin >> item.price;
        cout << "Edit the Item's Amount: ";
        cin >> item.amount;
        fstream updateInventoryFile{ MainInventoryFileName, ios_base::out };
        if (!doesFileExist(updateInventoryFile)) { return; }
        editDeletePurchaseItemFound(itemNameResults, item, operation, updateInventoryFile);
    }
    if (itemNameResults.size() == 1 && operation == "delete") {
        fstream updateInventoryFile{ MainInventoryFileName, ios_base::out };
        if (!doesFileExist(updateInventoryFile)) { return; }
        editDeletePurchaseItemFound(itemNameResults, item, operation, updateInventoryFile);
    }
    if (itemNameResults.size() == 1 && operation == "purchase") {
        float customerCash{ 0 }, customerChange{ 0 }, totalCost{ 0 };
        int itemPurchaseAmount{ 0 };
        item.name = Inventory[itemNameResults[0]].name;
        item.amount = Inventory[itemNameResults[0]].amount;
        item.price = Inventory[itemNameResults[0]].price;
        cout << "\nCustomer Requested Amount: ";
        cin >> itemPurchaseAmount;

        totalCost = itemPurchaseAmount * item.price;
        cout << "Total Cost = $" << totalCost << endl;
        cout << "\nEnter Customer's Pay: ";
        cin >> customerCash;
        flushInputBuffer();

        if (itemPurchaseAmount > item.amount || itemPurchaseAmount < 0) {
            cout << "\nThat Amount is Not Available...\n";
            cout << "Amount of " << item.name << " = " << item.amount;
            return;
        }
        if (totalCost > customerCash) {
            cout << "\nThere is not enough cash to pay...\n";
            cout << "Customer needs $" << (totalCost - customerCash) << " more to purchase...\n";
            return;
        }
        item.amount = item.amount - itemPurchaseAmount;
        customerChange = customerCash - totalCost;
        cout << endl
             << item.name << " purchased...\n";
        cout << "Customer's Change = $" << customerChange << endl;
        fstream updateInventoryFile{ MainInventoryFileName, ios_base::out };
        if (!doesFileExist(updateInventoryFile)) { return; }
        editDeletePurchaseItemFound(itemNameResults, item, operation, updateInventoryFile);
    }
}

void editDeletePurchaseItemFound(vector<string>& itemNameResults, Item& item, string& operation, fstream& updateInventoryFile) {
    map<string, Item>::iterator itemBeingEdited = Inventory.find(itemNameResults[0]);
    if (operation == "delete") {
        createLogEntry(itemBeingEdited->second, operation);
        Inventory.erase(itemBeingEdited);
        cout << "Deleted... \n";
    }
    if (operation == "edit" || operation == "purchase") {
        createLogEntry(itemBeingEdited->second, operation);
        Inventory.erase(itemBeingEdited);
        Inventory.insert(make_pair(item.name, item));
    }
    for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
        updateInventoryFile << it->second.name << "," << it->second.price << "," << it->second.amount << endl;
    }
    localInventoryIsUpdated = false;
}

void searchForInventoryItem(string& query, vector<string>& itemNameResults) {
    itemNameResults.clear();
    for (auto& c : query) { c = toupper(c); }
    for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
        string currentItemName{ it->first };
        for (auto& c : currentItemName) { c = toupper(c); }
        if (query == currentItemName) {
            itemNameResults.clear();
            itemNameResults.push_back(query);
            break;
        }
        if ((currentItemName.find(query) != string::npos)) {
            printRowOfInventory(it->second);
            itemNameResults.push_back(it->second.name);
        }
    }
    if (itemNameResults.size() == 0) {
        cout << "No Matches...\n";
    }
}

void createTableHeaders() {
    int colWidth{ 60 };
    cout << setfill('*') << setw(colWidth) << "*" << endl;
    printf("| %-20s | %-20s | %s\n", "Item Name", "Item Price", "Item Amount");
    cout << setfill('*') << setw(colWidth) << "*" << endl;
}

void printRowOfInventory(Item& item) {
    printf("| %-20s | $%-19.2f | %d\n", item.name.c_str(), item.price, item.amount);
}

bool is_digits(const string& inputString) {
    return inputString.find_first_not_of("0123456789") == string::npos;
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

bool doesFileExist(fstream& file) {
    if (file.good()) {
        return true;
    } else {
        cout << "Inventory File Could Not Be Opened\n";
        return false;
    }
}

bool doesItemAlreadyExist(string& itemName) {
    string tempItem = itemName;
    for (auto& c : tempItem) { c = toupper(c); }
    for (map<string, Item>::iterator it = Inventory.begin(); it != Inventory.end(); it++) {
        string currentItemName{ it->first };
        for (auto& c : currentItemName) { c = toupper(c); }
        if (itemName == currentItemName) {
            cout << "This Item already exists. Going back main menu..\n";
            return true;
        }
    }
    return false;
}

void createLogEntry(Item& item, string& operation) {
    // todo: Fix this up
    fstream logFile{ "Log.csv", ios_base::app };
    time_t t = time(0);
    localtime(&t);
    if (!doesFileExist(logFile)) { return; }
    if (operation == "add") {
        logFile << "ADDITION," << item.name << "," << ctime(&t);
    }
    if (operation == "edit") {
        logFile << "EDIT," << item.name << "," << ctime(&t);
    }
    if (operation == "delete") {
        logFile << "DELETION," << item.name << "," << ctime(&t);
    }
    if (operation == "purchase") {
        logFile << "PURCHASE," << item.name << "," << ctime(&t);
    }
}

void clearScreen() {
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    system("clear");
#endif

#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#endif
}
