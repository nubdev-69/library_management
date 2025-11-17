#include <iostream>
#include "library.h"

using namespace std;

int main() {
    Library lib;
    int choice;
    lib.loadBookFromFiles();
    lib.loadMemFromFiles();
    lib.loadBorrowFromFiles();
    lib.loadCategoryFromFiles();

    do {
        cout << "=== Library Management System ===\n";
        cout << "1. Add Book\n";
        cout << "2. Add Member\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. Display All Books\n";
        cout << "6. Display All Books Based on categories\n";
        cout << "7. Navigate Books in category\n";
        cout << "8. Display All Members\n";
        cout << "9. Display Member with Borrowed Books\n";
        cout << "10. Display Book Status\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1: {
                int id;
                string title, author;
                cout << "Enter Book ID: "; cin >> id;
                // lib.addBook(id);
                cin.ignore();
                cout << "Enter Title: "; getline(cin, title);
                cout << "Enter Author: "; getline(cin, author);
                lib.addBook(Book(id, title, author));
                break;
            }

            case 2: {
                int id;
                string name;
                cout << "Enter Member ID: "; cin >> id;
                cin.ignore();
                // cout << "Enter Name: "; getline(cin, name);
                // lib.addMember(Member(id, name));
                lib.addMember(id);
                break;
            }

            case 3: {
                int memberId, bookId;
                cout << "Enter Member ID: "; cin >> memberId;
                cout << "Enter Book ID: "; cin >> bookId;
                lib.issueBook(memberId, bookId);
                break;
            }

            case 4: {
                int bookId;
                cout << "Enter Book ID to return: "; cin >> bookId;
                lib.returnBook(bookId);
                break;
            }

            case 5:
                lib.displayBook();
                break;
            case 6:
                lib.displayBookWithCategories();
                break;
            case 7:{
                lib.searchBookWithCategories();
                break;
            }
            case 8:
                lib.displayMember();
                break;

            case 9: {
                int memberId;
                cout << "Enter Member ID: "; cin >> memberId;
                lib.displayMemberWithBook(memberId);
                break;
            }

            case 10: {
                int bookId;
                cout << "Enter Book ID: "; cin >> bookId;
                lib.displayBookStatus(bookId);
                break;
            }

            case 0:
                cout << "Exiting the system...\n";
                break;

            default:
                cout << "Invalid choice! Try again.\n";
        }
    } while(choice != 0);

    lib.saveBooksToFiles();
    lib.saveMemToFiles();
    lib.saveBorrowToFiles();
    lib.saveCategoryToFiles();
    return 0;
}
