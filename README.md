# 📖 **Bank Management System** 💼🏦  

Welcome to the **Bank Management System** project! This system allows users to manage bank accounts efficiently, supporting operations such as account creation, deletion, transactions, and balance management. This README will guide you through the structure, logic, and usage of the system, based on the `bank.c` implementation.

# Author
Adapted for `bank.c`

---

## 🛠️ **Features**  

- **Account Creation**  
  Create new bank accounts with specified account types (Savings or Current). Accounts are assigned a unique number, reusing deleted account numbers where possible to optimize number allocation.

- **Account Deletion**  
  Delete an existing account by specifying the account holder's name and type (Savings/Current). The deleted account number is recycled for future use.

- **Transactions**  
  Perform transactions such as deposits and withdrawals. Minimum balance rules apply for Savings accounts.

- **Account Display**  
  Display all accounts sorted by account number, including details such as name, account type, and balance.

- **Low Balance Accounts**  
  List all accounts with balances below a specified threshold (currently Rs 100.00).

- **Duplicate Account Prevention**
  Prevents creation of accounts with the same name and account type.

- **Memory Management**
  Properly frees allocated memory upon program exit.

---

## 🧑‍💻 **Project Structure**  

```plaintext
.
├── bank.c      # Main logic file with all function implementations
└── README.md   # This file
```

---

## 🔑 **Core Concepts**

### 1. **AccountType Enum**  
This enum defines two types of bank accounts:  
```c
typedef enum {
    SAVINGS,  // Savings account type
    CURRENT   // Current account type
} AccountType;
```

### 2. **AccountNode Struct (Linked List for Accounts)**  
Each account has the following fields, stored in a dynamically allocated linked list:
```c
typedef struct Node {
    int AccountNumber;        // Unique account number
    char *Name;               // Account holder's name (dynamically allocated)
    AccountType accountType;  // Account type (SAVINGS or CURRENT)
    float Amount;             // Account balance
    struct Node *next;        // Pointer to the next account node
} AccountNode;

typedef AccountNode *AccountList; // Pointer to the head of the account list
```

### 3. **DeletedAccountNumNode Struct (Linked List for Recycled Numbers)**  
This linked list stores account numbers that have been deleted and can be reused:
```c
typedef struct Node1 {
    int AccountNum;           // Deleted account number
    struct Node1 *next;       // Pointer to the next deleted number node
} DeletedAccountNumNode;

typedef DeletedAccountNumNode *DeletedAccountNumList; // Pointer to the head of the deleted numbers list
```

### 4. **Global Account Number Counter**
```c
int globalNextAccountNumber = 100; // Starting point for new account numbers if no recycled numbers are available.
```

---

## 📝 **Key Functions in `bank.c`**

### 1. **`AccountList createAccount(DeletedAccountNumList *deletedNumsListHead, AccountList list, AccountType accountType, const char *Name, float Amount)`**  
  Creates a new account. It first checks if there are any recycled account numbers in `deletedNumsListHead`. If so, it uses the smallest available recycled number. Otherwise, it generates a new number using `globalNextAccountNumber`. Dynamically allocates memory for the account name.

### 2. **`AccountList deleteAccount(AccountList list, AccountType accountType, const char *Name, int *deletedAccountNumber)`**  
  Deletes an account based on the name and account type. The account number of the deleted account is captured and returned via `deletedAccountNumber` to be added to the recycled numbers list.

### 3. **`void display(AccountList l)`**  
  Displays all accounts. The list is typically sorted by account number before display.

### 4. **`AccountList sortAccountListByNumber(AccountList list)`**  
  Sorts the accounts in the `AccountList` by their `AccountNumber` in ascending order using a selection sort algorithm.

### 5. **`DeletedAccountNumList addDeletedAccountNum(DeletedAccountNumList list, int accountNumToAdd)`**  
  Adds a deleted account number to the `DeletedAccountNumList`.

### 6. **`DeletedAccountNumList sortDeletedAccountNumList(DeletedAccountNumList list)`**  
  Sorts the `DeletedAccountNumList` in ascending order. This ensures that when an account is created, the smallest recycled account number is used first.

### 7. **`AccountList transaction(AccountList list, int transactionAccountNumber, float amount, int code)`**  
  Performs a transaction (deposit if `code == 1`, withdrawal if `code == 0`) for a specified account. Includes checks for minimum balance for savings accounts and prevents overdrawing for current accounts.

### 8. **`void lowBalanceAccounts(AccountList l)`**  
  Displays accounts with balances lower than Rs 100.00.

### 9. **`int checkDuplicateAccount(AccountList list, const char *Name, AccountType accountType)`**  
  Checks if an account with the given name and account type already exists in the list. Returns `1` if a duplicate is found, `0` otherwise.

### 10. **`main()`**
    The main driver function that handles user input, calls the appropriate banking functions, and manages the main program loop. It also handles the cleanup of dynamically allocated memory for both account lists before exiting.

---

## 🚀 **How to Run**  

1. **Save the code** as `bank.c`.

2. **Compile the program** (using GCC as an example):
   ```bash
   gcc bank.c -o bank_system
   ```

3. **Run the program**:
   ```bash
   ./bank_system
   ```
   (On Windows, you might run `.\bank_system.exe` if compiled with MinGW/GCC)

4. **Interact with the program**: 
   - Type one of the following commands when prompted:
     - `CREATE`: Create a new account
     - `DELETE`: Delete an account
     - `DISPLAY`: Display all accounts (sorted by account number)
     - `TRANSACTION`: Perform a deposit/withdrawal transaction
     - `LOWBALANCE`: Display accounts with low balances (sorted by account number)
     - `EXIT`: Exit the program and free allocated memory

---

## ⚙️ **Example Workflow**  

**Creating an Account**  
Input:
```plaintext
CREATE
savings
JohnDoe
1500
```
Output (Example - Account Number may vary based on recycled numbers):
```plaintext
Account Created Successfully
Account Number: 100 
Account Holder: JohnDoe
Account Type: savings
Balance: Rs 1500.00
```

**Deleting an Account**  
Input:
```plaintext
DELETE
savings
JohnDoe
```
Output (Example):
```plaintext
Account deleted successfully! Account Number: 100
```

**Displaying All Accounts**  
Input:
```plaintext
DISPLAY
```
Output (Example - after creating JohnDoe and JaneSmith):
```plaintext
Account Number		Account Type		Name                                              		  Balance
--------------------------------------------------------------------------------------------------------------------------
100			savings			JohnDoe                                           		  1500.00
101			current			JaneSmith                                         		  2500.00
--------------------------------------------------------------------------------------------------------------------------
```

---

## 🎯 **Logic Explanation**

1.  **Account Number Recycling**:  
    The system maintains a separate sorted linked list of deleted account numbers. When a new account is created, it first attempts to reuse the smallest number from this list. If the list is empty, a new number is generated sequentially.

2.  **Dynamic Linked List Operations**:  
    Accounts are stored in a dynamically allocated linked list (`AccountList`), allowing for flexible addition and removal of accounts without a predefined limit (other than available memory).

3.  **Transaction Handling**:  
    The system allows both deposits and withdrawals. For savings accounts, a minimum balance of Rs 100.00 is enforced during withdrawals. Current accounts cannot be overdrawn by a withdrawal operation.

4.  **Account Validation**:  
    Before creating an account, the system checks for duplicates (same name and account type). Deletion and transaction operations also validate if the specified account exists.

5.  **Memory Management**:
    Dynamic memory allocated for account names and list nodes is explicitly freed when accounts are deleted and when the program exits, preventing memory leaks.

---

## 💡 **Potential Enhancements (Future)**  
- Implement **file handling** to persist account information across sessions.
- Add **security features**, like password protection for transactions or user logins.
- Introduce **interest calculation** for savings accounts.
- Enhance **error handling** and input validation (e.g., for non-numeric inputs where numbers are expected).
- Implement a more robust **sorting algorithm** if performance becomes an issue with a very large number of accounts.

---

