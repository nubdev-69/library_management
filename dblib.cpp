#include "dblib.h"
#include <iostream>
#include <cstring>

using namespace std;

// Constructor - open DB once
LIBRARY2::LIBRARY2(const string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        db = nullptr;
    }
}

LIBRARY2::~LIBRARY2() {
    if (db) sqlite3_close(db);
}

// Helper: execute simple query without binding
bool LIBRARY2::executeQuery(const char* sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool LIBRARY2::bindAndStep(sqlite3_stmt* stmt) {
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "SQL step failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    return true;
}

// Utility functions
string LIBRARY2::getMemberName(int id) const {
    string name = "Unknown";
    string sql = "SELECT name FROM Members WHERE memberId = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            name = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return name;
}

string LIBRARY2::getBookTitle(int id) const {
    string title = "Unknown";
    string sql = "SELECT title FROM books WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            title = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return title;
}

bool LIBRARY2::memberExists(int id) const {
    string sql = "SELECT 1 FROM Members WHERE memberId = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

bool LIBRARY2::bookExists(int id) const {
    string sql = "SELECT 1 FROM books WHERE id = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

bool LIBRARY2::isBookAvailable(int bookId) const {
    string sql = "SELECT 1 FROM Issues WHERE bookId = ? AND returnDate IS NULL LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, bookId);
    bool issued = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return !issued;
}

// Core functions
void LIBRARY2::issueBook(int memId, int bookId) {
    if (!memberExists(memId) || !bookExists(bookId)) {
        cerr << "Invalid member or book ID\n";
        return;
    }
    if (!isBookAvailable(bookId)) {
        cout << "Book " << bookId << " is already issued\n";
        return;
    }

    string sql = "INSERT INTO Issues (bookId, memberId, issueDate) VALUES (?, ?, date('now'));";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);
        sqlite3_bind_int(stmt, 2, memId);
        if (bindAndStep(stmt)) {
            cout << "Book " << bookId << " \"" << getBookTitle(bookId)
                 << "\" issued to " << getMemberName(memId) << endl;
        }
        sqlite3_finalize(stmt);
    }
}

void LIBRARY2::returnBook(int bookId) {
    if (!bookExists(bookId) || isBookAvailable(bookId)) {
        cerr << "Book is not issued or invalid\n";
        return;
    }

    string sql = "UPDATE Issues SET returnDate = date('now') WHERE bookId = ? AND returnDate IS NULL;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);
        if (bindAndStep(stmt)) {
            cout << getMemberName(0) /* will be fetched properly below */ << " returned book " << bookId << endl;
        }
        sqlite3_finalize(stmt);
    }

    // Show who returned it (latest issue)
    sql = "SELECT m.name FROM Issues i JOIN Members m ON i.memberId = m.memberId "
          "WHERE i.bookId = ? ORDER BY i.issueDate DESC LIMIT 1;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << (const char*)sqlite3_column_text(stmt, 0)
                 << " returned the book " << bookId << " \"" << getBookTitle(bookId) << "\"\n";
        }
        sqlite3_finalize(stmt);
    }
}

void LIBRARY2::displayAllBooks() {
    cout << "\n=== All Books in LIBRARY2 ===\n";
    string sql = R"(
        SELECT b.id, b.title, b.author, c.name, i.bookId IS NULL
        FROM books b
        LEFT JOIN categories c ON b.categoryId = c.id
        LEFT JOIN Issues i ON b.id = i.bookId AND i.returnDate IS NULL;
    )";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string title = (const char*)sqlite3_column_text(stmt, 1);
        string author = (const char*)sqlite3_column_text(stmt, 2);
        string cat = sqlite3_column_text(stmt, 3) ? (const char*)sqlite3_column_text(stmt, 3) : "General";
        bool avail = sqlite3_column_int(stmt, 4);

        cout << "ID: " << id << " | " << title << " by " << author
             << " | Category: " << cat << " | " << (avail ? "Available" : "Issued") << endl;
    }
    sqlite3_finalize(stmt);
}

// Add other functions (listIssuedBooks, search, etc.) using same clean pattern...

int LIBRARY2::addBook(const string& title, const string& author, const string& category, const string& filePath) {
    // ... (same as before, but use clean helpers)
    // Keep your working version - just remove openDB() calls
}