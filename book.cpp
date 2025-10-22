#include "Book.h"
#include<iostream>
using namespace std;

Book::Book(int id,string t,string a,bool status){
    bookId=id;
    title=t;
    author=a;
    isIssued=false;
}
   
//Getters function
int Book::getBookId() const{ return bookId; }
string Book::getTitle() const{ return title; }
string Book::getauthor() const{ return author; }
bool Book::isAvailable() const{ return !isIssued; }

//setters
void Book::setBookId(int id){bookId = id;};
void Book::setAuthor(string a){author = a;}
void Book::setTitle(string t){title = t;}
void Book::issuedStatus(bool status){isIssued = status;}

//functions

void Book::displayBook() const{
    cout<<"Book Id : "<< bookId<<endl;
    cout<<"Title : "<<title<<endl;
    cout<<"Author : "<<author<<endl;
    cout<<"Status : "<< (isIssued?"Issued":"Not Issued")<<endl;
}
void Book::issueBook(){
    if(!isIssued){
        isIssued=true;
        cout<<title<<" has been issued\n";
    }else{
        cout<<title<<" is already issued\n";
    }
}
void Book::returnBook(){
    if(isIssued){
        isIssued=false;
        cout<<title<<" has been returned \n";
    }else{
        cout<<title<<" was not issued \n";
    }
}