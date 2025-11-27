#ifndef BOOK_H
#define BOOK_H

#include<string>
using namespace std;
class Book{
private:
    int bookId;
    string title;
    string author;
    string category;
    // bool isIssued;
public:
    Book(int id=0,string t="",string a="");

    //Getters:
    int getBookId() const;
    string getTitle() const;
    string getauthor() const;
    string getCategory() const;
    // bool isAvailable() const;

    //Setters

    void setBookId(int id);
    void setTitle(string t);
    void setAuthor(string a);
    void setCategory(string cat);
    // void issuedStatus(bool status);

    //functional methods
    void displayBook() const;
    // void issueBook();
    // void returnBook();

};
#endif