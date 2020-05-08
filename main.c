#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <strings.h>

/**
 * function printPerson() 
 * print records from person.db
 * **/
void printPerson();

/**
 * function printAddress() 
 * print records from address.db
 * **/
void printAddress();

/**
 * function addPerson() 
 * add new record in table person.db
 * return string to print
 * **/
char* addPerson();

/**
 * function addAddress()
 * add new record in table address.db
 * return string to print
 * **/
char* addAddress();

/**
 * function checkString()
 * check if string consists only letters
 * return true/false
 * **/
bool checkString( const char s[] );

/**
 * struct Person
 * **/
struct Person {
    int ID;
    char firstName[30];
    char lastName[30];
    int age;
    char gender;
};

/**
 * struct Person
 * **/
struct Address {
    int ID;
    int personID;
    char city[20];
    char postalCode[10];
    char street[20];
    int nr;
};

static int personID = 0;
static int addressID = 0;

/**
 * function getPersonID
 * getter personID
 * return personID
 * **/
int getPersonID(){ return personID; }

/**
 * function menu
 * console inteface
 * **/
void menu();

/**
 * function fillPerson
 * filling struct Person
 * **/
void fillPerson(struct Person* p, const char* first, const char* sur, int age, char gender) {
    p->ID = personID;
    strncpy(p->firstName, first, sizeof(p->firstName));
    strncpy(p->lastName, sur, sizeof(p->lastName));
    p->age = age;
    p->gender = gender;
}

/**
 * function fillAddress
 * filling struct Address
 * **/
void fillAddress(struct Address* a, int persID, const char* cit, const char* postal, const char* stre, int nr) {
    a->ID = addressID;
    a->personID = persID;
    strncpy(a->city, cit, sizeof(a->city));
    strncpy(a->postalCode, postal, sizeof(a->postalCode));
    strncpy(a->street, stre, sizeof(a->street));
    a->nr = nr;
}

/**
 * Function setID 
 * read from files how many record are in table and set variable
 * **/
void setID();

/**
 * Function deletePerson
 * delete chosen record from table Person
 * **/
void deletePerson();

/**
 * Function deletePerson
 * delete chosen record from table Address
 * **/
void deleteAddress();

/**
 * Function replaceAddress
 * replace chosen record from table Address
 * return string to print
 * **/
char* replaceAddress();

/**
 * Function replacePerson
 * replace chosen record from table Person
 * return string to print
 * **/
char* replacePerson();

/**
 * Function printPersonAddress()
 * print record from table Adress with chosen PersonID
 * **/
void printPersonAddress();

/**
 * Function main
 * **/
int main() {
    setID();
    menu();
}

void setID(){
    const int fdp = open("mmfiles/person.db", O_RDONLY, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fdp, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }
    
    if (fileInfo.st_size == 0){
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        exit(EXIT_FAILURE);
    }
    const int p_records = fileInfo.st_size/sizeof(struct Person);
    struct Person* person =  mmap(0, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fdp, 0);
    if ( person == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    personID = (person+p_records-1)->ID;
    if ( munmap( person,  fileInfo.st_size) ) {
        perror("unmap");
        exit(-1);
    }
    close(fdp);
    const int fda = open("mmfiles/address.db", O_RDONLY, S_IRUSR | S_IWUSR );
    struct stat fileInfo2;
    if (fstat(fda, &fileInfo2) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }
    const int a_records = fileInfo2.st_size/sizeof(struct Address);
    struct Address* address =  mmap(0, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fda, 0);
    if ( address == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    addressID = (address+a_records-1)->ID;
    if ( munmap( address,  fileInfo.st_size) ) {
        perror("unmap");
        exit(-1);
    }
    close(fda);
}

void menu(){
    char c = '0';
    int i;
    printf("\nEnter nubmer:\n1.Print table 'person'\n2.Print table 'address'\n3.Add to table 'person'\n4.Add to table 'address'\n5.Delete from table 'person'\n6.Delete from table 'address'\n7.Replace in table 'person'.\n8.Replace in table 'address'\n9.Print adress for personID\n0.Exit\nYour choice: ");
    if (scanf("%d", &i) == 0 || i < 0 || i > 9) {
        puts("\nYou entered wrong value. Try one more time: ");
        do {
            c = getchar();
        }
        while (!isdigit(c));
            ungetc(c, stdin);
    } 

    switch(i){
        case(1):
            printPerson();
            menu();
            break;
        case(2):
            printAddress();
            menu();
            break;
        case(3):
            puts(addPerson());
            menu();
            break;
        case(4):
            puts(addAddress());
            menu();
            break;
        case(5):
            deletePerson();
            menu();
            break;
        case(6):
            deleteAddress();
            menu();
            break;
        case(7):
            puts(replacePerson());
            menu();
            break;
        case(8):
            puts(replaceAddress());
            menu();
            break;
        case(9):
            printPersonAddress();
            menu();
            break;
        case(0):
            break;
        default:
            break;
    }
}

void printPerson(){
    puts("\nTable person:\n[id]    [firstName]        [lastName]        [age]    [gender]");
    const int fd = open("mmfiles/person.db", O_RDONLY, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }
    
    if (fileInfo.st_size == 0){
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        exit(EXIT_FAILURE);
    }
    const int n_records = fileInfo.st_size/sizeof(struct Person);
    
    struct Person* person =  mmap(0, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if ( person == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    for ( int r = 0; r < n_records; ++r )
        if((person+r)->ID != 0)
            printf( "%d\t%-19s%-18s%-14d%c\n",
                (person+r)->ID, (person+r)->firstName, (person+r)->lastName, (person+r)->age, (person+r)->gender );
    
    if ( munmap( person,  fileInfo.st_size) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
}

void printAddress(){
    puts("\nTable address:\n[id]    [personId]    [city]        [postalCode]    [street]        [nr]");
    const int fd = open("mmfiles/address.db", O_RDONLY, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }
    if (fileInfo.st_size == 0){
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        exit(EXIT_FAILURE);
    }
    const int n_records = fileInfo.st_size/sizeof(struct Address);
    struct Address* address =  mmap(0, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if ( address == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    for ( int r = 0; r < n_records; ++r )
        if((address+r)->ID != 0)
            printf( "%d\t%-14d%-14s%-16s%-16s%d\n",
                (address+r)->ID, (address+r)->personID, (address+r)->city, (address+r)->postalCode, (address+r)->street, (address+r)->nr );
    
    if ( munmap( address,  fileInfo.st_size) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
}

bool checkString( const char s[] ){
    unsigned char c;
    while ( ( c = *s ) && ( isalpha( c ) || isblank( c ) ) ) ++s;
    if (*s == '\0')
        return false;
    return true;
}

char* addPerson(){
    // #################################### Enter data ####################################
    printf("Add record to table person:\n");
    int age;

    puts("Enter age(0-122):");
    scanf("%d", &age);
    if (age<0 || age>122) { //122, because oldest man on earth was 122 yo :D.
        puts("You entered wrong age.");
        return "Wrong values";
    }

    char gender;
    puts("Enter gender(F/M): ");
    scanf(" %c", &gender);
    if (gender != 'M' && gender != 'F') { // M - male, F - female
        puts("You entered wrong gender.");
        return "Wrong values";
    }

    char firstName[30];
    puts("Enter first name(letters in length: 2 - 30):");
    scanf("%s", firstName);
    if (strlen(firstName) < 2 || strlen(firstName) > 30 || checkString(firstName)) { 
        puts("You entered wrong firstName.");
        return "Wrong values";
    }

    char lastName[30];
    puts("Enter last name(letters in length: 2 - 30):");
    scanf("%s", lastName);
    if (strlen(lastName) < 2 || strlen(lastName) > 30 || checkString(lastName)) { 
        puts("You entered wrong lastName.");
        return "Wrong values";
    }

    // ####################################################################################
    personID += 1;
    const int fd = open("mmfiles/person.db", O_RDWR, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }

    off_t DBsize = sizeof(struct Person) + fileInfo.st_size;
    int n_records = fileInfo.st_size/sizeof(struct Person);
    const int status = ftruncate( fd,  DBsize );
    if ( status != 0 ) {
        perror("ftruncate");
        exit(-1);
    }
    struct Person* person =  mmap(0, DBsize, PROT_WRITE, MAP_SHARED, fd, 0);
    if ( person == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    
    fillPerson(person+n_records, firstName, lastName, age, gender);

    if ( munmap( person, DBsize ) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);

    return "######## Person Added ########";
}

char* addAddress(){
    printf("AddRecordToTable:\n");

    // #################################### Enter data ####################################
    int persID;
    printf("Enter personID whom is this address(1-%d):\n", getPersonID());
    scanf("%d", &persID);
    if (persID<1 || persID>getPersonID()) {
        puts("You entered wrong personID.");
        return "Wrong values";
    }

    char city[20];
    puts("Enter city(letters length: 2 - 20): ");
    scanf("%s", city);
    if (strlen(city) < 2 || strlen(city) > 20 || checkString(city)) { 
        puts("You entered wrong city.");
        return "Wrong values";
    }

    char postalCode[10];
    puts("Enter postalCode(length: 2 - 10):");
    scanf("%s", postalCode);
    if (strlen(postalCode) < 2 || strlen(postalCode) > 30 || !checkString(postalCode)) { 
        puts("You entered wrong postalCode.");
        return "Wrong values";
    }

    char street[20];
    puts("Enter street(letters in length: 2 - 20):");
    scanf("%s", street);
    if (strlen(street) < 2 || strlen(street) > 30 || checkString(street)) { 
        puts("You entered wrong lastName.");
        return "Wrong values";
    }

    int nr;
    puts("Enter house number:");
    scanf("%d", &nr);
    if (nr<1) {
        puts("You entered wrong house number.");
        return "Wrong values";
    }

    // ####################################################################################
    addressID += 1;
    const int fd = open("mmfiles/address.db", O_RDWR, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }
    
    off_t DBsize = sizeof(struct Address) + fileInfo.st_size;
    int n_records = fileInfo.st_size/sizeof(struct Address);
    const int status = ftruncate( fd,  DBsize );
    if ( status != 0 ) {
        perror("ftruncate");
        exit(-1);
    }
    struct Address* address =  mmap(0, DBsize, PROT_WRITE, MAP_SHARED, fd, 0);
    if ( address == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    
    fillAddress(address+n_records, persID, city, postalCode, street, nr);

    if ( munmap( address, DBsize ) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
    return "######## Address Added ########";
}

void deletePerson(){
    puts("Enter ID of record u want to delete:");
    int delID;
    scanf("%d", &delID);
    const int fd = open("mmfiles/person.db", O_RDWR, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }

    struct Person* person =  mmap(0, fileInfo.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if ( person == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    
    bzero(person+delID-1, sizeof(struct Person));

    if ( munmap( person, fileInfo.st_size ) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
    puts("\n######## Person Deleted ########\n");
}

void deleteAddress(){
    puts("Enter ID of record u want to delete:");
    int delID;
    scanf("%d", &delID);
    const int fd = open("mmfiles/address.db", O_RDWR, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }

    struct Address* address =  mmap(0, fileInfo.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if ( address == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    
    bzero(address+delID-1, sizeof(struct Address));

    if ( munmap( address, fileInfo.st_size ) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
    puts("\n######## Address Deleted ########\n");
}

char* replacePerson(){
    puts("Enter ID of record u want to replace:");
    int repID;
    scanf("%d", &repID);
    // #################################### Enter data ####################################
    int age;

    puts("Enter age(0-122):");
    scanf("%d", &age);
    if (age<0 || age>122) { //122, because oldest man on earth was 122 yo :D.
        puts("You entered wrong age.");
        return "Wrong values";
    }

    char gender;
    puts("Enter gender(F/M): ");
    scanf(" %c", &gender);
    if (gender != 'M' && gender != 'F') { // M - male, F - female
        puts("You entered wrong gender.");
        return "Wrong values";
    }

    char firstName[30];
    puts("Enter first name(letters in length: 2 - 30):");
    scanf("%s", firstName);
    if (strlen(firstName) < 2 || strlen(firstName) > 30 || checkString(firstName)) { 
        puts("You entered wrong firstName.");
        return "Wrong values";
    }

    char lastName[30];
    puts("Enter last name(letters in length: 2 - 30):");
    scanf("%s", lastName);
    if (strlen(lastName) < 2 || strlen(lastName) > 30 || checkString(lastName)) { 
        puts("You entered wrong lastName.");
        return "Wrong values";
    }
    
    // ####################################################################################

    const int fd = open("mmfiles/person.db", O_RDWR, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }

    struct Person* person =  mmap(0, fileInfo.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if ( person == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }

    const int n_records = fileInfo.st_size/sizeof(struct Person);

    fillPerson(person+repID-1, firstName, lastName, age, gender);
    (person+repID-1)->ID = repID;

    if ( munmap( person, fileInfo.st_size ) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
    
    return "######## Person Replaced ########";
}

char* replaceAddress(){
    puts("Enter ID of record u want to replace:");
    int repID;
    scanf("%d", &repID);

    // #################################### Enter data ####################################
    int persID;
    printf("Enter addressID whom is this address(1-%d):\n", getPersonID());
    scanf("%d", &persID);
    if (persID<1 || persID>getPersonID()) {
        puts("You entered wrong addressID.");
        return "Wrong values";
    }

    char city[20];
    puts("Enter city(letters length: 2 - 20): ");
    scanf("%s", city);
    if (strlen(city) < 2 || strlen(city) > 20 || checkString(city)) { 
        puts("You entered wrong city.");
        return "Wrong values";
    }

    char postalCode[10];
    puts("Enter postalCode(length: 2 - 10):");
    scanf("%s", postalCode);
    if (strlen(postalCode) < 2 || strlen(postalCode) > 30 || !checkString(postalCode)) { 
        puts("You entered wrong postalCode.");
        return "Wrong values";
    }

    char street[20];
    puts("Enter street(letters in length: 2 - 20):");
    scanf("%s", street);
    if (strlen(street) < 2 || strlen(street) > 30 || checkString(street)) { 
        puts("You entered wrong lastName.");
        return "Wrong values";
    }

    int nr;
    puts("Enter house number:");
    scanf("%d", &nr);
    if (nr<1) {
        puts("You entered wrong house number.");
        return "Wrong values";
    }

    // ####################################################################################
    
    const int fd = open("mmfiles/address.db", O_RDWR, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }

    struct Address* address =  mmap(0, fileInfo.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if ( address == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    
    fillAddress(address+repID-1, persID, city, postalCode, street, nr);
    (address+repID-1)->ID = repID;

    if ( munmap( address, fileInfo.st_size ) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
    
    return "######## address Replaced ########";
}

void printPersonAddress(){
    puts("Enter ID of person whose adress you want to print:");
    int chosenPersonID;
    scanf("%d", &chosenPersonID);
    puts("\nTable address:\n[id]    [personId]    [city]        [postalCode]    [street]        [nr]");
    const int fd = open("mmfiles/address.db", O_RDONLY, S_IRUSR | S_IWUSR );
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }
    if (fileInfo.st_size == 0){
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        exit(EXIT_FAILURE);
    }
    const int n_records = fileInfo.st_size/sizeof(struct Address);
    struct Address* address =  mmap(0, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if ( address == MAP_FAILED ) {
        perror("mmap failed");
        exit(-1);
    }
    for ( int r = 0; r < n_records; ++r )
        if((address+r)->personID == chosenPersonID)
            printf( "%d\t%-14d%-14s%-16s%-16s%d\n",
                (address+r)->ID, (address+r)->personID, (address+r)->city, (address+r)->postalCode, (address+r)->street, (address+r)->nr );

    if ( munmap( address,  fileInfo.st_size) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
}