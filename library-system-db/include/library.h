#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>

#include <sqlite3.h>

class LIBRARY {
private:
    sqlite3* db;

    // Helper functions (private)
    bool executeQuery(const char* sql); // for simple non-select
    bool bindAndStep(sqlite3_stmt* stmt);
    void initSchema();

public:
    LIBRARY(const std::string& dbName = "library.db");
    ~LIBRARY();
    void seedDatabase();  

    // Core functions
    int  addBook(const std::string& title, const std::string& author,
                 const std::string& category = "", const std::string& filePath = "");
    void addMember(const std::string& role);

    void issueBook(int memberId, int bookId);
    void returnBook(int bookId);

    void displayAllBooks();
    void listIssuedBooks();
    void searchBookByTitle(const std::string& title);
    void listBooksByMember(int memberId);

    void deleteBook(int bookId);
    void deleteMember(int memberId);

    // Utility
    bool memberExists(int id) const;
    bool bookExists(int id) const;
    bool isBookAvailable(int bookId) const;
    std::string getMemberName(int id) const;
    std::string getBookTitle(int id) const;
    int getCategoryId(const std::string& category) const;
};

#endif