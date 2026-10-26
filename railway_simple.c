// Railway Reservation System - SDF Lab-1 mini project
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Stop {            // one station on the route of a train
    char name[20];
    char time[10];       // a time with +1 or +2 means the next day or the day after
    int km;              // distance from the first station
};

struct Passenger {
    char name[30];
    int age;
};

struct ClassType {       // Sleeper, 3AC, 2AC
    char name[10];
    float mult;          // fare multiplier of the class
};

struct Food {
    char name[20];
    float price;
};

struct Train {
    int no;
    char name[30];
    char source[20];
    char dest[20];
    char dep[8];
    char arr[8];
    char distance[12];
    char duration[12];
    float fare;
    int cap[3];          // seats in Sleeper, 3AC, 2AC
    int nStops;          // number of stations on the route
    struct Stop stops[8];
};

struct Ticket {
    int no;
    char owner[20];
    int train;           // index of the train in trains[]
    int cls;             // 0 = Sleeper, 1 = 3AC, 2 = 2AC
    int qty;
    struct Passenger pax[6];
    int seats[6];
    char date[12];
    int status;          // 1 = confirmed, 2 = waitlist, 3 = cancelled
    float fare;
    float food;
    int notified;        // 0 = user not told yet that waitlist got confirmed
    int from;            // index of boarding station in stops[] of the train
    int to;              // index of destination station
};

struct Train trains[6] = {
    {12001, "Northern Express", "New Delhi", "Lucknow", "06:00", "12:30", "512 km", "6h 30m", 500, {60, 40, 20}, 6,
        {{"New Delhi", "06:00", 0}, {"Ghaziabad", "06:35", 25}, {"Moradabad", "08:40", 167}, {"Bareilly", "09:50", 250}, {"Shahjahanpur", "10:35", 320}, {"Lucknow", "12:30", 512}}},
    {12002, "Deccan Express", "Mumbai", "Pune", "07:10", "10:25", "192 km", "3h 15m", 450, {50, 30, 20}, 5,
        {{"Mumbai", "07:10", 0}, {"Thane", "07:40", 25}, {"Kalyan", "08:00", 54}, {"Lonavala", "09:20", 128}, {"Pune", "10:25", 192}}},
    {12301, "Eastern Express", "New Delhi", "Kolkata", "16:55", "10:00", "1451 km", "17h 05m", 900, {80, 50, 30}, 7,
        {{"New Delhi", "16:55", 0}, {"Kanpur", "22:15", 440}, {"Prayagraj", "00:45 +1", 634}, {"Gaya", "05:00 +1", 990}, {"Dhanbad", "07:30 +1", 1240}, {"Asansol", "08:15 +1", 1290}, {"Kolkata", "10:00 +1", 1451}}},
    {12401, "Desert Express", "New Delhi", "Jaipur", "06:15", "10:45", "303 km", "4h 30m", 400, {60, 40, 20}, 6,
        {{"New Delhi", "06:15", 0}, {"Gurgaon", "06:55", 28}, {"Rewari", "07:35", 82}, {"Alwar", "08:35", 160}, {"Bandikui", "09:40", 235}, {"Jaipur", "10:45", 303}}},
    {12951, "Western Rajdhani", "Mumbai", "New Delhi", "17:00", "08:35", "1384 km", "15h 35m", 1200, {0, 50, 30}, 6,
        {{"Mumbai", "17:00", 0}, {"Surat", "20:25", 263}, {"Vadodara", "21:55", 392}, {"Ratlam", "00:55 +1", 654}, {"Kota", "03:20 +1", 920}, {"New Delhi", "08:35 +1", 1384}}},
    {12627, "Southern Express", "New Delhi", "Chennai", "21:05", "05:45", "2180 km", "32h 40m", 1500, {80, 50, 30}, 7,
        {{"New Delhi", "21:05", 0}, {"Agra", "23:50", 195}, {"Jhansi", "03:20 +1", 415}, {"Bhopal", "07:10 +1", 701}, {"Nagpur", "14:30 +1", 1090}, {"Vijayawada", "00:30 +2", 1735}, {"Chennai", "05:45 +2", 2180}}}
};

struct ClassType classes[3] = {{"Sleeper", 1.0}, {"3AC", 1.8}, {"2AC", 2.5}};

struct Food foods[8] = {{"Veg Thali", 120}, {"Non-Veg Thali", 180}, {"Veg Biryani", 150}, {"Sandwich", 60},
                        {"Snack Box", 50}, {"Tea", 20}, {"Coffee", 30}, {"Water Bottle", 20}};

struct Ticket tk[2000];
int count = 0;                       // number of tickets made so far

int isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isDigit(char c) {
    return c >= '0' && c <= '9';
}

int isBlank(char c) {        // space, tab or similar
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

// reads one whole line safely. Extra characters are thrown away so they
// do not go into the next question
void getLine(char s[], int size) {
    int len, ch;
    if (fgets(s, size, stdin) == NULL) exit(0);
    len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF);
    }
}

// read a number between min and max (keeps asking if wrong)
int getNum(int min, int max) {
    char line[50];
    int n, i, digits;
    while (1) {
        getLine(line, 50);
        i = 0;
        while (line[i] == ' ' || line[i] == '\t') i++;      // spaces before the number
        if (line[i] == '+') i++;
        n = 0;
        digits = 0;
        while (isDigit(line[i])) {
            if (n <= 100000000) n = n * 10 + (line[i] - '0');   // keeps a very long number from overflowing
            digits++;
            i++;
        }
        while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r') i++;   // spaces after the number
        if (digits > 0 && line[i] == '\0' && n >= min && n <= max) return n;
        printf("Enter a number between %d and %d : ", min, max);
    }
}

// ask a y/n question, returns 1 for yes
int askYes(char msg[]) {
    char line[10];
    printf("%s", msg);
    getLine(line, 10);
    while (line[0] != 'y' && line[0] != 'Y' && line[0] != 'n' && line[0] != 'N') {
        printf("Please enter y or n : ");
        getLine(line, 10);
    }
    return (line[0] == 'y' || line[0] == 'Y');
}

// reads a name (letters, spaces and dots only)
void getName(char msg[], char name[]) {
    int ok;
    while (1) {
        printf("%s", msg);
        getLine(name, 30);
        ok = (name[0] != '\0');
        for (int i = 0; name[i] != '\0'; i++) {
            if (!isLetter(name[i]) && name[i] != ' ' && name[i] != '.') ok = 0;
        }
        if (ok == 1) return;
        printf("Invalid name. Use letters, spaces and dots only.\n");
    }
}

// reads the travel date as dd-mm-yyyy and checks it
void getDate(char date[]) {
    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    char line[40];
    int d = 0, m = 0, y = 0, maxDay, ok, i;
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);
    int today = (lt->tm_year + 1900) * 10000 + (lt->tm_mon + 1) * 100 + lt->tm_mday;
    while (1) {
        printf("Enter travel date (dd-mm-yyyy) : ");
        getLine(line, 40);
        ok = (strlen(line) == 10 && line[2] == '-' && line[5] == '-');
        for (i = 0; i < 10 && ok == 1; i++) {
            if (i != 2 && i != 5 && !isDigit(line[i])) ok = 0;   // every other character must be a digit
        }
        if (ok == 1) {
            d = (line[0] - '0') * 10 + (line[1] - '0');
            m = (line[3] - '0') * 10 + (line[4] - '0');
            y = (line[6] - '0') * 1000 + (line[7] - '0') * 100 + (line[8] - '0') * 10 + (line[9] - '0');
        }
        if (ok == 0 || m < 1 || m > 12 || y < 2000 || y > 2100) {
            printf("Invalid format. Example: 25-12-2026\n");
            continue;
        }
        maxDay = days[m - 1];
        if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) maxDay = 29;   // leap year
        if (d < 1 || d > maxDay) {
            printf("That day does not exist in the given month.\n");
            continue;
        }
        if (y * 10000 + m * 100 + d < today) {
            printf("The date is in the past. Enter a future date.\n");
            continue;
        }
        strcpy(date, line);
        return;
    }
}

// makes a lowercase copy of a string (used for searching)
void lower(char from[], char to[]) {
    int i;
    for (i = 0; from[i] != '\0'; i++) {
        if (from[i] >= 'A' && from[i] <= 'Z') to[i] = from[i] + 32;
        else to[i] = from[i];
    }
    to[i] = '\0';
}

void saveData() {
    int header[2];
    FILE *fp = fopen("bookings.dat", "wb");
    if (fp == NULL) {
        printf("Could not save bookings.\n");
        return;
    }
    header[0] = count;
    header[1] = sizeof(struct Ticket);     // used to check the file when loading
    fwrite(header, sizeof(int), 2, fp);
    fwrite(tk, sizeof(struct Ticket), count, fp);
    fclose(fp);
}

// load old tickets from the file
void loadData() {
    int header[2];
    FILE *fp = fopen("bookings.dat", "rb");
    if (fp == NULL) return;
    if (fread(header, sizeof(int), 2, fp) != 2 || header[1] != (int)sizeof(struct Ticket) ||
        header[0] < 0 || header[0] > 2000 ||
        (int)fread(tk, sizeof(struct Ticket), header[0], fp) != header[0]) {
        printf("Warning: bookings.dat is not readable. Starting with no bookings.\n");
        fclose(fp);
        return;
    }
    fclose(fp);
    count = header[0];
}

// Fills row[] for one train + class + date.
// A seat is full (row[i] = ticket number) if a confirmed ticket has it on that date.
void buildRow(int t, int c, char date[], int row[]) {
    for (int i = 0; i < 700; i++) row[i] = 0;
    for (int i = 0; i < count; i++) {
        if (tk[i].status == 1 && tk[i].train == t && tk[i].cls == c && strcmp(tk[i].date, date) == 0) {
            for (int k = 0; k < tk[i].qty; k++) row[tk[i].seats[k] - 1] = tk[i].no;
        }
    }
}

int freeSeats(int t, int c, char date[]) {
    int row[700], n = 0;
    buildRow(t, c, date, row);
    for (int i = 0; i < trains[t].cap[c]; i++) {
        if (row[i] == 0) n++;
    }
    return n;
}

// total seats that waiting passengers have asked for (on that date)
int waitingSeats(int t, int c, char date[]) {
    int n = 0;
    for (int i = 0; i < count; i++) {
        if (tk[i].status == 2 && tk[i].train == t && tk[i].cls == c && strcmp(tk[i].date, date) == 0) n += tk[i].qty;
    }
    return n;
}

// position of ticket i in the waitlist (same train, class and date)
int waitPos(int i) {
    int pos = 1;
    for (int j = 0; j < i; j++) {
        if (tk[j].status == 2 && tk[j].train == tk[i].train && tk[j].cls == tk[i].cls &&
            strcmp(tk[j].date, tk[i].date) == 0) pos++;
    }
    return pos;
}

// fare of one seat from stop 'from' to stop 'to' (full route = full fare)
float segFare(int t, int c, int from, int to) {
    float km = trains[t].stops[to].km - trains[t].stops[from].km;
    float total = trains[t].stops[trains[t].nStops - 1].km;
    return trains[t].fare * classes[c].mult * km / total;
}

// returns the position of a station in the route of train t (-1 if not on the route)
// the word must already be in lowercase
int stopIndex(int t, char word[]) {
    char name[20];
    for (int i = 0; i < trains[t].nStops; i++) {
        lower(trains[t].stops[i].name, name);
        if (strstr(name, word) != NULL) return i;
    }
    return -1;
}

// ---------------- login and register ----------------

// simple hash so the password is not saved as plain text
int hashPass(char s[]) {
    int h = 5381;
    for (int i = 0; s[i] != '\0'; i++) {
        h = (h * 33 + s[i]) % 1000003;     // % keeps the number small
    }
    return h;
}

// returns 1 if the username is already in users.txt
int userExists(char name[]) {
    char u[20];
    int h;
    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL) return 0;
    while (fscanf(fp, "%19s %d", u, &h) == 2) {
        if (strcmp(u, name) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void registerUser() {
    char user[40], pass[40], pass2[40];
    int ok;
    printf("\n---------- REGISTER ----------\n");
    while (1) {
        printf("Choose a username (3-19 letters, digits or _) : ");
        getLine(user, 40);
        ok = (strlen(user) >= 3 && strlen(user) <= 19);
        for (int i = 0; user[i] != '\0'; i++) {
            if (!isLetter(user[i]) && !isDigit(user[i]) && user[i] != '_') ok = 0;
        }
        if (ok == 0) printf("Invalid username.\n");
        else if (userExists(user) == 1) printf("This username is already taken.\n");
        else break;
    }
    while (1) {
        printf("Choose a password (4-19 characters, no spaces) : ");
        getLine(pass, 40);
        ok = (strlen(pass) >= 4 && strlen(pass) <= 19);
        for (int i = 0; pass[i] != '\0'; i++) {
            if (isBlank(pass[i])) ok = 0;
        }
        if (ok == 0) {
            printf("Invalid password.\n");
            continue;
        }
        printf("Confirm password : ");
        getLine(pass2, 40);
        if (strcmp(pass, pass2) != 0) {
            printf("Passwords do not match.\n");
            continue;
        }
        break;
    }
    FILE *fp = fopen("users.txt", "a");
    if (fp == NULL) {
        printf("Could not open users.txt for writing.\n");
        return;
    }
    fprintf(fp, "%s %d\n", user, hashPass(pass));
    fclose(fp);
    printf("Account created. Please login.\n");
}

// returns 1 if login is successful
int loginUser(char user[]) {
    char name[40], pass[40], u[20];
    int h;
    printf("\n---------- LOGIN ----------\n");
    for (int attempt = 1; attempt <= 3; attempt++) {
        printf("Username : ");
        getLine(name, 40);
        printf("Password : ");
        getLine(pass, 40);
        FILE *fp = fopen("users.txt", "r");
        if (fp != NULL) {
            while (fscanf(fp, "%19s %d", u, &h) == 2) {
                if (strcmp(u, name) == 0 && h == hashPass(pass)) {
                    fclose(fp);
                    strcpy(user, name);
                    printf("\nLogin successful. Welcome, %s!\n", user);
                    return 1;
                }
            }
            fclose(fp);
        }
        printf("Wrong username or password (attempt %d of 3).\n", attempt);
    }
    printf("Too many wrong attempts.\n");
    return 0;
}

// ---------------- trains ----------------

void viewTrains() {
    printf("\n%-7s %-18s %-11s %-11s %-6s %-6s\n", "Code", "Train Name", "From", "To", "Dep", "Arr");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < 6; i++) {
        printf("%-7d %-18s %-11s %-11s %-6s %-6s\n", trains[i].no, trains[i].name,
               trains[i].source, trains[i].dest, trains[i].dep, trains[i].arr);
    }
}

void printTrain(struct Train *t, int index) {
    printf("\n------------------------------------------------------------\n");
    printf(" Train Code  : %d\n", t->no);
    printf(" Train Name  : %s\n", t->name);
    printf(" Route       : %s  ->  %s\n", t->source, t->dest);
    printf(" Departure   : %s     Arrival : %s\n", t->dep, t->arr);
    printf(" Distance    : %s     Duration: %s\n", t->distance, t->duration);
    printf(" Base Fare   : Rs. %.2f\n", t->fare);
    printf(" Classes     :\n");
    for (int c = 0; c < 3; c++) {
        if (t->cap[c] == 0) {
            printf("   %-8s : not available on this train\n", classes[c].name);
        } else {
            printf("   %-8s : %3d seats | Rs. %.2f per seat (full route)\n", classes[c].name, t->cap[c], t->fare * classes[c].mult);
        }
    }
    printf("\n Complete route (%d stations):\n", trains[index].nStops);
    printf("   %-4s %-15s %-10s %s\n", "No", "Station", "Time", "Km from start");
    for (int s = 0; s < trains[index].nStops; s++) {
        printf("   %-4d %-15s %-10s %d\n", s + 1, trains[index].stops[s].name, trains[index].stops[s].time, trains[index].stops[s].km);
    }
    printf("\n For free seats on a date use 'Check seat availability' in the menu.\n");
    printf("------------------------------------------------------------\n");
}

// search by train number or by city name
void searchTrain() {
    char word[40], lw[40], a[30], b[30];
    int found = 0, allDigits = 1, num = 0;
    printf("Enter train number or a city name : ");
    getLine(word, 40);
    if (word[0] == '\0') {
        printf("Nothing entered.\n");
        return;
    }
    for (int i = 0; word[i] != '\0'; i++) {
        if (!isDigit(word[i])) allDigits = 0;
        else if (num <= 100000000) num = num * 10 + (word[i] - '0');
    }
    lower(word, lw);
    for (int i = 0; i < 6; i++) {
        lower(trains[i].source, a);
        lower(trains[i].dest, b);
        if ((allDigits == 1 && trains[i].no == num) ||
            (allDigits == 0 && (strstr(a, lw) != NULL || strstr(b, lw) != NULL))) {
            printTrain(&trains[i], i);
            found = 1;
        }
    }
    if (found == 0) printf("No train found. Use 'View all trains' to see valid codes.\n");
}

// shows the trains and asks for a train code, returns index (or -1 to go back)
int pickTrain() {
    viewTrains();
    printf("\nEnter train code (0 to go back) : ");
    int code = getNum(0, 99999);
    while (code != 0) {
        for (int i = 0; i < 6; i++) {
            if (trains[i].no == code) return i;
        }
        printf("Train not found. Enter a valid train code (0 to go back) : ");
        code = getNum(0, 99999);
    }
    return -1;
}

// shows classes of train t (fare from stop 'from' to stop 'to', seats for the given date)
// and asks for one, returns 0..2 (or -1 to go back)
int pickClass(int t, char msg[], char date[], int from, int to) {
    printf("\nClasses on %s for %s:\n", trains[t].name, date);
    for (int c = 0; c < 3; c++) {
        if (trains[t].cap[c] == 0) {
            printf("  %d. %-8s - not available\n", c + 1, classes[c].name);
        } else {
            printf("  %d. %-8s Rs. %8.2f/seat | Free: %3d | Waitlisted: %d\n", c + 1, classes[c].name,
                   segFare(t, c, from, to), freeSeats(t, c, date), waitingSeats(t, c, date));
        }
    }
    printf("%s", msg);
    int choice = getNum(0, 3);
    while (choice != 0 && trains[t].cap[choice - 1] == 0) {
        printf("That class is not available. Choose again : ");
        choice = getNum(0, 3);
    }
    return choice - 1;
}

// ---------------- seat matrix ----------------

// row points to the seats of one class of one train
void showMatrix(int *row, int cap, char trainName[], char cls[], char date[]) {
    int empty = 0;
    printf("\n=========== SEAT MATRIX : %s - %s - %s ===========\n", trainName, cls, date);
    printf("  [E] = Empty seat     [F] = Full (booked) seat\n");
    printf("  Seat number = first number of the row + (column - 1)\n\n");
    printf("  Column -> ");
    for (int col = 1; col <= 10; col++) printf("%-4d", col);
    for (int i = 0; i < cap; i++) {
        if (i % 10 == 0) {
            int last = i + 10;
            if (last > cap) last = cap;
            printf("\n  %03d-%03d :", i + 1, last);
        }
        if (row[i] == 0) {
            printf(" [E]");
            empty++;
        } else {
            printf(" [F]");
        }
    }
    printf("\n\n  Total: %d | Empty: %d | Full: %d\n", cap, empty, cap - empty);
}

void checkAvailability() {
    char date[12];
    int row[700];
    int t = pickTrain();
    if (t == -1) return;
    printf("\n");
    getDate(date);
    int c = pickClass(t, "View seat matrix of class (0 to go back) : ", date, 0, trains[t].nStops - 1);
    while (c != -1) {
        buildRow(t, c, date, row);
        showMatrix(row, trains[t].cap[c], trains[t].name, classes[c].name, date);
        c = pickClass(t, "View seat matrix of class (0 to go back) : ", date, 0, trains[t].nStops - 1);
    }
}

// ---------------- tickets ----------------

void printTicket(int i) {
    struct Ticket *p = &tk[i];
    printf("\n============================================================\n");
    printf("                      RAILWAY TICKET\n");
    printf("============================================================\n");
    printf(" Ticket Number : %d\n", p->no);
    if (p->status == 1) printf(" Status        : CONFIRMED\n");
    else if (p->status == 2) printf(" Status        : WAITLISTED  (WL/%d)\n", waitPos(i));
    else printf(" Status        : CANCELLED\n");
    printf("------------------------------------------------------------\n");
    printf(" Train         : %d - %s\n", trains[p->train].no, trains[p->train].name);
    printf(" From          : %s\n", trains[p->train].stops[p->from].name);
    printf(" To            : %s\n", trains[p->train].stops[p->to].name);
    printf(" Date          : %s\n", p->date);
    printf(" Departure     : %s     Arrival : %s\n", trains[p->train].stops[p->from].time, trains[p->train].stops[p->to].time);
    printf(" Class         : %s\n", classes[p->cls].name);
    printf("------------------------------------------------------------\n");
    printf(" Passengers    :\n");
    for (int k = 0; k < p->qty; k++) {
        if (p->seats[k] > 0) printf("   %d. %-22s Age %-3d Seat %d\n", k + 1, p->pax[k].name, p->pax[k].age, p->seats[k]);
        else printf("   %d. %-22s Age %-3d Seat: to be allotted\n", k + 1, p->pax[k].name, p->pax[k].age);
    }
    printf("------------------------------------------------------------\n");
    printf(" Seat Fare     : Rs. %.2f\n", p->fare);
    printf(" Food Charges  : Rs. %.2f\n", p->food);
    printf(" TOTAL FARE    : Rs. %.2f\n", p->fare + p->food);
    printf("============================================================\n");
    if (p->status == 1) printf("                   HAVE A SAFE JOURNEY!\n");
    else if (p->status == 2) printf("   Seats will be allotted automatically on cancellation.\n");
    else printf("                  THIS TICKET IS CANCELLED\n");
    printf("============================================================\n");
}

// prints seat numbers of ticket i like 5, 6, 7
void printSeats(int i) {
    for (int k = 0; k < tk[i].qty; k++) {
        if (k > 0) printf(", ");
        printf("%d", tk[i].seats[k]);
    }
}

float orderFood() {
    float total = 0;
    if (askYes("\nWould you like to order food? (y/n) : ") == 0) return 0;
    printf("\n------------------- FOOD MENU -------------------\n");
    for (int i = 0; i < 8; i++) {
        printf("  %d. %-16s Rs. %6.2f\n", i + 1, foods[i].name, foods[i].price);
    }
    printf("-------------------------------------------------\n");
    printf("Select item (0 = done) : ");
    int item = getNum(0, 8);
    while (item != 0) {
        printf("Quantity (1-10) : ");
        int q = getNum(1, 10);
        total = total + foods[item - 1].price * q;
        printf("Added %d x %s. Food total so far: Rs. %.2f\n", q, foods[item - 1].name, total);
        printf("Select item (0 = done) : ");
        item = getNum(0, 8);
    }
    return total;
}

// Books a ticket. If t is -1 the user picks the train and the whole route is booked.
// Otherwise train, stations and date come from the station search.
void bookTicket(char user[], int t, int from, int to, char date[]) {
    struct Ticket n = {0};    // the new ticket (everything starts as 0), it is added to tk[] after confirmation
    int row[700];             // seats of this train, class and date
    if (count >= 2000) {
        printf("Booking database is full.\n");
        return;
    }
    printf("\n------------------------ TICKET COUNTER ------------------------\n");
    if (t == -1) {
        t = pickTrain();
        if (t == -1) return;
        from = 0;
        to = trains[t].nStops - 1;
        printf("\n");
        getDate(n.date);
    } else {
        strcpy(n.date, date);
        printf("%s: %s to %s on %s\n", trains[t].name, trains[t].stops[from].name, trains[t].stops[to].name, n.date);
    }
    int c = pickClass(t, "Choose class (0 to go back) : ", n.date, from, to);
    if (c == -1) return;

    printf("Number of seats to book (1-6) : ");
    int qty = getNum(1, 6);
    int cap = trains[t].cap[c];
    buildRow(t, c, n.date, row);
    int free = 0;
    for (int i = 0; i < cap; i++) {
        if (row[i] == 0) free++;
    }

    strcpy(n.owner, user);
    n.train = t;
    n.cls = c;
    n.qty = qty;
    n.notified = 1;
    n.from = from;
    n.to = to;

    if (free >= qty) {
        // enough seats, so the user chooses seats from the matrix
        n.status = 1;
        showMatrix(row, cap, trains[t].name, classes[c].name, n.date);
        for (int i = 0; i < qty; i++) {
            printf("\n--- Passenger %d of %d ---\n", i + 1, qty);
            getName("Passenger name : ", n.pax[i].name);
            printf("Passenger age : ");
            n.pax[i].age = getNum(1, 120);
            printf("Choose seat number from the matrix : ");
            int s = getNum(1, cap);
            int ok = 0;
            while (ok == 0) {
                ok = 1;
                if (row[s - 1] != 0) {
                    printf("Seat %d is [F] already booked. Choose an [E] seat : ", s);
                    ok = 0;
                } else {
                    for (int k = 0; k < i; k++) {
                        if (n.seats[k] == s) {
                            printf("You already selected seat %d. Choose another seat : ", s);
                            ok = 0;
                        }
                    }
                }
                if (ok == 0) s = getNum(1, cap);
            }
            n.seats[i] = s;
            printf("Seat %d selected.\n", s);
        }
    } else {
        // not enough seats, so offer the waitlist
        if (free == 0) printf("\nAll %s seats on this train are FULL for %s.\n", classes[c].name, n.date);
        else printf("\nOnly %d %s seat(s) are free but you asked for %d.\n", free, classes[c].name, qty);
        printf("You can join the waitlist. If seats of this class are cancelled,\n");
        printf("your booking is confirmed automatically.\n");
        if (askYes("Join the waitlist? (y/n) : ") == 0) {
            printf("Booking cancelled.\n");
            return;
        }
        n.status = 2;
        for (int i = 0; i < qty; i++) {
            printf("\n--- Passenger %d of %d ---\n", i + 1, qty);
            getName("Passenger name : ", n.pax[i].name);
            printf("Passenger age : ");
            n.pax[i].age = getNum(1, 120);
        }
    }

    n.food = orderFood();
    n.fare = segFare(t, c, from, to) * qty;

    printf("\n-------------------- BOOKING SUMMARY --------------------\n");
    printf(" Train      : %d - %s\n", trains[t].no, trains[t].name);
    printf(" Journey    : %s -> %s (%d km)\n", trains[t].stops[from].name, trains[t].stops[to].name, trains[t].stops[to].km - trains[t].stops[from].km);
    printf(" Class      : %s x %d  (%s)\n", classes[c].name, qty, n.status == 1 ? "confirmed seats" : "WAITLIST");
    if (n.status == 1) {
        printf(" Seats      : ");
        for (int k = 0; k < qty; k++) {
            if (k > 0) printf(", ");
            printf("%d", n.seats[k]);
        }
        printf("\n");
    }
    printf(" Date       : %s\n", n.date);
    printf(" Seat fare  : Rs. %.2f\n", n.fare);
    printf(" Food       : Rs. %.2f\n", n.food);
    printf(" TOTAL      : Rs. %.2f\n", n.fare + n.food);
    printf("---------------------------------------------------------\n");
    if (askYes("Confirm booking? (y/n) : ") == 0) {
        printf("Booking discarded. No seats were reserved.\n");
        return;
    }

    // make a 6 digit ticket number. 7919 and 900000 have no common factor,
    // so two tickets never get the same number (up to 900000 tickets)
    n.no = (count * 7919 + 123457) % 900000 + 100000;
    tk[count] = n;      // a confirmed ticket automatically makes its seats [F] for that date
    count++;
    saveData();

    if (n.status == 2) printf("\nYou are on the WAITLIST at position WL/%d.\n", waitPos(count - 1));
    else printf("\nBooking confirmed!\n");
    printTicket(count - 1);
}

// search trains between two stations on a date, show free seats, then book if the user wants
void searchBetween(char user[]) {
    char from[30], to[30], lf[30], lt[30], date[12];
    int match[6], fi[6], ti[6], n = 0;
    printf("\n---------- SEARCH TRAINS BETWEEN STATIONS ----------\n");
    printf("From station : ");
    getLine(from, 30);
    printf("To station : ");
    getLine(to, 30);
    if (from[0] == '\0' || to[0] == '\0') {
        printf("Please enter both stations.\n");
        return;
    }
    lower(from, lf);
    lower(to, lt);
    getDate(date);

    for (int i = 0; i < 6; i++) {
        int f = stopIndex(i, lf);
        int g = stopIndex(i, lt);
        if (f != -1 && g != -1 && f < g) {      // the train must reach 'from' before 'to'
            match[n] = i;
            fi[n] = f;
            ti[n] = g;
            n++;
        }
    }
    if (n == 0) {
        printf("No train runs from %s to %s.\n", from, to);
        return;
    }

    printf("\n%d train(s) found from %s to %s on %s:\n", n, from, to, date);
    for (int j = 0; j < n; j++) {
        int t = match[j];
        printf("\n%d - %s\n", trains[t].no, trains[t].name);
        printf("   %s %s  ->  %s %s   (%d km)\n", trains[t].stops[fi[j]].name, trains[t].stops[fi[j]].time,
               trains[t].stops[ti[j]].name, trains[t].stops[ti[j]].time, trains[t].stops[ti[j]].km - trains[t].stops[fi[j]].km);
        for (int c = 0; c < 3; c++) {
            if (trains[t].cap[c] == 0) continue;
            int free = freeSeats(t, c, date);
            printf("   %-8s Rs. %8.2f/seat : ", classes[c].name, segFare(t, c, fi[j], ti[j]));
            if (free > 0) printf("%d seats free\n", free);
            else printf("FULL (waitlist: %d seats)\n", waitingSeats(t, c, date));
        }
    }

    printf("\nEnter train code to book (0 to go back) : ");
    int code = getNum(0, 99999);
    while (code != 0) {
        for (int j = 0; j < n; j++) {
            if (trains[match[j]].no == code) {
                bookTicket(user, match[j], fi[j], ti[j], date);
                return;
            }
        }
        printf("Enter a train code from the list above (0 to go back) : ");
        code = getNum(0, 99999);
    }
}

void viewTicket(char user[]) {
    printf("Enter ticket number (0 to go back) : ");
    int no = getNum(0, 999999);
    if (no == 0) return;
    for (int i = 0; i < count; i++) {
        if (tk[i].no == no && strcmp(tk[i].owner, user) == 0) {
            printTicket(i);
            return;
        }
    }
    printf("Ticket not found for your account.\n");
}

// called after seats are freed. Waiting tickets of the same train, class and date
// (oldest first) get the free seats. Returns how many waiting tickets became confirmed
int promoteWaitlist(int t, int c, char date[]) {
    int promoted = 0, row[700], freeCount;
    for (int i = 0; i < count; i++) {
        if (tk[i].status == 2 && tk[i].train == t && tk[i].cls == c && strcmp(tk[i].date, date) == 0) {
            buildRow(t, c, date, row);
            freeCount = 0;
            for (int j = 0; j < trains[t].cap[c]; j++) {
                if (row[j] == 0) freeCount++;
            }
            if (freeCount >= tk[i].qty) {
                // give the lowest free seats to this ticket
                for (int k = 0; k < tk[i].qty; k++) {
                    int j = 0;
                    while (row[j] != 0) j++;
                    row[j] = tk[i].no;
                    tk[i].seats[k] = j + 1;
                }
                tk[i].status = 1;
                tk[i].notified = 0;
                promoted++;
                printf("  >> Waitlisted ticket %d (%s) is now CONFIRMED, seat(s): ", tk[i].no, tk[i].owner);
                printSeats(i);
                printf("\n");
            }
        }
    }
    return promoted;
}

void cancelTicket(char user[]) {
    printf("Enter ticket number to cancel (0 to go back) : ");
    int no = getNum(0, 999999);
    if (no == 0) return;
    int i = 0;
    while (i < count && !(tk[i].no == no && strcmp(tk[i].owner, user) == 0)) i++;
    if (i == count) {
        printf("Ticket not found for your account.\n");
        return;
    }
    if (tk[i].status == 3) {
        printf("This ticket is already cancelled.\n");
        return;
    }
    printTicket(i);
    if (askYes("Are you sure you want to cancel this ticket? (y/n) : ") == 0) {
        printf("Cancellation aborted.\n");
        return;
    }

    int wasConfirmed = (tk[i].status == 1);
    tk[i].status = 3;       // seats become [E] again because only confirmed tickets hold seats
    printf("\nTicket %d cancelled successfully.\n", no);

    if (wasConfirmed) {
        printf("Seat(s) released. Checking the waitlist...\n");
        if (promoteWaitlist(tk[i].train, tk[i].cls, tk[i].date) == 0)
            printf("  No waitlisted booking could use the released seat(s).\n");
    } else {
        promoteWaitlist(tk[i].train, tk[i].cls, tk[i].date);
    }
    saveData();
}

void bookingHistory(char user[]) {
    int n = 0;
    printf("\n==================== BOOKING HISTORY ====================\n");
    for (int i = 0; i < count; i++) {
        if (strcmp(tk[i].owner, user) != 0) continue;
        n++;
        printf(" Booking #%d\n", n);
        printf("   Ticket No : %d\n", tk[i].no);
        printf("   Train     : %d - %s (%s -> %s)\n", trains[tk[i].train].no, trains[tk[i].train].name,
               trains[tk[i].train].stops[tk[i].from].name, trains[tk[i].train].stops[tk[i].to].name);
        printf("   Date      : %s\n", tk[i].date);
        printf("   Class     : %s x %d\n", classes[tk[i].cls].name, tk[i].qty);
        if (tk[i].status == 1) {
            printf("   Seats     : ");
            printSeats(i);
            printf("\n   Status    : CONFIRMED\n");
        } else if (tk[i].status == 2) {
            printf("   Status    : WAITLISTED (WL/%d)\n", waitPos(i));
        } else {
            printf("   Status    : CANCELLED\n");
        }
        printf("   Total Fare: Rs. %.2f\n", tk[i].fare + tk[i].food);
        printf("---------------------------------------------------------\n");
    }
    if (n == 0) printf(" No bookings found for your account.\n");
}

// tells the user if a waitlisted ticket got confirmed while he was logged out
void showNotifications(char user[]) {
    int changed = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(tk[i].owner, user) == 0 && tk[i].status == 1 && tk[i].notified == 0) {
            if (changed == 0) printf("\n*************** NOTIFICATIONS ***************\n");
            printf(" Good news! Waitlisted ticket %d (%s) is now CONFIRMED.\n", tk[i].no, trains[tk[i].train].name);
            printf(" Your seat(s): ");
            printSeats(i);
            printf("\n");
            tk[i].notified = 1;
            changed = 1;
        }
    }
    if (changed == 1) {
        printf("*********************************************\n");
        saveData();
    }
}

void mainMenu(char user[]) {
    int choice = 0;
    while (choice != 9) {
        printf("\n===================== MAIN MENU =====================\n");
        printf(" Logged in as: %s\n", user);
        printf("  1. View all trains\n");
        printf("  2. Search train (number or city) with full route\n");
        printf("  3. Search trains between two stations (with seats)\n");
        printf("  4. Check seat availability (seat matrix)\n");
        printf("  5. Book ticket\n");
        printf("  6. View ticket\n");
        printf("  7. Cancel ticket\n");
        printf("  8. View booking history\n");
        printf("  9. Logout\n");
        printf("Enter your choice : ");
        choice = getNum(1, 9);
        if (choice == 1) viewTrains();
        else if (choice == 2) searchTrain();
        else if (choice == 3) searchBetween(user);
        else if (choice == 4) checkAvailability();
        else if (choice == 5) bookTicket(user, -1, 0, 0, "");
        else if (choice == 6) viewTicket(user);
        else if (choice == 7) cancelTicket(user);
        else if (choice == 8) bookingHistory(user);
        else printf("\nLogged out.\n");
    }
}

int main() {
    char user[20];
    int choice = 0;
    loadData();
    printf("========================================\n");
    printf("|      RAILWAY RESERVATION SYSTEM      |\n");
    printf("========================================\n");
    while (choice != 3) {
        printf("\n==================== WELCOME ====================\n");
        printf("  1. Login\n  2. Register\n  3. Exit\n");
        printf("Enter your choice : ");
        choice = getNum(1, 3);
        if (choice == 1) {
            if (loginUser(user) == 1) {
                showNotifications(user);
                mainMenu(user);
            }
        } else if (choice == 2) {
            registerUser();
        }
    }
    printf("\nThank you for using the Railway Reservation System.\n");
    return 0;
}
