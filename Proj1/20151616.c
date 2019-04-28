#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX 100

char* instruction[] = {"help", "dir", "quit", "history", "dump", "edit", "address", "value", "fill", "reset", "opcode mnemonic", "opcodelist", "h", "d", "q", "hi", "du", "e", "f"};

typedef struct __node
{
	char InputString[MAX];
	struct __node* link;
} NODE;

void PrintInstruction();
void PrintDir();
void PrintHistory(NODE*);
void InputHistory(NODE**, char*);
int IsFormal(int* num1, int* num2, int* addr, char* input);
//Checks if the input is in right form. If the form is not right, return 0. Depending on the form, calls the PrintMem in correct index
void PrintMem(int start, int end, int* addr);
//Prints the specific memory addr from start to end. Sets the addr to end index

int main()
{
	NODE* head = NULL;
	char input[MAX];
	int num1=0,num2=0,num3=0;
	int addr=0;;
	do
	{
		printf("sicsim> ");
		fgets(input,MAX,stdin);
		input[strlen(input)-1] = '\0';
		//changes the last index from '\n' to '\0'. Enables it to be used in strcmp and check it's end

		if(!strcmp(input,instruction[0])||!strcmp(input,instruction[12]))
		{
			InputHistory(&head, input);
			PrintInstruction();
		}
		else if(!strcmp(input,instruction[1])||!strcmp(input,instruction[13]))
		{
			InputHistory(&head, input);
			PrintDir();
		}
		else if(!strcmp(input,instruction[3])||!strcmp(input,instruction[15]))
		{
			InputHistory(&head, input);
			PrintHistory(head);
		}
		else if(!strncmp(input,instruction[4],4)||!strncmp(input,instruction[16],2))
		{
			if(IsFormal(&num1,&num2,&addr,input))
			{
				InputHistory(&head,input);
			}
		}

	}while((strcmp(input, "quit")&&strcmp(input, "q")));
	return 1;
}

//When making dump use sscanf

void PrintInstruction()
{
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
	return;
}

void PrintDir()
{
	DIR* dname=opendir(".");
	struct dirent* dir;
	struct stat filestat;
	while((dir=readdir(dname)) != NULL)
	{
		printf("%s",dir->d_name);
		stat(dir->d_name,&filestat);


//find a way to check executable files @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


		if(S_ISREG(filestat.st_mode))
		{
			printf("*");
		}
		else if(S_ISDIR(filestat.st_mode))
		{
			printf("/");
		}
		printf("\t");
	}
	printf("\n");
	closedir(dname);
}

void InputHistory(NODE** head, char* Input)
{
	NODE* newnode;
	NODE* currnode=*head;
	newnode = (NODE*)malloc(sizeof(NODE));
	if(newnode == NULL)
	{
		printf("Error creating new node!\n");
		return;
	}
	strcpy(newnode->InputString,Input);
	newnode->link = NULL;
	if(*head == NULL)
	{
		*head = newnode;	
	return;	
	}	
	while(currnode->link != NULL)
	{
		currnode = currnode -> link;
	}
	currnode->link = newnode;
	return;
}

void PrintHistory(NODE* head)
{	
	int count=0;
	NODE* currnode = head;
	if(head == NULL)
	{
		printf("list is empty!\n");
		return;
	}
	do
	{
		printf("%d\t%s\n",++count,currnode->InputString);
		currnode = currnode->link;
	}
	while(currnode != NULL);
	return;
}

char HexaToAscii(char HInput[])
{
	char Asc=0;
	char temp=HInput[0];
	if('/' < temp < ':')
	{
		Asc = temp-48;
	}
	else if('@' < temp < 'G')
	{
		Asc = temp-55;
	}
	else
	{
		//exception for non-hexa input!
		printf("Non-Hexa Input Detected!\n");
		return Asc;
	}
	return Asc;
}

int IsNum(char inputchar)
{
	if('/'<inputchar && inputchar<':')
	{
		return 1;
	}
	return 0;
}

int IsCap(char inputchar)
{
	if('@'<inputchar && inputchar<'[')
	{
		return 1;
	}
	return 0;
}

int IsLow(char inputchar)
{
	if('`'<inputchar && inputchar<'{')
	{
		return 1;
	}
	return 0;
}

int IsFormal(int* num1, int* num2, int* addr, char* input)
{
	int i=0;
	int val=0;
	int curraddr=*addr;
	if(input[2] == '\0')
	{
		//when user typed 'du' only
		PrintMem(*addr+1, *addr+160, addr);
		//PrintMem will check the addr when addr+10 is beyond end
		return 1;
	}
	if(!strncmp(input,instruction[4],4) && input[4] == '\0')
	{
		//when user typed 'dump' only
		PrintMem(*addr+1, *addr+160, addr);
		//PrintMem will check the addr when addr+10 is beyond end
		return 1;
	}
	if(!strncmp(input,instruction[4],4))
	{
		//when user typed 'dump' + sth
		i=4;
	}
	else if(!strncmp(input,instruction[16],2))
	{
		//when user typed 'du' + sth
		i=2;
	}
	else
	{
		return 0;
	}
	//error check for types like 'dust' or 'dume'
	if(input[i] != ' ')
	{
		return 0;
	}
	//input's index i is currently on index of ' ' after 'dump' or 'du'
	if(!IsNum(input[i+1]))
	{
		//after 'dump ' or 'du ' must come a number
		return 0;
	}
	while(IsNum(input[++i]))
	{
printf("printing %d\n", input[i]);
		val*=10;
		val+=input[i];
		val-=48;
	}
	*num1 = val;
	val =0;
	//end of num1 input
	
	if(input[i] == '\0')
	{
		//when user typed 'dump %d' or 'du %d'
		PrintMem(*num1, *num1+159, addr);
		//PrintMem will print total of 160 elements, so index will be start to start+159
		return 1;
	}

	if((input[i] != ',') || (input[++i] != ' '))
	{
		return 0;
	}
	if(!IsNum(input[i+1]))
	{
		//when the first letter after 'dump %d, ' is not a number, exception error
		return 0;
	}
	while(IsNum(input[++i]))
	{
		val*=10;
		val+=input[i];
		val-=48;
	}
	*num2 = val;
	val=0;
	//end of num2 input
	
	if(input[i] == '\0')
	{
		//when user typed 'dump %d, %d' or 'du %d, %d'
		if(*num1 > *num2)
		{
			return 0;
		}
		PrintMem(*num1, *num2, addr);
		//PrintMem will print from start(num1) to end(num2)
		return 1;
	}
	return 0;

}

void PrintMem(int start, int end, int* addr)
{
	*addr = end;
	printf("start : %d, end : %d, addr : %d\n", start, end, *addr);
	return;
}
