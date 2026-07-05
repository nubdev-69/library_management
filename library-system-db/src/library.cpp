#include "library.h"
#include <iostream>
#include <cstring>
#include<string>
#include <regex>
using namespace std;

// Constructor - open DB once
LIBRARY::LIBRARY(const string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        db = nullptr;
        return;
    }

    // Enforce FKs -- off by default in sqlite, must be set per-connection
    executeQuery("PRAGMA foreign_keys = ON;");

    initSchema();
    seedDatabase();
}

void LIBRARY::initSchema() {
    const char* schema = R"SQL(
        CREATE TABLE IF NOT EXISTS categories (
            id   INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS members (
            memberid INTEGER PRIMARY KEY AUTOINCREMENT,
            role     TEXT NOT NULL,
            name     TEXT NOT NULL,
            email    TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS books (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            title      TEXT NOT NULL,
            author     TEXT NOT NULL,
            categoryId INTEGER,
            filepath   TEXT,
            FOREIGN KEY (categoryId) REFERENCES categories(id)
        );

        CREATE TABLE IF NOT EXISTS issues (
            bookId     INTEGER NOT NULL,
            memberId   INTEGER NOT NULL,
            issueDate  TEXT NOT NULL,
            returnDate TEXT,
            FOREIGN KEY (bookId)   REFERENCES books(id),
            FOREIGN KEY (memberId) REFERENCES members(memberid)
        );
    )SQL";

    if (!executeQuery(schema)) {
        cerr << "Failed to initialize schema\n";
    }
}

LIBRARY::~LIBRARY() {
    if (db) sqlite3_close(db);
}

// database seeding with sample data:
struct BookSeed {
    int id;
    string title;
    string author;
    string category;
};

struct MemberSeed {
    int id;
    string name;
    string email;
    string password;
    string role;
};

struct IssueSeed {
    int bookId;
    int memberId;
};

void LIBRARY::seedDatabase() {
    // Idempotency guard: don't reseed if books already exist
    sqlite3_stmt* checkStmt;
    int bookCount = 0;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM books;", -1, &checkStmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(checkStmt) == SQLITE_ROW) bookCount = sqlite3_column_int(checkStmt, 0);
        sqlite3_finalize(checkStmt);
    }
    if (bookCount > 0) {
        cout << "Database already seeded (" << bookCount << " books found). Skipping.\n";
        return;
    }

    vector<BookSeed> books = {
        {100, "Shadow Slave", "G3", "fiction"},
        {103, "A Brief History of Time", "Stephen Hawking", "Science"},
        {642, "Frankenstein", "Mary Shelley", "Science Fiction"},
        {389, "1984", "George Orwell", "Dystopian"},
        {687, "Wuthering Heights", "Emily Bronte", "Romance"},
        {734, "The Lord of the Rings", "J.R.R. Tolkien", "Fantasy"},
        {781, "Catch-22", "Joseph Heller", "War Fiction"},
        {102, "1984", "George Orwell", "Dystopian"},
        {247, "To Kill a Mockingbird", "Harper Lee", "Classic"},
        {926, "The Wind in the Willows", "Kenneth Grahame", "Children"},
        {738, "Dracula", "Bram Stoker", "Horror"},
        {659, "Charlotte's Web", "E.B. White", "Children"},
        {853, "The Catcher in the Rye", "J.D. Salinger", "Coming-of-Age"},
        {648, "Moby-Dick", "Herman Melville", "Adventure"},
        {437, "War and Peace", "Leo Tolstoy", "Historical Fiction"},
        {276, "The Great Gatsby", "F. Scott Fitzgerald", "Classic"},
        {643, "A Game of Thrones", "George R.R. Martin", "Fantasy"},
        {934, "Dune", "Frank Herbert", "Science Fiction"},
        {746, "Fahrenheit 451", "Ray Bradbury", "Dystopian"},
        {752, "The Silence of the Lambs", "Thomas Harris", "Thriller"},
        {558, "Jane Eyre", "Charlotte Bronte", "Romance"},
        {921, "Crime and Punishment", "Fyodor Dostoevsky", "Psychological Fiction"},
        {592, "A Tale of Two Cities", "Charles Dickens", "Historical Fiction"},
        {310, "The Picture of Dorian Gray", "Oscar Wilde", "Gothic Fiction"},
        {365, "Anna Karenina", "Leo Tolstoy", "Romance"},
        {459, "The Odyssey", "Homer", "Epic"},
        {265, "The Martian", "Andy Weir", "Science Fiction"},
        {894, "The Hobbit", "J.R.R. Tolkien", "Fantasy"},
        {312, "The Color Purple", "Alice Walker", "Drama"},
        {935, "The Brothers Karamazov", "Fyodor Dostoevsky", "Philosophy"},
        {512, "Pride and Prejudice", "Jane Austen", "Romance"},
        {888, "The Alchemist", "Paulo Coelho", "Philosophy"},
        {324, "The Kite Runner", "Khaled Hosseini", "Drama"},
        {472, "Les Miserables", "Victor Hugo", "Historical Fiction"},
        {864, "The Grapes of Wrath", "John Steinbeck", "Classic"},
        {476, "The Stranger", "Albert Camus", "Philosophy"},
        {817, "One Hundred Years of Solitude", "Gabriel Garcia Marquez", "Magical Realism"},
        {429, "Beloved", "Toni Morrison", "Historical Fiction"},
        {915, "The Old Man and the Sea", "Ernest Hemingway", "Classic"},
        {439, "The Hunger Games", "Suzanne Collins", "Dystopian"},
        {859, "The Handmaid's Tale", "Margaret Atwood", "Dystopian"},
        {568, "Brave New World", "Aldous Huxley", "Dystopian"},
        {956, "It", "Stephen King", "Horror"},
        {286, "Life of Pi", "Yann Martel", "Adventure"},
        {674, "Slaughterhouse-Five", "Kurt Vonnegut", "War Fiction"},
        {101, "The Hobbit", "J.R.R. Tolkien", "Fantasy"},
        {392, "Gone Girl", "Gillian Flynn", "Thriller"},
        {863, "The Secret Garden", "Frances Hodgson Burnett", "Children"},
        {628, "The Book Thief", "Markus Zusak", "Historical Fiction"},
        {871, "The Shining", "Stephen King", "Horror"},
        {347, "The Road", "Cormac McCarthy", "Dystopian"},
        {629, "Don Quixote", "Miguel de Cervantes", "Classic"},
        {528, "The Name of the Wind", "Patrick Rothfuss", "Fantasy"},
        {413, "The Lion, the Witch and the Wardrobe", "C.S. Lewis", "Fantasy"},
        {941, "The Three Musketeers", "Alexandre Dumas", "Adventure"},
        {287, "The Count of Monte Cristo", "Alexandre Dumas", "Adventure"},
        {578, "Little Women", "Louisa May Alcott", "Classic"},
        {715, "The Fault in Our Stars", "John Green", "Romance"},
        {517, "The Da Vinci Code", "Dan Brown", "Mystery"},
        {584, "The Girl on the Train", "Paula Hawkins", "Thriller"},
        {496, "The Time Traveler's Wife", "Audrey Niffenegger", "Romance"},
        {845, "The Call of the Wild", "Jack London", "Adventure"},
        {109, "Theory of Everything", "Stephen Hawking", "Science"}
    };

    vector<MemberSeed> members = {
        {101, "Suryanshu", "suryanshu101@example.com", "Pass123", "member"},
        {102, "Aarav",     "aarav102@example.com",     "Pass123", "member"},
        {103, "Ayush",     "ayush103@example.com",     "Pass123", "member"},
        {104, "Rohan",     "rohan104@example.com",     "Pass123", "member"},
        {105, "Priya",     "priya105@example.com",     "Pass123", "member"},
        {106, "Arjun",     "arjun106@example.com",     "Pass123", "member"},
        {107, "Sneha",     "sneha107@example.com",     "Pass123", "member"},
        {108, "Ishaan",    "ishaan108@example.com",    "Pass123", "member"},
        {109, "Ananya",    "ananya109@example.com",    "Pass123", "member"},
        {110, "Vihaan",    "vihaan110@example.com",    "Pass123", "member"},
        {111, "Meera",     "meera111@example.com",     "Pass123", "member"},
        {112, "Aditya",    "aditya112@example.com",    "Pass123", "member"},
        {113, "Nisha",     "nisha113@example.com",     "Pass123", "member"},
        {114, "Devansh",   "devansh114@example.com",   "Pass123", "member"},
        {115, "Tanya",     "tanya115@example.com",     "Pass123", "member"},
        {116, "Karan",     "karan116@example.com",     "Pass123", "member"},
        {117, "Riya",      "riya117@example.com",      "Pass123", "member"},
        {118, "Arnav",     "arnav118@example.com",     "Pass123", "member"},
        {119, "Aditi",     "aditi119@example.com",     "Pass123", "member"},
        {120, "Shaurya",   "shaurya120@example.com",   "Pass123", "member"},
        {121, "Pooja",     "pooja121@example.com",     "Pass123", "member"},
        {122, "Krish",     "krish122@example.com",     "Pass123", "member"},
        {123, "Diya",      "diya123@example.com",      "Pass123", "member"},
        {124, "Neel",      "neel124@example.com",      "Pass123", "member"},
        {125, "Sanika",    "sanika125@example.com",    "Pass123", "member"},
        {126, "Harsh",     "harsh126@example.com",     "Pass123", "member"},
        {127, "Tanishka",  "tanishka127@example.com",  "Pass123", "member"},
        {128, "Aarohi",    "aarohi128@example.com",    "Pass123", "member"},
        {129, "Yash",      "yash129@example.com",      "Pass123", "member"},
        {130, "Ishita",    "ishita130@example.com",    "Pass123", "member"},
        {131, "Rudra",     "rudra131@example.com",     "Pass123", "member"}
    };

    vector<IssueSeed> issues = {
        {568, 106},  // Arjun currently has Brave New World
        {674, 107},  // Sneha currently has Slaughterhouse-Five
        {956, 110}   // Vihaan currently has It
    };

    executeQuery("BEGIN TRANSACTION;");

    // Insert books
    const char* bookSql = "INSERT OR IGNORE INTO books (id, title, author, categoryId) VALUES (?,?,?,?);";
    sqlite3_stmt* bookStmt;
    if (sqlite3_prepare_v2(db, bookSql, -1, &bookStmt, nullptr) != SQLITE_OK) {
        cerr << "seedDatabase book prepare failed: " << sqlite3_errmsg(db) << endl;
        executeQuery("ROLLBACK;");
        return;
    }
    for (const auto& b : books) {
        int catId = getCategoryId(b.category);
        sqlite3_bind_int(bookStmt, 1, b.id);
        sqlite3_bind_text(bookStmt, 2, b.title.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(bookStmt, 3, b.author.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(bookStmt, 4, catId);
        if (sqlite3_step(bookStmt) != SQLITE_DONE) {
            cerr << "seedDatabase book insert failed (id " << b.id << "): " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_reset(bookStmt);
    }
    sqlite3_finalize(bookStmt);

    // Insert members
    const char* memSql = "INSERT OR IGNORE INTO members (memberid, role, name, email, password) VALUES (?,?,?,?,?);";
    sqlite3_stmt* memStmt;
    if (sqlite3_prepare_v2(db, memSql, -1, &memStmt, nullptr) != SQLITE_OK) {
        cerr << "seedDatabase member prepare failed: " << sqlite3_errmsg(db) << endl;
        executeQuery("ROLLBACK;");
        return;
    }
    for (const auto& m : members) {
        sqlite3_bind_int(memStmt, 1, m.id);
        sqlite3_bind_text(memStmt, 2, m.role.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(memStmt, 3, m.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(memStmt, 4, m.email.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(memStmt, 5, m.password.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(memStmt) != SQLITE_DONE) {
            cerr << "seedDatabase member insert failed (id " << m.id << "): " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_reset(memStmt);
    }
    sqlite3_finalize(memStmt);

    // Insert issues (pre-existing loans)
    const char* issueSql = "INSERT INTO issues (bookId, memberId, issueDate, returnDate) VALUES (?,?,date('now'),NULL);";
    sqlite3_stmt* issueStmt;
    if (sqlite3_prepare_v2(db, issueSql, -1, &issueStmt, nullptr) != SQLITE_OK) {
        cerr << "seedDatabase issue prepare failed: " << sqlite3_errmsg(db) << endl;
        executeQuery("ROLLBACK;");
        return;
    }
    for (const auto& iss : issues) {
        sqlite3_bind_int(issueStmt, 1, iss.bookId);
        sqlite3_bind_int(issueStmt, 2, iss.memberId);
        if (sqlite3_step(issueStmt) != SQLITE_DONE) {
            cerr << "seedDatabase issue insert failed (book " << iss.bookId << "): " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_reset(issueStmt);
    }
    sqlite3_finalize(issueStmt);

    executeQuery("COMMIT;");
    cout << "Seeded " << books.size() << " books, " << members.size()
         << " members, " << issues.size() << " active issues.\n";
}

// Helper: execute simple query without binding
bool LIBRARY::executeQuery(const char* sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool LIBRARY::bindAndStep(sqlite3_stmt* stmt) {
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "SQL step failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    return true;
}
// Utility functions
string LIBRARY::getMemberName(int id) const {
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

string LIBRARY::getBookTitle(int id) const {
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

bool LIBRARY::memberExists(int id) const {
    string sql = "SELECT 1 FROM Members WHERE memberId = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

bool LIBRARY::bookExists(int id) const {
    string sql = "SELECT 1 FROM books WHERE id = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

bool LIBRARY::isBookAvailable(int bookId) const {
    string sql = "SELECT 1 FROM Issues WHERE bookId = ? AND returnDate IS NULL LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, bookId);
    bool issued = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return !issued;
}

// Core functions
void LIBRARY::issueBook(int memId, int bookId) {
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
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "issueBook insert prepare failed: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_bind_int(stmt, 1, bookId);
    sqlite3_bind_int(stmt, 2, memId);
    if (bindAndStep(stmt)) {
        cout << "Book " << bookId << " \"" << getBookTitle(bookId)
             << "\" issued to " << getMemberName(memId) << endl;
    }
    sqlite3_finalize(stmt);
}

void LIBRARY::returnBook(int bookId) {
    if (!bookExists(bookId) || isBookAvailable(bookId)) {
        cerr << "Book is not issued or invalid\n";
        return;
    }

    string sql = "UPDATE Issues SET returnDate = date('now') WHERE bookId = ? AND returnDate IS NULL;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, bookId);
        if (bindAndStep(stmt)) {
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

void LIBRARY::displayAllBooks() {
    cout << "\n=== All Books in LIBRARY ===\n";
    string sql = R"(
        SELECT b.id, b.title, b.author, c.name, i.bookId IS NULL
        FROM books b
        LEFT JOIN categories c ON b.categoryId = c.id
        LEFT JOIN Issues i ON b.id = i.bookId AND i.returnDate IS NULL;
    )";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK){
        sqlite3_finalize(stmt);
        return;
    }

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

void LIBRARY::listIssuedBooks(){
    cout<< "\n <----All Issued Books by Member---->\n";
    const char* query =
        "SELECT m.memberid, m.name, b.id, b.title, b.author, c.name, i.issueDate "
        "FROM issues i "
        "LEFT JOIN members m ON i.memberId=m.memberid "
        "LEFT JOIN books b ON i.bookId=b.id "
        "LEFT JOIN categories c ON b.categoryid=c.id "
        "WHERE i.returnDate IS NULL;";

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK){
        cerr << "listIssuedBooks prepare failed: " << sqlite3_errmsg(db) << endl;
        return;
    }

    bool any = false;
    while(sqlite3_step(stmt)==SQLITE_ROW){
        any = true;
        int memberId=sqlite3_column_int(stmt,0);
        string memberName=(const char*)sqlite3_column_text(stmt,1);
        int bookId=sqlite3_column_int(stmt,2);
        string title=(const char*)sqlite3_column_text(stmt,3);
        string author=(const char*)sqlite3_column_text(stmt,4);
        string category=sqlite3_column_text(stmt,5) ? (const char*)sqlite3_column_text(stmt,5) : "General";
        string issueDate=(const char*)sqlite3_column_text(stmt,6);

        cout<<"Member Id : "<<memberId<<" | Name : "<<memberName<<endl
        <<"Book Id : " <<bookId<<" | Title : "<<title<<" | Author : "<<author<<" | Category : "<<category<<endl
        <<"Date of Issue : "<<issueDate<<endl<<endl;
    }
    if (!any) cout << "No books currently issued.\n";
    sqlite3_finalize(stmt);
}
void LIBRARY::searchBookByTitle(const string& title){
    const char* query="select b.id, b.title, b.author, c.name from books b "
                        "left join categories c on c.id = b.categoryId "
                        "where b.title like ?";
    sqlite3_stmt* stmt;
    string pattern="%"+title+"%";
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK){
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

        while(sqlite3_step(stmt)==SQLITE_ROW){
        int id=sqlite3_column_int(stmt,0);
        string title=(const char*)sqlite3_column_text(stmt,1);
        string author=(const char*)sqlite3_column_text(stmt,2);
        string category=(const char*)sqlite3_column_text(stmt,3);
        string avaliable=isBookAvailable(id)?"Avaliable":"Not Avalilable";

        cout<<"Book Id : "<<id<<" | Title : "<<title<<" | Author : "
        <<author<<endl<<"Category : "<<category<<" | Status : "<<avaliable<<endl;
    }
    sqlite3_finalize(stmt);

}
void LIBRARY::listBooksByMember(int memberId){
    const char* query="select b.id, b.title, b.author, c.name, i.issueDate from issues i "
    "left join books b on b.id=i.bookId "
    "left join categories c on c.id=b.categoryId "
    "where i.memberId=? AND i.returndate is NULL";
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr) != SQLITE_OK){
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_bind_int(stmt,1,memberId);
    bool hasBook=false;
        while(sqlite3_step(stmt)==SQLITE_ROW){
            if(!hasBook){
                 cout<<"MemberId : "<<memberId<<" | "<<getMemberName(memberId)<<endl<<endl;
        cout<<"<----List of Active Issued Book---->" <<endl<<endl;
        hasBook=true;
            }
            int id=sqlite3_column_int(stmt,0);
            string title=(const char*)sqlite3_column_text(stmt,1);
            string author=(const char*)sqlite3_column_text(stmt,2);
            string category=(const char*)sqlite3_column_text(stmt,3);
            string issueDate=(const char*)sqlite3_column_text(stmt,4);

            cout<<"BookId : "<<id<<" | Title"<<title<<" | Author : "
            <<author<<" | Category : "<<category<<" | Date of Issue : "<<issueDate<<endl; 
        }
        if(!hasBook) cout<<"No active issued books for member"<<endl;
    sqlite3_finalize(stmt);
}

int LIBRARY::addBook(const string& title, const string& author, const string& category, const string& filePath) {

    if(!title.size() || !author.size() || !category.size()){
        cout<<"Enter Valid Book"<<endl;
        return 0;
    }
    const char* query="insert into books(title,author,categoryId,filepath) values(?,?,?,?);";
    sqlite3_stmt* stmt;
    int catId=getCategoryId(category);
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK){
        sqlite3_finalize(stmt);
        return -1;
    }

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
        if (i == 0 && !isalpha(password[i])) return false;
        if (!isalnum(password[i]) && password[i] != '#' && password[i] != '$' && password[i] != '@' /* etc */) return false;   
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
void LIBRARY::addMember(const string& role){
    string name,email,password;
    cout<<"Enter Your Name"<<endl;
    cin>>name;
    email=getEmail();
    password=getPassword();

    const char* query="insert into members(role,name,email,password) values(?,?,?,?);";
    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!=SQLITE_OK){
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_bind_text(stmt,1,role.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,name.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,3,email.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,4,password.c_str(),-1,SQLITE_TRANSIENT);

    if(sqlite3_step(stmt)!=SQLITE_DONE){
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    cout<<"New Member Successfully Added"<<endl;

}

int LIBRARY::getCategoryId(const string &name)const {
    if(name.empty()){
        return 0;
    }
    sqlite3_stmt* stmt;
    string query="select id from categories where name=?";
    if(sqlite3_prepare_v2(db,query.c_str(),-1, &stmt,nullptr)!=SQLITE_OK){
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_bind_text(stmt,1,name.c_str(),-1,SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return id;
    }
    sqlite3_finalize(stmt);

    // Insert new category
    sqlite3_stmt* insStmt;
    query = "INSERT INTO categories (name) VALUES (?);";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &insStmt, nullptr) != SQLITE_OK) {
        cerr << "getCategoryId insert prepare failed: " << sqlite3_errmsg(db) << endl;
        return 0;
    }
    sqlite3_bind_text(insStmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(insStmt) != SQLITE_DONE) {
        cerr << "getCategoryId insert failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(insStmt);
        return 0;
    }
    sqlite3_finalize(insStmt);

    return (int)sqlite3_last_insert_rowid(db);
}

void LIBRARY::deleteBook(int bookId){
    if(!bookExists(bookId)) return;
    sqlite3_stmt* stmt;
    const char* query="delete from books where id=?;";
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK){
        sqlite3_finalize(stmt);
        return;
    }
    
    sqlite3_bind_int(stmt,1,bookId);
    if(sqlite3_step(stmt)!= SQLITE_DONE){
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    cout<<"Book Successfully deleted"<<endl;
    return;
    
}
void LIBRARY::deleteMember(int memberId){
    if(!memberExists(memberId)) return;
    sqlite3_stmt* stmt;
    const char* query="delete from members where memberid=?;";
    if(sqlite3_prepare_v2(db,query,-1,&stmt,nullptr)!= SQLITE_OK){
        sqlite3_finalize(stmt);
        return;
    }
    
    sqlite3_bind_int(stmt,1,memberId);
    if(sqlite3_step(stmt)!= SQLITE_DONE){
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    cout<<"Member Successfully deleted"<<endl;
    return;
}
