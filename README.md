Here is the **perfect final `README.md`** for your **single, organized `library-system` repository** — with both versions cleanly separated.

Place this file at the **root** of your repo (replaces any old README):

```markdown
# Library Management System

A complete C++ Library Management System – **two versions in one repo**:

| Version                  | Folder                     | Status     | Recommended |
|--------------------------|----------------------------|------------|-------------|
| Modern SQLite Version    | [`library system db/`](./library%20system%20db/)       | Active     | YES         |
| Legacy Text-File Version | [`library system text/`](./library%20system%20text/)   | Archived   | Learning only |

**Use the SQLite version** – it's faster, safer, and production-ready.  
The text-file version is kept for learning and historical reference.

---

### Modern Version → `library system db/`

**Features**
- SQLite database (`data/library.db`)
- Proper project structure (`src/`, `include/`)
- Fast searches, issue/return tracking
- Clean console menu
- Auto category creation

**Build & Run**
```bash
cd "library system db"
g++ -std=c++17 src/*.cpp -I include -lsqlite3 -o library_system
./library_system
```

→ Full README inside: [`library-system-db/README.md`](./library-system-db/README.md)

---

### Legacy Version → `library system text/`

**Features**
- Pure text files (`books.txt`, `members.txt`, `borrow.txt`)
- Uses `unordered_map`, `unordered_set`
- Classic file I/O implementation

**Build & Run**
```bash
cd "library system text"
g++ -std=c++17 *.cpp -o library_text
./library_text
```

→ Full README inside: [`library-system-text/README.md`](./library-system-text/README.md)

---

### Why Two Versions?

This repo shows **evolution**:
- From basic file handling → professional database design
- Perfect for portfolios, college submissions, interviews
- Demonstrates growth in C++ and software design

**I recommend using and extending the SQLite version.**

---

Made with ❤️ in C++  
GitHub: [@nubdev-69](https://github.com/nubdev-69)  
Last updated: 2025
```