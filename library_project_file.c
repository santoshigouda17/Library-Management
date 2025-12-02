#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//library book management
struct book_node
{
	char name[50];
	char author[50];
	int pages;
	float price;
	struct book_node *next;
}*book_new,*book_head=NULL,*book_tail=NULL,*book_temp,*i;

void load_book(const char *filename);
void save_book(const char *filename);

int book_count=0;

void add_book()
{
	char name[50];
	char author[50];
	int pages;
	float price;
	
	book_new=(struct book_node*)malloc(sizeof(struct book_node));
	
	printf("Enter the book name : ");
	scanf("%s",name);
	printf("Enter the author name : ");
	scanf("%s",author);
	printf("Enter the pages : ");
	scanf("%d",&pages);
	printf("Enter the price : ");
	scanf("%f",&price);
	
	strcpy(book_new->name,name);
	strcpy(book_new->author,author);
	book_new->pages=pages;
	book_new->price=price;
	
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
	save_book("book.txt");
}

void display_book()
{
	if(book_count==0)
	{
		printf("Library is empty. No books to display\n");
	} 
	else
	{
		book_temp = book_head;
		printf("\n\nDetails of all books :\n\n ");
		while(book_temp!=NULL)
		{
			printf("\nName : %s\nAuthor : %s\nPages : %d\nPrice : %.2f\n\n",book_temp->name,book_temp->author,book_temp->pages,book_temp->price);
			book_temp=book_temp->next;
		}
		printf("\n\n");
	}
}

void search1_book()
{

	if(book_count==0)
	{
		printf("Library is empty. No books to search\n");
	} 
	else
	{
		char search[50];
		printf("Enter the book name you want to search :\n");
		scanf("%s",search);
		int found=0;
		book_temp=book_head;
		for(i=book_temp;book_temp!=NULL;book_temp=book_temp->next)
		{
			if(strcmp(book_temp->name,search)==0)
			{
				printf("\nBook found\n\n");
				printf("Name: %s\n", book_temp->name);
				printf("Author: %s\n", book_temp->author);
				printf("Pages: %d\n", book_temp->pages);
				printf("Price: %.2f\n\n", book_temp->price);
				
				found=1;
				break;	
			}
			else
			{
				continue;
			}
		}
		
		if(found!=1)
		{
			printf("Book with name %s not found\n",search);
		}
			
	}
}

void search2_book()
{

	if(book_count==0)
	{
		printf("Library is empty. No books to search\n");
	} 
	else
	{
		char search[50];
		printf("Enter the author name you want to search :\n");
		scanf("%s",search);
		int found=0;
		book_temp=book_head;
		for(i=book_temp;book_temp!=NULL;book_temp=book_temp->next)
		{
			if(strcmp(book_temp->author,search)==0)
			{
				printf("\nBook found\n\n");
				printf("Name: %s\n", book_temp->name);
				printf("Author: %s\n", book_temp->author);
				printf("Pages: %d\n", book_temp->pages);
				printf("Price: %.2f\n\n", book_temp->price);
				
				found=1;
				break;	
			}
			else
			{
				continue;
			}
		}
		
		if(found!=1)
		{
			printf("Book with the author name %s not found\n",search);
		}
			
	}
}


void delete_book()
{

	if(book_count==0)
	{
		printf("Library is empty. No books to delete\n");
	} 
	else
	{
		char search[50];
		printf("Enter the book name you want to delete :\n");
		scanf("%s",search);
		book_temp = book_head;
        struct book_node *book_prev = NULL;
        int found = 0;
        
		while (book_temp != NULL) 
		{
			if (strcmp(book_temp->name, search) == 0) 
			{
				if (book_prev == NULL) 
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
                book_count--;
                printf("Book deleted.\n");
                break;
            }
            book_prev = book_temp;
            book_temp = book_temp->next;
        }
        
        if (!found) 
        {
            printf("Book with the name %s not found.\n", search);
        }
        else
        {
        	save_book("book.txt");
        }
    }
}



void count_book()
{
	printf("The total number of books in Library : %d\n",book_count);
}



//library customer management


struct customer_node
{
	int id;
	char name[50];
	float cost; 
	float fine; 
	struct customer_node *next;
	struct book_node *borrowed_book;
}*new_customer,*customer_head,*customer_tail,*customer_temp;

void add_customer() 
{
	new_customer = (struct customer_node*)malloc(sizeof(struct customer_node));
    
	printf("Enter customer ID: ");
	scanf("%d", &new_customer->id);
	printf("Enter customer name: ");
	scanf("%s", new_customer->name);
		
	new_customer->cost = 0.0;
	new_customer->fine = 0.0;
	new_customer->borrowed_book = NULL;
	new_customer->next = NULL;
    
	if (customer_head == NULL) 
	{
		customer_head = new_customer;
		customer_tail = new_customer;
	} else 
	{
		customer_tail->next = new_customer;
		customer_tail = new_customer;
	}
}

void display_customer() 
{
	if (customer_head == NULL) 
	{
		printf("No customers to display.\n");
	} 
	else 
	{
		customer_temp = customer_head;
		printf("\n\nDetails of all customers:\n\n");
		while (customer_temp != NULL) 
		{	
			printf("ID: %d\nName: %s\nTotal Cost: %.2f\nTotal Fine: %.2f\n\n",customer_temp->id, customer_temp->name, customer_temp->cost, customer_temp->fine);
            customer_temp = customer_temp->next;
        }
    }
}


void search_customer()
{
	int id;
    
	if(customer_head == NULL) 
	{
		printf("No customers to delete.\n");
    } 
    else 
    {
		printf("Enter the customer ID you want to search: ");
		scanf("%d", &id);
        
        customer_temp = customer_head;
        struct customer_node *customer_prev = NULL;
		int found = 0;
		while (customer_temp != NULL) 
		{
			if(customer_temp->id==id)
			{
				printf("\nCustomer found.\n\n");
				printf("ID: %d\nName: %s\nTotal Cost: %.2f\nTotal Fine: %.2f\n\n",customer_temp->id, customer_temp->name, customer_temp->cost, customer_temp->fine);
				
				found=1;
				break;	
			}
			customer_temp = customer_temp->next;
		}
		
		if(found!=1)
		{
			printf("Customer with the id %d not found.\n",id);
		}
			
	}
}



void delete_customer() 
{
	int id;
    
	if(customer_head == NULL) 
	{
		printf("No customers to delete.\n");
    } 
    else 
    {
		printf("Enter the customer ID you want to delete: ");
		scanf("%d", &id);
        
        customer_temp = customer_head;
        struct customer_node *customer_prev = NULL;
		int found = 0;
        
		while (customer_temp != NULL) 
		{
			if(customer_temp->id == id) 
			{
				if(customer_prev == NULL) 
				{
					customer_head = customer_temp->next;
                } 
                else 
                {
					customer_prev->next = customer_temp->next;
                }
				if (customer_temp == customer_tail) 
				{
                    customer_tail = customer_prev;
                }
                free(customer_temp);
                found = 1;
                printf("Customer deleted.\n");
                break;
            }
            customer_prev = customer_temp;
            customer_temp = customer_temp->next;
        }
        
        if (!found)
		{
			printf("Customer with ID %d not found.\n", id);
		}
	}
}



void borrow_book() 
{
    int customer_id;
    char book_name[50];
    
	if (customer_head == NULL) 
	{
		printf("No customers to borrow books.\n");
		return;
    }
    
	if(book_head == NULL) 
	{
        printf("No books available to borrow.\n");
        return;
    }
    
	printf("Enter customer ID: ");
	scanf("%d", &customer_id);
    
	struct customer_node *customer = customer_head;
	while(customer!=NULL && customer->id!=customer_id) 
	{
        customer = customer->next;
	}
    
    if(customer == NULL) 
	{
		printf("Customer with ID %d not found.\n", customer_id);
		return;
	}
    
	printf("Enter the book name you want to borrow: ");
	scanf("%s", book_name);
    
	struct book_node *book = book_head;
	struct book_node *prev_book = NULL;
	struct book_node *borrow_book = NULL;
    
	while(book != NULL) 
	{
		if(strcmp(book->name, book_name) == 0) 
		{
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
        prev_book = book;
        book = book->next;
    }
    
    if(borrow_book == NULL) 
    {
        printf("Book with name %s not found.\n", book_name);
        return;
    }
    
    borrow_book->next = customer->borrowed_book;
    customer->borrowed_book = borrow_book;
    customer->cost += borrow_book->price;
    
    printf("Book borrowed successfully.\n");
    book_count--;
    save_book("book.txt");
}

void return_book() 
{
    int customer_id;
    char book_name[50];
    
    if(customer_head == NULL) 
    {
        printf("No customers to return books.\n");
        return;
    }
    
    printf("Enter customer ID: ");
    scanf("%d", &customer_id);
    
    struct customer_node *customer = customer_head;
    while(customer != NULL && customer->id != customer_id) 
    {
        customer = customer->next;
    }
    
    if (customer == NULL) 
    {
        printf("Customer with ID %d not found.\n", customer_id);
        return;
    }
    
    printf("Enter the book name you want to return: ");
    scanf("%s", book_name);
    
    struct book_node *temp_book = customer->borrowed_book;
    struct book_node *prev_book = NULL;
    struct book_node *returned_book = NULL;
    
    while(temp_book != NULL) 
    {
		if(strcmp(temp_book->name, book_name) == 0) 
		{
			if(prev_book == NULL) 
			{
                customer->borrowed_book = temp_book->next;
            } 
            else 
            {
                prev_book->next = temp_book->next;
            }
            returned_book = temp_book;
            break;
        }
        prev_book = temp_book;
        temp_book = temp_book->next;
    }
    
    if (returned_book == NULL) 
    {
        printf("Book with name %s not found in customer's borrowed books.\n", book_name);
        return;
    }
    
    returned_book->next = book_head;
    book_head = returned_book;
    
    printf("Book returned successfully.\n");
    book_count++;
    save_book("book.txt");
}


void fine() 
{
    int day;
    float fine = 10.5; 
    
    if (customer_head == NULL) 
    {
        printf("No customers to calculate fines.\n");
    }
    
    printf("Enter the number of days delayed: ");
    scanf("%d", &day);
    
    struct customer_node *customer = customer_head;
    while(customer != NULL) 
    {
        customer->fine +=  day * fine;
        customer->cost = 0; 
        customer = customer->next;
    }
    
    printf("Fines calculated and applied.\n");
}


void load_book(const char *filename) 
{
	FILE *file = fopen(filename, "r");
	if(file == NULL) 
	{
		printf("Error opening file %s\n", filename);
		return;
	}

	char name[50];
	char author[50];
	int pages;
	float price;

	while(fscanf(file, "%49s %49s %d %f", name, author, &pages, &price)!=EOF) 
	{
		book_new=(struct book_node*)malloc(sizeof(struct book_node));
		if(book_new == NULL) 
		{
			printf("Memory allocation failed\n");
			fclose(file);
			return;
		}

		strcpy(book_new->name, name);
		strcpy(book_new->author, author);
		book_new->pages = pages;
		book_new->price = price;
		book_new->next = NULL;

		if (book_head == NULL) 
		{
			book_head = book_new;
			book_tail = book_new;
        } 
        else 
        {
			book_tail->next = book_new;
			book_tail = book_new;
		}

		book_count++;
	}

	fclose(file);
	printf("Books loaded from file successfully.\n");
}


void save_book(const char *filename) 
{
	FILE *file = fopen(filename, "w");
	if(file == NULL)
	{
		printf("Error opening file %s\n", filename);
		return;
	}

	struct book_node *current = book_head;
	while(current != NULL) 
	{
		fprintf(file, "%s %s %d %.2f\n", current->name, current->author, current->pages, current->price);
		current = current->next;
	}

	fclose(file);
}

void free_memory()
{
   
    struct book_node *temp, *next;
    temp = book_head;
    while (temp != NULL)
	{
        next = temp->next;
        free(temp);
        temp = next;
    }
    book_head = book_tail = NULL;

    
    struct customer_node *temp_customer, *next_customer;
    temp_customer = customer_head;
    while (temp_customer != NULL) 
	{
        next_customer = temp_customer->next;
        free(temp_customer);
        temp_customer = next_customer;
    }
    customer_head = customer_tail = NULL;
}





void main()
{

	int choice_main;
	int choice ;
	int c;
	int ch ;
	
	do
	{
		printf("\n\tLIBRARY MANAGEMENT\n\t~~~~~~~~~~~~~~~~~~\n");
		printf("\n1. Books Management\n");
		printf("2.Costumer Management\n");
		printf("3.Load book from file\n");
		printf("4.Exit\n");
		
		printf("Enter your choice : ");
		scanf("%d",&choice_main);
		
		switch(choice_main)
		{
			case 1:
				
				
				do
				{
					printf("\n\n1. Add book\n");
					printf("2. Display book\n");
					printf("3. Search book\n");
					printf("4. Delete book\n");
					printf("5. Count of all books\n");
					printf("6. Exit\n");
					
					printf("Enter your choice : ");
					scanf("%d",&choice);
					
					switch(choice)
					{
						case 1:
							add_book();
							break;
							
						case 2:
							display_book();
							break;
							
						case 3:
							
								printf("1. Search by book name\n");
								printf("2. Search by author name\n");
								printf("Enter your choice : ");
								scanf("%d",&c);
								
								switch(c)
								{
									case 1:
										search1_book();
										break;
										
									case 2:
										search2_book();
										break;
										
									default:
										printf("Invalid choice\n");
										break;
								}
								break;
						case 4:
							delete_book();
							break;	
						
						case 5:
							count_book();
							break;	
							
						case 6:
							printf("Exiting\n");
							break;
							
						default :
							printf("Invalid choice \n");
							break;
					}
					printf("_________________________________________________________________________________________________________________________________\n\n");
				}while(choice!=6);
				break;
		
			case 2:
				
				
				do
				{
					printf("\n\n1. Add Customer\n");
					printf("2. Display Customer\n");
					printf("3. Delete Customer\n");
					printf("4. Search Customer\n");
					printf("5. Borrow book\n");
					printf("6. Return book\n");
					printf("7. Exit\n");
					
					printf("Enter your choice : ");
					scanf("%d",&ch);
					
					switch(ch)
					{
						case 1:
							add_customer();
							break;
							
						case 2:
							display_customer();
							break;
							
						case 3:
							delete_customer();
							break;
							
						case 4:
							search_customer();
							break;							
							
						case 5:
							borrow_book();
							break;	
						
						case 6:
							return_book();
							int c;
							printf("Did you delay for return ? (Yes-1,No-0)\n ");
							scanf("%d",&c);
							if(c==1)
							{
								fine();
							}
							else
							{
								printf("Okay Done.\n");
							}
							break;	
							
						case 7:
							printf("Exiting\n");
							break;
							
						default :
							printf("Invalid choice \n");
							break;
					}
					printf("_________________________________________________________________________________________________________________________________\n\n");
				}while(ch!=7);
				break;
			
			case 3:
				load_book("book.txt");
				break;
				
			case 4:
				printf("Exiting From the program\n");
				free_memory();
				break;
				
			default :
				printf("Invalid Choice\n");
				break;
		}
	}while(choice_main!=4);
	
}
				
		
					
				
	
			












	
	
