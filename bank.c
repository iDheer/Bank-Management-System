// Bank Management System
// Author: Shreyas Mehta (Original concept), Adapted for q1.c
// Date: May 25, 2025
// Description: This program implements a simple bank management system
// supporting account creation, deletion, transactions, display, and
// low balance reporting. It features account number recycling.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variable for generating unique account numbers
// Starts from 100 and increments for new accounts if no recycled numbers are available.
int globalNextAccountNumber = 100;

// Enum to define account types
typedef enum AccountType {
    SAVINGS,  // Represents a savings account
    CURRENT   // Represents a current account
} AccountType;

// Structure for account details (AccountNode)
// Forms a node in a linked list of bank accounts.
typedef struct Node {
    int AccountNumber;        // Unique identifier for the account
    char *Name;               // Name of the account holder (dynamically allocated)
    AccountType accountType;  // Type of the account (SAVINGS or CURRENT)
    float Amount;             // Current balance in the account
    struct Node *next;        // Pointer to the next account in the list
} AccountNode;

// Structure for storing deleted account numbers (DeletedAccountNumNode)
// Forms a node in a linked list of account numbers that can be recycled.
typedef struct Node1 {
    int AccountNum;           // A deleted account number
    struct Node1 *next;       // Pointer to the next deleted account number in the list
} DeletedAccountNumNode;

// Typedef for a pointer to an AccountNode, representing the head of the account list
typedef AccountNode *AccountList;
// Typedef for a pointer to a DeletedAccountNumNode, representing the head of the deleted numbers list
typedef DeletedAccountNumNode *DeletedAccountNumList;

// Displays all accounts in the provided list.
// If the list is empty, it prints a message indicating so.
void display(AccountList l) {
    if (l == NULL) {
        printf("No Accounts to display\n");
        return;
    }

    char accountTypeStr[][20] = {
        "savings",
        "current"
    };

    printf("Account Number\t\tAccount Type\t\tName                                              \t\t  Balance\n");
    printf("--------------------------------------------------------------------------------------------------------------------------\n");

    // Traverse the list and print details of each account
    while (l != NULL) {
        printf("%d\t\t\t%s\t\t\t%-50s\t\t%10.2f\n", l->AccountNumber, accountTypeStr[l->accountType], l->Name, l->Amount);
        l = l->next;
    }
    printf("--------------------------------------------------------------------------------------------------------------------------\n");
}

// Adds a deleted account number to the list of recyclable numbers.
// Allocates a new node for the number and appends it to the end of the list.
DeletedAccountNumList addDeletedAccountNum(DeletedAccountNumList list, int accountNumToAdd) {
    DeletedAccountNumNode *new_node = (DeletedAccountNumNode *)malloc(sizeof(DeletedAccountNumNode));
    if (!new_node) {
        perror("Failed to allocate memory for new deleted account number node");
        return list; // Return original list on allocation failure
    }
    new_node->AccountNum = accountNumToAdd;
    new_node->next = NULL;

    // If the list is empty, the new node becomes the head
    if (list == NULL) {
        return new_node;
    }

    // Traverse to the end of the list and append the new node
    DeletedAccountNumNode *current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
    return list;
}

// Sorts the list of deleted account numbers in ascending order.
// Uses a selection sort algorithm. This helps in reusing the smallest available account number first.
DeletedAccountNumList sortDeletedAccountNumList(DeletedAccountNumList list) {
    if (list == NULL || list->next == NULL) {
        return list; // Already sorted or empty, no need to sort
    }
    DeletedAccountNumNode *current = list;
    while (current != NULL) {
        DeletedAccountNumNode *minNode = current;
        DeletedAccountNumNode *runner = current->next;
        // Find the node with the minimum account number in the rest of the list
        while (runner != NULL) {
            if (runner->AccountNum < minNode->AccountNum) {
                minNode = runner;
            }
            runner = runner->next;
        }
        // Swap the account numbers if a smaller one is found
        if (minNode != current) {
            int temp = current->AccountNum;
            current->AccountNum = minNode->AccountNum;
            minNode->AccountNum = temp;
        }
        current = current->next;
    }
    return list;
}

// Creates a new bank account and adds it to the account list.
// It reuses an account number from the deleted list if available, otherwise generates a new one.
AccountList createAccount(DeletedAccountNumList *deletedNumsListHead, AccountList list, AccountType accountType, const char *Name, float Amount) {
    AccountNode *new_node = (AccountNode *)malloc(sizeof(AccountNode));
    if (!new_node) {
        perror("Failed to allocate memory for new account node");
        return list; // Return original list on allocation failure
    }
    new_node->accountType = accountType;
    new_node->Name = strdup(Name); // Duplicate the name string
    if (!new_node->Name) {
        perror("Failed to allocate memory for account name");
        free(new_node); // Free the allocated AccountNode
        return list;    // Return original list on allocation failure
    }
    new_node->Amount = Amount;

    // Assign account number:
    // 1. Try to recycle from the sorted list of deleted account numbers.
    // 2. If no recycled numbers, generate a new one using globalNextAccountNumber.
    if (*deletedNumsListHead != NULL) {
        new_node->AccountNumber = (*deletedNumsListHead)->AccountNum;
        DeletedAccountNumNode *temp = *deletedNumsListHead;
        *deletedNumsListHead = (*deletedNumsListHead)->next; // Remove the used number from the list
        free(temp); // Free the node of the recycled number
    } else {
        new_node->AccountNumber = globalNextAccountNumber++; // Use global counter and increment
    }

    new_node->next = NULL;

    printf("Account Created Successfully\n");
    printf("Account Number: %d\n", new_node->AccountNumber);
    printf("Account Holder: %s\n", new_node->Name);
    printf("Account Type: %s\n", accountType == SAVINGS ? "savings" : "current");
    printf("Balance: Rs %.2f\n\n", new_node->Amount);

    // If the account list is empty, the new node becomes the head
    if (list == NULL) {
        return new_node;
    }

    // Traverse to the end of the account list and append the new node
    AccountNode *current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
    return list;
}

// Deletes an account from the list based on account holder's name and account type.
// The account number of the deleted account is returned via the deletedAccountNumber pointer.
AccountList deleteAccount(AccountList list, AccountType accountType, const char *Name, int *deletedAccountNumber) {
    AccountNode *current = list;
    AccountNode *prev = NULL;
    *deletedAccountNumber = -1; // Initialize to -1 (indicates account not found/deleted)

    if (list == NULL) {
        printf("No Accounts to delete\n");
        return NULL;
    }

    // Traverse the list to find the account to delete
    while (current != NULL) {
        if (strcmp(current->Name, Name) == 0 && current->accountType == accountType) {
            *deletedAccountNumber = current->AccountNumber; // Capture the account number

            if (prev == NULL) { // Account to delete is the head node
                list = current->next;
            } else { // Account to delete is in the middle or at the end
                prev->next = current->next;
            }
            free(current->Name); // Free the dynamically allocated name
            free(current);       // Free the account node itself
            printf("Account deleted successfully! Account Number: %d\n", *deletedAccountNumber);
            return list; // Return the modified list
        }
        prev = current;
        current = current->next;
    }

    // If the loop completes, the account was not found
    printf("Invalid: Account '%s' of type %s does not exist for deletion\n", Name, accountType == SAVINGS ? "savings" : "current");
    return list; // Return original list if not found
}

// Displays accounts with a balance less than Rs 100.00.
void lowBalanceAccounts(AccountList l) {
    if (l == NULL) {
        printf("No Accounts to display\n");
        return;
    }
    int foundLowBalance = 0; // Flag to check if any low balance account is found
    printf("Accounts with balance less than Rs 100.00:\n");
    printf("Account Number\t\tName                                              \t\t     Balance\n");
    printf("----------------------------------------------------------------------------------------------------\n");

    // Traverse the list and print details of low balance accounts
    while (l != NULL) {
        if (l->Amount < 100) {
            printf("%d\t\t\t%-50s\t\t%10.2f\n", l->AccountNumber, l->Name, l->Amount);
            foundLowBalance = 1;
        }
        l = l->next;
    }
    if (!foundLowBalance) {
        printf("No accounts found with balance less than Rs 100.00\n");
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}

// Performs a transaction (deposit or withdrawal) on a specified account.
// 'code = 1' for deposit, 'code = 0' for withdrawal.
AccountList transaction(AccountList list, int transactionAccountNumber, float amount, int code) {
    AccountNode *current = list;
    int accountFound = 0; // Flag to check if the account for transaction exists

    if (list == NULL) {
        printf("No Accounts to display for transactions\n");
        return list;
    }

    // Traverse the list to find the account
    while (current != NULL) {
        if (current->AccountNumber == transactionAccountNumber) {
            accountFound = 1;
            if (code == 1) { // Deposit
                current->Amount += amount;
                printf("Deposit successful. Updated balance for account %d is Rs.%.2f\n", transactionAccountNumber, current->Amount);
            } else if (code == 0) { // Withdrawal
                // Check for minimum balance for SAVINGS account
                if (current->accountType == SAVINGS && current->Amount - amount < 100) {
                    printf("The balance is insufficient for the specified withdrawal (Minimum Rs 100.00 required for Savings)\n");
                // Check for overdrawing for CURRENT account (balance cannot go below 0)
                } else if (current->accountType == CURRENT && current->Amount - amount < 0) {
                     printf("The balance is insufficient for the specified withdrawal (Cannot overdraw)\n");
                }
                else { // Sufficient balance for withdrawal
                    current->Amount -= amount;
                    printf("Withdrawal successful. Updated balance for account %d is Rs.%.2f\n", transactionAccountNumber, current->Amount);
                }
            } else { // Invalid transaction code
                printf("Invalid Transaction Code (1 for deposit, 0 for withdrawal)\n");
            }
            return list; // Return after processing the transaction
        }
        current = current->next;
    }

    // If the loop completes, the account was not found
    if (!accountFound) {
        printf("Invalid: Account with number %d does not exist for transaction\n", transactionAccountNumber);
    }
    return list;
}

// Sorts the account list by account number in ascending order.
// Uses a selection sort algorithm.
AccountList sortAccountListByNumber(AccountList list) {
    if (list == NULL || list->next == NULL) {
        return list; // Already sorted or empty
    }
    AccountNode *current = list;
    // Using a simple selection sort
    while (current != NULL) {
        AccountNode *minNode = current;
        AccountNode *runner = current->next;
        // Find the node with the minimum account number in the rest of the list
        while (runner != NULL) {
            if (runner->AccountNumber < minNode->AccountNumber) {
                minNode = runner;
            }
            runner = runner->next;
        }
        // Swap data of current node and minNode if they are different
        // This swaps all account details: Number, Name, Type, Amount.
        if (minNode != current) {
            // Swap AccountNumber
            int tempAccountNumber = current->AccountNumber;
            current->AccountNumber = minNode->AccountNumber;
            minNode->AccountNumber = tempAccountNumber;

            // Swap Name (pointers)
            char *tempName = current->Name;
            current->Name = minNode->Name;
            minNode->Name = tempName;

            // Swap AccountType
            AccountType tempAccountType = current->accountType;
            current->accountType = minNode->accountType;
            minNode->accountType = tempAccountType;

            // Swap Amount
            float tempAmount = current->Amount;
            current->Amount = minNode->Amount;
            minNode->Amount = tempAmount;
        }
        current = current->next;
    }
    return list;
}

// Checks if an account with the given name and account type already exists in the list.
// Returns 1 if a duplicate is found, 0 otherwise.
int checkDuplicateAccount(AccountList list, const char *Name, AccountType accountType) {
    AccountNode *current = list;
    while (current != NULL) {
        if (strcmp(current->Name, Name) == 0 && current->accountType == accountType) {
            return 1; // Duplicate found
        }
        current = current->next;
    }
    return 0; // No duplicate
}


// Main function: Drives the bank management system.
// Handles user input for various banking operations.
int main() {
    DeletedAccountNumList deletedAccountNumbersHead = NULL; // Head of the list for recycled account numbers
    AccountList accountsHead = NULL;                       // Head of the list for bank accounts

    char commandInput[100];         // Buffer for user command
    char accountTypeInputStr[20];   // Buffer for account type string ("savings"/"current")
    AccountType accType;            // Variable for AccountType enum
    float amountInput;              // Buffer for transaction/deposit amount
    char nameInput[50];             // Buffer for account holder's name (max 49 chars + null terminator)
    int targetAccountNumberInput;   // Buffer for account number in transactions
    int transactionCodeInput;       // Buffer for transaction code (0 for withdrawal, 1 for deposit)

    printf("Bank Management System (q1.c enhanced)\n");
    printf("Commands: CREATE, DELETE, DISPLAY, TRANSACTION, LOWBALANCE, EXIT\n");

    // Main command loop
    while (1) {
        printf("\nEnter command: ");
        scanf("%s", commandInput); // Read the command

        // Exit command
        if (strcmp(commandInput, "EXIT") == 0) {
            printf("Exiting program. Goodbye!\n");
            // Free allocated memory for accounts before exiting
            AccountNode *currentAcc = accountsHead;
            while (currentAcc != NULL) {
                AccountNode *nextAcc = currentAcc->next;
                free(currentAcc->Name); // Free the duplicated name string
                free(currentAcc);       // Free the account node
                currentAcc = nextAcc;
            }
            // Free allocated memory for deleted account numbers list
            DeletedAccountNumNode *currentDel = deletedAccountNumbersHead;
            while (currentDel != NULL) {
                DeletedAccountNumNode *nextDel = currentDel->next;
                free(currentDel); // Free the deleted account number node
                currentDel = nextDel;
            }
            break; // Exit the loop and terminate the program
        }
        // Create account command
        else if (strcmp(commandInput, "CREATE") == 0) {
            printf("Enter account type (savings/current): ");
            scanf("%s", accountTypeInputStr);
            printf("Enter account holder's name: ");
            scanf("%s", nameInput); // Note: scanf("%s") can lead to buffer overflow if input is too long.
                                     // Consider using fgets for safer input.
            printf("Enter initial deposit amount: ");
            scanf("%f", &amountInput);

            // Convert account type string to enum
            if (strcmp(accountTypeInputStr, "savings") == 0) {
                accType = SAVINGS;
            } else if (strcmp(accountTypeInputStr, "current") == 0) {
                accType = CURRENT;
            } else {
                printf("Invalid Account Type: '%s'. Please use 'savings' or 'current'.\n", accountTypeInputStr);
                continue; // Go to the next iteration of the loop
            }

            // Check for duplicate account before creating
            if (checkDuplicateAccount(accountsHead, nameInput, accType)) {
                printf("Invalid: Account for '%s' of type '%s' already exists.\n", nameInput, accountTypeInputStr);
            } else {
                // Sort deleted numbers list to ensure the smallest is used first for recycling
                deletedAccountNumbersHead = sortDeletedAccountNumList(deletedAccountNumbersHead);
                accountsHead = createAccount(&deletedAccountNumbersHead, accountsHead, accType, nameInput, amountInput);
            }
        }
        // Delete account command
        else if (strcmp(commandInput, "DELETE") == 0) {
            printf("Enter account type to delete (savings/current): ");
            scanf("%s", accountTypeInputStr);
            printf("Enter account holder's name to delete: ");
            scanf("%s", nameInput);

            // Convert account type string to enum
            if (strcmp(accountTypeInputStr, "savings") == 0) {
                accType = SAVINGS;
            } else if (strcmp(accountTypeInputStr, "current") == 0) {
                accType = CURRENT;
            } else {
                printf("Invalid Account Type: '%s'. Please use 'savings' or 'current'.\n", accountTypeInputStr);
                continue; // Go to the next iteration of the loop
            }

            int deletedNum = -1; // To store the account number of the deleted account
            accountsHead = deleteAccount(accountsHead, accType, nameInput, &deletedNum);
            if (deletedNum != -1) { // If an account was successfully deleted
                deletedAccountNumbersHead = addDeletedAccountNum(deletedAccountNumbersHead, deletedNum);
                // Success message is printed inside deleteAccount
            }
            // "Account does not exist" message is also printed inside deleteAccount
        }
        // Display all accounts command
        else if (strcmp(commandInput, "DISPLAY") == 0) {
            accountsHead = sortAccountListByNumber(accountsHead); // Sort accounts before displaying
            display(accountsHead);
        }
        // Display low balance accounts command
        else if (strcmp(commandInput, "LOWBALANCE") == 0) {
            accountsHead = sortAccountListByNumber(accountsHead); // Sort accounts before displaying relevant ones
            lowBalanceAccounts(accountsHead);
        }
        // Transaction command
        else if (strcmp(commandInput, "TRANSACTION") == 0) {
            printf("Enter account number for transaction: ");
            scanf("%d", &targetAccountNumberInput);
            printf("Enter amount: ");
            scanf("%f", &amountInput);
            printf("Enter transaction code (1 for deposit, 0 for withdrawal): ");
            scanf("%d", &transactionCodeInput);
            accountsHead = transaction(accountsHead, targetAccountNumberInput, amountInput, transactionCodeInput);
        }
        // Invalid command
        else {
            printf("Invalid command: '%s'. Please use CREATE, DELETE, DISPLAY, TRANSACTION, LOWBALANCE, or EXIT.\n", commandInput);
        }
    }

    return 0; // Program exits successfully
}