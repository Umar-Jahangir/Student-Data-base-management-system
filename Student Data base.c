#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_STUDENTS 100
#define TEMP_FILE "temp.csv"
#define BACKUP_DIR "backups/"
#define MAX_PATH 256
#define MAX_COURSES 5
#define MAX_ATTENDANCE_RECORDS 100

// Enhanced student structure with attendance records
struct Student {
    int rollNo;
    char prn[20];
    char name[50];
    int age;
    char phone[15];
    char email[50];
    float gpa;
    // Attendance related fields
    int totalClasses;
    int attendedClasses;
    float attendancePercentage;
    char courses[MAX_COURSES][50];
    int numCourses;
    // Array to store attendance dates
    char attendanceDates[MAX_ATTENDANCE_RECORDS][11]; // Format: YYYY-MM-DD
    char attendanceStatus[MAX_ATTENDANCE_RECORDS];    // 'P' for present, 'A' for absent
    int attendanceCount;
};

// User authentication structure
struct User {
    char username[50];
    char password[50];
};

// Function to authenticate user
int authenticateUser() {
    struct User validUser = {"admin", "password123"}; // In real system, this should be stored securely
    char username[50];
    char password[50];

    printf("\nLogin Required\n");
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    if (strcmp(username, validUser.username) == 0 &&
        strcmp(password, validUser.password) == 0) {
        printf("\nLogin successful!\n");
        return 1;
    }

    printf("\nInvalid credentials!\n");
    return 0;
}

void createBackup(const char* filename) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char backupFile[MAX_PATH];

    #ifdef _WIN32
        system("mkdir backups 2> nul");
    #else
        system("mkdir -p backups");
    #endif

    sprintf(backupFile, "%sstudents_backup_%d%02d%02d_%02d%02d%02d.csv",
            BACKUP_DIR,
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    char command[MAX_PATH * 2];
    #ifdef _WIN32
        sprintf(command, "copy \"%s\" \"%s\"", filename, backupFile);
    #else
        sprintf(command, "cp \"%s\" \"%s\"", filename, backupFile);
    #endif

    system(command);
    printf("Backup created: %s\n", backupFile);
}

void displayMenu() {
    printf("\nStudent Database Management System\n");
    printf("1. Add Student\n");
    printf("2. Display All Students\n");
    printf("3. Search a Student\n");
    printf("4. Mark Attendance\n");
    printf("5. View Attendance Report\n");
    printf("6. Search and Modify Student\n");
    printf("7. Delete Student\n");
    printf("8. Create Backup\n");
    printf("9. Search student and modify their attendance\n");
    printf("10. Exit\n");
    printf("Enter your choice: ");
}

// Function to get current date as string
void getCurrentDate(char* dateStr) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    sprintf(dateStr, "%04d-%02d-%02d",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

void addStudent(const char* filename) {
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    struct Student student;
    printf("\nEnter Student Details:\n");

    printf("Enter Roll Number: ");
    scanf("%d", &student.rollNo);
    getchar();

    printf("Enter PRN: ");
    fgets(student.prn, 20, stdin);
    student.prn[strcspn(student.prn, "\n")] = 0;

    printf("Enter Name: ");
    fgets(student.name, 50, stdin);
    student.name[strcspn(student.name, "\n")] = 0;

    printf("Enter Age: ");
    scanf("%d", &student.age);
    getchar();

    printf("Enter Phone: ");
    fgets(student.phone, 15, stdin);
    student.phone[strcspn(student.phone, "\n")] = 0;

    printf("Enter Email: ");
    fgets(student.email, 50, stdin);
    student.email[strcspn(student.email, "\n")] = 0;

    printf("Enter GPA: ");
    scanf("%f", &student.gpa);

    printf("Enter number of courses (max %d): ", MAX_COURSES);
    scanf("%d", &student.numCourses);
    getchar();

    for (int i = 0; i < student.numCourses; i++) {
        printf("Enter course %d: ", i + 1);
        fgets(student.courses[i], 50, stdin);
        student.courses[i][strcspn(student.courses[i], "\n")] = 0;
    }

    // Initialize attendance
    student.totalClasses = 0;
    student.attendedClasses = 0;
    student.attendancePercentage = 0.0;
    student.attendanceCount = 0;

    // Write to CSV file
    fprintf(file, "%d,%s,%s,%d,%s,%s,%.2f,%d,%d,%.2f",
            student.rollNo, student.prn, student.name, student.age,
            student.phone, student.email, student.gpa,
            student.totalClasses, student.attendedClasses, student.attendancePercentage);

    // Write courses
    for (int i = 0; i < MAX_COURSES; i++) {
        if (i < student.numCourses) {
            fprintf(file, ",%s", student.courses[i]);
        } else {
            fprintf(file, ",");  // Empty course slot
        }
    }

    fprintf(file, "\n");
    fclose(file);
    printf("Student added successfully!\n");
}


void searchAndModifyAttendance(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char searchPRN[20];
    printf("Enter PRN to search and modify attendance: ");
    getchar();  // Clear input buffer
    fgets(searchPRN, 20, stdin);
    searchPRN[strcspn(searchPRN, "\n")] = 0;

    FILE* temp = fopen(TEMP_FILE, "w");
    if (temp == NULL) {
        printf("Error creating temporary file!\n");
        fclose(file);
        return;
    }

    char line[MAX_LINE];
    char originalLine[MAX_LINE];
    int found = 0;

    while (fgets(line, MAX_LINE, file)) {
        strcpy(originalLine, line);
        struct Student student;
        char* token = strtok(line, ",");

        // Parse student data
        student.rollNo = atoi(token);
        token = strtok(NULL, ","); strcpy(student.prn, token);

        if (strcmp(student.prn, searchPRN) == 0) {
            found = 1;
            // Continue parsing student data
            token = strtok(NULL, ","); strcpy(student.name, token);
            token = strtok(NULL, ","); student.age = atoi(token);
            token = strtok(NULL, ","); strcpy(student.phone, token);
            token = strtok(NULL, ","); strcpy(student.email, token);
            token = strtok(NULL, ","); student.gpa = atof(token);
            token = strtok(NULL, ","); student.totalClasses = atoi(token);
            token = strtok(NULL, ","); student.attendedClasses = atoi(token);
            token = strtok(NULL, ","); student.attendancePercentage = atof(token);

            printf("\nStudent Found:\n");
            printf("Name: %s\n", student.name);
            printf("Current Attendance: %d/%d classes (%.2f%%)\n",
                   student.attendedClasses, student.totalClasses,
                   student.attendancePercentage);

            char choice;
            printf("\nDo you want to modify attendance? (y/n): ");
            scanf(" %c", &choice);

            if (tolower(choice) == 'y') {
                int newTotal, newAttended;
                printf("Enter new total classes: ");
                scanf("%d", &newTotal);
                do {
                    printf("Enter new attended classes (should not exceed total classes): ");
                    scanf("%d", &newAttended);
                    if (newAttended > newTotal) {
                        printf("Error: Attended classes cannot exceed total classes!\n");
                    }
                } while (newAttended > newTotal);

                student.totalClasses = newTotal;
                student.attendedClasses = newAttended;
                student.attendancePercentage = (float)student.attendedClasses / student.totalClasses * 100;

                printf("\nAttendance Updated Successfully!\n");
                printf("New Attendance: %d/%d classes (%.2f%%)\n",
                       student.attendedClasses, student.totalClasses,
                       student.attendancePercentage);
            }

            // Write modified record
            fprintf(temp, "%d,%s,%s,%d,%s,%s,%.2f,%d,%d,%.2f",
                    student.rollNo, student.prn, student.name,
                    student.age, student.phone, student.email,
                    student.gpa, student.totalClasses,
                    student.attendedClasses, student.attendancePercentage);

            // Copy remaining course data from original line
            char* rest = strchr(originalLine, '\n');
            if (rest != NULL) {
                *rest = '\0';
            }
            int count = 0;
            char* pos = originalLine;
            while (count < 10 && (pos = strchr(pos + 1, ',')) != NULL) {
                count++;
            }
            if (pos != NULL) {
                fprintf(temp, "%s\n", pos);
            } else {
                fprintf(temp, "\n");
            }
        } else {
            fprintf(temp, "%s", originalLine);
        }
    }

    if (!found) {
        printf("Student with PRN %s not found.\n", searchPRN);
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename(TEMP_FILE, filename);
}


void searchAndModifyStudent(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char searchPRN[20];
    printf("Enter PRN to search and modify: ");
    getchar();  // Clear input buffer
    fgets(searchPRN, 20, stdin);
    searchPRN[strcspn(searchPRN, "\n")] = 0;

    FILE* temp = fopen(TEMP_FILE, "w");
    if (temp == NULL) {
        printf("Error creating temporary file!\n");
        fclose(file);
        return;
    }

    char line[MAX_LINE];
    int found = 0;

    while (fgets(line, MAX_LINE, file)) {
        char lineCopy[MAX_LINE];
        strcpy(lineCopy, line);
        struct Student student;
        char* token = strtok(line, ",");

        // Parse current student data
        student.rollNo = atoi(token);
        token = strtok(NULL, ","); strcpy(student.prn, token);

        if (strcmp(student.prn, searchPRN) == 0) {
            found = 1;
            // Parse remaining current data
            token = strtok(NULL, ","); strcpy(student.name, token);
            token = strtok(NULL, ","); student.age = atoi(token);
            token = strtok(NULL, ","); strcpy(student.phone, token);
            token = strtok(NULL, ","); strcpy(student.email, token);
            token = strtok(NULL, ","); student.gpa = atof(token);
            token = strtok(NULL, ","); student.totalClasses = atoi(token);
            token = strtok(NULL, ","); student.attendedClasses = atoi(token);
            token = strtok(NULL, ","); student.attendancePercentage = atof(token);

            printf("\nCurrent Student Details:\n");
            printf("Roll No: %d\n", student.rollNo);
            printf("PRN: %s\n", student.prn);
            printf("Name: %s\n", student.name);
            printf("Age: %d\n", student.age);
            printf("Phone: %s\n", student.phone);
            printf("Email: %s\n", student.email);
            printf("GPA: %.2f\n", student.gpa);

            printf("\nEnter new details (press Enter to keep current value):\n");

            // Get new Roll Number - Fixed implementation
            char rollInput[50];
            printf("New Roll Number [%d]: ", student.rollNo);
            fgets(rollInput, sizeof(rollInput), stdin);
            rollInput[strcspn(rollInput, "\n")] = 0;
            if (strlen(rollInput) > 0) {
                student.rollNo = atoi(rollInput);
            }

            // Get new PRN
            printf("New PRN [%s]: ", student.prn);
            char input[50];
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            if (strlen(input) > 0) {
                strcpy(student.prn, input);
            }

            // Get new Name
            printf("New Name [%s]: ", student.name);
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            if (strlen(input) > 0) {
                strcpy(student.name, input);
            }

            // Get new Age
            printf("New Age [%d]: ", student.age);
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            if (strlen(input) > 0) {
                student.age = atoi(input);
            }

            // Get new Phone
            printf("New Phone [%s]: ", student.phone);
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            if (strlen(input) > 0) {
                strcpy(student.phone, input);
            }

            // Get new Email
            printf("New Email [%s]: ", student.email);
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            if (strlen(input) > 0) {
                strcpy(student.email, input);
            }

            // Get new GPA
            printf("New GPA [%.2f]: ", student.gpa);
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            if (strlen(input) > 0) {
                student.gpa = atof(input);
            }

            // Write modified record
            fprintf(temp, "%d,%s,%s,%d,%s,%s,%.2f,%d,%d,%.2f",
                    student.rollNo, student.prn, student.name,
                    student.age, student.phone, student.email,
                    student.gpa, student.totalClasses,
                    student.attendedClasses, student.attendancePercentage);

            // Copy existing course data
            char* rest = strchr(lineCopy, '\n');
            if (rest != NULL) {
                *rest = '\0';
            }
            int count = 0;
            char* pos = lineCopy;
            while (count < 10 && (pos = strchr(pos + 1, ',')) != NULL) {
                count++;
            }
            if (pos != NULL) {
                fprintf(temp, "%s\n", pos);
            } else {
                fprintf(temp, "\n");
            }

            printf("\nStudent record modified successfully!\n");
        } else {
            fprintf(temp, "%s", lineCopy);
        }
    }

    if (!found) {
        printf("Student with PRN %s not found.\n", searchPRN);
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename(TEMP_FILE, filename);
}

void markAttendance(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char currentDate[11];
    getCurrentDate(currentDate);

    printf("\nMarking attendance for date: %s\n", currentDate);

    FILE* temp = fopen(TEMP_FILE, "w");
    if (temp == NULL) {
        printf("Error creating temporary file!\n");
        fclose(file);
        return;
    }

    char line[MAX_LINE];
    char originalLine[MAX_LINE];

    while (fgets(line, MAX_LINE, file)) {
        strcpy(originalLine, line);  // Keep original line for reference
        struct Student student;
        char* token = strtok(line, ",");

        // Parse student data
        student.rollNo = atoi(token);
        token = strtok(NULL, ","); strcpy(student.prn, token);
        token = strtok(NULL, ","); strcpy(student.name, token);
        token = strtok(NULL, ","); student.age = atoi(token);
        token = strtok(NULL, ","); strcpy(student.phone, token);
        token = strtok(NULL, ","); strcpy(student.email, token);
        token = strtok(NULL, ","); student.gpa = atof(token);
        token = strtok(NULL, ","); student.totalClasses = atoi(token);
        token = strtok(NULL, ","); student.attendedClasses = atoi(token);
        token = strtok(NULL, ","); student.attendancePercentage = atof(token);

        // Display current attendance before marking
        printf("\nStudent: %s (PRN: %s)\n", student.name, student.prn);
        printf("Current Attendance: %d/%d classes (%.2f%%)\n",
               student.attendedClasses, student.totalClasses,
               student.attendancePercentage);

        printf("Mark attendance (P/A): ");
        char attendance;
        scanf(" %c", &attendance);
        attendance = toupper(attendance);

        // Update attendance counts
        student.totalClasses++;
        if (attendance == 'P') {
            student.attendedClasses++;
        }
        student.attendancePercentage = (float)student.attendedClasses / student.totalClasses * 100;

        // Display updated attendance
        printf("Updated Attendance: %d/%d classes (%.2f%%)\n",
               student.attendedClasses, student.totalClasses,
               student.attendancePercentage);

        // Write updated record to temp file
        fprintf(temp, "%d,%s,%s,%d,%s,%s,%.2f,%d,%d,%.2f",
                student.rollNo, student.prn, student.name,
                student.age, student.phone, student.email,
                student.gpa, student.totalClasses,
                student.attendedClasses, student.attendancePercentage);

        // Copy remaining course data from original line
        char* rest = strchr(originalLine, '\n');
        if (rest != NULL) {
            *rest = '\0';  // Remove newline
        }
        // Find the position after the attendance percentage in original line
        int count = 0;
        char* pos = originalLine;
        while (count < 10 && (pos = strchr(pos + 1, ',')) != NULL) {
            count++;
        }
        if (pos != NULL) {
            fprintf(temp, "%s\n", pos);
        } else {
            fprintf(temp, "\n");
        }
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename(TEMP_FILE, filename);
    printf("\nAttendance marked successfully!\n");
}



void searchStudent(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char searchPRN[20];
    printf("Enter PRN to search: ");
    getchar();  // Clear input buffer
    fgets(searchPRN, 20, stdin);
    searchPRN[strcspn(searchPRN, "\n")] = 0;

    char line[MAX_LINE];
    int found = 0;

    while (fgets(line, MAX_LINE, file)) {
        struct Student student;
        char* token = strtok(line, ",");

        student.rollNo = atoi(token);
        token = strtok(NULL, ","); strcpy(student.prn, token);

        if (strcmp(student.prn, searchPRN) == 0) {
            token = strtok(NULL, ","); strcpy(student.name, token);
            token = strtok(NULL, ","); student.age = atoi(token);
            token = strtok(NULL, ","); strcpy(student.phone, token);
            token = strtok(NULL, ","); strcpy(student.email, token);
            token = strtok(NULL, ","); student.gpa = atof(token);
            token = strtok(NULL, ","); student.totalClasses = atoi(token);
            token = strtok(NULL, ","); student.attendedClasses = atoi(token);
            token = strtok(NULL, ","); student.attendancePercentage = atof(token);

            printf("\nStudent found!\n");
            printf("Roll No: %d\n", student.rollNo);
            printf("PRN: %s\n", student.prn);
            printf("Name: %s\n", student.name);
            printf("Age: %d\n", student.age);
            printf("Phone: %s\n", student.phone);
            printf("Email: %s\n", student.email);
            printf("GPA: %.2f\n", student.gpa);
            printf("Attendance: %.2f%%\n", student.attendancePercentage);
            printf("Classes Attended: %d out of %d\n", student.attendedClasses, student.totalClasses);

            printf("\nCourses:\n");
            int courseCount = 0;
            for (int i = 0; i < MAX_COURSES; i++) {
                token = strtok(NULL, ",");
                if (token && strlen(token) > 0) {
                    printf("- %s\n", token);
                    courseCount++;
                }
            }

            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student with PRN %s not found.\n", searchPRN);
    }

    fclose(file);
}

void viewAttendanceReport(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("\n=================== ATTENDANCE REPORT ===================\n\n");
    printf("%-8s | %-20s | %-10s | %-15s | %-15s | %-10s | %-10s\n",
           "Roll No", "Name", "PRN", "Total Classes", "Attended", "Percentage", "Status");
    printf("------------------------------------------------------------------------------------\n");

    char line[MAX_LINE];
    float totalAttendancePercentage = 0;
    int studentCount = 0;
    int criticalAttendanceCount = 0;  // Students below 75%

    while (fgets(line, MAX_LINE, file)) {
        struct Student student;
        char* token = strtok(line, ",");

        // Parse student data
        student.rollNo = atoi(token);
        token = strtok(NULL, ","); strcpy(student.prn, token);
        token = strtok(NULL, ","); strcpy(student.name, token);

        // Skip age, phone, email, gpa
        for (int i = 0; i < 4; i++) {
            token = strtok(NULL, ",");
        }

        // Get attendance data
        student.totalClasses = atoi(strtok(NULL, ","));
        student.attendedClasses = atoi(strtok(NULL, ","));
        student.attendancePercentage = atof(strtok(NULL, ","));

        // Print individual student attendance
        printf("%-8d | %-20s | %-10s | %-15d | %-15d | %6.2f%% | ",
               student.rollNo, student.name, student.prn,
               student.totalClasses, student.attendedClasses,
               student.attendancePercentage);

        // Status indicator with simple symbols
        if (student.attendancePercentage >= 75.0) {
            printf("GOOD\n");
        } else {
            printf("LOW\n");
            criticalAttendanceCount++;
        }

        totalAttendancePercentage += student.attendancePercentage;
        studentCount++;
    }

    printf("------------------------------------------------------------------------------------\n");

    // Print summary statistics
    if (studentCount > 0) {
        float averageAttendance = totalAttendancePercentage / studentCount;
        printf("\nSUMMARY:\n");
        printf("Total Students: %d\n", studentCount);
        printf("Class Average Attendance: %.2f%%\n", averageAttendance);
        printf("Students below 75%%: %d\n", criticalAttendanceCount);

        // Overall class status
        printf("Class Status: ");
        if (averageAttendance >= 75.0) {
            printf("Good");
        } else {
            printf("Needs Improvement");
        }
        printf(" (%.2f%%)\n", averageAttendance);
    } else {
        printf("\nNo student records found.\n");
    }

    printf("\n====================================================\n");

    fclose(file);
}
void displayStudents(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char line[MAX_LINE];
    printf("\n%-8s | %-10s | %-20s | %-4s | %-12s | %-25s | %-5s | %-10s\n",
           "Roll No", "PRN", "Name", "Age", "Phone", "Email", "GPA", "Attendance");
    printf("------------------------------------------------------------------------------\n");

    while (fgets(line, MAX_LINE, file)) {
        struct Student student;
        char* token = strtok(line, ",");

        // Parse basic info
        student.rollNo = atoi(token);
        token = strtok(NULL, ","); strcpy(student.prn, token);
        token = strtok(NULL, ","); strcpy(student.name, token);
        token = strtok(NULL, ","); student.age = atoi(token);
        token = strtok(NULL, ","); strcpy(student.phone, token);
        token = strtok(NULL, ","); strcpy(student.email, token);
        token = strtok(NULL, ","); student.gpa = atof(token);
        token = strtok(NULL, ","); student.totalClasses = atoi(token);
        token = strtok(NULL, ","); student.attendedClasses = atoi(token);
        token = strtok(NULL, ","); student.attendancePercentage = atof(token);

        printf("%-8d | %-10s | %-20s | %-4d | %-12s | %-25s | %-5.2f | %.2f%%\n",
               student.rollNo, student.prn, student.name, student.age,
               student.phone, student.email, student.gpa, student.attendancePercentage);
    }

    fclose(file);
}

void deleteStudent(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char searchPRN[20];
    printf("Enter PRN of student to delete: ");
    getchar();  // Clear input buffer
    fgets(searchPRN, 20, stdin);
    searchPRN[strcspn(searchPRN, "\n")] = 0;

    FILE* temp = fopen(TEMP_FILE, "w");
    if (temp == NULL) {
        printf("Error creating temporary file!\n");
        fclose(file);
        return;
    }

    char line[MAX_LINE];
    int found = 0;

    while (fgets(line, MAX_LINE, file)) {
        char lineCopy[MAX_LINE];
        strcpy(lineCopy, line);
        struct Student student;
        char* token = strtok(line, ",");

        student.rollNo = atoi(token);
        token = strtok(NULL, ",");
        strcpy(student.prn, token);

        if (strcmp(student.prn, searchPRN) == 0) {
            found = 1;
            token = strtok(NULL, ",");
            strcpy(student.name, token);
            printf("\nStudent found:\n");
            printf("PRN: %s\n", student.prn);
            printf("Name: %s\n", student.name);

            char confirm;
            printf("\nAre you sure you want to delete this student? (y/n): ");
            scanf(" %c", &confirm);

            if (tolower(confirm) != 'y') {
                fprintf(temp, "%s", lineCopy);
                printf("Deletion cancelled.\n");
            } else {
                printf("Student deleted successfully!\n");
            }
        } else {
            fprintf(temp, "%s", lineCopy);
        }
    }

    if (!found) {
        printf("Student with PRN %s not found.\n", searchPRN);
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename(TEMP_FILE, filename);
}

int main() {
    const char* filename = "students.csv";
    int choice;

    // Create file if it doesn't exist
    FILE* file = fopen(filename, "a");
    if (file != NULL) {
        fclose(file);
    }

    // Require authentication before proceeding
    if (!authenticateUser()) {
        printf("Authentication failed. Exiting...\n");
        return 1;
    }

    while (1) {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addStudent(filename);
                break;
            case 2:
                displayStudents(filename);
                break;
            case 3:
                searchStudent(filename);
                break;
            case 4:
                markAttendance(filename);
                break;
            case 5:
                viewAttendanceReport(filename);
                break;
            case 6:
                searchAndModifyStudent(filename);
                break;
            case 7:
                deleteStudent(filename);
                break;
            case 8:
                createBackup(filename);
                break;
            case 9:
                searchAndModifyAttendance(filename);
                break;
            case 10:
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}
