#include "Member.h"
#include<iostream>
using namespace std;

Member::Member(int id,string n){
    memberId=id;
    name=n;
}
// Member::Member(int id,string n,unordered_set<int> borrowedBooks){
//     memberId=id;
//     name=n;
//     this->borrowedBooks=borrowedBooks;
// }

//Getters
int Member::getMemberId() const{ return memberId; }
string Member::getMemberName() const{return name; }
unordered_set<int> Member::getBorrowedBooks() const{ return borrowedBooks; }
bool Member::hasBook(int id) const{
    if(borrowedBooks.find(id)==borrowedBooks.end()) return false;
    else return true;
}

//setters
void Member::setId(int id){ memberId=id; }
void Member::setName(string n){name=n; }

//function
void Member::displayMember() const{
    cout<<"Member Id : "<<memberId<<endl;
    cout<<"Member Name : "<<name<<endl;
    cout<<"Borrowed Books : ";
    if(borrowedBooks.empty()){
        cout<<"None" <<endl;
    }
    else{
        cout<<borrowedBooks.size()<<endl;
    }
}
void Member::borrowBook(int id){
    auto result=borrowedBooks.insert(id);
    // if(result.second){
    //     cout<<name<<" Borrowed Book Id "<<id<<endl;
    // }
    // else{
    //     cout << name << " already borrowed book ID " << id << endl;
    // }
}
void Member::returnBook(int id){
    borrowedBooks.erase(id);
}
