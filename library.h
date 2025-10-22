#ifndef LIBRARY_H
#define LIBRARY_H

#include<unordered_map>
#include "Book.h"
#include "Member.h"
using namespace std;
class Library{
private:
    unordered_map<int,int> borrow;
    unordered_map<int,Book> books;
    unordered_map<int,Member> members;

public:
    //file operation 
    void saveBooksToFiles()const;
    void saveMemToFiles() const;
    void loadBookFromFiles();
    void loadMemFromFiles();

    void addBook(const Book& book);
    void addMember(const Member& member);

    void issueBook(int memberId,int bookId);
    void returnBook(int bookId);

    void displayBook() const;
    void displayMember() const;
    void displayMemberWithBook(int memberId) const;
    void displayBookStatus(int bookId) const;
};
#endif
