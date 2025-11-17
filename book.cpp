#include "Book.h"
#include<iostream>
using namespace std;

Book::Book(int id,string t,string a /* ,string cat */){
    bookId=id;
    title=t;
    author=a;
    // category=cat;
    // isIssued=status;
}
   
//Getters function
int Book::getBookId() const{ return bookId; }
string Book::getTitle() const{ return title; }
string Book::getauthor() const{ return author; }
string Book::getCategory() const{ return category; }
// bool Book::isAvailable() const{ return !isIssued; }

//setters
void Book::setBookId(int id){bookId = id;};
void Book::setAuthor(string a){author = a;}
void Book::setTitle(string t){ title = t; }
void Book::setCategory(string cat){ category=cat; }
// void Book::issuedStatus(bool status) {isIssued = status;}

//functions

void Book::displayBook() const{
    cout<<"Book Id : "<< bookId<<endl;
    cout<<"Title : "<<title<<endl;
    cout<<"Author : "<<author<<endl;
    // cout<<"Status : " << (isIssued?"Issued":"Not Issued")<<endl;
}