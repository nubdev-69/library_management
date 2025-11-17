#include<iostream>
#include<sqlite3.h>
using namespace std;
sqlite3* db;

int insertMembersIntoDB(){
    int rc= sqlite3_open("library.db",&db);
    if(rc != SQLITE_OK){
        cerr<<"Cannot open database"<<endl;
        return 1;
    }
     const char* createQuery="create table if not exists members(id integer primary key,name text not null);";

    rc = sqlite3_exec(db,createQuery,nullptr,nullptr,nullptr);
    
    if(rc != SQLITE_OK){
        cerr<<"Failed creating table"<<endl;
        return 1;
    }
    const char* insertQuery=  "INSERT INTO members(id,name) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare INSERT statement\n";
        sqlite3_close(db);
        return 1;
    }
    struct Members
    {
        int id;
        const char* name;
    } members[]={
        {101, "Suryanshu"},
    {102, "Aarav"},
    {103, "Ayush"},
    {104, "Rohan"},
    {105, "Priya"},
    {106, "Arjun"},
    {107, "Sneha"},
    {108, "Ishaan"},
    {109, "Ananya"},
    {110, "Vihaan"},
    {111, "Meera"},
    {112, "Aditya"},
    {113, "Nisha"},
    {114, "Devansh"},
    {115, "Tanya"},
    {116, "Karan"},
    {117, "Riya"},
    {118, "Arnav"},
    {119, "Aditi"},
    {120, "Shaurya"},
    {121, "Pooja"},
    {122, "Krish"},
    {123, "Diya"},
    {124, "Neel"},
    {125, "Sanika"},
    {126, "Harsh"},
    {127, "Tanishka"},
    {128, "Aarohi"},
    {129, "Yash"},
    {130, "Ishita"},
    {131, "Rudra"}
    };
    for (auto &m : members) {
        sqlite3_bind_int(stmt, 1, m.id);
        sqlite3_bind_text(stmt, 2, m.name , -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            std::cerr << "Insert failed for: " << m.name << "\n";
        }

        sqlite3_reset(stmt);  // reset for next insert
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::cout << "Members inserted successfully.\n";
    return 0;
}
int insertBooksIntoDB(){
    int rc= sqlite3_open("library.db",&db);
    if(rc != SQLITE_OK){
        cerr<<"Cannot open database"<<endl;
        return 1;
    }
    const char* createQuery="create table if not exists books(id integer primary key,title text not null,author text not null,category_id integer references categories(id) on delete set null);";

    rc = sqlite3_exec(db,createQuery,nullptr,nullptr,nullptr);
    
    if(rc != SQLITE_OK){
        cerr<<"Failed creating table"<<endl;
        return 1;
    }

    const char* insertQuery=  "INSERT INTO books(id,title,author,category_id) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare INSERT statement\n";
        sqlite3_close(db);
        return 1;
    }
    struct Books{
        int id;
        const char* title;
        const char* author;
        int category_id;
    } books[]={
        {100,"Shadow Slave","G3",18},
        {103,"A Brief History of Time","Stephen Hawking",8},
        {642,"Frankenstein","Mary Shelley",5},
        {389,"1984","George Orwell",24},
        {687,"Wuthering Heights","Emily Brontë",11},
        {734,"The Lord of the Rings","J.R.R. Tolkien",4},
        {781,"Catch-22","Joseph Heller",17},
        {102,"1984","George Orwell",24},
        {247,"To Kill a Mockingbird","Harper Lee",3},
        {926,"The Wind in the Willows","Kenneth Grahame",7},
        {738,"Dracula","Bram Stoker",6},
        {659,"Charlotte’s Web","E.B. White",7},
        {853,"The Catcher in the Rye","J.D. Salinger",13},
        {648,"Moby-Dick","Herman Melville",22},
        {437,"War and Peace","Leo Tolstoy",9},
        {276,"The Great Gatsby","F. Scott Fitzgerald",3},
        {643,"A Game of Thrones","George R.R. Martin",4},
        {934,"Dune","Frank Herbert",5},
        {746,"Fahrenheit 451","Ray Bradbury",24},
        {752,"The Silence of the Lambs","Thomas Harris",21},
        {558,"Jane Eyre","Charlotte Brontë",11},
        {921,"Crime and Punishment","Fyodor Dostoevsky",10},
        {592,"A Tale of Two Cities","Charles Dickens",9},
        {310,"The Picture of Dorian Gray","Oscar Wilde",15},
        {365,"Anna Karenina","Leo Tolstoy",11},
        {459,"The Odyssey","Homer",14},
        {265,"The Martian","Andy Weir",5},
        {894,"The Hobbit","J.R.R. Tolkien",4},
        {312,"The Color Purple","Alice Walker",19},
        {935,"The Brothers Karamazov","Fyodor Dostoevsky",1},
        {512,"Pride and Prejudice","Jane Austen",11},
        {888,"The Alchemist","Paulo Coelho",1},
        {324,"The Kite Runner","Khaled Hosseini",19},
        {472,"Les Misérables","Victor Hugo",9},
        {864,"The Grapes of Wrath","John Steinbeck",3},
        {476,"The Stranger","Albert Camus",1},
        {817,"One Hundred Years of Solitude","Gabriel García Márquez",12},
        {429,"Beloved","Toni Morrison",9},
        {915,"The Old Man and the Sea","Ernest Hemingway",3},
        {439,"The Hunger Games","Suzanne Collins",24},
        {859,"The Handmaid’s Tale","Margaret Atwood",24},
        {568,"Brave New World","Aldous Huxley",24},
        {956,"It","Stephen King",6},
        {286,"Life of Pi","Yann Martel",22},
        {674,"Slaughterhouse-Five","Kurt Vonnegut",17},
        {101,"The Hobbit","J.R.R. Tolkien",4},
        {392,"Gone Girl","Gillian Flynn",21},
        {863,"The Secret Garden","Frances Hodgson Burnett",7},
        {628,"The Book Thief","Markus Zusak",9},
        {871,"The Shining","Stephen King",6},
        {347,"The Road","Cormac McCarthy",24},
        {629,"Don Quixote","Miguel de Cervantes",3},
        {528,"The Name of the Wind","Patrick Rothfuss",4},
        {413,"The Lion, the Witch and the Wardrobe","Unknown",4},
        {941,"The Three Musketeers","Alexandre Dumas",22},
        {287,"The Count of Monte Cristo","Alexandre Dumas",22},
        {578,"Little Women","Louisa May Alcott",3},
        {715,"The Fault in Our Stars","John Green",11},
        {517,"The Da Vinci Code","Dan Brown",2},
        {584,"The Girl on the Train","Paula Hawkins",21},
        {496,"The Time Traveler’s Wife","Audrey Niffenegger",11},
        {845,"The Call of the Wild","Jack London",22},
        {109,"Theory of Everything","Stephen Hawking",8}
    };
    // Insert each category
    for (auto &b : books) {
        sqlite3_bind_int(stmt, 1, b.id);
        sqlite3_bind_text(stmt, 2, b.title, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,3, b.author, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, b.category_id);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            std::cerr << "Insert failed for: " << b.title << "\n";
        }

        sqlite3_reset(stmt);  // reset for next insert
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::cout << "Books inserted successfully.\n";
    return 0;
    
}
int insertCategoryIntoDB(){
    int rc= sqlite3_open("library.db",&db);
    if(rc != SQLITE_OK){
        cerr<<"Cannot open database"<<endl;
        return 1;
    }

    const char* createQuery="CREATE TABLE IF NOT EXISTS categories ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT NOT NULL);";
    
    rc = sqlite3_exec(db,createQuery,nullptr,nullptr,nullptr);
    
    if(rc != SQLITE_OK){
        cerr<<"Failed creating table"<<endl;
        return 1;
    }
    
    const char* insertQuery=  "INSERT INTO categories (id,name) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare INSERT statement\n";
        sqlite3_close(db);
        return 1;
    }
    struct Category {
        int id;
        const char* name;
    } categories[] = {
        {1, "Philosophy"},
        {2, "Mystery"},
        {3, "Classic"},
        {4, "Fantasy"},
        {5, "Science Fiction"},
        {6, "Horror"},
        {7, "Children"},
        {8, "Science"},
        {9, "Historical Fiction"},
        {10, "Psychological Fiction"},
        {11, "Romance"},
        {12, "Magical Realism"},
        {13, "Coming-of-Age"},
        {14, "Epic"},
        {15, "Gothic Fiction"},
        {16, "Gothic Horror"},
        {17, "War Fiction"},
        {18, "Fiction"},
        {19, "Drama"},
        {20, "Gothic Science Fiction"},
        {21, "Thriller"},
        {22, "Adventure"},
        {23, "C.S. Lewis"},
        {24, "Dystopian"}
    };

    // Insert each category
    for (auto &c : categories) {
        sqlite3_bind_int(stmt, 1, c.id);
        sqlite3_bind_text(stmt, 2, c.name, -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            std::cerr << "Insert failed for: " << c.name << "\n";
        }

        sqlite3_reset(stmt);  // reset for next insert
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::cout << "Categories inserted successfully.\n";
    return 0;
}
int main(){
    
    return insertMembersIntoDB();
}