#include "library.h"
#include<limits>
#include<fstream>
#include<sstream>
#include<istream>
#include<algorithm>
#include<iostream>
using namespace std;

const string bookFile="book.txt";
const string memFile="mem.txt";
const string borrowFiles="borrow.txt";
const string categoryFile="category.txt";

void Library::saveBooksToFiles()const{
    ofstream fout(bookFile);
    for(const auto& pair:books){
        const Book& book=pair.second;
        fout<<book.getBookId()<<","<<book.getTitle()<<","<<book.getauthor()<<","<<book.getCategory()
        /*<<(book.isAvailable()?"0":"1")*/ <<"\n";

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
void Library::saveCategoryToFiles() const{
    ofstream fout(categoryFile);
    for(const auto& pair: categories){
        fout<<pair.first<<",";
        bool first=true;
        for(int id:pair.second){
            if(!first) fout<<";";
            fout<<id;
            first=false;
        }
        fout<<"\n";
    }
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
        string idStr,title,author,issuedStr,category;

        getline(ss,idStr,',');
        getline(ss,title,',');
        getline(ss,author,',');
        getline(ss,category,',');
        
        // getline(ss,issuedStr,',');
        int id=stoi(idStr);
        categories[category].insert(id);
        // bool issued= (issuedStr == "1");
        books[id]=Book(id,title,author/*,issued*/);
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
void Library::loadCategoryFromFiles(){
    ifstream fin(categoryFile);
    if(!fin.is_open()){
        cerr<<"Error: Could not open file "<<categoryFile<<endl;
        return;
    }
    string line;
    while(getline(fin,line)){
        stringstream ss(line);
        string category,ids;
        unordered_set<int> set;
        getline(ss,category,',');
        getline(ss,ids);

        stringstream bs(ids);
        string bId;
        while(getline(bs,bId,';')){
            if(!bId.empty()) set.insert(stoi(bId));
            books[stoi(bId)].setCategory(category);
        }
        categories[category]=set;

    }
}

bool Library::isAvailable(int bookId) const{
    return borrow.find(bookId)==borrow.end();
}


// void Library::addBook(int bookId){
//     auto it=books.find(bookId);
//     string title,author,category;
//     char choice='y';
//     if(books.find(bookId)!=books.end()){
//         cout<<"BookId exist in Library"<<endl;
//         cout<<"Enter y to Edit book:";
//         cin.ignore();
//         cin.get(choice);
//         if(choice!='y'){
//             return;
//         }
//     }
//     cin.ignore();
//     cout << "Enter Title: "; getline(cin, title);
//     cout << "Enter Author: "; getline(cin, author);
//     if(choice=='y'){
//         it->second.setTitle(title);
//         it->second.setAuthor(author);
//         cout << "Enter Category: "; getline(cin, category);
//         it->second.setCategory(category);
//         categories[category].insert(bookId);
//         return;
//     }
//     books[bookId]=Book(bookId,title,author);
// }

void Library::addBook(const Book& book){
    if(books.find(book.getBookId())!=books.end()){
        cout<<"BookId exist in Library"<<endl;
        return;
        // cout<< "Do you want to replace book\n";
        // cout<<"Enter y to continue"<<endl;
        // char choice;
        // if(choice=='y'){
        //     books.find(book.getBookId())->second=book;
        // }
        // return;
    }
    books[book.getBookId()]=book;
    cout<<"Enter Category : ";
    string cat;
    getline(cin,cat);
    
    categories[cat].insert(book.getBookId());
    books[book.getBookId()].setCategory(cat);
    cout<<book.getTitle() << " book added successfully in Library" <<endl;

    //was thinking of appending bookfile with book data when they are added
    // but problem is it wont get availablity and when next time it will create duplicacy

    // ofstream out(bookFile,ios::app);
    // out<<book.getBookId()<<","
    // <<book.getTitle()<<","
    // <<book.getauthor()<<","
    // << books[book.getBookId()].isAvailable()<<"\n";
    // out.close();
}

void Library::addMember(int memberId){
    if(members.find(memberId)!=members.end()){
        cout<<"Member Id already exist"<<endl;
        return;
    }
    string name;
    cout<<"Enter Name : "; getline(cin,name);
    members[memberId]=Member(memberId,name);
    cout<<members[memberId].getMemberName() <<" joined Library"<<endl;
}


void Library::addMember(const Member& member){
    if(members.find(member.getMemberId())!=members.end()){
        cout<<"Member Id exist"<<endl;
        return;
    }

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

/* 
void Library::editBook(int bookId){
    auto book=books.find(bookId)->second;
    int choice;
    do{
        cout<<"\n <--- Edit Book --->\n";
        cout<<"1. Edit Title"<<endl;
        cout<<"2. Edit Author"<<endl;
        cout<<"3. Edit Category"<<endl;
        cout<<"0. Exit"<<endl;
        cin>>choice;
        switch(choice){
            case 1:
            {
                string title;
                cout << "Enter Title: "; 
                getline(cin, title);
                break;
            }
            case 2:
            {
                string Author;
                cout << "Enter Author: "; 
                getline(cin, Author);
                break;
            }
            case 3:
            {
                string Category;
                cout << "Enter Category: "; 
                getline(cin, Category);
                break;
            }
            case 0:
                break;
            
            default:
                cout<< "Invalid choice! Try again.\n";
        }
        


    }while(choice);
}

*/

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
    
    // auto findBorrow=borrow.find(bookIt->second.getBookId());
    // if(findBorrow!=borrow.end()){
    //     if(borrow[bookIt->first]==memIt->second.getMemberId())
    //         cout<<bookIt->second.getTitle()<<"already issued to "<<memIt->second.getMemberName();
    //     cout<<bookIt->second.getTitle()<<"borrowed by Someone"; //<<memIt->second.getMemberName();
    //     return;
    // }

    // bookIt->second.issuedStatus(true);
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
    // books[bookId].returnBook();
    members[memId].returnBook(bookId);
    cout<<books[bookId].getTitle() << " returned by "<<members[memId].getMemberName()<<endl;
}

void Library::displayBook() const{
    if(books.empty()){
        cout<<"No Books in the Library"<<endl;
        return;
    }
    cout<<"\n<----------List Of Books---------->\n";
    for(const auto& pair: books){
        pair.second.displayBook();
        cout<<"Status : "<<(isAvailable(pair.first)?"Available":"Not Available")<<endl;
        // cout<<endl;
        }
}

void Library::displayBookWithCategories() const{
    if(books.empty() || categories.empty()){
        displayBook();
        return;
    }

    cout<<"Total Books : "<<books.size()<<endl;
    cout<<"Total categories : "<<categories.size()<<endl;

    for(auto const& pair:categories){
        cout<<"\n<====="<<pair.first<<" : "<<pair.second.size()<<" Books "<<"=====>\n\n";
        for(auto id:pair.second){
            books.find(id)->second.displayBook();
            cout<<"Status : "<<(isAvailable(id)?"Available":"Not Available")<<endl;
        }
    }
}

void Library::searchBookWithCategories() const{
    cout<<"Total Books : "<<books.size()<<endl;
    cout<<"Total categories : "<<categories.size()<<endl;
    if(books.empty() || categories.empty()){
        displayBook();
        return;
    }
    cout<<"All Categories of books available in library"<<endl;
    for(auto x:categories){
        cout<<x.first<<endl;
    }

    cout<<"Enter category to see Book in that Category: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string cat;
    getline(cin,cat);
    auto it=categories.find(cat);
    if(it==categories.end()){
        cout<<"No such categories in library \n";
        return;
    }
    for(int id:it->second){
        cout<<endl;
        books.find(id)->second.displayBook();
        cout<<"Status : "<<(isAvailable(id)?"Available":"Not Available")<<endl;
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
    cout<<endl;
    book.displayBook();
    if(isAvailable(bookId)){
        cout<<"Status : Available"<<endl;
    }else{
        cout<<"Status : Not Available"<<endl;
        auto it = borrow.find(bookId);
        if (it != borrow.end()) {
            int memberId = it->second;
            cout << "Borrower : " << members.at(memberId).getMemberName() << endl;
        }
    }

