#include "LIBRARY2.h"
#include <iostream>
#include <string>
using namespace std;

int main() {
    LIBRARY2 lib("library.db");

    int choice;
    do {
        // system("cls||clear");  // Works on Windows & Linux/macOS
        cout << "====================================\n";
        cout << "     LIBRARY MANAGEMENT SYSTEM      \n";
        cout << "====================================\n";
        cout << "1. Add New Book\n";
        cout << "2. Add New Member\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. Display All Books\n";
        cout << "6. Search Book by Title\n";
        cout << "7. List All Issued Books\n";
        cout << "8. List Books Issued to a Member\n";
        cout << "9. Delete Book\n";
        cout << "10. Delete Member\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // clear buffer

        string title, author, category, filepath;
        int memId, bookId;

        switch (choice) {
            case 1: // Add Book
                cout << "\n--- Add New Book ---\n";
                cout << "Title: ";     getline(cin, title);
                cout << "Author: ";    getline(cin, author);
                cout << "Category: ";  getline(cin, category);
                cout << "File Path (optional, press Enter to skip): ";
                getline(cin, filepath);
                lib.addBook(title, author, category, filepath.empty() ? "" : filepath);
                break;

            case 2: // Add Member
                cout << "\n--- Register New Member ---\n";
                lib.addMember("member"); // or ask for role: "admin"/"member"
                break;

            case 3: // Issue Book
                cout << "\n--- Issue Book ---\n";
                cout << "Member ID: "; cin >> memId;
                cout << "Book ID: ";   cin >> bookId;
                lib.issueBook(memId, bookId);
                break;

            case 4: // Return Book
                cout << "\n--- Return Book ---\n";
                cout << "Book ID: "; cin >> bookId;
                lib.returnBook(bookId);
                break;

            case 5: // Display All Books
                lib.displayAllBooks();
                break;

            case 6: // Search by Title
                cout << "\n--- Search Book ---\n";
                cout << "Enter title (partial): ";
                cin.ignore();
                getline(cin, title);
                lib.searchBookByTitle(title);
                break;

            case 7: // All Issued Books
                lib.listIssuedBooks();
                break;

            case 8: // Books by Member
                cout << "\n--- Books Issued to Member ---\n";
                cout << "Member ID: "; cin >> memId;
                lib.listBooksByMember(memId);
                break;

            case 9: // Delete Book
                cout << "\n--- Delete Book ---\n";
                cout << "Book ID: "; cin >> bookId;
                lib.deleteBook(bookId);
                break;

            case 10: // Delete Member
                cout << "\n--- Delete Member ---\n";
                cout << "Member ID: "; cin >> memId;
                lib.deleteMember(memId);
                break;

            case 0:
                cout << "Thank you for using the Library System!\n";
                break;

            default:
                cout << "Invalid choice! Try again.\n";
        }

        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();  // pause
        }

    } while (choice != 0);

    return 0;
}