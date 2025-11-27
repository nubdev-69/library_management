Here’s a **perfect, clean, and professional `README.md`** for your **old text-file-based version** — the one using `.txt` files, `unordered_map`, `unordered_set`, and separate headers (`book.h`, `member.h`, `library.h`).

Put this in your **old-text-version/** folder or in a separate legacy repo:

```markdown
# Library Management System (Legacy Text-File Version)

This is the **original text-file-based** Library Management System written in C++ using plain `.txt` files for persistence and `std::unordered_map` / `std::unordered_set` for fast in-memory operations.

> **Note:** This version has been fully replaced by the modern [SQLite version](https://github.com/yourusername/library-system-db).  
> Kept here for reference, nostalgia, and learning purposes.

## Features

- Add, delete, issue, return books and members  
- Search books by title  
- List all books, issued books, or books by member  
- Categories stored in separate file  
- Borrow/return history in `borrow.txt`  
- Same console menu as the new version (for easy comparison)

## File Structure

```
old-text-version/
├── book.h
├── member.h
├── library.h
├── main.cpp              → Console menu & logic
├── books.txt             → All book records
├── members.txt           → All member records
├── category.txt          → List of categories
├── borrow.txt            → Issue/return log
└── README.md             → This file
```

## Data Format (Example)

**books.txt**
```
1|The C++ Programming Language|Bjarne Stroustrup|Programming
2|Clean Code|Robert C. Martin|Software Engineering
```

**members.txt**
```
101|John Doe|john@example.com
102|Jane Smith|jane@lib.org
```

**borrow.txt**
```
1|101|2025-04-01|
2|101|2025-04-05|2025-04-15
```

## How to Compile & Run

```bash
g++ -std=c++17 main.cpp -o library_text
./library_text          # Linux/macOS
# or
library_text.exe        # Windows
```

Data files are loaded on startup and saved on exit.

## Key Differences from SQLite Version

| Feature                  | Text-File Version         | SQLite Version            |
|--------------------------|---------------------------|---------------------------|
| Storage                  | `.txt` files              | `library.db` (SQLite)     |
| Speed                    | Slower on large data      | Very fast                 |
| Concurrency              | Not safe                  | Safe (with care)          |
| Data integrity           | Risk of corruption        | Strong ACID guarantees    |
| Search & queries         | Manual parsing            | Fast SQL queries          |
| Recommended for use      | Learning / small data     | Real projects             |

## Why We Moved to SQLite

- No risk of file corruption  
- 10–50× faster search and updates  
- Proper constraints and relationships  
- Industry-standard persistence  

This old version was excellent for learning OOP, file I/O, and data structures — but **not suitable for real-world use**.

## Author

Shubh
Originally built: 12-11-2025  
Modern version: https://github.com/nubdev-69/library-system-db

----