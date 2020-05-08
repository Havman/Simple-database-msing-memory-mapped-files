char* replaceAddress(){
    puts("Enter ID of record u want to replace:");
    int repID;
    scanf("%d", &repID);

    // #################################### Enter data ####################################
    int persID;
    printf("Enter addressID whom is this address(1-%d):\n", getaddressID());
    scanf("%d", &persID);
    if (persID<1 || persID>getaddressID()) {
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

    const int n_records = fileInfo.st_size/sizeof(struct Address);
    int counter = 0;
    for(int i = 0; i<repID; i++){
        if((address+i)->ID == 0)
            counter--;
        counter++;
    }
    
    int tempID = (address+counter)->ID;
    filladdress(address+counter, firstName, lastName, age, gender);
    (address+counter)->ID = tempID;

    if ( munmap( address, fileInfo.st_size ) ) {
        perror("unmap");
        exit(-1);
    }
    close(fd);
    
    return "######## address Replaced ########";
}