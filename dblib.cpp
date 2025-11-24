#include "dblib.h"
#include <iostream>
#include <cstring>
#include <regex>
using namespace std;

// Constructor - open DB once
LIBRARY2::LIBRARY2(const string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        db = nullptr;
    }
}

LIBRARY2::~LIBRARY2() {
    if (db) sqlite3_close(db);
}

// Helper: execute simple query without binding
bool LIBRARY2::executeQuery(const char* sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool LIBRARY2::bindAndStep(sqlite3_stmt* stmt) {
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "SQL step failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    return true;
}

// Utility functions
string LIBRARY2::getMemberName(int id) const {
    string name = "Unknown";
    string sql = "SELECT name FROM Members WHERE memberId = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            name = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return name;
}

string LIBRARY2::getBookTitle(int id) const {
    string title = "Unknown";
    string sql = "SELECT title FROM books WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            title = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return title;
}

bool LIBRARY2::memberExists(int id) const {
    string sql = "SELECT 1 FROM Members WHERE memberId = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

bool LIBRARY2::bookExists(int id) const {
    string sql = "SELECT 1 FROM books WHERE id = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

bool LIBRARY2::isBookAvailable(int bookId) const {
    string sql = "SELECT 1 FROM Issues WHERE bookId = ? AND returnDate IS NULL LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, bookId);
    bool issued = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return !issued;
}

// Core functions
void LIBRARY2::issueBook(int memId, int bookId) {
    if (!memberExists(memId) || !bookExists(bookId)) {
        cerr << "Invalid member or book ID\n";
        return;
    }
    if (!isBookAvailable(bookId)) {
        cout << "Book " << bookId << " is already issued\n";
        return;
    }

    string sql = "INSERT INTO Issues (bookId, memberId, issueDate) VALUES (?, ?, date('now'));";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);
        sqlite3_bind_int(stmt, 2, memId);
        if (bindAndStep(stmt)) {
            cout << "Book " << bookId << " \"" << getBookTitle(bookId)
                 << "\" issued to " << getMemberName(memId) << endl;
        }
        sqlite3_finalize(stmt);
    }
}

void LIBRARY2::returnBook(int bookId) {
    if (!bookExists(bookId) || isBookAvailable(bookId)) {
        cerr << "Book is not issued or invalid\n";
        return;
    }

    string sql = "UPDATE Issues SET returnDate = date('now') WHERE bookId = ? AND returnDate IS NULL;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);
        if (bindAndStep(stmt)) {
            cout << getMemberName(0) /* will be fetched properly below */ << " returned book " << bookId << endl;
        }
        sqlite3_finalize(stmt);
    }

    // Show who returned it (latest issue)
    sql = "SELECT m.name FROM Issues i JOIN Members m ON i.memberId = m.memberId "
          "WHERE i.bookId = ? ORDER BY i.issueDate DESC LIMIT 1;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << (const char*)sqlite3_column_text(stmt, 0)
                 << " returned the book " << bookId << " \"" << getBookTitle(bookId) << "\"\n";
        }
        sqlite3_finalize(stmt);
    }
}

void LIBRARY2::displayAllBooks() {
    cout << "\n=== All Books in LIBRARY2 ===\n";
    string sql = R"(
        SELECT b.id, b.title, b.author, c.name, i.bookId IS NULL
        FROM books b
        LEFT JOIN categories c ON b.categoryId = c.id
        LEFT JOIN Issues i ON b.id = i.bookId AND i.returnDate IS NULL;
    )";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string title = (const char*)sqlite3_column_text(stmt, 1);
        string author = (const char*)sqlite3_column_text(stmt, 2);
        string cat = sqlite3_column_text(stmt, 3) ? (const char*)sqlite3_column_text(stmt, 3) : "General";
        bool avail = sqlite3_column_int(stmt, 4);

        cout << "ID: " << id << " | " << title << " by " << author
             << " | Category: " << cat << " | " << (avail ? "Available" : "Issued") << endl;
    }
    sqlite3_finalize(stmt);
}

void LIBRARY2::listIssuedBooks(){
    cout<< "\n <----All Issued Books by Member---->\n";
    const char* query="SELECT m.id, m.name, b.id, b.title, b.author, c.name, i.issueDate from issues i"
                        "left join members m on i.memberId=m.memberid"
                        "left join books b on i.bookId=b.id"
                        "left join categories c on b.categoryid=c.id";

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK) return;

    while(sqlite3_step(stmt)==SQLITE_ROW){
        int memberId=sqlite3_column_int(stmt,0);
        string memberName=(const char*)sqlite3_column_text(stmt,1);
        int bookId=sqlite3_column_int(stmt,2);
        string title=(const char*)sqlite3_column_text(stmt,3);
        string author=(const char*)sqlite3_column_text(stmt,4);
        string category=(const char*)sqlite3_column_text(stmt,5);
        string issueDate=(const char*)sqlite3_column_text(stmt,6);

        cout<<"Member Id : "<<memberId<<" | Name : "<<memberName<<endl
        <<"Book Id : " <<bookId<<" | Title : "<<title<<" | Author : "<<author<<" | Category : "<<category<<endl
        <<"Date of Issue : "<<issueDate<<endl;
    }
    sqlite3_finalize(stmt);
}
void LIBRARY2::searchBookByTitle(const string& title){
    const char* query="select b.id, b.title, b.author, c.name from books b"
                        "left join categories c on c.id = b.categoryId"
                        "where b.title like ?";
    sqlite3_stmt* stmt;
    string pattern="%"+title+"%";
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK) return;

    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    if(sqlite3_step(stmt)){

        while(sqlite3_step(stmt)==SQLITE_ROW){
        int id=sqlite3_column_int(stmt,0);
        string title=(const char*)sqlite3_column_text(stmt,1);
        string author=(const char*)sqlite3_column_text(stmt,2);
        string category=(const char*)sqlite3_column_text(stmt,3);
        string avaliable=isBookAvailable(id)?"Avaliable":"Not Avalilable";

        cout<<"Book Id : "<<id<<" | Title : "<<title<<" | Author : "
        <<author<<endl<<"Category : "<<category<<" | Status : "<<avaliable<<endl;
    }
    }else{
        cout<<"No books available by this id";
    }
    sqlite3_finalize(stmt);

}
void LIBRARY2::listBooksByMember(int memberId){
    const char* query="select b.id, b.title, b.author, c.name, i.issueDate from issues i"
    "left join books b on b.id=i.bookId"
    "left join categories c on c.id=b.categoryId"
    "where i.memberId=? AND i.returndate is NULL";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr) != SQLITE_OK) return;
    sqlite3_bind_int(stmt,1,memberId);
    if(sqlite3_step(stmt)){
        cout<<"MemberId : "<<memberId<<" | "<<getMemberName(memberId)<<endl<<endl;
        cout<<"<----List of Active Issued Book---->" <<endl<<endl;
        
        while(sqlite3_step(stmt)==SQLITE_ROW){
            int id=sqlite3_column_int(stmt,0);
            string title=(const char*)sqlite3_column_text(stmt,1);
            string author=(const char*)sqlite3_column_text(stmt,2);
            string category=(const char*)sqlite3_column_text(stmt,3);
            string issueDate=(const char*)sqlite3_column_text(stmt,4);

            cout<<"BookId : "<<id<<" | Title"<<title<<" | Author : "
            <<author<<" | Category : "<<category<<" | Date of Issue"<<issueDate<<endl; 
        }
    }else{
        cout<<"The Member does not have any Active Issued Book"<<endl;
    }
    sqlite3_finalize(stmt);
}
// Add other functions (listIssuedBooks, search, etc.) using same clean pattern...

int LIBRARY2::addBook(const string& title, const string& author, const string& category, const string& filePath) {
    // ... (same as before, but use clean helpers)
    // Keep your working version - just remove openDB() calls
    if(!title.size() || !author.size() || !category.size()){
        cout<<"Enter Valid Book"<<endl;
        return;
    }
    const char* query="insert into books(title,author,categoryId,filepath) values(?,?,?,?);";
    sqlite3_stmt* stmt;
    int catId=getCategoryId(category);
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK) return -1;

    sqlite3_bind_text(stmt,1,title.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,author.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,3,catId);
    if(filePath.empty()){
        sqlite3_bind_null(stmt,4);
    }else sqlite3_bind_text(stmt,4,filePath.c_str(),-1,SQLITE_TRANSIENT);
    if(sqlite3_step(stmt)!= SQLITE_DONE){
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_finalize(stmt);
    int newId = (int)sqlite3_last_insert_rowid(db);
    cout << "Book added → ID: " << newId << endl;
    if (!filePath.empty()) cout << "File linked: " << filePath << endl;
    return newId;  
}
bool isValidEmail(const std::string& email) {
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
    return std::regex_match(email, pattern);
}
string getEmail(){
    string email;
    while(email.empty()){
        cout<<"Enter Your Email"<<endl;
        cin>>email;
        if(!isValidEmail(email)){
            cout<<"Enter a Valid Email"<<endl;
            email="";
        }
    }
    return email;
}
bool validPassword(string password){
    for(int i=0;i<password.length();i++){
        if(i==0 && !((password[i]>=65 && password[i]<=90) || (password[i]>=97 && password[i]<=122)) ){
            return false;
        }
        if(!((password[i]>=64 && password[i]<=90) || (password[i]>=97 && password[i]<=122) || (password[i]>=48 && password[i]<=57) || password[i]==35 || password[i]==36)) return false;
    }
    return true;
}
string getPassword(){
    string password;
    while(password.empty()){
        cout<<"Enter Password"<<endl;
        cin>>password;
        if(!validPassword(password)){
            cout<<"Enter a Valid Password"<<endl;
            password="";
        }
    }
    return password;
    
}
void LIBRARY2::addMember(const string& role){
    string name,email,password;
    cout<<"Enter Your Name"<<endl;
    cin>>name;
    email=getEmail();
    password=getPassword();

    const char* query="insert into members(role,name,email,password) values(?,?,?,?);";
    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK) return;

    sqlite3_bind_text(stmt,1,role.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,name.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,3,email.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,4,password.c_str(),-1,SQLITE_TRANSIENT);
}

int LIBRARY2::getCategoryId(const string &name)const {
    if(name.empty()){
        return 0;
    }
    sqlite3_stmt* stmt;
    string query="select id from categories where name=?";
    sqlite3_prepare_v2(db,query.c_str(),-1, &stmt,nullptr);

    sqlite3_bind_text(stmt,1,name.c_str(),-1,SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return id;
    }
    sqlite3_finalize(stmt);

    // Insert new category
    query = "INSERT INTO Categories (categoryName) VALUES (?);";
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (int)sqlite3_last_insert_rowid(db);
}
