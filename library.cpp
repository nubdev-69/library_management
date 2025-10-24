#include "library.h"
#include<fstream>
#include<sstream>
#include<istream>
#include<algorithm>
#include<iostream>
using namespace std;

const string bookFile="book.txt";
const string memFile="mem.txt";
const string borrowFiles="borrow.txt";

void Library::saveBooksToFiles()const{
    ofstream fout(bookFile);
    for(const auto& pair:books){
        const Book& book=pair.second;
        fout<<book.getBookId()<<","<<book.getTitle()<<","<<book.getauthor()<<","
        <<(book.isAvailable()?"0":"1")<<"\n";

    }
    fout.close();
}
void Library::saveMemToFiles() const{
    ofstream fout(memFile);
    for(const auto& pair:members){
        const Member& member=pair.second;
        fout<<member.getMemberId()<<","<<member.getMemberName()<<",";
        const auto& borrowed=member.getBorrowedBooks();
        bool first=true;
        for(int id:borrowed){
            if(!first) fout<<";";
            fout<<id;
            first=false;
        }
        fout<<"\n";
    }
    fout.close();
}
void Library::saveBorrowToFiles()const{
    ofstream fout(borrowFiles);
    for(const auto& pair:borrow){
        fout<<pair.first<<","<<pair.second<<"\n";
    }
    fout.close();
}
void Library::loadBookFromFiles(){
    ifstream fin(bookFile);
    if (!fin.is_open()) {
        cerr << "Error: Could not open file " << bookFile << endl;
        return;
    }
    string line;
    while(getline(fin,line)){
        stringstream ss(line);
        string idStr,title,author,issuedStr;

        getline(ss,idStr,',');
        getline(ss,title,',');
        getline(ss,author,',');
        getline(ss,issuedStr,',');
        int id=stoi(idStr);
        bool issued= (issuedStr == "1");
        books[id]=Book(id,title,author,issued);
    }
    fin.close();
    // cout << "Books loaded successfully from " << bookFile << endl;
}
void Library::loadMemFromFiles(){
    ifstream fin(memFile);
    if (!fin.is_open()) {
        cerr << "Error: Could not open file " << memFile << endl;
        return;
    }
    string line;
    while(getline(fin,line)){
        stringstream ss(line);
        string idstr,name,borrowedStr;
        getline(ss,idstr,',');
        getline(ss,name,',');
        getline(ss,borrowedStr,',');

        int id=stoi(idstr);
        Member member(id,name);

        stringstream bs(borrowedStr);
        string bId;
        while(getline(bs,bId,';')){
            if(!bId.empty()) member.borrowBook(stoi(bId));
        }
        members[id]=member;
    }
    fin.close();
    // cout << "Members loaded successfully from " << memFile << endl;
}
void Library::loadBorrowFromFiles(){
    ifstream fin(borrowFiles);
    if(!fin.is_open()){
        cerr << "Error: Could not open file " << borrowFiles << endl;
        return;
    }
    string line;
    while(getline(fin,line)){
        stringstream ss(line);
        string borrowBookId,borrowMemId;
        getline(ss,borrowBookId,',');
        getline(ss,borrowMemId,',');
        borrow[stoi(borrowBookId)]=stoi(borrowMemId);
    }
    fin.close();
    // cout << "Borrow records loaded from " << borrowFiles << endl;
}

void Library::addBook(const Book& book){
    books[book.getBookId()]=book;
    cout<<book.getTitle() << "book added successfully in Library" <<endl;

    //was thinking of appending bookfile with book data when they are added
    // but problem is it wont get availablity and when next time it will create duplicacy

    // ofstream out(bookFile,ios::app);
    // out<<book.getBookId()<<","
    // <<book.getTitle()<<","
    // <<book.getauthor()<<","
    // << books[book.getBookId()].isAvailable()<<"\n";
    // out.close();
}
void Library::addMember(const Member& member){
    members[member.getMemberId()]=member;
    cout<<member.getMemberName() <<" joined Library"<<endl;

    // same here with members
    // initially they dont have any borrowed books but after sometime they may borrow some books
    // and that will create duplicacy

    // const auto& borrowed=member.getBorrowedBooks();

    // ofstream fout(memFile);
    // out<<member.getMemberId()<<","<<member.getMemberName()<<",";
    // bool first=true;
    // for(int id:borrowed){
    //     if(!first) out<<";";
    //     out<<id;
    //     first=false;
    // }
    // out<<"\n";
    // fout.close();
}

void Library::issueBook(int memberId,int bookId){
    auto bookIt=books.find(bookId);
    auto memIt=members.find(memberId);

    if(bookIt==books.end() || memIt==members.end()){
        cout<<"Invalid BookId or MemberId\n";
        return;
    }
    if(borrow.find(bookId)!=borrow.end()){
        if(borrow[bookId]==memberId)
            cout<<books[bookId].getTitle()<<" already issued to "<<members[memberId].getMemberName();
        else
            cout<<"Book Not Available \n";
        return;
    }
    
    auto findBorrow=borrow.find(bookIt->second.getBookId());
    // if(findBorrow!=borrow.end()){
    //     if(borrow[bookIt->first]==memIt->second.getMemberId())
    //         cout<<bookIt->second.getTitle()<<"already issued to "<<memIt->second.getMemberName();
    //     cout<<bookIt->second.getTitle()<<"borrowed by Someone"; //<<memIt->second.getMemberName();
    //     return;
    // }
    bookIt->second.issuedStatus(true);
    borrow[bookId]=memberId;
    memIt->second.borrowBook(bookId);
    cout<<memIt->second.getMemberId()<<" "<<memIt->second.getMemberName()
    <<" borrowed book : "<<bookIt->second.getBookId()<<" "<<bookIt->second.getTitle();
}
void Library::returnBook(int bookId){
    auto it=borrow.find(bookId);
    if(it==borrow.end()){
        cout<<"Book currently not borrowed";
        return;
    }
    int memId=it->second;
    borrow.erase(bookId);
    books[bookId].returnBook();
    members[memId].returnBook(bookId);
    cout<<books[bookId].getTitle() << " returned by "<<members[memId].getMemberName();
}
void Library::displayBook() const{
    if(books.empty()){
        cout<<"No Books in the Library"<<endl;
        return;
    }
    cout<<"\n<----------List Of Books---------->\n";
    for(const auto& pair: books){
        pair.second.displayBook();
        cout<<endl;
        }
}
void Library::displayMember() const{
    cout<<"\n<----------List Of Members---------->\n";
    if(members.empty()){
        cout<<"No Listed members :(\n";
        return;
    }
    for(const auto& pair: members){
        pair.second.displayMember();
        cout<<endl;
    }
}
void Library::displayMemberWithBook(int memberId) const{
    auto it=members.find(memberId);
    if(it==members.end()){
        cout<<"Member not Found"<<endl;
        return;
    }
    const Member& member=it->second;
    cout<<"\n<------Members Details------>\n";
    cout<<"ID : "<<member.getMemberId()<<endl<<"Name: "<<member.getMemberName()<<endl;
    auto borrowed=member.getBorrowedBooks();
    if(borrowed.empty()){
        cout<<"No Books borrowed"<<endl;
        return;
    }
    cout<<"<------Borrowed Books------>\n";
    for(int id:borrowed){
        auto bid=books.find(id);
        if(bid!=books.end()){
            cout<<"BookId : "<< id<<" | Title: "<<bid->second.getTitle()<<endl;
        }
    }
}
void Library::displayBookStatus(int bookId) const{
    auto bookIt=books.find(bookId);
    if(bookIt==books.end()){
        cout<<"Book not Found\n";
        return;
    }
    const Book& book=bookIt->second;
    cout<<endl<<"BookId : "<< bookId<<endl;
    cout<<"Title : "<<book.getTitle() <<endl;
    cout<<"Author : "<<book.getauthor() <<endl;
    cout<<"Status : "<<(book.isAvailable()?"Available":"Not Available")<<endl;
    if (!book.isAvailable()) {
    auto it = borrow.find(bookId);
    if (it != borrow.end()) {
        int memberId = it->second;
        cout << "Borrower : " << members.at(memberId).getMemberName() << endl;
    }
}
}
