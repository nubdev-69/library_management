#ifndef MEMBER_H
#define MEMBER_H
#include <string>
#include<unordered_set>
using namespace std;

class Member{
private:
    int memberId;
    string name;
    unordered_set<int> borrowedBooks;
public:
    Member(int id=0,string n="");
    // Member(int id=0,string n="",unordered_set<int> borrowedBooks = {});
    
    //getters
    int getMemberId() const;
    string getMemberName() const;
    unordered_set<int> getBorrowedBooks() const;
    bool hasBook(int id) const;

    //setters
    void setId(int id);
    void setName(string n);
    
    //function
    void displayMember() const;
    void borrowBook(int id);
    void returnBook(int id);
};
#endif