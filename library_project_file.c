#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<cjson/cJSON.h>
#include<stdbool.h>
#include<time.h>
#define _XOPEN_SOURCE 500

#define MAX_BOOKS_PER_STUDENT 5
#define MAX_DAYS_TO_RETURN 20
#define FINE_PER_DAY 2


struct book_node
{
	char ID[50];
    char name[50];
	char author[50];
	char edition[50];
	char department[50];
    char language[50];
    int count;
    int borrowed;
	struct book_node *next;
}*book_new,*book_head=NULL,*book_tail=NULL,*book_temp,*i;


struct student_node
{
	char ID[20];
	char name[20];
	char mobile[20];
	char address[50];
    int borrowed;
    int fine;
	struct student_node *next;
    struct book_node *borrowed_books;
}*new_student,*student_head,*student_tail,*student_temp;
int student_count=0,book_count=0,employee_count=0;


struct borrowed_books
{
    char stu_ID[50];
    char ID[50];
    char name[50];
	char author[50];
	char edition[50];
	char department[50];
    char language[50];
    int count;
    char borrowed_date[50];
    char returned_date[50];
    char issuer[50];
    char reciever[50];
    int is_returned;
    int fine;
	struct borrowed_books *next;
}*borrowed_new,*borrowed_head,*borrowed_tail,*borrowed_temp;

struct employee_node
{
    char ID[50];
    char name[50];
    char password[50];
    struct  employee_node *next;
}*new,*employee_head,*employee_tail,employee_temp;


char passwd[20];
char employee[20];



void save_employee(const char *filename) 
{
    cJSON *root = cJSON_CreateArray();

    struct employee_node *temp = employee_head;
    while (temp != NULL) 
    {
        cJSON *employee = cJSON_CreateObject();
        cJSON_AddStringToObject(employee, "ID", temp->ID);
        cJSON_AddStringToObject(employee, "name", temp->name);
        cJSON_AddStringToObject(employee, "password", temp->password);
        
        cJSON_AddItemToArray(root, employee);
        temp = temp->next;
    }

    char *json_string = cJSON_Print(root);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) 
    {
        printf("Error opening file\n");
        return;
    }
    fputs(json_string, fp);
    fclose(fp);
    cJSON_Delete(root);
    free(json_string);
}




void change_password()
{
    char ID[50];
    int found=0;
    id:
    printf("Enter your Employee ID : ");
    fgets(ID,50,stdin);
    ID[strcspn(ID, "\n")] = '\0';
    if(strlen(ID)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto id;
    }

    struct employee_node *temp=employee_head;
    while(temp!=NULL)
    {
                    

        if(strcasecmp(temp->ID,ID)==0)
        {
            found = 1;

            again:
            printf("Enter your current password: ");
            scanf("%s", passwd);
            if (strcmp(temp->password, passwd) == 0)
            {
                printf("Enter new password: ");
                there:
                scanf("%s", passwd);
                if (strlen(passwd) >= 7)
                {
                    strcpy(temp->password, passwd);

                    printf("Password changed successfully\n\n");
                }
                else
                {
                    printf("Enter minimum 7 characters... Enter again\n");
                    goto there;
                }
            }
            else
            {
                int c;
                printf("Wrong password... Do you want to enter again (if yes-1, no-0)\n");
                enter:
                if (scanf("%d", &c) != 1)
                {
                    printf("Invalid input. Please enter a number.\n");
                    while (getchar() != '\n') {}
                    goto enter;
                }
                if (c == 1)
                {
                    goto again;
                }
                else if (c == 0)
                {
                    return;
                }
                else
                {
                    printf("Enter 1-Enter again, 0-No..\n");
                    goto enter;
                }
            }
            break;
        }
        temp=temp->next;
    }
    if(found==0)
    {
        printf("Employee not found...\n");
        return;
    }
    save_employee("employee.json");
}




void load_books_from_json(const char *filename) 
{
    FILE *fp = fopen(filename, "a+"); 
    if (fp == NULL) 
    {
        printf("Error opening file\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size == 0)
    {
        fclose(fp);
        return; 
    }
    fseek(fp, 0, SEEK_SET);
    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, fp);
    fclose(fp);
    buffer[size] = '\0';
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL)
    {
        free(buffer);
        return;
    }

    int array_size = cJSON_GetArraySize(root);
    for (int i = 0; i < array_size; i++) 
    {
        cJSON *book = cJSON_GetArrayItem(root, i);

        book_new = (struct book_node *)malloc(sizeof(struct book_node));
        if (book_new == NULL)
        {
            printf("Memory allocation failed\n");
            cJSON_Delete(root);
            free(buffer);
            return;
        }
        
        memset(book_new, 0, sizeof(struct book_node));

        strcpy(book_new->ID, cJSON_GetObjectItemCaseSensitive(book, "ID")->valuestring);
        strcpy(book_new->name, cJSON_GetObjectItemCaseSensitive(book, "name")->valuestring);
        strcpy(book_new->author, cJSON_GetObjectItemCaseSensitive(book,"author")->valuestring);
        strcpy(book_new->edition , cJSON_GetObjectItemCaseSensitive(book,"edition")->valuestring);
        strcpy(book_new->department, cJSON_GetObjectItemCaseSensitive(book, "department")->valuestring);
        strcpy(book_new->language, cJSON_GetObjectItemCaseSensitive(book, "language")->valuestring);
        book_new->count=cJSON_GetObjectItemCaseSensitive(book, "count")->valueint;
        book_new->borrowed=cJSON_GetObjectItemCaseSensitive(book, "borrowed")->valueint;

       

        book_new->next = NULL;

        if (book_head == NULL) 
        {
            book_head = book_new;
            book_tail = book_new;
        } else 
        {
            book_tail->next = book_new;
            book_tail = book_new;
        }
        book_count++;
    }

    cJSON_Delete(root);
    free(buffer);
}



void load_books_from_borrowed(const char *filename) 
{
    FILE *fp = fopen(filename, "a+");
    if (fp == NULL) 
    {
        printf("Error opening file\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size == 0)
    {
        fclose(fp);
        return; 
    }
    fseek(fp, 0, SEEK_SET);
    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, fp);
    fclose(fp);
    buffer[size] = '\0';

    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL)
    {
        free(buffer);
        return;
    }

    int array_size = cJSON_GetArraySize(root);
    for (int i = 0; i < array_size; i++) 
    {
        cJSON *book = cJSON_GetArrayItem(root, i);

        borrowed_new = (struct borrowed_books *)malloc(sizeof(struct borrowed_books));
        if (borrowed_new == NULL)
        {
            printf("Memory allocation failed\n");
            cJSON_Delete(root);
            free(buffer);
            return;
        }

        strcpy(borrowed_new->stu_ID, cJSON_GetObjectItemCaseSensitive(book, "stu_ID")->valuestring);
        strcpy(borrowed_new->ID, cJSON_GetObjectItemCaseSensitive(book, "ID")->valuestring);
        strcpy(borrowed_new->name, cJSON_GetObjectItemCaseSensitive(book, "name")->valuestring);
        strcpy(borrowed_new->author, cJSON_GetObjectItemCaseSensitive(book,"author")->valuestring);
        strcpy(borrowed_new->edition , cJSON_GetObjectItemCaseSensitive(book,"edition")->valuestring);
        strcpy(borrowed_new->department, cJSON_GetObjectItemCaseSensitive(book, "department")->valuestring);
        strcpy(borrowed_new->language, cJSON_GetObjectItemCaseSensitive(book, "language")->valuestring);
        borrowed_new->count=cJSON_GetObjectItemCaseSensitive(book, "count")->valueint;
        strcpy(borrowed_new->borrowed_date, cJSON_GetObjectItemCaseSensitive(book, "borrowed_date")->valuestring);
        strcpy(borrowed_new->returned_date, cJSON_GetObjectItemCaseSensitive(book, "returned_date")->valuestring);
        strcpy(borrowed_new->issuer, cJSON_GetObjectItemCaseSensitive(book, "issuer")->valuestring);
        strcpy(borrowed_new->reciever, cJSON_GetObjectItemCaseSensitive(book, "reciever")->valuestring);
        borrowed_new->is_returned=cJSON_GetObjectItemCaseSensitive(book, "is_returned")->valueint;
        borrowed_new->fine=cJSON_GetObjectItemCaseSensitive(book, "fine")->valueint;


        borrowed_new->next = NULL;

        if (borrowed_head == NULL) 
        {
            borrowed_head = borrowed_new;
            borrowed_tail = borrowed_new;
        } else 
        {
            borrowed_tail->next = borrowed_new;
            borrowed_tail = borrowed_new;
        }

    }

    cJSON_Delete(root);
    free(buffer);
}


void load_students_from_json(const char *filename) 
{
    FILE *fp = fopen(filename, "a+");
    if (fp == NULL) 
    {
        printf("Error opening file\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size == 0)
    {
        fclose(fp);
        return; 
    }
    fseek(fp, 0, SEEK_SET);
    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, fp);
    fclose(fp);
    buffer[size] = '\0';

    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL)
    {
        free(buffer);
        return;
    }

    int array_size = cJSON_GetArraySize(root);
    for (int i = 0; i < array_size; i++) 
    {
        cJSON *student = cJSON_GetArrayItem(root, i);

        new_student = (struct student_node *)malloc(sizeof(struct student_node));
        if (new_student== NULL)
        {
            printf("Memory allocation failed\n");
            cJSON_Delete(root);
            free(buffer);
            return;
        }
        memset(new_student, 0, sizeof(struct student_node));

        strcpy(new_student->ID , cJSON_GetObjectItemCaseSensitive(student, "ID")->valuestring);
        strcpy(new_student->name, cJSON_GetObjectItemCaseSensitive(student, "name")->valuestring);
        strcpy(new_student->mobile , cJSON_GetObjectItemCaseSensitive(student, "mobile")->valuestring);
        strcpy(new_student->address, cJSON_GetObjectItemCaseSensitive(student,"address")->valuestring);
        new_student->borrowed= cJSON_GetObjectItemCaseSensitive(student,"borrowed")->valueint;
        new_student->fine= cJSON_GetObjectItemCaseSensitive(student,"fine")->valueint;
       
        new_student->next = NULL;

        if (student_head == NULL) 
        {
            student_head = new_student;
            student_tail = new_student;
        } else 
        {
            student_tail->next = new_student;
            student_tail = new_student;
        }
        student_count++;
    }
   
   cJSON_Delete(root);
   free(buffer);
   
}




void save_borrowed(const char *filename) 
{
    cJSON *root = cJSON_CreateArray();

    struct borrowed_books *temp = borrowed_head;
    while (temp != NULL) 
    {
        cJSON *book = cJSON_CreateObject();
        cJSON_AddStringToObject(book, "stu_ID", temp->stu_ID);
        cJSON_AddStringToObject(book, "ID", temp->ID);
        cJSON_AddStringToObject(book, "name", temp->name);
        cJSON_AddStringToObject(book, "author", temp->author);
        cJSON_AddStringToObject(book, "edition", temp->edition);
        cJSON_AddStringToObject(book, "department", temp->department);
        cJSON_AddStringToObject(book, "language", temp->language);
        cJSON_AddNumberToObject(book, "count", temp->count);
        cJSON_AddStringToObject(book, "borrowed_date", temp->borrowed_date);
        cJSON_AddStringToObject(book, "returned_date", temp->returned_date);
        cJSON_AddStringToObject(book, "issuer", temp->issuer);
        cJSON_AddStringToObject(book, "reciever", temp->reciever);
        cJSON_AddNumberToObject(book, "is_returned", temp->is_returned);
        cJSON_AddNumberToObject(book, "fine", temp->fine);

        cJSON_AddItemToArray(root, book);
        temp = temp->next;
    }

    char *json_string = cJSON_Print(root);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) 
    {
        printf("Error opening file\n");
        return;
    }
    fputs(json_string, fp);
    fclose(fp);
    cJSON_Delete(root);
    free(json_string);
}




void save_student(const char *filename) 
{
    cJSON *root = cJSON_CreateArray();

    struct student_node *temp = student_head;
    while (temp != NULL) 
    {
        cJSON *student = cJSON_CreateObject();
        cJSON_AddStringToObject(student, "ID", temp->ID);
        cJSON_AddStringToObject(student, "name", temp->name);
        cJSON_AddStringToObject(student, "mobile", temp->mobile);
        cJSON_AddStringToObject(student, "address", temp->address);
        cJSON_AddNumberToObject(student, "borrowed", temp->borrowed);
        cJSON_AddNumberToObject(student, "fine", temp->fine);
        cJSON_AddItemToArray(root, student);
        temp = temp->next;
    }

    char *json_string = cJSON_Print(root);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) 
    {
        printf("Error opening file\n");
        return;
    }
    fputs(json_string, fp);
    fclose(fp);
    cJSON_Delete(root);
    free(json_string);
}


void save_book(const char *filename) 
{
    cJSON *root = cJSON_CreateArray();

    struct book_node *temp = book_head;
    while (temp != NULL) 
    {
        cJSON *book = cJSON_CreateObject();
        cJSON_AddStringToObject(book, "ID", temp->ID);
        cJSON_AddStringToObject(book, "name", temp->name);
        cJSON_AddStringToObject(book, "author", temp->author);
        cJSON_AddStringToObject(book, "edition", temp->edition);
        cJSON_AddStringToObject(book, "department", temp->department);
        cJSON_AddStringToObject(book, "language", temp->language);
        cJSON_AddNumberToObject(book, "count", temp->count);
        cJSON_AddNumberToObject(book, "borrowed", temp->borrowed);
        cJSON_AddItemToArray(root, book);
        temp = temp->next;
    }

    char *json_string = cJSON_Print(root);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) 
    {
        printf("Error opening file\n");
        return;
    }
    fputs(json_string, fp);
    fclose(fp);
    cJSON_Delete(root);
    free(json_string);
}


int is_all_alphabetic(const char *str) 
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
    {
        if(isspace(str[0]))
        {
            printf("Don't enter space 1st.. ");
            return 0;
        }
        else if (!isalpha(str[i]) && !isspace(str[i]))
        { 
        
            return 0; 
        }
        
    }
    return 1; 
}

int is_all_numeric(const char *str) 
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i])) 
        {
            return 0; 
        }
    }
    return 1; 
}

int is_all_alphanumeric(const char *str) 
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
    {
        if(isspace(str[0]))
        {
            printf("Don't enter space 1st.. ");
            return 0;
        }
        else if (!isalnum(str[i])&& !isspace(str[i]))
        {
            return 0; 
        }
    }
    return 1; 
}


int is_valid_mobile_number(const char *mobile_number) {
    int i;

    if (mobile_number == NULL || strlen(mobile_number) == 0) {
        return 0;
    }

    if (strlen(mobile_number) != 10) {
        return 0;
    }

    for (i = 0; i < 10; i++) {
        if (!isdigit(mobile_number[i])) {
            printf("Don't enter alphabets. ");
            return 0;
        }
    }

    return 1;
}


int is_valid_pincode(const char *pin) {
    int i;

    if (pin == NULL || strlen(pin) == 0) {
        return 0;
    }

    if (strlen(pin) != 6) {
        return 0;
    }

    for (i = 0; i < 6; i++) {
        if (!isdigit(pin[i])) {
            printf("Don't enter alphabets. ");
            return 0;
        }
    }

    return 1;
}



void add_employee()
{
    int ch;
    char ID[50];
    char name[50];
    char password[50];
    int num_employee;

    printf("How many emplyees do you want to add?\n");
    if (scanf("%d", &num_employee) != 1 || num_employee <= 0)
    {
        printf("Invalid input...enter again.\n");
        while (getchar() != '\n') {} 
        return;
    }

    getchar(); 

    for (int n = 0; n < num_employee; n++)
    {
        printf("\nAdding employee %d:\n", n + 1);

        new=(struct employee_node*)malloc(sizeof(struct employee_node));
    if (new == NULL)
        {
            printf("Memory allocation failed\n");
            
            return;
        }
        memset(new, 0, sizeof(struct employee_node));
	
     ID:
    printf("Employee ID : ");
   
    fgets(ID,50,stdin);
    ID[strcspn(ID, "\n")] = '\0';
    if(strlen(ID)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto ID;
    }

    if (!is_all_alphanumeric(ID)) 
    {
        printf("Enter alphanumerics only.. Enter again\n");
        goto ID;
    }

    for (int i = 0; i < strlen(ID); i++) 
    {
        ID[i] = tolower(ID[i]);
    }

    struct employee_node* temp = employee_head;
    while (temp != NULL) 
    {
        if (strcmp(temp->ID , ID)==0) 
        {
            printf("Id is used.. Entere unique ID again\n");
            goto ID;
        }
        temp = temp->next;
    }

    name:
    printf("Employee name : ");
    fgets(name,50,stdin);
    name[strcspn(name, "\n")] = '\0';
    if(strlen(name)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto name;
    }

    if (!is_all_alphabetic(name)) 
    {
        printf("Enter alphabetics only.. Enter again\n");
        goto name;
    }
    for (int i = 0; i < strlen(name); i++) 
    {
        name[i] = tolower(name[i]);
    }

    password:
    printf("Password : ");
    fgets(password,50,stdin);
    password[strcspn(password, "\n")] = '\0';
    if(strlen(password)<7)
    {
        printf("Password should be 7 characters..Enter again\n");
        goto password;
    }

    for (int i = 0; i < strlen(name); i++) 
    {
        name[i] = tolower(name[i]);
    }

    strcpy(new->ID,ID);
	strcpy(new->name,name);
	strcpy(new->password,password);
    new->next=NULL;
		
	if(employee_head==NULL)
	{
		employee_head=new;
		employee_tail=new;
	}
	else
	{
		employee_tail->next=new;
		employee_tail=new;
	}
	employee_count++;
    printf("Employee added successfully.\n\n");
	save_employee("employee.json");

}
}


void load_employee_from_json(const char *filename) 
{
    FILE *fp = fopen(filename, "a+"); 
    if (fp == NULL) 
    {
        printf("Error opening file\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size == 0)
    {
        add_employee();
        fclose(fp);
        return; 
    }
    fseek(fp, 0, SEEK_SET);
    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, fp);
    fclose(fp);
    buffer[size] = '\0';
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL)
    {
        free(buffer);
        return;
    }

    int array_size = cJSON_GetArraySize(root);
    for (int i = 0; i < array_size; i++) 
    {
        cJSON *employee = cJSON_GetArrayItem(root, i);

        new = (struct employee_node *)malloc(sizeof(struct employee_node));
        if (new == NULL)
        {
            printf("Memory allocation failed\n");
            cJSON_Delete(root);
            free(buffer);
            return;
        }
        
        memset(new, 0, sizeof(struct employee_node));

        strcpy(new->ID, cJSON_GetObjectItemCaseSensitive(employee, "ID")->valuestring);
        strcpy(new->name, cJSON_GetObjectItemCaseSensitive(employee, "name")->valuestring);
        strcpy(new->password, cJSON_GetObjectItemCaseSensitive(employee,"password")->valuestring);
        new->next = NULL;

        if (employee_head == NULL) 
        {
            employee_head = new;
            employee_tail = new;
        } else 
        {
            employee_tail->next = new;
            employee_tail = new;
        }
        employee_count++;
    }

    cJSON_Delete(root);
    free(buffer);
}





void add_book()
{
	int ch;
    char ID[50];
    char name[50];
	char author[50];
	char edition[50];
	char department[50];
    char language[50];
    int num_books;
    getchar();
	
    printf("How many books do you want to add? \n");
    if (scanf("%d", &num_books) != 1 || num_books <= 0)
    {
        printf("Invalid number. Please enter a positive number.\n");
        while (getchar() != '\n') {} 
        return;
    }

    getchar(); 

    for (int n = 0; n < num_books; n++)
    {
        printf("\nAdding book %d:\n", n + 1);

	book_new=(struct book_node*)malloc(sizeof(struct book_node));
    if (book_new == NULL)
        {
            printf("Memory allocation failed\n");
            
            return;
        }
        memset(book_new, 0, sizeof(struct book_node));
	
     ID:
    printf("Book ID : ");
   
    fgets(ID,50,stdin);
    ID[strcspn(ID, "\n")] = '\0';
    if(strlen(ID)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto ID;
    }

    if (!is_all_alphanumeric(ID)) 
    {
        printf("Enter alphanumerics only.. Enter again\n");
        goto ID;
    }

    for (int i = 0; i < strlen(ID); i++) 
    {
        ID[i] = tolower(ID[i]);
    }

    struct book_node* temp = book_head;
    while (temp != NULL) 
    {
        if (strcmp(temp->ID , ID)==0) 
        {
            printf("Id is used.. Entere unique ID again\n");
            goto ID;
        }
        temp = temp->next;
    }

    name:
    printf("Book name : ");
    fgets(name,50,stdin);
    name[strcspn(name, "\n")] = '\0';
    if(strlen(name)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto name;
    }

    if (!is_all_alphanumeric(name)) 
    {
        printf("Enter alphanumerics only.. Enter again\n");
        goto name;
    }
    for (int i = 0; i < strlen(name); i++) 
    {
        name[i] = tolower(name[i]);
    }

    author:
    printf("Author name : ");
    fgets(author,50,stdin);
    author[strcspn(author, "\n")] = '\0';
    if(strlen(author)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto author;
    }
    if (!is_all_alphabetic(author)) 
    {
        printf("Enter alphabets only.. Enter again\n");
        goto author;
    }
    for (int i = 0; i < strlen(author); i++) 
    {
        author[i] = tolower(author[i]);
    }

    edition:
    printf("Edition : ");
    fgets(edition,50,stdin);
    edition[strcspn(edition, "\n")] = '\0';
    if(strlen(edition)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto edition;
    }

    if (!is_all_numeric(edition)) 
    {
        printf("Enter numbers only.. Enter again\n");
        goto edition;
    }
    int edition_int;
    edition_int = atoi(edition);
    if(!(edition_int >= 1 && edition_int <= 1000))
    {
        printf("Invalid edition.. (Enter between 1-1000)\n");
        goto edition;
    }


    
    department:
    printf("choose the department : \n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("\t1. Computer science\n");
    printf("\t2. Electronics and communication\n");
    printf("\t3. Mechanical\n");
    printf("\t4. Civil\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("Enter your choice : ");
    if (scanf("%d", &ch) != 1) 
		{
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n') {}
            goto department;
        }


    switch(ch)
    {
        case 1:
            strcpy(department,"computer science");
            printf("department : computer science\n");
            break;

        case 2:
            strcpy(department,"electronics and communication");
            printf("department : electronics and communication\n");
            break;

        case 3:
            strcpy(department,"mechanical");
            printf("department : mechanical\n");
            break;

        case 4:
            strcpy(department,"civil");
            printf("department : civil\n");
            break;

        default:
            printf("\nChoose valid department again\n");
            goto department;
            break;
    }
    
    getchar();
        


    language:
    printf("choose the language : \n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("\t1. English\n");
    printf("\t2. Hindi\n");
    printf("\t3. Kannada\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("Enter your choice : ");
    if (scanf("%d", &ch) != 1) 
		{
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n') {}
            goto language;
        }

    switch(ch)
    {
        case 1:
            strcpy(language,"english");
            printf("language : english\n");
            break;

        case 2:
            strcpy(language,"hindi");
            printf("language : hindi\n");
            break;

        case 3:
            strcpy(language,"kannada");
            printf("language : kannada\n");
            break;

        default:
            printf("\nChoose valid language again\n");
            goto language;
            break;
    }
    
    getchar();
        
    
	
    strcpy(book_new->ID,ID);
	strcpy(book_new->name,name);
	strcpy(book_new->author,author);
	strcpy(book_new->edition,edition);
	strcpy(book_new->department,department);
    strcpy(book_new->language,language);
    book_new->count=1;
    book_new->borrowed=0;
    temp = book_head;
    while(temp!=NULL)
    {
        if(strcasecmp(temp->name,book_new->name)==0 && strcasecmp(temp->author,book_new->author)==0 && strcasecmp(temp->edition,book_new->edition)==0 &&
        strcasecmp(temp->department,book_new->department)==0 && strcasecmp(temp->language,book_new->language)==0)
        {
            book_new->count++;
            temp->count++;
        }
        temp=temp->next;
    }
	
	book_new->next=NULL;
		
	if(book_head==NULL)
	{
		book_head=book_new;
		book_tail=book_new;
	}
	else
	{
		book_tail->next=book_new;
		book_tail=book_new;
	}
	book_count++;
    printf("Book added successfully.\n\n");
	save_book("book.json");
}
}


void display_book() 
{
    FILE *fp = fopen("book.json", "r");
    if (fp == NULL) 
    {
        printf("There is no data\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, fp);
    fclose(fp);
    buffer[size] = '\0';

    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) 
    {
        free(buffer);
        return;
    }

    int array_size = cJSON_GetArraySize(root);
    for (int i = 0; i < array_size; i++) 
    {
        cJSON *book = cJSON_GetArrayItem(root, i);

        int id = cJSON_GetObjectItemCaseSensitive(book, "ID")->valueint;
        const char *name = cJSON_GetObjectItemCaseSensitive(book, "name")->valuestring;
        const char *author = cJSON_GetObjectItemCaseSensitive(book, "author")->valuestring;
        int edition = cJSON_GetObjectItemCaseSensitive(book, "edition")->valueint;
        const char *department = cJSON_GetObjectItemCaseSensitive(book, "department")->valuestring;
        const char *language = cJSON_GetObjectItemCaseSensitive(book, "language")->valuestring;

        printf("ID: %d\n", id);
        printf("Name: %s\n", name);
        printf("Author: %s\n", author);
        printf("Edition: %d\n", edition);
        printf("Department: %s\n", department);
        printf("Language: %s\n\n", language);
    }

    cJSON_Delete(root);
    free(buffer);
}



void display_books_in_department_by_language(char department[],char language[])
{
   
    struct book_node *temp = book_head;
    bool books_found = false;

    while (temp != NULL) 
    {
        if (strcasecmp(temp->department, department) == 0 && strcasecmp(temp->language,language)==0) 
        {
            printf("\nID: %s\n", temp->ID);
            printf("Name: %s\n", temp->name);
            printf("Author: %s\n", temp->author);
            printf("Edition: %s\n", temp->edition);
            printf("Department: %s\n", temp->department);
            printf("Language: %s\n", temp->language);
            printf("Count: %d\n",temp->count);
            printf("--------------------------\n");
            books_found = true;
            
        }
        temp = temp->next;


    }

    if (!books_found) 
    {
        printf("No books found in this department in this language \n");
    }
}


void display_books_in_department_by_language_by_alphabet(char department[],char language[],char alpha)
{
   
    struct book_node *temp = book_head;
    bool books_found = false;

    while (temp != NULL) 
    {
        if (strcasecmp(temp->department, department) == 0 && strcasecmp(temp->language,language)==0 && temp->name[0] == alpha) 
        {
            printf("\nID: %s\n", temp->ID);
            printf("Name: %s\n", temp->name);
            printf("Author: %s\n", temp->author);
            printf("Edition: %s\n", temp->edition);
            printf("Department: %s\n", temp->department);
            printf("Language: %s\n", temp->language);
            printf("Count: %d\n",temp->count);
            printf("--------------------------\n");
            books_found = true;
            
        }
        temp = temp->next;


    }

    if (!books_found) 
    {
        printf("No books found in this department in this language \n");
    }
}


void display_books_by_department(char department[])
{
    struct book_node *temp = book_head;
    bool books_found = false;
    int ch;
    char language[20];

    while (temp != NULL) 
    {
        if (strcasecmp(temp->department, department) == 0) 
        {
            
            printf("\nID: %s\n", temp->ID);
            printf("Name: %s\n", temp->name);
            printf("Author: %s\n", temp->author);
            printf("Edition: %s\n", temp->edition);
            printf("Department: %s\n", temp->department);
            printf("Language: %s\n", temp->language);
            printf("Count: %d\n",temp->count);
            printf("--------------------------\n");
            books_found = true;
            
        }
        temp = temp->next;


    }

    

    if (!books_found) 
    {
        printf("No books found in this department \n");
        
    }
    else
    {
        printf("\n\nDo you want to search books by language :\n(1-yes,0-no)\n");
        again:
        if (scanf("%d", &ch) != 1) 
        {
            while (getchar() != '\n') {}
            goto again;

        }


        if(ch==1)
        {
                                                                            
            int ab;
            lang:
            printf("\nChoose the language : ");
            printf("\n1. English\n2. Kannada\n3. Hindi\n\n");

            printf("Enter your choice : ");
            if (scanf("%d", &ab) != 1) 
            {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n') {}

            }
            switch(ab)
            {
                case 1 :
                    strcpy(language,"english");
                    break;

                case 2 :
                    strcpy(language,"kannada");
                    break;

                case 3 :
                    strcpy(language,"hindi");
                    break;

                default :
                    printf("Invalid chioce...Enter valid choice\n");
                    goto lang;
                    break;

            }
            printf("-----------------------------------------------------------------------------------\n");
            display_books_in_department_by_language(department,language) ;
            printf("-----------------------------------------------------------------------------------\n");
            return;
        }
        else if(ch == 0)
        {
            return;
        }
        else
        {
            printf("Enter 1 or 0..\n");
            goto again;
        }
        
    }
    
}


void display_books_by_department_using_alphabets(char department[])
{
    struct book_node *temp = book_head;
    bool books_found = false;
    int ch;
    char language[20];
    char alpha;
    char al[10];
    printf("Enter the 1st letter of the book you want to sort :\n");
    alpha:
    getchar();
    fgets(al,sizeof(al),stdin);
    al[strcspn(al,"\n")]='\0';
    if(!is_all_alphabetic(al))
    {
        printf("Enter one alphabet ...enter again..\n");
        goto alpha;
    }
    alpha=al[0];
    
    alpha=tolower(alpha);
    while (temp != NULL) 
    {
        if ((strcasecmp(temp->department, department) == 0 )&& (temp->name[0] == alpha)) 
        {
            
            printf("\nID: %s\n", temp->ID);
            printf("Name: %s\n", temp->name);
            printf("Author: %s\n", temp->author);
            printf("Edition: %s\n", temp->edition);
            printf("Department: %s\n", temp->department);
            printf("Language: %s\n", temp->language);
            printf("Count: %d\n",temp->count);
            printf("--------------------------\n");
            books_found = true;
            
        }
        temp = temp->next;


    }

    

    if (!books_found) 
    {
        printf("No books found in this department in this alphabet '%c'. \n",alpha);
        
    }
    else
    {
        printf("\n\nDo you want to search books by language :\n(1-yes,0-no)\n");
        again:
        if (scanf("%d", &ch) != 1) 
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n') {}
            goto again;

        }


        if(ch==1)
        {
            int ab;
            lang:
            printf("\nChoose the language : ");
            printf("\n1. English\n2. Kannada\n3. Hindi\n\n");

            printf("Enter your choice : ");
            if (scanf("%d", &ab) != 1) 
            {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n') {}

            }
            switch(ab)
            {
                case 1 :
                    strcpy(language,"english");
                    break;

                case 2 :
                    strcpy(language,"kannada");
                    break;

                case 3 :
                    strcpy(language,"hindi");
                    break;

                default :
                    printf("Invalid chioce...Enter valid choice\n");
                    goto lang;
                    break;

            }
            printf("-----------------------------------------------------------------------------------\n");
            display_books_in_department_by_language_by_alphabet(department,language,alpha) ;
            printf("-----------------------------------------------------------------------------------\n");
            return;
        }
        else if(ch == 0)
        {
            return;
        }
        else
        {
            printf("Enter 1 or 0..\n");
            goto again;
        }
        
    }
    
}



void display_books_in_department_by_book_name_by_language(char department[],char book_name[],char language[])
{
    
    struct book_node *temp = book_head;
    bool books_found = false;

    while (temp != NULL) 
    {
        if (strcasecmp(temp->department, department) == 0 && strcasecmp(temp->name,book_name)==0 && strcasecmp(temp->language,language)==0) 
        {
            printf("\nID: %s\n", temp->ID);
            printf("Name: %s\n", temp->name);
            printf("Author: %s\n", temp->author);
            printf("Edition: %s\n", temp->edition);
            printf("Department: %s\n", temp->department);
            printf("Language: %s\n", temp->language);
            printf("Count: %d\n",temp->count);
            printf("--------------------------\n");
            books_found = true;
            
        }
        temp = temp->next;


    }

    if (!books_found) 
    {
        printf("No books found in this language \n");
    }
}


void display_books_in_department_by_book_name(char department[],char book_name[])
{
    
    struct book_node *temp = book_head;
    bool books_found = false; 
    int ch;
    char language[20];

    while (temp != NULL) 
    {
        if (strcasecmp(temp->department, department) == 0 && strcasecmp(temp->name,book_name)==0) 
        {
            
            printf("\nID: %s\n", temp->ID);
            printf("Name: %s\n", temp->name);
            printf("Author: %s\n", temp->author);
            printf("Edition: %s\n", temp->edition);
            printf("Department: %s\n", temp->department);
            printf("Language: %s\n", temp->language);
            printf("Count: %d\n",temp->count);
            printf("--------------------------\n");
            books_found = true;
            if(temp->count>1)
            {
                return;
            }
        }
        temp = temp->next;


    }

    if (!books_found) 
    {
        printf("No books found in this book name \n");
    }
    else
    {
        printf("\n\nDo you want to search books by language :\n(1-yes,0-no)\n");
        again:
        if (scanf("%d", &ch) != 1) 
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n') {}
            goto again;

        }


            if(ch==1)
            {
            int ab;
            lang:
            printf("\nChoose the language : ");
            printf("\n1. English\n2. Kannada\n3. Hindi\n\n");

            printf("Enter your choice : ");
            if (scanf("%d", &ab) != 1) 
            {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n') {}

            }
            switch(ab)
            {
                case 1 :
                    strcpy(language,"english");
                    break;

                case 2 :
                    strcpy(language,"kannada");
                    break;

                case 3 :
                    strcpy(language,"hindi");
                    break;

                default :
                    printf("Invalid chioce...Enter valid choice\n");
                    goto lang;
                    break;

            }
                printf("-----------------------------------------------------------------------------------\n");
                display_books_in_department_by_book_name_by_language(department,book_name,language);
                printf("-----------------------------------------------------------------------------------\n");
                return;
            }
            else if(ch == 0)
            {
                return;
            }
            else
            {
                printf("Enter 1 or 0..\n");
                goto again;
            }

    }
    
}



void display_books_in_department_by_author_name_by_language(char department[],char author_name[],char language[])
{
    
    struct book_node *temp = book_head;
    bool books_found = false;

    while (temp != NULL) 
    {
        if (strcasecmp(temp->department, department) == 0 && strcasecmp(temp->author,author_name)==0 && strcasecmp(temp->language,language)==0) 
        {
            printf("\nID: %s\n", temp->ID);
            printf("Name: %s\n", temp->name);
            printf("Author: %s\n", temp->author);
            printf("Edition: %s\n", temp->edition);
            printf("Department: %s\n", temp->department);
            printf("Language: %s\n", temp->language);
            printf("Count: %d\n",temp->count);
            printf("--------------------------\n");
            books_found = true;
            
        }
        temp = temp->next;


    }

    if (!books_found) 
    {
        printf("No books found in this language \n");
    }
}


void display_books_in_department_by_author_name(char department[],char author_name[])
{
    
    struct book_node *temp = book_head;
    bool books_found = false;
    int ch;
    char language[20];

    while (temp != NULL) 
    {
        if (strcasecmp(temp->department, department) == 0 && strcasecmp(temp->author,author_name)==0) 
        {
            
            printf("\nID: %s\n", temp->ID);
            printf("Name: %s\n", temp->name);
            printf("Author: %s\n", temp->author);
            printf("Edition: %s\n", temp->edition);
            printf("Department: %s\n", temp->department);
            printf("Language: %s\n", temp->language);
            printf("Count: %d\n",temp->count);
            printf("--------------------------\n");
            books_found = true;
            if(temp->count>1)
            {
                return;
            }
            
                                                                    
        }
        temp = temp->next;


    }

    if (!books_found) 
    {
        printf("No books found in this author name \n");
    }
    else
    {
        printf("\n\nDo you want to search books by language :\n(1-yes,0-no)\n");
        again:
        if (scanf("%d", &ch) != 1) 
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n') {}
            goto again;

        }
            if(ch==1)
            {
                
            int ab;
            lang:
            printf("\nChoose the language : ");
            printf("\n1. English\n2. Kannada\n3. Hindi\n\n");

            printf("Enter your choice : ");
            if (scanf("%d", &ab) != 1) 
            {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n') {}

            }
            switch(ab)
            {
                case 1 :
                    strcpy(language,"english");
                    break;

                case 2 :
                    strcpy(language,"kannada");
                    break;

                case 3 :
                    strcpy(language,"hindi");
                    break;

                default :
                    printf("Invalid chioce...Enter valid choice\n");
                    goto lang;
                    break;

            }
                printf("-----------------------------------------------------------------------------------\n");
                display_books_in_department_by_author_name_by_language(department,author_name,language);
                printf("-----------------------------------------------------------------------------------\n");
                return;
            }
            else if(ch == 0)
            {
                return;
            }
            else
            {
                printf("Enter 1 or 0..\n");
                goto again;
            }
    }
    
}



void delete_book() 
{
	char id[50];
    
	if(book_head == NULL) 
	{
		printf("No books to delete.\n");
    } 
    else 
    {
		printf("Enter the book ID you want to delete: ");
		getchar();
        id:
        fgets(id,50,stdin);
        id[strcspn(id, "\n")] = '\0';
        if(strlen(id)==0)
        {
            printf("You entered nothing. Enter again\n");
            goto id;
        }

        
        book_temp = book_head;
        struct book_node *book_prev = NULL;
		int found = 0;
        struct book_node *temp = book_head;
		while (book_temp != NULL) 
		{
			if(strcasecmp(book_temp->ID , id) == 0)
			{
				
                while(temp!=NULL)
                {
                    if(strcasecmp(temp->name,book_temp->name)==0 && strcasecmp(temp->author,book_temp->author)==0 && strcasecmp(temp->edition,book_temp->edition)==0 &&
                        strcasecmp(temp->department,book_temp->department)==0 && strcasecmp(temp->language,book_temp->language)==0)
                    {
                        temp->count--;
                    } 
                    temp=temp->next;
                }
                        
                
                
                if(book_prev == NULL) 
				{
					book_head = book_temp->next;
                } 
                else 
                {
					book_prev->next = book_temp->next;
                }
				if (book_temp == book_tail) 
				{
                    book_tail = book_prev;
                }
                free(book_temp);
                found = 1;
                printf("Book deleted.\n");
                book_count--;
                break;
            }
            book_prev = book_temp;
            book_temp = book_temp->next;
        }
        
        if (!found)
		{
			printf("book with ID %s not found.\n", id);
		}
        save_book("book.json");
	}
}

void update_book()
{
    char id[50];
    
	if(book_head == NULL) 
	{
		printf("No books to Update.\n");
    } 
    else 
    {
		printf("Enter the book ID you want to Update: ");
		getchar();
        id:
        fgets(id,50,stdin);
        id[strcspn(id, "\n")] = '\0';
        if(strlen(id)==0)
        {
            printf("You entered nothing. Enter again\n");
            goto id;
        }
        
        book_temp = book_head;
		int found = 0,ch;
        struct book_node *temp = book_head;
        
		while (book_temp != NULL) 
		{
            if(strcasecmp(book_temp->ID , id) == 0)
			{
                while(temp!=NULL)
                {
                    if(strcasecmp(temp->name,book_temp->name)==0 && strcasecmp(temp->author,book_temp->author)==0 && strcasecmp(temp->edition,book_temp->edition)==0 &&
                        strcasecmp(temp->department,book_temp->department)==0 && strcasecmp(temp->language,book_temp->language)==0)
                    {
                        temp->count--;
                    } 
                    temp=temp->next;
                }
                name:
                printf("Book name : ");
                fgets(book_temp->name,50,stdin);
                book_temp->name[strcspn(book_temp->name, "\n")] = '\0';
                if(strlen(book_temp->name)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto name;
                }

                if (!is_all_alphanumeric(book_temp->name)) 
                {
                    printf("Enter alphanumerics only.. Enter again\n");
                    goto name;
                }
                for (int i = 0; i < strlen(book_temp->name); i++) 
                {
                    book_temp->name[i] = tolower(book_temp->name[i]);
                }

                author:
                printf("Author name : ");
                fgets(book_temp->author,50,stdin);
                book_temp->author[strcspn(book_temp->author, "\n")] = '\0';
                if(strlen(book_temp->author)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto author;
                }

                if (!is_all_alphabetic(book_temp->author)) 
                {
                    printf("Enter alphabets only.. Enter again\n");
                    goto author;
                }
                for (int i = 0; i < strlen(book_temp->author); i++) 
                {
                    book_temp->author[i] = tolower(book_temp->author[i]);
                }


                edition:
                printf("Edition : ");
                fgets(book_temp->edition,50,stdin);
                book_temp->edition[strcspn(book_temp->edition, "\n")] = '\0';
                if(strlen(book_temp->edition)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto edition;
                }

                if (!is_all_numeric(book_temp->edition)) 
                {
                    printf("Enter numbers only.. Enter again\n");
                    goto edition;
                }
                int edition_int;
                edition_int = atoi(book_temp->edition);
                if(!(edition_int >= 1 && edition_int <= 1000))
                {
                    printf("Invalid edition.. (Enter between 1-1000)\n");
                    goto edition;
                }

                
                department:
                printf("\nchoose the department : \n");
                printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                printf("\t1. Computer science\n");
                printf("\t2. Electronics and communication\n");
                printf("\t3. Mechanical\n");
                printf("\t4. Civil\n");
                printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                printf("Enter your choice : ");
                if (scanf("%d", &ch) != 1) 
                    {
                        printf("Invalid input. Please enter a number.\n");
                        while (getchar() != '\n') {}
                        goto department;
                    }


                switch(ch)
                {
                    case 1:
                        strcpy(book_temp->department,"computer science");
                        printf("department : computer science\n");
                        break;

                    case 2:
                        strcpy(book_temp->department,"electronics and communication");
                        printf("department : electronics and communication\n");
                        break;

                    case 3:
                        strcpy(book_temp->department,"mechanical");
                        printf("department : mechanical\n");
                        break;

                    case 4:
                        strcpy(book_temp->department,"civil");
                        printf("department : civil\n");
                        break;

                    default:
                        printf("\nChoose valid department again\n");
                        goto department;
                        break;
                }
                
                getchar();

                language:
                printf("\nchoose the language : \n");
                printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                printf("\t1. English\n");
                printf("\t2. Hindi\n");
                printf("\t3. Kannada\n");
                printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                printf("Enter your choice : ");
                if (scanf("%d", &ch) != 1) 
                    {
                        printf("Invalid input. Please enter a number.\n");
                        while (getchar() != '\n') {}
                        goto language;
                    }

                switch(ch)
                {
                    case 1:
                        strcpy(book_temp->language,"english");
                        printf("language : english\n");
                        break;

                    case 2:
                        strcpy(book_temp->language,"hindi");
                        printf("language : hindi\n");
                        break;

                    case 3:
                        strcpy(book_temp->language,"kannada");
                        printf("language : kannada\n");
                        break;

                    default:
                        printf("\nChoose valid language again\n");
                        goto language;
                        break;
                }
                
                getchar();
                
                            
                printf("\nBook updated...\n\n");
				found=1;
    
                temp = book_head;
                 while(temp!=NULL)
                {
                    if(strcasecmp(temp->name,book_temp->name)==0 && strcasecmp(temp->author,book_temp->author)==0 && strcasecmp(temp->edition,book_temp->edition)==0 &&
                        strcasecmp(temp->department,book_temp->department)==0 && strcasecmp(temp->language,book_temp->language)==0)
                    {
                        
                        temp->count++;
                       
                    } 
                    temp=temp->next;
                }
                
                book_temp->count=0;
                struct book_node *var = book_head;
                temp=book_head;     
                while(var!=NULL)
                {
                    if(strcasecmp(book_temp->ID , var->ID) == 0)
			        {
                        while(temp!=NULL)
                        {
                            if(strcasecmp(temp->name,var->name)==0 && strcasecmp(temp->author,var->author)==0 && strcasecmp(temp->edition,var->edition)==0 &&
                                strcasecmp(temp->department,var->department)==0 && strcasecmp(temp->language,var->language)==0)
                            {
                                var->count++;
                            } 
                            temp=temp->next;
                        }
                

                    }
                var=var->next;
                
                }

				break;	
			}
			book_temp = book_temp->next;
		}
		
		if(found!=1)
		{
			printf("Book with the id %s not found.\n",id);
		}
        save_book("book.json");
			
	}
}


void count_book()
{
	printf("------------------------------------------------\n");
    printf("The total number of books in Library : %d\n",book_count);

    int EC_count=0,CS_count=0,civil_count=0,mech_count=0;
    struct book_node *temp = book_head;
    while (temp != NULL) 
    {
        if (strcasecmp(temp->department, "computer science") == 0) 
        {
            CS_count++;
        }
        else if (strcasecmp(temp->department, "electronics and communication") == 0) 
        {
            EC_count++;
        }
        else if (strcasecmp(temp->department, "mechanical") == 0) 
        {
            mech_count++;
        }
        else if (strcasecmp(temp->department, "civil") == 0) 
        {
            civil_count++;
        }
        temp=temp->next;

    }

    printf("\n-------------------------------------------------\n");
    printf("Number of books in each departments :\n\n\tComputer science : %d\n\tElectronics and Communication : %d\n\tMechanical : %d\n\tCivil : %d\n",CS_count,EC_count,mech_count,civil_count);
    printf("--------------------------------------------------\n\n");
        
    
}





void add_student()
{
	char ID[20];
    char name[20];
    char mobile[20];
	char address[50]="\0";
    int num_students;
   
	getchar();

    printf("How many students do you want to add? ");
    if (scanf("%d", &num_students) != 1 || num_students <= 0)
    {
        printf("Invalid number. Please enter a positive number.\n");
        while (getchar() != '\n') {}  
        return;
    }

    getchar(); 

    for (int i = 0; i < num_students; i++)
    {
        printf("\nAdding student %d:\n", i + 1);
	new_student=(struct student_node*)malloc(sizeof(struct student_node));
     if (new_student== NULL)
        {
            printf("Memory allocation failed\n");
            return;
        }
        memset(new_student, 0, sizeof(struct student_node));
	
    ID_stu:
    printf("Student ID : ");
    fgets(ID,20,stdin);
    ID[strcspn(ID, "\n")] = '\0';
    if(strlen(ID)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto ID_stu;
    }

    if (!is_all_alphanumeric(ID)) 
    {
        printf("Enter alphanumerics only.. Enter again\n");
        goto ID_stu;
    }
    for (int i = 0; i < strlen(ID); i++) 
    {
        ID[i] = tolower(ID[i]);
    }

    struct student_node* temp = student_head;
    while (temp != NULL) 
    {
        if (strcmp(temp->ID , ID)==0) 
        {
            printf("Id is used.. Entere unique ID again\n");
            goto ID_stu;
        }
        temp = temp->next;
    }
    

    name_stu:
    printf("Student name : ");
    fgets(name,20,stdin);
    name[strcspn(name, "\n")] = '\0';
    if(strlen(name)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto name_stu;
    }

    if (!is_all_alphabetic(name)) 
    {
        printf("Enter alphabets only.. Enter again\n");
        goto name_stu;
    }
    for (int i = 0; i < strlen(name); i++) 
    {
        name[i] = tolower(name[i]);
    }
    
    mobile:
    printf("Mobile number : ");
    getchar();
    fgets(mobile,20,stdin);
    mobile[strcspn(mobile, "\n")] = '\0';
    if(strlen(mobile)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto mobile;
    }

    if (!is_valid_mobile_number(mobile)) 
    {
        printf("Enter 10 numbers only.. Enter again\n");
        goto mobile;
    }
    
    char street[20],city[20],state[20],pin[20];
    printf("Enter the address : \n");
    
    
    street:
    printf("Street :");
    fgets(street,20,stdin);
    street[strcspn(street, "\n")] = '\0';
    if(strlen(street)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto street;
    }
    if (!is_all_alphabetic(street)) 
    {
        printf("Enter alphabets only.. Enter again\n");
        goto street;
    }
    for (int i = 0; i < strlen(street); i++) 
    {
        street[i] = tolower(street[i]);
    }

    city:
    printf("City :");
    fgets(city,20,stdin);
    city[strcspn(city, "\n")] = '\0';
    if(strlen(city)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto city;
    }
    if (!is_all_alphabetic(city)) 
    {
        printf("Enter alphabets only.. Enter again\n");
        goto city;
    }
    for (int i = 0; i < strlen(city); i++) 
    {
        city[i] = tolower(city[i]);
    }

    state:
    printf("State :");
    fgets(state,20,stdin);
    state[strcspn(state, "\n")] = '\0';
    if(strlen(state)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto state;
    }
    if (!is_all_alphabetic(state)) 
    {
        printf("Enter alphabets only.. Enter again\n");
        goto state;
    }
    for (int i = 0; i < strlen(state); i++) 
    {
        state[i] = tolower(state[i]);
    }

    pin:
    printf("Pincode :");
    fgets(pin,20,stdin);
    pin[strcspn(pin, "\n")] = '\0';
    if(strlen(pin)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto pin;
    }
    if (!is_valid_pincode(pin)) 
    {
        printf("Enter 6 numbers only.. Enter again\n");
        goto pin;
    }

    strcat(address,street);
    strcat(address," ");
    strcat(address,city);
    strcat(address," ");
    strcat(address,state);
    strcat(address," ");
    strcat(address,pin);
	
    strcpy(new_student->ID,ID);
	strcpy(new_student->name,name);
    strcpy(new_student->mobile,mobile);
	strcpy(new_student->address,address);
	new_student->borrowed=0;
    new_student->fine=0;
	new_student->next=NULL;
		
	if(student_head==NULL)
	{
		student_head=new_student;
		student_tail=new_student;
	}
	else
	{
		student_tail->next=new_student;
		student_tail=new_student;
	}
	student_count++;
    printf("\nStudent added successfully\n\n");
	save_student("student.json");
}
}



void search_student()
{
	char id[20];
    
	if(student_head == NULL) 
	{
		printf("No students to search.\n");
    } 
    else 
    {
		printf("Enter the Student ID you want to search: ");
		getchar();
        id:
        fgets(id,20,stdin);
        id[strcspn(id, "\n")] = '\0';
        if(strlen(id)==0)
        {
            printf("You entered nothing. Enter again\n");
            goto id;
        }
        
        student_temp = student_head;
		int found = 0;
		while (student_temp != NULL) 
		{
			if(strcasecmp(student_temp->ID,id)==0)
			{
				printf("\nStudent found.\n\n");
				printf("ID: %s\nName: %s\nMobile: %s\nAddress: %s\nBorrowed: %d\n\n",student_temp->ID, student_temp->name, student_temp->mobile, student_temp->address,student_temp->borrowed);
				
				found=1;
				break;	
			}
			student_temp = student_temp->next;
		}
		
		if(found!=1)
		{
			printf("Student with the id %s not found.\n",id);
		}
			
	}
}



void delete_student() 
{
	char id[20];
    
	if(student_head == NULL) 
	{
		printf("No student to delete.\n");
    } 
    else 
    {
		printf("Enter the student ID you want to delete: ");
		getchar();
        id:
        fgets(id,20,stdin);
        id[strcspn(id, "\n")] = '\0';
        if(strlen(id)==0)
        {
            printf("You entered nothing. Enter again\n");
            goto id;
        }
        
        student_temp = student_head;
        struct student_node *student_prev = NULL;
		int found = 0;
        
		while (student_temp != NULL) 
		{
			if(strcasecmp(student_temp->ID ,id) ==0)
			{
                if(student_temp->borrowed == 0)
                {    
                    if(student_prev == NULL) 
                    {
                        student_head = student_temp->next;
                    } 
                    else 
                    {
                        student_prev->next =student_temp->next;
                    }
                    if (student_temp == student_tail) 
                    {
                        student_tail = student_prev;
                    }
                    free(student_temp);
                    found = 1;
                    printf("Student deleted.\n");
                    student_count--;
                    break;
                }
                else
                {
                    found = 1;
                    printf("Can't delete student...Student has borrowed books\n\n");
                    break;
                }
            }
            student_prev = student_temp;
            student_temp = student_temp->next;
        }
        
        if (!found)
		{
			printf("student with ID %s not found.\n", id);
		}
        save_student("student.json");
	}
}


void update_student()
{
    char id[20];
    
	if(student_head == NULL) 
	{
		printf("No students to Update.\n");
    } 
    else 
    {
		printf("Enter the Student ID you want to Update: ");
		getchar();
        id:
        fgets(id,20,stdin);
        id[strcspn(id, "\n")] = '\0';
        if(strlen(id)==0)
        {
            printf("You entered nothing. Enter again\n");
            goto id;
        }
        
        student_temp = student_head;
		int found = 0;
        
		while (student_temp != NULL) 
		{
            if(strcasecmp(student_temp->ID,id)==0)
			{
                
                name_stu:
                printf("Student name : ");
                fgets(student_temp->name,20,stdin);
                student_temp->name[strcspn(student_temp->name, "\n")] = '\0';
                if(strlen(student_temp->name)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto name_stu;
                }

                if (!is_all_alphabetic(student_temp->name)) 
                {
                    printf("Enter alphabetics only.. Enter again\n");
                    goto name_stu;
                }
                for (int i = 0; i < strlen(student_temp->name); i++) 
                {
                    student_temp->name[i] = tolower(student_temp->name[i]);
                }
                

                mobile:
                printf("Mobile number: ");
                fgets(student_temp->mobile,20,stdin);
                student_temp->mobile[strcspn(student_temp->mobile, "\n")] = '\0';
                if(strlen(student_temp->mobile)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto mobile;
                }

                if (!is_valid_mobile_number(student_temp->mobile)) 
                {
                    printf("Enter 10 numbers only.. Enter again\n");
                    goto mobile;
                }

               char street[20],city[20],state[20],pin[20];
                printf("Enter the address : \n");
                
                
                street:
                printf("Street :");
                fgets(street,20,stdin);
                street[strcspn(street, "\n")] = '\0';
                if(strlen(street)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto street;
                }
                if (!is_all_alphabetic(street)) 
                {
                    printf("Enter alphabets only.. Enter again\n");
                    goto street;
                }
                for (int i = 0; i < strlen(street); i++) 
                {
                    street[i] = tolower(street[i]);
                }

                city:
                printf("City :");
                fgets(city,20,stdin);
                city[strcspn(city, "\n")] = '\0';
                if(strlen(city)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto city;
                }
                if (!is_all_alphabetic(city)) 
                {
                    printf("Enter alphabets only.. Enter again\n");
                    goto city;
                }
                for (int i = 0; i < strlen(city); i++) 
                {
                    city[i] = tolower(city[i]);
                }

                state:
                printf("State :");
                fgets(state,20,stdin);
                state[strcspn(state, "\n")] = '\0';
                if(strlen(state)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto state;
                }
                if (!is_all_alphabetic(state)) 
                {
                    printf("Enter alphabets only.. Enter again\n");
                    goto state;
                }
                for (int i = 0; i < strlen(state); i++) 
                {
                    state[i] = tolower(state[i]);
                }
                pin:
                printf("Pincode :");
                fgets(pin,20,stdin);
                pin[strcspn(pin, "\n")] = '\0';
                if(strlen(pin)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto pin;
                }
                if (!is_valid_pincode(pin)) 
                {
                    printf("Enter 6 numbers only.. Enter again\n");
                    goto pin;
                }
                strcpy(student_temp->address,"\0");
                strcat(student_temp->address,street);
                strcat(student_temp->address," ");
                strcat(student_temp->address,city);
                strcat(student_temp->address," ");
                strcat(student_temp->address,state);
                strcat(student_temp->address," ");
                strcat(student_temp->address,pin);
                
                

				found=1;
				break;	
			}
			student_temp = student_temp->next;
		}
		
		if(found!=1)
		{
			printf("student with the id %s not found.\n",id);
		}
        save_student("student.json");
			
	}
}


void count_student()
{
	printf("The total number of students in Library : %d\n",student_count);
}


void free_memory()
{
    
    struct book_node *temp_book;
    while (book_head != NULL)
    {
        temp_book = book_head;
        book_head = book_head->next;
        free(temp_book);
    }
    book_head = NULL;

   
    struct student_node *temp_student;
    while (student_head != NULL)
    {
        temp_student = student_head;
        student_head = student_head->next;

        
        struct book_node *temp_borrowed;
        while (temp_student->borrowed_books != NULL)
        {
            temp_borrowed = temp_student->borrowed_books;
            temp_student->borrowed_books = temp_student->borrowed_books->next;
            free(temp_borrowed);
        }

       
        free(temp_student);
    }
    student_head = NULL;


    struct borrowed_books *temp2;
    while (borrowed_head != NULL)
    {
        temp2= borrowed_head;
        borrowed_head = borrowed_head->next;
        free(temp2);
    }
    borrowed_head = NULL;

    struct employee_node *temp_employee;
    while (employee_head != NULL)
    {
        temp_employee = employee_head;
        employee_head = employee_head->next;
        free(temp_employee);
    }
    employee_head = NULL;

}




struct book_node* find_book_by_id(char* id) 
{
    struct book_node* temp = book_head;
    struct book_node* temp_temp = book_head;
    while (temp_temp != NULL) 
    {
        if (strcasecmp(temp_temp->ID , id)==0) 
        {
           
           while(temp!=NULL)
           {if(strcasecmp(temp->name,temp_temp->name)==0 && strcasecmp(temp->author,temp_temp->author)==0 && strcasecmp(temp->edition,temp_temp->edition)==0 &&
            strcasecmp(temp->department,temp_temp->department)==0 && strcasecmp(temp->language,temp_temp->language)==0)
            {
                temp->count--;
            } 
            temp=temp->next;
           }
            return temp_temp;
        }
        temp_temp = temp_temp->next;
    }
    return NULL; 
}

struct student_node* find_student_by_id(char* id) 
{
    struct student_node* temp = student_head;
    while (temp != NULL) 
    {
        if (strcasecmp(temp->ID , id)==0) 
        {
            printf("\nStudent found.\n\n");
			printf("ID: %s\nName: %s\nMobile: %s\nAddress: %s\nBorrowed: %d\n\n",
            temp->ID, temp->name, temp->mobile, temp->address,temp->borrowed);
            return temp;
        }
        temp = temp->next;
    }
    return NULL; 
}




void borrow_book(struct student_node *student) 
{
    if (student == NULL) {
        printf("Student not found.\n");
        return;
    }

    if (student->borrowed >= MAX_BOOKS_PER_STUDENT) {
        printf("Student has already borrowed the maximum number of books.\n");
        return;
    }
    int fine_count=0;
    struct borrowed_books *var = borrowed_head;
    while(var!=NULL)
    {
        if(strcasecmp(student->ID,var->stu_ID)==0 && var->is_returned==1)
        {
            if(var->fine != 0)
            {
                fine_count++;
            }
            else
            {
                break;
            }
            
        }
        var=var->next;
    }
    var = borrowed_head;
    while(var!=NULL)
    {
        if(strcasecmp(student->ID,var->stu_ID)==0)
        {
            if(fine_count==5)
            {
                printf("\n-------------------------------------------------\n");
                printf("\t\tWARNING\n");
                printf("You paid fine continuously 5 times..\nSo you can't borrow book");
                printf("\n-------------------------------------------------\n");
                return;
            }
            if(fine_count==3)
            {
                printf("\n-------------------------------------------------\n");
                printf("\t\tWARNING\n");
                printf("You paid fine continuously 3 times(Only 2 left)..\nTry to avoid this..\nOr else future days you can't borrow books\n");
                printf("\n-------------------------------------------------\n\n");
                
            }
            if(fine_count==4)
            {
                printf("\n-------------------------------------------------\n");
                printf("\t\t LAST WARNING\n");
                printf("You paid fine continuously 4 times(only 1 left)..\nTry to avoid this..\nOr else future days you can't borrow books\n");
                printf("\n-------------------------------------------------\n\n");
            
            }
            break;
        }
        var=var->next;
    }

    printf("Enter the book ID: ");
    char book_id[50];
    getchar();
    id:
    fgets(book_id,50,stdin);
    book_id[strcspn(book_id, "\n")] = '\0';
    if(strlen(book_id)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto id;
    }

    struct book_node *book = book_head;
	struct book_node *prev_book = NULL;
	struct book_node *borrow_book = NULL;
    

    struct book_node* temp = book_head;
    struct book_node* temp_temp = book_head;
   
    int ch;


	while(book != NULL) 
	{
		if(strcasecmp(book->ID, book_id) == 0) 
		{
            printf("\nBook found.\n\n");
            printf("ID: %s\nName: %s\nAuthor: %s\nEdition: %s\nDepartment: %s\nLanguage: %s\nCount: %d\n\n",
            book->ID, book->name, book->author, book->edition,book->department,book->language,book->count);
                    
            printf("For conformation press - 1 or exit press - 0\n");
            again:
            if (scanf("%d", &ch) != 1) 
            {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n') {}
                goto again;

            }
            if(ch==1)
            {
                while (temp_temp != NULL) 
                {
                    if (strcasecmp(temp_temp->ID , book_id)==0) 
                    {
                        
                        
                            while(temp!=NULL)
                            {
                                if(strcasecmp(temp->name,temp_temp->name)==0 && strcasecmp(temp->author,temp_temp->author)==0 && strcasecmp(temp->edition,temp_temp->edition)==0 &&
                                strcasecmp(temp->department,temp_temp->department)==0 && strcasecmp(temp->language,temp_temp->language)==0)
                                {
                                    temp->count--;
                                    temp->borrowed++;
                                } 
                                temp=temp->next;
                            }
                        
                    
                    }
                    temp_temp = temp_temp->next;
                }


                
                    if (prev_book == NULL) 
                    {
                        book_head = book->next;
                    } 
                    else 
                    {
                        prev_book->next = book->next;
                    }
                    if(book == book_tail) 
                    {
                        book_tail = prev_book;
                    }
                    borrow_book = book;
                    break;
            }
            else if(ch==0)
            {
                printf("Cancelled...\n ");
                return;
            }
            else
            {
                printf("Enter 1 or 0....enter again\n");
                goto again;
            }
            

        }
        prev_book = book;
        book = book->next;
    }
        
    if(borrow_book == NULL) 
    {
        printf("Book with ID %s not found.\n", book_id);
        return;
    }
    borrow_book->next = student->borrowed_books;
    student->borrowed_books = borrow_book;
    student->borrowed++;

    printf("ID : %s\nName : %s\nAuthor : %s\nEdition :%s\nDepartment : %s\nLanguage : %s\nCount : %d\n\n",student->borrowed_books->ID,student->borrowed_books->name,
    student->borrowed_books->author,student->borrowed_books->edition,student->borrowed_books->department,student->borrowed_books->language,
    student->borrowed_books->count);
    


    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char borrowed_date[20];
    strftime(borrowed_date, sizeof(borrowed_date), "%Y-%m-%d", &tm);
    printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    printf("\nBorrowed date:%s\n",borrowed_date);

    tm.tm_mday += 20;
    mktime(&tm); 

    char return_date[20];
    strftime(return_date, sizeof(return_date), "%Y-%m-%d", &tm);
    
    printf("Return date: %s\n", return_date);
     printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");


    struct borrowed_books *borrowed_new = (struct borrowed_books *)malloc(sizeof(struct borrowed_books));
    if (borrowed_new == NULL) 
    {
        printf("Memory allocation failed.\n");
        return;
    }
    strcpy(borrowed_new->stu_ID,student->ID);
    strcpy(borrowed_new->ID, student->borrowed_books->ID);
    strcpy(borrowed_new->name, student->borrowed_books->name);
    strcpy(borrowed_new->author, student->borrowed_books->author);
    strcpy(borrowed_new->edition, student->borrowed_books->edition);
    strcpy(borrowed_new->department, student->borrowed_books->department);
    strcpy(borrowed_new->language, student->borrowed_books->language);
    borrowed_new->count = 1;
    strcpy(borrowed_new->borrowed_date, borrowed_date);
    strcpy(borrowed_new->returned_date, return_date);
    strcpy(borrowed_new->issuer,employee);
    strcpy(borrowed_new->reciever,"-");
    borrowed_new->is_returned = 0;
    borrowed_new->fine = 0;

    borrowed_new->next=NULL;
		
	if(borrowed_head==NULL)
	{
		borrowed_head=borrowed_new;
		borrowed_tail=borrowed_new;
	}
	else
	{
		borrowed_tail->next=borrowed_new;
		borrowed_tail=borrowed_new;
	}
	

    
    printf("Book borrowed successfully.\n");
    book_count--;
    save_book("book.json");
    save_student("student.json");
    save_borrowed("borrowed_books.json");
    
    
}



time_t convert_date(char* date)
{
    
    struct tm tm = {0}; 
    //char *strptime(char *date,tm tm);
    if (strptime(date, "%Y-%m-%d", &tm) == 0) {
        
        return -1; 
    }

    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    return mktime(&tm);

}


void return_book(struct student_node *student) 
{
    if (student == NULL) {
        printf("Student not found.\n");
        return;
    }
printf("\n------------------------------------------------------------------------------------------------------------------------------------");
printf("\n\t\t\t\t\tBorrowed History\n\n");
    struct borrowed_books *var = borrowed_head;
    while(var!=NULL)
    {
        if(strcasecmp(student->ID,var->stu_ID)==0)
        {
            printf("\n-----------------------------\n");
            printf("Book_ID : %s\nBook_name : %s\nBorrowed_date : %s\nReturn_date : %s\nIssuer : %s\nReciever : %s\nFine : %d\nIs_returned : %d\n",
            var->ID,var->name,var->borrowed_date,var->returned_date,var->issuer,var->reciever,var->fine,var->is_returned);
            printf("\n------------------------------\n");
        }
        var=var->next;
    }
printf("\n------------------------------------------------------------------------------------------------------------------------------------");


    printf("\n\nEnter the book ID to return: ");
    char book_id[50];
    getchar();
    id:  
    fgets(book_id, sizeof(book_id), stdin);
    book_id[strcspn(book_id, "\n")] = '\0';
    if(strlen(book_id)==0)
    {
        printf("You entered nothing. Enter again\n");
        goto id;
    }


    


    struct borrowed_books *prev_borrowed = NULL;
    struct borrowed_books *borrowed = borrowed_head;

    while (borrowed != NULL) 
    {
        if (strcasecmp(borrowed->ID, book_id) == 0 && borrowed->is_returned==0) 
        {
            
            printf("\nReturning Book...\n");
            printf("ID: %s\nName: %s\nAuthor: %s\nEdition: %s\nDepartment: %s\nLanguage: %s\nBorrowed Date: %s\n",
                   borrowed->ID, borrowed->name, borrowed->author, borrowed->edition, borrowed->department, borrowed->language, borrowed->borrowed_date);
            
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            strftime(borrowed->returned_date, sizeof(borrowed->returned_date), "%Y-%m-%d", &tm);
            printf("Returned date :%s\n\n", borrowed->returned_date);
        
            time_t date1 = convert_date(borrowed->borrowed_date);
            time_t date2 = convert_date(borrowed->returned_date);

            double sec_diff = difftime(date2, date1);
            int days_diff = sec_diff / (60 * 60 * 24);
            days_diff-=20;
        
            if (days_diff > 0) 
            {
                int fine = days_diff * FINE_PER_DAY;
                printf("Book returned %d days late. Fine: %d\n", days_diff, fine);
        
                student->fine += fine;
                printf("Do you want to pay the fine? (1 for Yes, 0 for No): ");
                int pay_fine;
                while (scanf("%d", &pay_fine) != 1 || (pay_fine != 0 && pay_fine != 1)) {
                    printf("Invalid input. Enter 1 for Yes or 0 for No: ");
                    while (getchar() != '\n') {}  
                }

                if (pay_fine == 0) {
                    printf("Returning book cancelled. Fine not paid.\n");
                    save_student("student.json");
                    return;
                }
            }

            
            struct book_node *new = (struct book_node*)malloc(sizeof(struct book_node));
            if (new == NULL) {
                printf("Memory allocation failed.\n");
                return;}
          
        
            memset(new, 0, sizeof(struct book_node));
	
            strcpy(new->ID, borrowed->ID);
            strcpy(new->name, borrowed->name);
            strcpy(new->author, borrowed->author);
            strcpy(new->edition, borrowed->edition);
            strcpy(new->department, borrowed->department);
            strcpy(new->language, borrowed->language);
            new->count = 1;
            new->next = book_head;
            book_head = new;

            struct book_node *temp = book_head;
            while(temp!=NULL)
            {
                if(strcasecmp(temp->name,new->name)==0 && strcasecmp(temp->author,new->author)==0 && strcasecmp(temp->edition,new->edition)==0 &&
                strcasecmp(temp->department,new->department)==0 && strcasecmp(temp->language,new->language)==0)
                {
                    temp->count++;
                    temp->borrowed--;
                 }
                temp=temp->next;
            }

            struct book_node *var = book_head;
                temp=book_head;     
                while(var!=NULL)
                {
                    if(strcasecmp(borrowed->ID , var->ID) == 0)
			        {   var->count=0;
                        while(temp!=NULL)
                        {
                            if(strcasecmp(temp->name,var->name)==0 && strcasecmp(temp->author,var->author)==0 && strcasecmp(temp->edition,var->edition)==0 &&
                                strcasecmp(temp->department,var->department)==0 && strcasecmp(temp->language,var->language)==0)
                            {
                                var->count++;
                                var->borrowed=temp->borrowed;
                            } 
                            temp=temp->next;
                        }
                

                    }
                var=var->next;
                
                }


    
            // if (prev_borrowed == NULL) 
            // {
            //     borrowed_head = borrowed->next;
            // } 
            // else 
            // {
            //     prev_borrowed->next = borrowed->next;
            // }

            // if (borrowed_head == NULL) {
            //     printf("All books have been returned.\n");
            // }
            
            // free(borrowed);

            strcpy(borrowed->reciever,employee);
            borrowed->is_returned=1;
            borrowed->fine=student->fine;
            student->borrowed--;
            student->fine=0;
            save_borrowed("borrowed_books.json");
            save_book("book.json");
            save_student("student.json");

            printf("Book returned successfully.\n");
            return;
        }
        prev_borrowed = borrowed;
        borrowed = borrowed->next;
    }

    printf("Book with ID %s not found in borrowed list.\n", book_id);
}



void borrowed_details()
{
    if(borrowed_head==NULL)
    {
        printf("\nBorrowed book list is empty...\n\n");
        return;
    }
    
    struct borrowed_books *temp = borrowed_head;
    while(temp!=NULL)
    {
        printf("\n--------------------------------\n");
        printf("\nStudent_ID : %s\nBook_ID : %s\nBook_name : %s\nEdition : %s\nDepartmet : %s\nLanguage : %s\nBorrowed_Date : %s\nReturn_date : %s\n\n",
            temp->stu_ID,temp->ID,temp->name,temp->edition,temp->department,temp->language,temp->borrowed_date,temp->returned_date);
        printf("\n---------------------------------\n");
        temp=temp->next;
    }
}





void main()
{
    load_books_from_json("book.json");
    load_students_from_json("student.json");
    load_books_from_borrowed("borrowed_books.json");
    
    char language[50];
    char book_name[50],author_name[50];
    char department[50];
    int choice_main,choice,ch;
    char password[20];
    char employee_ID[20];
    int c,found=0;
    struct student_node *student;
    char student_id[20];
    

    do
    { 
        label:
        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
        printf("\t\t\t\t\t\t\tBOOK BAR LIBRARY\n");
        printf("\n\t\tLibrary Management\n\t\t------------------\n");
        printf("\t\t1. Management\n");
        printf("\t\t2. Search Books\n");
        printf("\t\t0. Exit\n\n");
        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
        printf("Enter your choice :");
        if (scanf("%d", &choice_main) != 1) 
		{
            printf("Invalid choice. Enter valid.\n");
            while (getchar() != '\n') {}
            continue;
        }
        while (getchar() != '\n'); 

        switch(choice_main)
        {
            case 1:
                load_employee_from_json("employee.json");
                this:
                printf("Employee ID : ");
                fgets(employee_ID,sizeof(employee_ID),stdin);
                employee_ID[strcspn(employee_ID,"\n")]='\0';
                if(strlen(employee_ID)==0)
                {
                    printf("You entered nothing. Enter again\n");
                    goto this;
                }
                struct employee_node *temp=employee_head;
                while(temp!=NULL)
                {
                    

                    if(strcasecmp(temp->ID,employee_ID)==0)
                    {
                        found = 1;
                        that:
                        printf("Password : ");
                        fgets(password,sizeof(password),stdin);
                        password[strcspn(password,"\n")]='\0';
                        if(strlen(password)==0)
                        {
                            printf("You entered nothing. Enter again\n");
                            goto that;
                        }
                        if(strcmp(temp->password,password)==0)
                        {
                            printf("Correct password.You logged in...\n");
                            strcpy(employee,temp->name);

                    do
                    {
                        printf("_________________________________________________________________________________________________________________________________________\n");
                        printf("\n\t\t1. Books Management\n");
                        printf("\t\t2. Students Management\n");
                        printf("\t\t3. Employees and Password management\n");
                        printf("\t\t4. Book issues\n");
                        printf("\t\t0. Exit\n\n");
                        printf("_________________________________________________________________________________________________________________________________________\n\n");
                        printf("Enter your choice : ");
                        if (scanf("%d", &choice) != 1) 
                        {
                            printf("Invalid choice. Enter valid.\n");
                            while (getchar() != '\n') {}
                            continue;
                        }
                        while (getchar() != '\n'); 

                        switch(choice)
                        {
                            case 1:
                                do
                                {
                                    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                    printf("\n\t\t1. Add Books\n");
                                    printf("\t\t2. Display Books\n");
                                    printf("\t\t3. Update Books\n");
                                    printf("\t\t4. Delete Books\n");
                                    printf("\t\t5. Count of Books\n");
                                    printf("\t\t0. Exit\n");
                                    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                    printf("Enter your choice : ");
                                    if (scanf("%d", &choice) != 1) 
                                    {
                                        printf("Invalid choice. Enter valid..\n");
                                        while (getchar() != '\n') {}
                                        continue;
                                    }
                                    while (getchar() != '\n');


                                    switch(choice)
                                    {
                                        case 1:
                                            add_book();
                                            break;

                                        case 2:
                                            do
                                            {
                                                printf("\n--------------------------------------------------------------------------------\n");
                                                printf("\n_____________Choose Department_____________\n\n");
                                                printf("\n\t\t1. Computer Science\n");
                                                printf("\t\t2. Electronics and Communication\n");
                                                printf("\t\t3. Mechanical\n");
                                                printf("\t\t4. Civil\n");
                                                printf("\t\t0. Exit\n");
                                                printf("\n--------------------------------------------------------------------------------\n\n");
                                                printf("Enter your choice : ");
                                                if (scanf("%d", &choice) != 1) 
                                                {
                                                    printf("Invalid choice. Enter valid..\n");
                                                    while (getchar() != '\n') {}
                                                    continue;
                                                }
                                                while (getchar() != '\n');
                                                switch(choice)
                                                {
                                                    case 1:
                                                        do
                                                        {
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                                            printf("\n\t\t1. Display all books\n");
                                                            printf("\t\t2. Display books by alphabets\n");
                                                            printf("\t\t3. By Book Name\n");
                                                            printf("\t\t4. By Author Name\n");
                                                            printf("\t\t0. Exit\n");
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                                            printf("Enter your choice : ");
                                                            if (scanf("%d", &choice) != 1) 
                                                            {
                                                                printf("Invalid choice. Enter valid..\n");
                                                                while (getchar() != '\n') {}
                                                                continue;
                                                            }
                                                            while (getchar() != '\n');

                                                            switch(choice)
                                                            {
                                                                case 1:
                                                                    strcpy(department,"computer science");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;
                                                                
                                                                case 2:
                                                                    strcpy(department,"computer science");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department_using_alphabets(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;
                                                                


                                                                case 3:
                                                                    strcpy(department,"computer science");
                                                                    printf("Enter book name : ");
                                                                    getchar();
                                                                    fgets(book_name,50,stdin);
                                                                    book_name[strcspn(book_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_book_name(department,book_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    

                                                                    
                                                                    break;

                                                                case 4:
                                                                    strcpy(department,"computer science");
                                                                    printf("Enter author name : ");
                                                                    getchar();
                                                                    fgets(author_name,50,stdin);
                                                                    author_name[strcspn(author_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_author_name(department,author_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    
                                                                    break;

                                                                case 0:
                                                                    printf("Exiting.......\n");
                                                                    break;

                                                                default:
                                                                    printf("Invalid.. Enter Valid choice\n");
                                                                    break;

                                                            }
                                                        }while(choice!=0);
                                                        break;


                                                    case 2:
                                                        do
                                                        {
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                                            printf("\n\t\t1. Display all books\n");
                                                            printf("\t\t2. Display books by alphabets\n");
                                                            printf("\t\t3. By Book Name\n");
                                                            printf("\t\t4. By Author Name\n");
                                                            printf("\t\t0. Exit\n");
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                                            printf("Enter your choice : ");
                                                            if (scanf("%d", &choice) != 1) 
                                                            {
                                                                printf("Invalid choice. Enter valid..\n");
                                                                while (getchar() != '\n') {}
                                                                continue;
                                                            }
                                                            while (getchar() != '\n');

                                                            switch(choice)
                                                            {
                                                                case 1:
                                                                    strcpy(department,"electronics and communication");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    

                                                                    break;

                                                                case 2:
                                                                    strcpy(department,"electronics and communication");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department_using_alphabets(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;

                                                                case 3:
                                                                    strcpy(department,"electronics and communication");
                                                                    printf("Enter book name : ");
                                                                    getchar();
                                                                    fgets(book_name,50,stdin);
                                                                    book_name[strcspn(book_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_book_name(department,book_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    

                                                                    break;

                                                                case 4:
                                                                    strcpy(department,"electronics and communication");
                                                                    printf("Enter author name : ");
                                                                    getchar();
                                                                    fgets(author_name,50,stdin);
                                                                    author_name[strcspn(author_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_author_name(department,author_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    
                                                                    break;

                                                                case 0:
                                                                    printf("Exiting.......\n");
                                                                    break;

                                                                default:
                                                                    printf("Invalid.. Enter Valid choice\n");
                                                                    break;

                                                            }
                                                        }while(choice!=0);
                                                        break;

                                                    case 3:
                                                        do
                                                        {
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                                            printf("\n\t\t1. Display all books\n");
                                                            printf("\t\t2. Display books by alphabets\n");
                                                            printf("\t\t3. By Book Name\n");
                                                            printf("\t\t4. By Author Name\n");
                                                            printf("\t\t0. Exit\n");
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                                            printf("Enter your choice : ");
                                                            if (scanf("%d", &choice) != 1) 
                                                            {
                                                                printf("Invalid choice. Enter valid..\n");
                                                                while (getchar() != '\n') {}
                                                                continue;
                                                            }
                                                            while (getchar() != '\n');

                                                            switch(choice)
                                                            {
                                                                case 1:
                                                                    strcpy(department,"mechanical");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    

                                                                    break;

                                                                case 2:
                                                                    strcpy(department,"mechanical");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department_using_alphabets(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;


                                                                case 3:
                                                                    strcpy(department,"mechanical");
                                                                    printf("Enter book name : ");
                                                                    getchar();
                                                                    fgets(book_name,50,stdin);
                                                                    book_name[strcspn(book_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_book_name(department,book_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    

                                                                    break;

                                                                case 4:
                                                                    strcpy(department,"mechanical");
                                                                    printf("Enter author name : ");
                                                                    getchar();
                                                                    fgets(author_name,50,stdin);
                                                                    author_name[strcspn(author_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_author_name(department,author_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    
                                                                    break;

                                                                case 0:
                                                                    printf("Exiting.......\n");
                                                                    break;

                                                                default:
                                                                    printf("Invalid.. Enter Valid choice\n");
                                                                    break;

                                                            }
                                                        }while(choice!=0);
                                                        break;

                                                    case 4:
                                                        do
                                                        {
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                                            printf("\n\t\t1. Display all books\n");
                                                            printf("\t\t2. Display books by alphabets\n");
                                                            printf("\t\t3. By Book Name\n");
                                                            printf("\t\t4. By Author Name\n");
                                                            printf("\t\t0. Exit\n");
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                                            printf("Enter your choice : ");
                                                            if (scanf("%d", &choice) != 1) 
                                                            {
                                                                printf("Invalid choice. Enter valid..\n");
                                                                while (getchar() != '\n') {}
                                                                continue;
                                                            }
                                                            while (getchar() != '\n');

                                                            switch(choice)
                                                            {
                                                                case 1:
                                                                    strcpy(department,"civil");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                

                                                                    break;

                                                                case 2:
                                                                    strcpy(department,"civil");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department_using_alphabets(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;


                                                                case 3:
                                                                    strcpy(department,"civil");
                                                                    printf("Enter book name : ");
                                                                    getchar();
                                                                    fgets(book_name,50,stdin);
                                                                    book_name[strcspn(book_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_book_name(department,book_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    

                                                                    break;

                                                                case 4:
                                                                    strcpy(department,"civil");
                                                                    printf("Enter author name : ");
                                                                    getchar();
                                                                    fgets(author_name,50,stdin);
                                                                    author_name[strcspn(author_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_author_name(department,author_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    
                                                                    break;

                                                                case 0:
                                                                    printf("Exiting.......\n");
                                                                    break;

                                                                default:
                                                                    printf("Invalid.. Enter Valid choice\n");
                                                                    break;

                                                            }
                                                        }while(choice!=0);
                                                        break;

                                                    case 0:
                                                        printf("Exiting.....\n");
                                                        break;
                                                    
                                                    default :
                                                        printf("Invalid choice.. Enter valid\n");
                                                        break;
                                                        
                                                }

                                            }while(choice!=0);
                                            break;

                                        case 3:
                                            update_book();
                                            break;

                                        case 4:
                                            delete_book();
                                            break;

                                        case 5:
                                            count_book();
                                            break;

                                        case 0:
                                            printf("Exiting......\n");
                                            break;

                                        default:
                                            printf("Invalid choice...Enter valid\n");
                                            break;
                                        

                                    }

                                }while(choice!=0);
                                break;

                            case 2:
                                do
                                {
                                    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                    printf("\n\t\t1. Add Students\n");
                                    printf("\t\t2. Search Student\n");
                                    printf("\t\t3. Update Student\n");
                                    printf("\t\t4. Delete Student\n");
                                    printf("\t\t5. Count of Students\n");
                                    printf("\t\t6. Borrowed students details\n");
                                    printf("\t\t0. Exit\n");
                                    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                    printf("Enter your choice : ");
                                    if (scanf("%d", &choice) != 1) 
                                    {
                                        printf("Invalid choice. Enter valid..\n");
                                        while (getchar() != '\n') {}
                                        continue;
                                    }
                                    while (getchar() != '\n');
                                    switch(choice)
                                    {
                                        case 1:
                                            add_student();
                                            break;

                                        case 2:
                                            search_student();
                                            break;
                                        
                                        case 3:
                                            update_student();
                                            break;

                                        case 4:
                                            delete_student();
                                            break;

                                        case 5:
                                            count_student();
                                            break;

                                        case 6:
                                            borrowed_details();
                                            break;

                                        case 0:
                                            printf("Exiting...\n");
                                            break;
                                        
                                        default :
                                            printf("Invalid choice ... Enter valid\n");
                                            break;
                                    }

                                }while(choice!=0);
                                break; 

                            case 3:

                                do
                                {
                                    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                    printf("\n\t\t1. Add Employees\n");
                                    printf("\t\t2. Change password\n");
                                    printf("\t\t0. Exit\n");
                                    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                    printf("Enter your choice : ");
                                    if (scanf("%d", &choice) != 1) 
                                    {
                                        printf("Invalid choice. Enter valid..\n");
                                        while (getchar() != '\n') {}
                                        continue;
                                    }
                                    while (getchar() != '\n'); 

                                    switch(choice)
                                    {
                                        case 1:
                                            add_employee();
                                            break;

                                        case 2:
                                            change_password();
                                            break;

                                        case 0:
                                            printf("Exiting...\n");
                                            break;

                                        default :
                                            printf("Invald choice... Enter valid..\n");
                                            break;
                                    }
                                }while(choice!=0);
                                
                                break;   

                            case 4:

                                do
                                {
                                  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                  printf("\t\t1. Borrow book\n");
                                  printf("\t\t2. Return book\n");
                                  printf("\t\t0. Exit\n");
                                  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");   
                                  printf("Enter your choice : ");
                                  if (scanf("%d", &choice) != 1) 
                                    {
                                        printf("Invalid choice. Enter valid..\n");
                                        while (getchar() != '\n') {}
                                        continue;
                                    }
                                    while (getchar() != '\n'); 

                                    switch(choice)
                                    {
                                        case 1:
                                            printf("Enter student ID :");
                                            scanf("%s",student_id);
                                            struct student_node *student = find_student_by_id(student_id);
                                            if (student != NULL) {
                                                borrow_book(student);
                                            } else {
                                                printf("Student not found.\n");
                                            }
                                            break;

                                        case 2:
                                            printf("Enter student ID :");
                                            scanf("%s",student_id);
                                            student = find_student_by_id(student_id);
                                            if (student != NULL) {
                                                return_book(student);
                                            } else {
                                                printf("Student not found.\n");
                                            }
                                            break;

                                        case 0:
                                            printf("Exiting....\n\n");
                                            break;

                                        default:
                                            printf("Invalid choice..Enter valid\n");
                                            break;
                                    }

                                }while(choice!=0);
                                break;
                            
                            case 0:
                                printf("Exiting ....\n");
                                break;

                            default:
                                printf("Invalid choice. Enter valid.\n");
                                break;

                        }

                    }while(choice!=0);
                    break;

                         
                        }
                        else
                        {
                            printf("Wrong password\n");
                            break;
                        }

                        
                    }
                    temp = temp->next;
                }
                if(found==0)
                {
                    printf("Employee not found..\n");
                    break;
                }
                
                    break; 
                

            case 2:
                do
                {
                    printf("\n--------------------------------------------------------------------------------\n");
                    printf("\n_____________Choose Department_____________\n\n");
                    printf("\n\t\t1. Computer Science\n");
                    printf("\t\t2. Electronics and Communication\n");
                    printf("\t\t3. Mechanical\n");
                    printf("\t\t4. Civil\n");
                    printf("\t\t0. Exit\n");
                    printf("\n--------------------------------------------------------------------------------\n\n");
                    printf("Enter your choice : ");
                    if (scanf("%d", &choice) != 1) 
                    {
                        printf("Invalid choice. Enter valid..\n");
                        while (getchar() != '\n') {}
                        continue;
                    }
                    while (getchar() != '\n');
                    switch(choice)
                    {
                        case 1:
                            do
                                                        {
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                                            printf("\n\t\t1. Display all books\n");
                                                            printf("\t\t2. Display books by alphabets\n");
                                                            printf("\t\t3. By Book Name\n");
                                                            printf("\t\t4. By Author Name\n");
                                                            printf("\t\t0. Exit\n");
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                                            printf("Enter your choice : ");
                                                            if (scanf("%d", &choice) != 1) 
                                                            {
                                                                printf("Invalid choice. Enter valid..\n");
                                                                while (getchar() != '\n') {}
                                                                continue;
                                                            }
                                                            while (getchar() != '\n'); 

                                                            switch(choice)
                                                            {
                                                                case 1:
                                                                    strcpy(department,"computer science");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;
                                                                
                                                                case 2:
                                                                    strcpy(department,"computer science");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department_using_alphabets(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;
                                                                


                                                                case 3:
                                                                    strcpy(department,"computer science");
                                                                    printf("Enter book name : ");
                                                                    getchar();
                                                                    fgets(book_name,50,stdin);
                                                                    book_name[strcspn(book_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_book_name(department,book_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    

                                                                    
                                                                    break;

                                                                case 4:
                                                                    strcpy(department,"computer science");
                                                                    printf("Enter author name : ");
                                                                    getchar();
                                                                    fgets(author_name,50,stdin);
                                                                    author_name[strcspn(author_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_author_name(department,author_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    
                                                                    break;

                                                                case 0:
                                                                    printf("Exiting.......\n");
                                                                    break;

                                                                default:
                                                                    printf("Invalid.. Enter Valid choice\n");
                                                                    break;

                                                            }
                                                        }while(choice!=0);
                            
                        case 2:
                            do
                                                        {
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                                            printf("\n\t\t1. Display all books\n");
                                                            printf("\t\t2. Display books by alphabets\n");
                                                            printf("\t\t3. By Book Name\n");
                                                            printf("\t\t4. By Author Name\n");
                                                            printf("\t\t0. Exit\n");
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                                            printf("Enter your choice : ");
                                                            if (scanf("%d", &choice) != 1) 
                                                            {
                                                                printf("Invalid choice. Enter valid..\n");
                                                                while (getchar() != '\n') {}
                                                                continue;
                                                            }
                                                            while (getchar() != '\n'); 

                                                            switch(choice)
                                                            {
                                                                case 1:
                                                                    strcpy(department,"electronics and communication");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    

                                                                    break;

                                                                case 2:
                                                                    strcpy(department,"electronics and communication");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department_using_alphabets(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;

                                                                case 3:
                                                                    strcpy(department,"electronics and communication");
                                                                    printf("Enter book name : ");
                                                                    getchar();
                                                                    fgets(book_name,50,stdin);
                                                                    book_name[strcspn(book_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_book_name(department,book_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    

                                                                    break;

                                                                case 4:
                                                                    strcpy(department,"electronics and communication");
                                                                    printf("Enter author name : ");
                                                                    getchar();
                                                                    fgets(author_name,50,stdin);
                                                                    author_name[strcspn(author_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_author_name(department,author_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    
                                                                    break;

                                                                case 0:
                                                                    printf("Exiting.......\n");
                                                                    break;

                                                                default:
                                                                    printf("Invalid.. Enter Valid choice\n");
                                                                    break;

                                                            }
                                                        }while(choice!=0);
                            
                        case 3:
                            do
                                                        {
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                                            printf("\n\t\t1. Display all books\n");
                                                            printf("\t\t2. Display books by alphabets\n");
                                                            printf("\t\t3. By Book Name\n");
                                                            printf("\t\t4. By Author Name\n");
                                                            printf("\t\t0. Exit\n");
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                                            printf("Enter your choice : ");
                                                            if (scanf("%d", &choice) != 1) 
                                                            {
                                                                printf("Invalid choice. Enter valid..\n");
                                                                while (getchar() != '\n') {}
                                                                continue;
                                                            }
                                                            while (getchar() != '\n'); 

                                                            switch(choice)
                                                            {
                                                                case 1:
                                                                    strcpy(department,"mechanical");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    

                                                                    break;

                                                                case 2:
                                                                    strcpy(department,"mechanical");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department_using_alphabets(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;


                                                                case 3:
                                                                    strcpy(department,"mechanical");
                                                                    printf("Enter book name : ");
                                                                    getchar();
                                                                    fgets(book_name,50,stdin);
                                                                    book_name[strcspn(book_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_book_name(department,book_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    

                                                                    break;

                                                                case 4:
                                                                    strcpy(department,"mechanical");
                                                                    printf("Enter author name : ");
                                                                    getchar();
                                                                    fgets(author_name,50,stdin);
                                                                    author_name[strcspn(author_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_author_name(department,author_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    
                                                                    break;

                                                                case 0:
                                                                    printf("Exiting.......\n");
                                                                    break;

                                                                default:
                                                                    printf("Invalid.. Enter Valid choice\n");
                                                                    break;

                                                            }
                                                        }while(choice!=0);
                            
                        case 4:
                            do
                                                        {
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                                                            printf("\n\t\t1. Display all books\n");
                                                            printf("\t\t2. Display books by alphabets\n");
                                                            printf("\t\t3. By Book Name\n");
                                                            printf("\t\t4. By Author Name\n");
                                                            printf("\t\t0. Exit\n");
                                                            printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                                                            printf("Enter your choice : ");
                                                            if (scanf("%d", &choice) != 1) 
                                                            {
                                                                printf("Invalid choice. Enter valid..\n");
                                                                while (getchar() != '\n') {}
                                                                continue;
                                                            }
                                                            while (getchar() != '\n'); 

                                                            switch(choice)
                                                            {
                                                                case 1:
                                                                    strcpy(department,"civil");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                

                                                                    break;

                                                                case 2:
                                                                    strcpy(department,"civil");
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_by_department_using_alphabets(department);
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    
                                                                    break;


                                                                case 3:
                                                                    strcpy(department,"civil");
                                                                    printf("Enter book name : ");
                                                                    getchar();
                                                                    fgets(book_name,50,stdin);
                                                                    book_name[strcspn(book_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_book_name(department,book_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    

                                                                    break;

                                                                case 4:
                                                                    strcpy(department,"civil");
                                                                    printf("Enter author name : ");
                                                                    getchar();
                                                                    fgets(author_name,50,stdin);
                                                                    author_name[strcspn(author_name, "\n")] = '\0';
                                                                    printf("-----------------------------------------------------------------------------------\n");
                                                                    display_books_in_department_by_author_name(department,author_name) ;
                                                                    printf("-----------------------------------------------------------------------------------\n");

                                                                    
                                                                    break;

                                                                case 0:
                                                                    printf("Exiting.......\n");
                                                                    break;

                                                                default:
                                                                    printf("Invalid.. Enter Valid choice\n");
                                                                    break;

                                                            }
                                                        }while(choice!=0);
                            
                        case 0:
                            printf("Exiting.....\n");
                            break;
                        
                        default :
                            printf("Invalid choice.. Enter valid\n");
                            break;
                    }
                            
                
                }while(choice!=0);
                
                break;

            case 0:
                printf("Exiting from library management\n\n");
                free_memory();
                break;

            default :
                printf("Invalid choice ... Enter valid\n");
                break;


        }

    }while(choice_main!=0);
    
}









