```markdown
# Library Management System (SQLite Version)

A clean, fast, and fully functional **Library Management System** built in C++ using **SQLite** as the database backend.

This is the **modern database version** of the previous text-file-based project.

## Features

- Add / Delete Books & Members  
- Issue & Return books with proper tracking  
- Search books by title (partial match)  
- View all books with availability status  
- List all issued books or books issued to a specific member  
- Automatic category creation  
- Proper input validation (email, etc.)  
- Clean console menu  

## Project Structure

```
library-system-db/
├── src/               → Source files (.cpp)
├── include/           → Header files
├── data/              → Database file (created automatically)
├── old-text-version/  → Legacy text-file version (archived)
└── README.md          → This file
```

## Requirements

- C++17 compiler (g++ / clang++ / MSVC)
- SQLite3 development library

**Ubuntu/Debian:**
```bash
sudo apt install libsqlite3-dev
```

**Fedora:**
```bash
sudo dnf install sqlite-devel
```

**macOS (with Homebrew):**
```bash
brew install sqlite
```

**Windows:** Install [SQLite](https://www.sqlite.org/download.html) or use MSYS2/MinGW

## How to Build & Run

```bash
# Clone the repo
git clone https://github.com/nubdev-69/library-system-db.git
cd library-system-db

# Compile
g++ -std=c++17 src/*.cpp -I include -lsqlite3 -o library_system

# Run
./library_system          # Linux/macOS
# or
library_system.exe        # Windows
```

First run automatically creates `data/library.db`

## Sample Menu

```
====================================
     LIBRARY MANAGEMENT SYSTEM     
====================================
1. Add New Book
2. Add New Member
3. Issue Book
4. Return Book
5. Display All Books
6. Search Book by Title
7. List All Issued Books
8. List Books Issued to a Member
9. Delete Book
10. Delete Member
0. Exit
```

## Notes

- This is a complete rewrite using SQLite — **much faster and reliable** than text files.
- Old text-file version is preserved in `old-text-version/` folder for reference.
- Database file `data/library.db` is auto-created on first launch.

## Author

Shubh
GitHub: [@nubdev-69](https://github.com/nubdev-69)

---