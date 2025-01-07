#include <iostream>
#include <string>
#include <map>
#include <ctime>
#include <fstream>
#include <sstream>
//#include <vector>
#include <cstdlib> // For system()
using namespace std;


// Function to print a welcome message
void welcome() {
    cout << "                         ***********************************************************************************" << endl;
    cout << "                           *         *   *******   *         ********  ********  *        *   ***********" << endl;
    cout << "                           *         *   *         *         *         *      *  *  *  *  *   *" << endl;
    cout << "                           *    *    *   *******   *         *         *      *  *    *   *   ***********" << endl;
    cout << "                           *  *   *  *   *         *         *         *      *  *        *   *" << endl;
    cout << "                           *         *   *******   *******   ********  ********  *        *   ***********" << endl;
    cout << "                        ************************************************************************************" << endl;
}

// Function to clear the screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to wait for user input
void pressAnyKeyToContinue() {
    cout << "Press any key to continue...";
    cin.ignore(); // Ignore the newline character left in the input buffer
    cin.get();    // Wait for user input
    clearScreen();
}

// Structure to store transaction details
struct Transaction {
    string type;
    double amount;
    string date;
    Transaction* next; // Pointer to the next transaction
};

// Class to manage a linked list of transactions
class TransactionList {
public:
    Transaction* head;
    TransactionList() : head(nullptr) {} // singly linked list

    ~TransactionList() { // desctructor for memory management
        while (head) {
            Transaction* temp = head;
            head = head->next; 
            delete temp;
        }
    }

    // Add a new transaction to the list
    void addTransaction(const string& type, double amount, const string& date) // type means deposit withdrwal loan
     {
        Transaction* newTransaction = new Transaction{type, amount, date, head};
        head = newTransaction;
    }

    // Display all transactions
    void displayTransactions() const {
        Transaction* current = head;
        if (!current) {
            cout << "No transactions available." << endl;
            return;
        }
        while (current) {
            cout << current->date << " - " << current->type << ": " << current->amount << endl;
            current = current->next;
        }
    }

    // Calculate total expenses
    double calculateTotalExpenses() const {
        double total = 0.0;
        Transaction* current = head;
        while (current) {
            if (current->type == "Expense") {
                total += current->amount;
            }
            current = current->next;
        }
        return total;
    }
};

// Helper function to get the current date and time
string getCurrentDateTime() {
    time_t now = time(0);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", localtime(&now));
    return string(buf);
}

// Class for managing a bank account
class Account {
public:
    string userName;  // Make userName public
    string password;
    double balance;
    double budget;
    double monthlyIncome;
    double loanAmount; // New member variable to store the loan amount
    TransactionList transactions;
    string userID;

public:
    Account() : balance(0.0), budget(0.0), monthlyIncome(0.0), loanAmount(0.0) {}

    // Set user details
    void setDetails(const string& name, const string& pass) {
        userName = name;
        password = pass;
        userID = to_string(100000 + rand() % 900000);
    }

bool loadFromFile(const string& fileName) {
    ifstream file(fileName);
    if (file.is_open()) {
        string line;

        // Read basic user details
        getline(file, userName);  // Username
        getline(file, password);  // Password
        file >> balance >> budget >> monthlyIncome >> loanAmount;  // Numbers
        file.ignore();  // Ignore the leftover newline after the numbers

        // Read transaction history
        getline(file, line);  // Read "Transaction History:"
        if (line != "Transaction History:") {
            cerr << "Error: Missing 'Transaction History:' section." << endl;
            return false;
        }

        // Parse transaction data
        while (getline(file, line)) {
            if (line == "Finance Summary:") break;  // Stop at "Finance Summary:"
            
            string dateTime, type;
            double amount;

            // Parse date and time
            dateTime = line.substr(0, 19); // Extract first 19 characters for "YYYY-MM-DD HH:MM:SS"

            // Parse transaction type and amount
            size_t typeStart = line.find("- ") + 2; // Find start of transaction type
            size_t typeEnd = line.find(":"); // Find end of transaction type
            type = line.substr(typeStart, typeEnd - typeStart); // Extract type

            // Parse amount
            amount = stod(line.substr(typeEnd + 2)); // Convert amount to double

            // Add transaction to the list
            transactions.addTransaction(type, amount, dateTime);  
        }

        return true;
    }

    cerr << "Error: Could not open file " << fileName << endl;
    return false;
}


   void saveToFile() const {
    ofstream file(userName + ".txt", ios::trunc);  // Overwrite the file
    if (file.is_open()) {
        file << userName << endl;
        file << password << endl;
        file << balance << endl;
        file << budget << endl;
        file << monthlyIncome << endl;
        file << loanAmount << endl;

        // Save transactions
        file << "Transaction History:\n";
        Transaction* current = transactions.head;
        while (current) {
            file << current->date << " - " << current->type << ": " << current->amount << endl;
            current = current->next;
        }

        // Save finance summary
        double totalExpenses = transactions.calculateTotalExpenses();
        file << "Finance Summary:\n";
        file << "Monthly Income: " << monthlyIncome << endl;
        file << "Budget: " << budget << endl;
        file << "Total Expenses: " << totalExpenses << endl;
        if (budget > 0) {
            file << ((totalExpenses <= budget) ? "You are within your budget!\n"
                                               : "You have exceeded your budget by " + to_string(totalExpenses - budget) + "!\n");
        } else {
            file << "No budget set." << endl;
        }
    } else {
        cerr << "Error: Unable to save user data to file." << endl;
    }
}
    // Create a new account (consider case of same username)
    void createAccount(map<string, Account>& accounts) {
        cout << "Enter your name: ";
        cin >> userName;
        cout << "Enter password: ";
        cin >> password;

        // Check if the account already exists (check file)
        if (ifstream(userName + ".txt").good()) {
            cout << "User  already exists! Please log in.\n";
        } else {
            setDetails(userName, password);
            accounts[userName] = *this;
            saveToFile();  // Save account details to file
            cout << "Account created! Your ID is: " << userID << endl;
        }
    }

    // Login to the account (load data from file)
 // Login to the account (load data from file)
bool login(map<string, Account>& accounts) {
    cout << "Enter your username: ";
    cin >> userName;
    cout << "Enter your password: ";
    cin >> password;

    // Check if the user exists and the password is correct
    if (ifstream(userName + ".txt").good()) {
        Account tempAccount;
        if (tempAccount.loadFromFile(userName + ".txt") && tempAccount.password == password) {
            cout << "Login successful!\n";
            
            // Manually copy all attributes to ensure `TransactionList` works correctly
            userName = tempAccount.userName;
            password = tempAccount.password;
            balance = tempAccount.balance;
            budget = tempAccount.budget;
            monthlyIncome = tempAccount.monthlyIncome;
            loanAmount = tempAccount.loanAmount;

            // Copy the transaction list manually
            transactions = tempAccount.transactions;

            // Update the account in the map
            accounts[userName] = *this;

            return true;
        }
    }
    cout << "Invalid credentials!\n";
    return false;
}


void deposit(double amount) {
    if (amount > 0) {
        balance += amount;
        transactions.addTransaction("Deposit", amount, getCurrentDateTime());
        saveToFile();  // Save updated balance and transaction
        cout << "Deposit successful! Balance: " << balance << endl;
    } else {
        cout << "Invalid deposit amount." << endl;
    }
}

void withdraw(double amount) {
    if (amount > 0 && amount <= balance) {
        balance -= amount;
        transactions.addTransaction("Withdrawal", amount, getCurrentDateTime());
        saveToFile();  // Save updated balance and transaction
        cout << "Withdrawal successful! Balance: " << balance << endl;
    } else {
        cout << "Invalid amount or insufficient balance." << endl;
    }
}

    // Check balance
    void checkBalance() const {
        cout << "Current balance: " << balance << endl;
    }

    // View transaction history
   // View transaction history
void viewTransactions() const {
    cout << "Transaction History:" << endl;
    Transaction* current = transactions.head;
    while (current) {
        cout << current->date << " - " << current->type << ": " << current->amount << endl;
        current = current->next;
    }
}

 // Transfer money to another account
void transfer(const string& targetUser , double amount) {
    if (amount > 0 && amount <= balance) {
        

        // Check if the target user exists in their file
        string targetFileName = targetUser  + ".txt";
        ifstream targetFile(targetFileName);
        if (targetFile.is_open()) {
            Account targetAccount;
            if (targetAccount.loadFromFile(targetFileName)) {
                // Update target account balance
                targetAccount.balance += amount;
                targetAccount.transactions.addTransaction("Received Transfer", amount, getCurrentDateTime());

                // Save updated target account back to file
                targetAccount.saveToFile();

                cout << "Transfer of " << amount << " to " << targetUser  << " successful!" << endl;
                // Withdraw from current user's account
        withdraw(amount);
            } else {
                cout << "Failed to load target account details." << endl;
            }
        } else {
            cout << "Recipient user not found." << endl;
        }
    } else {
        cout << "Invalid amount or insufficient balance." << endl;
    }
}
    // Set a monthly budget
    void setBudget(double amount) {
        if (amount > 0) {
            budget = amount;
            cout << "Budget set to: " << budget << endl;
        } else {
            cout << "Invalid budget amount." << endl;
        }
    }

    // Log monthly income
    void setMonthlyIncome(double income) {
        if (income > 0) {
            monthlyIncome = income;
            cout << "Monthly income set to: " << monthlyIncome << endl;
        } else {
            cout << "Invalid income amount." << endl;
        }
    }

    // Add an expense
    void addExpense(double amount) {
        if (amount > 0) {
            transactions.addTransaction("Expense", amount, getCurrentDateTime());
            cout << "Expense of " << amount << " added successfully." << endl;
        } else {
            cout << "Amount must be positive." << endl;
        }
    }

    // Check if budget allows additional expense
    void checkExpenseFeasibility(double expense) const {
        double totalExpenses = transactions.calculateTotalExpenses();
        if (budget > 0 && (totalExpenses + expense) <= budget) {
            cout << "You can afford this expense within your budget." << endl;
        } else if (budget > 0) {
            cout << "This expense exceeds your budget by " << ((totalExpenses + expense) - budget) << "!" << endl;
        } else {
            cout << "No budget set. Expense feasibility cannot be determined." << endl;
        }
    }

    // Display finance summary
    void displayFinanceSummary() const {
        double totalExpenses = transactions.calculateTotalExpenses();
        cout << "Finance Summary:" << endl;
        cout << "Monthly Income: " << monthlyIncome << endl;
        cout << "Budget: " << budget << endl;
        cout << "Total Expenses: " << totalExpenses << endl;
        if (budget > 0) {
            if (totalExpenses <= budget) {
                cout << "You are within your budget!" << endl;
            } else {
                cout << "You have exceeded your budget by " << (totalExpenses - budget) << "!" << endl;
            }
        } else {
            cout << "No budget set." << endl;
        }
    }
// New method to check the loan amount
    void checkLoanAmount() const {
        cout << "Your current loan amount is: " << loanAmount << endl;
    }
   // New method to return the loan
void returnLoan(double amount) {
    cout << "Your current loan amount is: " << loanAmount << endl; // Display current loan amount
    if (amount > 0 && amount <= loanAmount) {
        loanAmount -= amount;
        balance += amount;
        transactions.addTransaction("Loan Return", amount, getCurrentDateTime());
        saveToFile(); // Save updated balance and loan amount to file
        cout << "Loan returned successfully! Remaining loan amount: " << loanAmount << endl;
    } else if (amount > loanAmount) {
        cout << "The amount you entered exceeds your current loan amount of " << loanAmount << "." << endl;
    } else {
        cout << "Invalid amount or insufficient loan balance." << endl;
    }
}
    // New method to get a loan
    
void getLoan(double amount) {
    if (amount > 0 && loanAmount + amount <= 5000) {
        loanAmount += amount;
        balance += amount;
        transactions.addTransaction("Loan", amount, getCurrentDateTime());
        saveToFile();  // Save updated loan amount
        cout << "Loan granted! Total loan amount: " << loanAmount << endl;
    } else {
        cout << "Loan amount exceeds limit. You can borrow up to 5000, and your current loan is: " << loanAmount << endl;
    }
}
};

bool checkLoggedIn(bool loggedIn) {
    if (!loggedIn) {
        cout << "Please login first." << endl;
    }
    return loggedIn;
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    map<string, Account> accounts;
    Account currentUser;
    bool loggedIn = false;

    welcome();

    int option;
    do {
        cout << "Enter one (1) to continue: ";
        cin >> option;
        if (option != 1) {
            cout << "Invalid key entered. Please try again." << endl;
        }
    } while (option != 1);

    // Outer loop for login/register menu
    while (true) {
        do {
            cout << "\nMain Menu:\n"
                 << "1. Login\n"
                 << "2. Register\n"
                 << "3. Exit\n"
                 << "Enter your choice: ";
            cin >> option;

            if (option == 1) {
                if (currentUser.login(accounts)) {
                    loggedIn = true;
                    pressAnyKeyToContinue();
                    break; // Exit to the main menu after login
                }
            } else if (option == 2) {
                Account newAccount;
                newAccount.createAccount(accounts);
            } else if (option == 3) {
                cout << "Goodbye!" << endl;
                return 0; // Exit the program
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
        } while (!loggedIn);

        // Secondary menu after login
        do {
            cout << "------------------------------------------------------------------------------" << endl;
            cout << "                                     MAIN MENU                                           " << endl;
            cout << "--------------------------------------------------------------------------------" << endl;
            cout << "1. Finance Management\n"
                 << "2. Account Management\n"
                 << "3. Logout\n"
                 << "Enter your choice: ";
            cin >> option;

            if (option == 1) {
                // Finance Management Menu
                int financeOption;
                do {
                    clearScreen();
                    cout << "--------------------------------------------------------------------------" << endl;
                    cout << "                          FINANCE MANAGEMENT SECTION" << endl;
                    cout << "---------------------------------------------------------------------------" << endl;
                    cout << "1. Set Monthly Income\n"
                         << "2. Add Expense\n"
                         << "3. Set Budget\n"
                         << "4. Display Finance Summary\n"
                         << "5. Check Expense Feasibility\n"
                         << "6. Back to Main Menu\n"
                         << "Enter your choice: ";
                    cin >> financeOption;

                    switch (financeOption) {
                        case 1: {
                            double income;
                            cout << "Enter your monthly income: ";
                            cin >> income;
                            currentUser.setMonthlyIncome(income);
                            pressAnyKeyToContinue();
                            break;
                        }
                        case 2: {
                            double expense;
                            cout << "Enter expense amount: ";
                            cin >> expense;
                            currentUser.addExpense(expense);
                            pressAnyKeyToContinue();
                            break;
                        }
                        case 3: {
                            double budget;
                            cout << "Enter your budget: ";
                            cin >> budget;
                            currentUser.setBudget(budget);
                            pressAnyKeyToContinue();
                            break;
                        }
                        case 4:
                            currentUser.displayFinanceSummary();
                            pressAnyKeyToContinue();
                            break;
                        case 5: {
                            double expense;
                            cout << "Enter expense amount: ";
                            cin >> expense;
                            currentUser.checkExpenseFeasibility(expense);
                            pressAnyKeyToContinue();
                            break;
                        }
                        case 6:
                            break;
                        default:
                            cout << "Invalid choice. Please try again." << endl;
                    }
                } while (financeOption != 6);

            } else if (option == 2) {
                // Account Management Menu
                int accountOption;
                do {
                    clearScreen();
                    cout << "--------------------------------------------------------------------------" << endl;
                    cout << "                          ACCOUNTS MANAGEMENT SECTION" << endl;
                    cout << "---------------------------------------------------------------------------" << endl;
                    cout << "1. Deposit\n"
                         << "2. Withdraw\n"
                         << "3. Check Balance\n"
                         << "4. Transfer Money\n"
                         << "5. Loan\n"
                         << "6. View Transactions\n"
                         << "7. Back to Main Menu\n"
                         << "Enter your choice: ";
                    cin >> accountOption;

                    switch (accountOption) {
                        case 1: {
                            double amount;
                            cout << "Enter amount to deposit: ";
                            cin >> amount;
                            currentUser.deposit(amount);
                            pressAnyKeyToContinue();
                            break;
                        }
                        case 2: {
                            double amount;
                            cout << "Enter amount to withdraw: ";
                            cin >> amount;
                            currentUser.withdraw(amount);
                            pressAnyKeyToContinue();
                            break;
                        }
                        case 3:
                            currentUser.checkBalance();
                            pressAnyKeyToContinue();
                            break;
                        case 4: {
                            string targetUser;
                            double amount;
                            cout << "Enter recipient's username: ";
                            cin >> targetUser;
                            cout << "Enter amount to transfer: ";
                            cin >> amount;
                            currentUser.transfer(targetUser, amount);
                            pressAnyKeyToContinue();
                            break;
                        }
                        case 5: {
                            // Loan Management
                            clearScreen();
                            int loanOption;
                            do {
                                cout << "--------------------------------------------------------------------------" << endl;
                                cout << "                          LOAN SECTION" << endl;
                                cout << "---------------------------------------------------------------------------" << endl;
                                cout << "1. Check Loan Amount\n"
                                     << "2. Return Loan\n"
                                     << "3. Get Loan\n"
                                     << "4. Back to Account Management\n"
                                     << "Enter your choice: ";
                                cin >> loanOption;

                                switch (loanOption) {
                                    case 1:
                                        currentUser.checkLoanAmount();
                                        pressAnyKeyToContinue();
                                        break;
                                    case 2: {
                                        double amount;
                                        cout << "Enter amount to return: ";
                                        cin >> amount;
                                        currentUser.returnLoan(amount);
                                        pressAnyKeyToContinue();
                                        break;
                                    }
                                    case 3: {
                                        double amount;
                                        cout << "Enter amount to get: ";
                                        cin >> amount;
                                        currentUser.getLoan(amount);
                                        pressAnyKeyToContinue();
                                        break;
                                    }
                                    case 4:
                                        break;
                                    default:
                                        cout << "Invalid choice. Please try again." << endl;
                                }
                            } while (loanOption != 4);
                            break;
                        }
                        case 6:
                            currentUser.viewTransactions();
                            pressAnyKeyToContinue();
                            break;
                        case 7:
                            break;
                        default:
                            cout << "Invalid choice. Please try again." << endl;
                    }
                } while (accountOption != 7);

            } else if (option == 3) {
                loggedIn = false; // Log out and return to login/register menu
                cout << "Logged out successfully." << endl;
                pressAnyKeyToContinue();
                break;
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
        } while (loggedIn);
    }

    return 0;
}
