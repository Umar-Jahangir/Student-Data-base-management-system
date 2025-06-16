# Student-Data-base-management-system(SDMS)

A command-line C application to manage student records and track attendance efficiently. Designed for educational institutions, it enables secure record handling, real-time attendance tracking, and generates insightful attendance reports.

---

## 🔧 Features

- 👤 **Add & Manage Students**  
  Store PRN, name, contact, GPA, age, courses, etc.

- 📅 **Attendance Tracker**  
  Mark attendance with date, calculate percentage & status (GOOD/LOW).

- 📊 **Reports**  
  View detailed reports with summaries, averages, and below-threshold alerts.

- 🛠️ **Edit & Update**  
  Modify student details or attendance records individually.

- 🧾 **CSV Data Storage**  
  Saves all records and updates in `students.csv`.

- 💾 **Backup System**  
  Automatically creates timestamped backups in the `backups/` directory.

- 🔐 **Authentication**  
  Admin login system for secure access.

---

## 📁 File Structure
![image](https://github.com/user-attachments/assets/bb8a9435-613d-4500-a5b1-86101e7b70af)



---

## 🛠 Technologies Used

- **Language**: C  
- **Headers**: `stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `time.h`  
- **Data Handling**: CSV file operations  
- **Cross-platform**: Works on both Windows and Linux (backup paths handled)

---

## 🚀 Getting Started

### 🧰 Requirements
- GCC compiler (or any standard C compiler)
- Terminal / Command Prompt

### ▶️ Compilation & Run

```bash
gcc sdms.c -o sdms
./sdms
