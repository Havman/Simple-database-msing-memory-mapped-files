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

char* printTableFromMMFile(const char* filepath);

bool addToMMFileTable(const char* filepath, char* recordBuff);

char* concatenateEnteredValuesToPerson();

char* concatenateEnteredValuesToAddress();

bool checkString( const char s[] );

void menu();

int main(int argc, const char *argv[]){
    // menu();
    puts("\n");  
    int sfd, dfd;
    char *src, *dest;
    size_t filesize;

    /* SOURCE */
    sfd = open("hello.c", O_RDONLY);
    filesize = lseek(sfd, 0, SEEK_END);

    src = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, sfd, 0);

    /* DESTINATION */
    dfd = open("dest", O_RDWR | O_CREAT, 0666);

    ftruncate(dfd, filesize);

    dest = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dfd, 0);

    /* COPY */

    memcpy(dest, src, filesize);

    munmap(src, filesize);
    munmap(dest, filesize);

    close(sfd);
    close(dfd);

    return 0;
}

void menu(){
    char c = '0';
    int i;
    char* recordBuff;
    printf("Enter nubmer:\n1.Print table 'person'\n2.Print table 'address'\n3.Add to table 'person'\n4.Add to table 'address'\n0.Exit\n");
    if (scanf("%d", &i) == 0 || i < 0 || i > 4) {
        puts("You entered wrong value. Try one more time:\n");
        do {
            c = getchar();
        }
        while (!isdigit(c));
            ungetc(c, stdin);
    } 

    switch(i){
        case(1):
            puts("Table person:\n[id]    [age]    [gender]    [firstName]    [lastName]");
            printTableFromMMFile("mmfiles/person.bin");
            menu();
            break;
        case(2):
            puts("Table address:\n[id]    [personId]    [city]    [postalCode]    [street]    [nr]");
            printTableFromMMFile("mmfiles/address.bin");
            menu();
            break;
        case(3):
            recordBuff = concatenateEnteredValuesToPerson();
            if(addToMMFileTable("mmfiles/person.bin", recordBuff))
                puts("Record added.");
            menu();
            break;
        case(4):
            recordBuff = concatenateEnteredValuesToAddress();
            if(addToMMFileTable("mmfiles/person.bin", recordBuff))
                puts("Record added.");
            menu();
            break;
        case(0):
            break;
        default:
            break;
    }
}

char* printTableFromMMFile(const char* filepath){
    int fd = open(filepath, O_RDONLY, (mode_t)0600);
    
    if (fd == -1){
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }        

    struct stat fileInfo = {0};
    
    if (fstat(fd, &fileInfo) == -1){
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }
    
    if (fileInfo.st_size == 0){
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        exit(EXIT_FAILURE);
    }
    
    char* map = mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED){
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    static char ret[1000];
    strcpy(ret, map);
    // printing
    for (int i = 0; i < fileInfo.st_size; i++){
        if(map[i] == '[' && map[i+1] == ' '){
            i+=2;
            puts("\n");
        }
        if( map[i] == ']' && map[i+1] == ' ' && map[i+2] == ']' ){
            printf("%c", map[i]);
            i+=3;
        }
        printf("%c", map[i]);
    }
   
    // free the mmapped memory
    if (munmap(map, fileInfo.st_size) == -1){
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    close(fd);
    puts("\n\n");
    return ret;
}

bool checkString( const char s[] ){
    unsigned char c;
    while ( ( c = *s ) && ( isalpha( c ) || isblank( c ) ) ) ++s;
    if (*s == '\0')
        return false;
    return true;
}

char* concatenateEnteredValuesToPerson(){
    static char record[100] = {'[',' ','['};
    static int personID = 1;
    char* format = "] [";
    printf("AddRecordToTable:\n");
    int age;

    puts("Enter age(0-122):");
    scanf("%d", &age);
    if (age<0 || age>122) { //122, because oldest man on earth was 122 yo :D.
        puts("You entered wrong age.");
        return "Wrong values";
    }
    char buffer[3];
    sprintf(buffer, "%d", personID++);
    strcat(record, buffer);
    strcat(record, format);
    sprintf(buffer, "%d", age);
    strcat(record, buffer);
    strcat(record, format);

    char gender;
    puts("Enter gender(F/M): ");
    scanf(" %c", &gender);
    if (gender != 'M' && gender != 'F') { // M - male, F - female
        puts("You entered wrong gender.");
        return "Wrong values";
    }
    sprintf(buffer, "%c", gender);
    strcat(record, buffer);
    strcat(record, format);

    char firstName[15];
    puts("Enter first name(letters in length: 2 - 15):");
    scanf("%s", firstName);
    if (strlen(firstName) < 2 || strlen(firstName) > 15 || checkString(firstName)) { 
        puts("You entered wrong firstName.");
        return "Wrong values";
    }
    strcat(record, firstName);
    strcat(record, format);

    char lastName[15];
    puts("Enter last name(letters in length: 2 - 15):");
    scanf("%s", lastName);
    if (strlen(lastName) < 2 || strlen(lastName) > 15 || checkString(lastName)) { 
        puts("You entered wrong lastName.");
        return "Wrong values";
    }
    
    strcat(record, lastName);
    strcat(record, "] ] ");

    printf("record = %s\n", record);

    return record;
}

char* concatenateEnteredValuesToAddress(){
    static char record[100];
    char* format = "] [";
    printf("AddRecordToTable:\n");
    int age;
    bool flag = true;

    puts("Enter age(0-122):");
    scanf("%d", &age);
    if (age<0 || age>122) { //122, because oldest man on earth was 122 yo :D.
        puts("You entered wrong age.");
        flag = false;
    }
    char buffer[3] = "[ [";
    strcat(record, buffer);
    sprintf(buffer, "%d", age);
    strcat(record, buffer);
    strcat(record, format);

    char gender;
    puts("Enter gender(F/M): ");
    scanf(" %c", &gender);
    if (gender != 'M' && gender != 'F') { // M - male, F - female
        puts("You entered wrong gender.");
        flag = false;
    }
    sprintf(buffer, "%c", gender);
    strcat(record, buffer);
    strcat(record, format);

    char firstName[15];
    puts("Enter first name(letters in length: 2 - 15):");
    scanf("%s", firstName);
    if (strlen(firstName) < 2 || strlen(firstName) > 15 || checkString(firstName)) { 
        puts("You entered wrong firstName.");
        flag = false;
    }
    strcat(record, firstName);
    strcat(record, format);

    char lastName[15];
    puts("Enter last name(letters in length: 2 - 15):");
    scanf("%s", lastName);
    if (strlen(lastName) < 2 || strlen(lastName) > 15 || checkString(lastName)) { 
        puts("You entered wrong lastName.");
        flag = false;
    }
    
    strcat(record, lastName);
    strcat(record, "] ] ");

    printf("record = %s\n", record);
    if(flag){
        return record;
    }
    return "Wrong values";
}

bool addToMMFileTable(const char* filepath, char* recordBuff){
    if(!strcmp(recordBuff, "Wrong values"))
        return false;
    char* fileContent = printTableFromMMFile("mmfiles/person.bin");
    // system("clear");
    int tempSize = strlen(fileContent) + strlen(recordBuff);
    char text[tempSize];
    strcpy(text, fileContent);
    strcat(text, " ");
    strcat(text, recordBuff);
    printf("text = %s\t", text);

    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    
    if (fd == -1){
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    // Stretch the file size to the size of the (mmapped) array of char
    size_t textsize = strlen(text); 
    printf("textsize = %ld\t", textsize);
    if (lseek(fd, textsize, SEEK_SET) == -1){
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }
    
    if (write(fd, "", 1) == -1){
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }
    

    // Now the file is ready to be mmapped.
    char *map = mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("map = %ld", strlen(map));
    if (map == MAP_FAILED){
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < textsize; i++){
        map[i] = text[i];
    }
    map[tempSize] = map[tempSize + 1] = '\0';
    // Write it now to disk
    if (msync(map, textsize, MS_SYNC) == -1){
        perror("Could not sync the file to disk");
    }
    
    // Don't forget to free the mmapped memory
    if (munmap(map, textsize) == -1){
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Un-mmaping doesn't close the file, so we still need to do that.
    close(fd);
    puts("\n\n");
    return true;
}