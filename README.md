# ⚡ Energy Trading System

An efficient, file-backed **Energy Trading System** implemented in C using **B+ Trees** for transaction storage and **Hash Tables** to manage and organize buyer and seller data. The system supports persistent storage and provides rich features to search, sort, and analyze energy transactions.

---

## 🚀 Features

- Add and persist energy transactions
- Fast lookup with **B+ Trees**
- Efficient seller and buyer organization using **Hash Tables**
- Create buyer–seller pair sets for analysis
- Search transactions by energy range and time period
- Sort buyers based on energy bought
- Analyze buyer-seller pairs by number of transactions
- Data persistence via CSV file
- Modular, maintainable C codebase

---

## 📂 Project Structure

├── main.c                    # Menu-driven system
├── transaction.c/.h          # Transaction + B+ Tree logic
├── seller.c/.h               # Seller data handling
├── buyer.c/.h                # Buyer data handling
├── buyersellerpair.c/.h      # Buyer-Seller Pair data handling
├── transactions.csv          # Persistent transaction records
├── files.c/.h                # All the file operations required



---

## 🧠 Data Structures Used

- **B+ Tree**  
  - Used for storing and searching transaction records
  - Supports efficient range and time-based queries

- **Hash Tables**  
  - Used for organizing buyers and sellers
  - Enables O(1) average-case insertion and search
  - Each bucket contains linked lists to handle collisions and store multiple transactions

- **Static Counters**  
  - For generating unique Transaction IDs across sessions

---

## 💡 Functionalities

- 🔄 Load and save transactions from/to `transactions.csv`
- ➕ Add new transaction with automatic timestamp and ID
- 📋 Display all transactions in sorted order (by ID)
- 🧍 Create and display a set of transactions per buyer or seller
- ⏳ Find transactions in a specific time range
- 💰 Calculate total revenue by seller
- 📊 Display transactions within an energy range (ascending)
- 📈 Sort buyers based on total energy bought
- 📌 Rank buyer-seller pairs by number of transactions
- 🔎 Search for a particular buyer or seller by ID

---

## 🛠 How to Compile

Ensure you have GCC or any standard C compiler. Run:

```bash

gcc main.c transaction.c seller.c buyer.c buyersellerpair.c files.c -o energy_system
./energy_system
```
---
## 📌 Future Enhancements
Export reports in Excel or PDF
Build a GUI using GTK or Qt
Add role-based access for buyer/seller/admin
Simulate live trading updates using threads

---

## 🙌 Acknowledgments
Special thanks to our faculty and peers who guided the development of this project. It was created as part of an academic curriculum to explore systems programming and data structures.

---
## 🧑‍💻 Author
Nidhi Lodha
Second Year CSE Student, VNIT Nagpur
GitHub: github.com/niddzeez
