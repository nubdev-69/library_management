#ifndef LIBRARY_H
#define LIBRARY_H

#include<unordered_map>
#include<unordered_set>
#include <string>
#include "Book.h"
#include "Member.h"
using namespace std;
class Library{
private:
    unordered_map<int,int> borrow; //want to add book with memberid and time & date to track it
    unordered_map<int,Book> books;
    unordered_map<string,unordered_set<int>> categories;
    unordered_map<int,Member> members;

public:
    //file operation 
    void saveBooksToFiles()const;
    void saveMemToFiles() const;
    void saveBorrowToFiles()const;
    void saveCategoryToFiles() const;

    void loadBookFromFiles();
    void loadMemFromFiles();
    void loadBorrowFromFiles();
    void loadCategoryFromFiles();

    bool isAvailable(int bookId) const;
    void addBook(const Book& book);
    void addBook(int bookId);
    void addMember(int memberId);
    void addMember(const Member& member);

    // void editBook(int bookId);
    void issueBook(int memberId,int bookId);
    void returnBook(int bookId);

    void displayBook() const;
    void displayBookWithCategories()const;
    void searchBookWithCategories() const;
    void displayMember() const;
    void displayMemberWithBook(int memberId) const;
    void displayBookStatus(int bookId) const;
};
#endif
