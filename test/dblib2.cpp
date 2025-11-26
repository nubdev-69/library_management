#include"library.h"
#include<sqlite3.h>
#include<string>
#include<iostream>
#include<algorithm>

sqlite3* db;
int rc=sqlite3_open("library.db",&db);
bool openDB(){
    if(rc != SQLITE_OK){
        cout<<"cannot open database";
        return false;
    }
    return true;
}

string getMemberName(int memId){
    if(!openDB()) return;
    const char* query="select name from members where memberid=?";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK) return;
    sqlite3_bind_int(stmt,1,memId);
    string columnValue;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0); // 0 is the column index
        if (text) {
            columnValue = reinterpret_cast<const char*>(text);
        } else {
            columnValue = ""; // Handle NULL values if needed
        }
    } else {
        std::cout << "No row found or error occurred." << std::endl;
    }
    sqlite3_finalize(stmt);
    return columnValue;
}


string getBookName(int bookId){
    if(!openDB()) return;
    const char* query="select title from books where id=?";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK) return;
    sqlite3_bind_int(stmt,1,bookId);
    string columnValue;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0); // 0 is the column index
        if (text) {
            columnValue = reinterpret_cast<const char*>(text);
        } else {
            columnValue = ""; // Handle NULL values if needed
        }
    } else {
        std::cout << "No row found or error occurred." << std::endl;
    }
    sqlite3_finalize(stmt);
    return columnValue;
}


bool memberExists(int memberId){
    if(!openDB()) return false;
    const char* query="select 1 from members where memberId=? limit 1;";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK){
        return false;
    }
    sqlite3_bind_int(stmt,1,memberId);
    bool exists=(sqlite3_step(stmt)==SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;

}
bool bookExists(int bookId){
    if(!openDB()) return false;
    const char* query="select 1 from Books where id=? limit 1;";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK){
        return false;
    }
    sqlite3_bind_int(stmt,1,bookId);
    bool exists=(sqlite3_step(stmt)==SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
    
}
bool bookExists(const string& title,const string& author){
    const char* query="select 1 FROM Books WHERE title = ? AND author = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    
    sqlite3_bind_text(stmt,1,title.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,author.c_str(),-1,SQLITE_TRANSIENT);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}
int getCategoryId(const string &name){
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
string getCategory(int id){
    if(!openDB()) return;
    const char* query="select name from categories where id=?;";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK) return;
    sqlite3_bind_int(stmt,1,id);
    if(sqlite3_step(stmt) != SQLITE_OK){
        sqlite3_finalize(stmt);
        return;
    }
    string cat=(const char*)sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return cat;
}
bool isAvailable(int bookId){
    if(!openDB()) return;
    const char* query="select 1 from issue where bookid =? AND returndate IS NULL limit 1;";
    sqlite3_stmt* stmt=nullptr;

    if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt,1,bookId);

    int cp=sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return cp != SQLITE_ROW;
}
int addBook(const string& title, const string& author,const string& category,const string& filePath){
    if(!openDB()) return;

    if(bookExists(title,author)){
        cout<<"Book Already Exists!"<<endl;
        return -1;
    }
    int catId=getCategoryId(category);

    const char* query="insert into books(title, author,categoryId,filePath) values(?,?,?,?)";
    sqlite3_stmt* stmt=nullptr;
    if(sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
        return;
    
    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, category.empty() ? 0 : catId);  // 0 = no category
   if (filePath.empty())
        sqlite3_bind_null(stmt, 4);
    else
        sqlite3_bind_text(stmt, 4, filePath.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Error adding book\n";
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);
    int newId = (int)sqlite3_last_insert_rowid(db);
    cout << "Book added → ID: " << newId << endl;
    if (!filePath.empty()) cout << "File linked: " << filePath << endl;
    return newId;
}
void addMember(const string &name){
    if(!openDB()) return;

    string query="insert into members(name) values(?)";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query.c_str(),-1,&stmt,nullptr)!=SQLITE_OK)
        return;
    sqlite3_bind_text(stmt,1,name.c_str(),-1,SQLITE_TRANSIENT);
    if(sqlite3_step(stmt)!=SQLITE_DONE){
        cerr << "Error adding book\n";
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    int newId = (int)sqlite3_last_insert_rowid(db);
    cout << "Member added → ID: " << newId << endl;
}
void Library::issueBook(int memId,int bookId){
    if(!memberExists(memId) || !bookExists(bookId)){
        cerr<<"Invalid memberId or BookId"<<endl;
        return;
    }
    string member=getMemberName(memId);
    string book=getBookName(bookId);
    if(!isAvailable(bookId)){
        cout<<bookId<<" : "<<book<<" is currently not available";
        return;
    }
    if(!openDB()) return;
    const char* query="insert into issues(bookId,memberId,issuedate) values(?,?,date(now))";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK){
        return;
    }
    sqlite3_bind_int(stmt,2,memId);
    sqlite3_bind_int(stmt,1,bookId);
    if(sqlite3_step(stmt) != SQLITE_DONE){
        return;
    }
    cout<<bookId<<" "<<book<<" issued to "<<member<<endl;
    sqlite3_finalize(stmt);

}
void returnBook(int bookId){
    if(!openDB) return;
    if(!bookExists(bookId) || isAvailable(bookId)){
        cerr<<"Invalid Book";
        return;
    }
    const char* query="update issues set returndate=date('now') where bookid=? and returndate is null;";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK){
        return;
    }
    if(sqlite3_step(stmt) != SQLITE_DONE){
        sqlite3_finalize(stmt);
        return;
    }

    const char* query="select members.name from issues join members on issues.memberid=members.memberid where issues.bookid=?; ";
    stmt=nullptr;

    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK){
        return;
    }
    sqlite3_bind_int(stmt,1,bookId);
    string name;
    if(sqlite3_step(stmt) == SQLITE_ROW){
        const unsigned char* val = sqlite3_column_text(stmt,0);
        if(val) name= reinterpret_cast<const char*>(val);
    }
    else{
        sqlite3_finalize(stmt);
        return;
    }
    cout<<name<<" returned the book "<<bookId<<" "<<getBookName(bookId)<<endl;

    sqlite3_finalize(stmt);
}
void displayBook(){
    if(!openDB()) return;
    const char* query="select b.id,b.title,b.author,c.name from books b left join categories c on b.categoryId=c.id;";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK){
        return;
    }
    cout<<"<----- Books in Library ----->";
    while(sqlite3_step(stmt)==SQLITE_ROW){
        int id=sqlite3_column_int(stmt,0);
        string title = (const char*)sqlite3_column_text(stmt,1);
        string author = (const char*)sqlite3_column_text(stmt,2);
        string cat=sqlite3_column_text(stmt, 3)?(const char*) sqlite3_column_text(stmt,3):"General";
        string avl= isAvailable(id)?"Available": "Not Available";
        cout<<"Book ID | "<< id <<endl
        <<"Title : "<<title<< endl
        <<"Author | "<<author<<endl
        <<"Category : "<<cat<<endl
        << "Status : "<< avl <<endl;
    }
    sqlite3_finalize(stmt);
}
void listOfIssuedBooks(){
    if(!openDB())return;
    const char* query="select b.id,b.title,m.memberid,m.name,i.issuedate from issues join books b on i.bookid=b.id join members m on i.memberId=m.memberid where i.returndate is null;";
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK) return;
    cout<<"<-----List of Issued Book ----->";

    while(sqlite3_step(stmt)== SQLITE_ROW){
        int id=sqlite3_column_int(stmt,0);
        string title=(const char*)sqlite3_column_text(stmt,1);
        int mid=sqlite3_column_int(stmt,2);
        string name=(const char*)sqlite3_column_text(stmt,3);
        string date=(const char*)sqlite3_column_text(stmt,4);

        cout<<"Book ID : "<<id<<endl
        <<"Title : "<<title<<endl
        <<"Borrower ID: "<<mid<<endl
        <<"Name : "<<name<<endl
        <<"Date of Issue : "<<date<<endl;
    }
    sqlite3_finalize(stmt);
}
void searchBookByTitle(const string &name){
    if(!openDB()) return;
    const char* query="select id,title,author,category from books where title=?";
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK) return;
    sqlite3_bind_text(stmt,1,name.c_str(),-1,SQLITE_TRANSIENT);
    if(sqlite3_step(stmt)==SQLITE_ROW){
        int id = sqlite3_column_int(stmt,0);
        string title= (const char*)sqlite3_column_text(stmt,1);
        string author=(const char*)sqlite3_column_text(stmt,2);
        int catId=sqlite3_column_int(stmt,3);
        
        cout<<"Book Id : "<<id <<endl
        <<"Title : "<<title<<endl
        <<"Author : "<<author<<endl
        <<"Category : "<<getCategory(id)<<endl;
    }
    sqlite3_finalize(stmt);
}
void listBookIssuedToMember(int memberId){
    if(!openDB()) return;
    if(!memberExists(memberId)){
        cout<<"Member not exist";
    }
    const char* query="select b.id,b.title,b.author,i.issueDate,i.returndate from issues i join books b on i.bookid=b.id where i.memberid=?, order by i.issuedate desc;";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK) return;
    sqlite3_bind_int(stmt,1,memberId);

    bool hasAny=false;

    while(sqlite3_step(stmt)==SQLITE_ROW){
        hasAny = true;
        int bookId=sqlite3_column_int(stmt,0);
        string title=(const char*) sqlite3_column_text(stmt,1);
        string author=(const char*) sqlite3_column_text(stmt,2);
        string issueDate=(const char*) sqlite3_column_text(stmt,3);
        const char* ret=(const char*) sqlite3_column_text(stmt,4);

        string status=(ret==nullptr)?"Not Returned":"Returned on : "+string(ret);
        cout<<bookId<<" | "<<title<<" | "<<author
        <<" | "<<issueDate<<" | "<<status<<endl;

    }
    if(!hasAny)cout<<"No books ever issued to this member"<<endl;
    sqlite3_finalize(stmt);
}
void deleteBook(int bookId){
    if(!isAvailable(bookId)){
        cout<<"Book is currently issued to someone"<<endl
        <<"Cannot delete the Book"<<endl;
        return;
    }
    if(!openDB()) return;
    const char* query="delete from books where id=?";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK) return;
    sqlite3_bind_int(stmt,1,bookId);
    if(sqlite3_step(stmt)!=SQLITE_OK) return;
    cout<<"Book successfully deleted from Library\n";
    sqlite3_finalize(stmt);
}
bool hasActiveIssue(int memberId){
    if(!memberExists){
        cerr<<"member does not exists"<<endl;
        return;
    }
    if(!openDB()) return;
    const char* query="select issueid from issues where memberId=? and returndate is null limit 1";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK) return false;
    sqlite3_bind_int(stmt,1,memberId);

    bool has=(sqlite3_step(stmt)==SQLITE_ROW);
    sqlite3_finalize(stmt);
    return has;
}
void deleteMember(int memberId){
    if(!memberExists){
        cerr<<"member does not exists"<<endl;
        return;
    }
    if(hasActiveIssue(memberId)){
        cout<<memberId<<" "<<getMemberName(memberId)<<"has currenlty active issued books"<<endl
        <<"Cannot delete the member"<<endl;
        return;
    }
    if(!openDB()) return;
    const char* query="delete from members where memberid=?";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK) return;
    sqlite3_bind_int(stmt,1,memberId);
    if(sqlite3_step(stmt)!=SQLITE_OK){
        cout<<"Delete failed"<<endl;
    }else cout<<"Member successfully deleted from Library\n";
    sqlite3_finalize(stmt);
}