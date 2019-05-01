#include "20151616.h"

int main()
{
		NODE* head = NULL;
		//headnode of the linked list of history
		char input[MAX];
		//array to save the input line
		int num1=0,num2=0,num3=0;
		//int variables to save the number inputs in dump, edit, fill etc.
		int addr=-1;
		unsigned int tempaddr=0;
		//saves the index of the last visited memory
		memset(mem,0,sizeof(char)*1048576);
		memset(input,0,sizeof(char)*MAX);
		CreateHash();
		//Creates Hash table with hash function. Constructs linked list. If the file crashes in the first place, first point to check
		do
		{
				printf("sicsim> ");
				fgets(input,MAX,stdin);
				input[strlen(input)-1] = '\0';
				//changes the last index from '\n' to '\0'. Enables it to be used in strcmp and check it's end

				if(!strcmp(input,instruction[0])||!strcmp(input,instruction[10]))
						//compares the input and instruction to determine the operation. Either two kinds of input is valid for 'help' instruction
				{
						InputHistory(&head, input);
						//When Valid instruction was input, saves the line in the history lists.
						PrintInstruction();
						//Operation for 'help'
				}
				else if(!strcmp(input,instruction[1])||!strcmp(input,instruction[11]))
				{
						InputHistory(&head, input);
						PrintDir();
						//Operation for 'dir'
				}
				else if(!strcmp(input,instruction[3])||!strcmp(input,instruction[13]))
				{
						InputHistory(&head, input);
						PrintHistory(head);
						//Operation for 'history'
				}
				else if(!strncmp(input,instruction[4],4)||!strncmp(input,instruction[14],2))
						//for dump, edit and fill instruction, program needs to compare first few characters only. Therefore used strncmp.
				{
						if(IsFormal(&num1,&num2,&addr,input))
								//'dump' instruction has several forms. IsFormal function checks if each of the instruction is valid and returns 1 for valid ones, 0 for wrong ones.
						{
								InputHistory(&head,input);
								//Only valid inputs will be saved in the history.
						}
				}
				else if(!strncmp(input, instruction[5],4)||!strncmp(input,instruction[15],1))
				{
						if(IsFormalE(&num1, &num2, input))
								//Similar kind to IsFormal function. Used for Edit instruction.
						{
								InputHistory(&head,input);
						}
				}
				else if(!strncmp(input, instruction[6],4)||!strncmp(input,instruction[16],1))
				{
						if(IsFormalF(&num1, &num2, &num3, input))
						{
								InputHistory(&head,input);
						}
				}
				else if(!strcmp(input,instruction[7]))
				{
						memset(mem,0,sizeof(char)*1048576);
						//initializes the memory's value to 0. 1048576 is the decimal value of hexadecimal FFFFF.
						InputHistory(&head, input);
				}
				else if(!strncmp(input,instruction[8],7))
				{
						if(PrintOp(input))
								InputHistory(&head, input);
				}
				else if(!strcmp(input, instruction[9]))
				{
						PrintOpList();
						InputHistory(&head, input);
				}
				else if(!strncmp(input, instruction[17],9))
				{
						if(!IsAssemble(input))
								//take file name input in the function and determine.
						{
								printf("File unavailable!\n");
								continue;
						}
						InputHistory(&head, input);
				}
				else if(!strncmp(input, instruction[18], 5))
						//compares till space
				{
						if(PrintFile(input))
						{
								InputHistory(&head, input);
						}
						else
						{
								printf("File with such name does not exist\n");
						}
				}
				else if(!strcmp(input, instruction[19]))
				{
						PrintSymbol();
						InputHistory(&head, input);
				}
				else if(!strncmp(input, instruction[20], 8))
				{
					if(!Progaddr(input))
					{
						printf("Input value not available!\n");
						continue;
					}
					InputHistory(&head, input);
				}
				else if(!strncmp(input, instruction[21],7))
				{
					tempaddr = startaddr;
					etemp = esymboltable;
					esymboltable = NULL;
					if(!Loader(input))
					{
						esymboltable = etemp;
						printf("Input file does not exist or in a wrong format!\n");
						startaddr = tempaddr;
						continue;
					}
					startaddr = tempaddr;
					PrintExtsymtab();
					totallength=0;
					ResetEsym();
					InputHistory(&head, input);
				}
				else if(!strncmp(input, instruction[22],3))
				{
					SetBp(input);
					InputHistory(&head, input);
				}
				else if(!strcmp(input, instruction[23]))
				{
					PrintBp();
					InputHistory(&head, input);
				}

		}while((strcmp(input, "quit")&&strcmp(input, "q")));
		//terminates the program when input was q or quit
		return 1;
}

void PrintInstruction()
		//Prints all instructions that can be done in the program.
{
		printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");
		return;
}

int IsAssemble(char* input)
{
		int i=0, j=0,opcode=0,niflag=0,xbpeflag=0,disp=0, format=0,errorflag=0, addrtemp=0;
		int line=0, loc=startaddr, bloc=0, byte=0,reg1=0,reg2=0;
		int len=0;
		char* fname = malloc(sizeof(char)*20);
		char* buf1=malloc(sizeof(char)*100);
		char* buf2=malloc(sizeof(char)*20);
		char* sub = malloc(sizeof(char)*7);
		//sub for subroutine
		char *iname = malloc(sizeof(char)*7);
		//iname for instruction name
		char *oper = malloc(sizeof(char)*20);
		//oper for operand
		char* oper2=malloc(sizeof(char)*20);
		FILE* fp, *lstfp, *objfp;
		int wnum=0;
		//wnum for word number(number of words in a single line)
		sscanf(input,"%s %s",buf1,fname);
		for(i=0;fname[i] != '.';++i)
				//find the name part of file, ex) 2_5.asm-> 2_5
		{
				if(fname[i] == '\0')
				{
						printf("Source file extension not available!\n");
						return 0;
				}
		}
		if(strcmp(".asm",fname+i))
		{
				printf("Source file extension not available!\n");
				return 0;
		}
		Pass1(fname);
		if(!(fp = fopen(fname,"r")))
		{
				printf("No existing file named %s\n",fname);
				return 0;
		}
		fname[i] = '\0';
		strcat(fname,".lst");
		if(!(lstfp = fopen(fname,"w")))
		{
				printf("\nFailed to create a file %s\n",fname);
				return 0;
		}
		printf("\t\toutput file : [%s]",fname);
		fname[i] = '\0';
		strcat(fname,".obj");
		if(!(objfp = fopen(fname,"w")))
		{
				printf("\nFailed to create a file %s\n",fname);
				return 0;
		}
		printf(", [%s]\n",fname);
		fname[i] = '\0';
		//completed printing output file
		while(fgets(buf1,MAX,fp))
		{
				line+=5;
				if(sscanf(buf1,"%s",buf2)<1)
				{
						break;
				}
				fprintf(lstfp,"%5d\t",line);
				if(buf1[0] == '.')
				{
						fprintf(lstfp,"%s",buf1);
				}
				else
				{
						if(IsCap(buf1[0]))
								//First letter is a capital. Which means the line is a subroutine
						{
								wnum=sscanf(buf1,"%s %s %s %s",sub,iname,oper,oper2);
								buf1[strlen(buf1)-1]='\t';
								//directives@@@
								if((GetOp(iname)<0)&&(GetForm(iname)!=4))
								{
										format=0;
										if(!strcmp(iname,"START"))
										{
												sscanf(oper,"%X",&addrtemp);
												loc = startaddr + addrtemp;
												fprintf(lstfp,"%04X\t%s\n",loc,buf1);
										}
										else if(!strcmp(iname,"END"))
										{
												fprintf(lstfp,"\t%s\n",buf1);
										}
										else if(!strcmp(iname,"RESW"))
										{
												fprintf(lstfp,"%04X\t%s\n",loc,buf1);
												format = (StrToDec(oper)*3);
												i=0;
												while(oper[i]!='\0')
												{
													if(!IsNum(oper[i]))
													{
														errorflag = 1;
													}
													i++;
												}
										}
										else if(!strcmp(iname,"RESB"))
										{
												fprintf(lstfp,"%04X\t%s\n",loc,buf1);
												format = StrToDec(oper);
												i=0;
												while(oper[i]!='\0')
												{
													if(!IsNum(oper[i]))
													{
														errorflag = 1;
													}
													i++;
												}
										}
										else if(!strcmp(iname,"WORD"))
										{
												fprintf(lstfp,"%04X\t%s%06X\n",loc,buf1,StrToDec(oper));
												format=3;
										}
										else if(!strcmp(iname,"BYTE"))
										{
												fprintf(lstfp,"%04X\t%s",loc,buf1);
												if(oper[0] == 'X')
												{
														sscanf(oper+2,"%X",&byte);
														format=1;
														fprintf(lstfp,"%02X\n",byte);
												}
												else if(oper[0] == 'C')
												{
														format = strlen(oper)-3;
														i=0;
														while(oper[i+2]!='\0')
														{
																oper[i] = oper[i+2];
																i++;
														}
														oper[i]='\0';
														for(j=0;oper[j]!='\0';++j)
														{
																if(IsCap(oper[j]))
																{
																		fprintf(lstfp,"%02X",(int)oper[j]);
																}
														}
														fprintf(lstfp,"\n");
												}
												else
												{
													errorflag=1;
												}
										}
										else if(!strcmp(iname,"BASE"))
										{
												fprintf(lstfp,"\t%s\n",buf1);
										}
										else
										{
												errorflag =1;
										}
										len = format;
										format =5;
								}
								//end of directives
								else
								{
										fprintf(lstfp,"%04X\t",loc);
										fprintf(lstfp,"%s",buf1);
										if(wnum == 4)
												//When input was a subroutine and 2 operands
										{
												opcode = GetOp(iname);
												format = GetForm(iname);
												if(GetCom(oper))
												{
														strchr(oper,',')[0] = '\0';
												}
										}
										else if(wnum == 3)
												//a subroutine command with one operand
										{
												opcode = GetOp(iname);
												format = GetForm(iname);
												if(GetCom(oper))
												{
														strchr(oper,',')[0] = '\0';
												}
										}
										else if(wnum == 2)
										{
												opcode = GetOp(iname);
												format = GetForm(iname);
												if(GetCom(oper))
												{
														strchr(oper,',')[0] = '\0';
												}
										}
										else if(wnum == 1)
										{
										}
										//format 4 disp calculation
										len = format;
								}
						}
						else if(buf1[0] == ' ')
						{
								wnum = sscanf(buf1,"%s %s %s",iname,oper,oper2);
								buf1[strlen(buf1)-1]='\t';
								if((GetOp(iname)<0)&&(GetForm(iname)!=4))
								{
										format=0;
										if(!strcmp(iname,"START"))
										{
												sscanf(oper,"%X",&addrtemp);
												loc += addrtemp;
												fprintf(lstfp,"%04X\t%s\n",loc,buf1);
										}
										else if(!strcmp(iname,"END"))
										{
												fprintf(lstfp,"\t%s",buf1);
										}
										else if(!strcmp(iname,"RESW"))
										{
												fprintf(lstfp,"%04X%s\n",loc,buf1);
												format = (StrToDec(oper)*3);
												i=0;
												while(oper[i]!='\0')
												{
													if(!IsNum(oper[i]))
													{
														errorflag = 1;
													}
													i++;
												}
										}
										else if(!strcmp(iname,"RESB"))
										{
												fprintf(lstfp,"%04X%s\n",loc,buf1);
												format = StrToDec(oper);
												i=0;
												while(oper[i]!='\0')
												{
													if(!IsNum(oper[i]))
													{
														errorflag = 1;
													}
													i++;
												}
										}
										else if(!strcmp(iname,"WORD"))
										{
												fprintf(lstfp,"%04X\t%s%06X\n",loc,buf1,StrToDec(oper));
												format=3;
										}
										else if(!strcmp(iname,"BYTE"))
										{
												fprintf(lstfp,"%04X%s",loc,buf1);
												if(oper[0] == 'X')
												{
														sscanf(oper+2,"%X",&byte);
														format=1;
														fprintf(lstfp,"%02X\n",byte);
												}
												else if(oper[0] == 'C')
												{
														format = strlen(oper)-3;
														i=0;
														while(oper[i+2]!='\0')
														{
																oper[i] = oper[i+2];
																i++;
														}
														oper[i]='\0';
														for(j=0;oper[j]!='\0';++j)
														{
																if(IsCap(oper[j]))
																{
																		fprintf(lstfp,"%02X",(int)oper[j]);
																}
														}
														fprintf(lstfp,"\n");

												}
												else
												{
													errorflag=1;
												}
										}
										else if(!strcmp(iname,"BASE"))
										{
												fprintf(lstfp,"\t%s\n",buf1);
										}
										else
										{
												errorflag=1;
										}
										len = format;
										format =5;
								}
								else
								{
										fprintf(lstfp,"%04X\t",loc);
										fprintf(lstfp,"%s",buf1);
										if(wnum == 3)
										{
												opcode = GetOp(iname);
												format = GetForm(iname);
												if(GetCom(oper))
												{
														strchr(oper,',')[0] = '\0';
												}
										}
										else if(wnum == 2)
										{
												opcode = GetOp(iname);
												format = GetForm(iname);
												if(GetCom(oper))
												{
														strchr(oper,',')[0] = '\0';
												}
										}
										else if(wnum == 1)
										{
												opcode = GetOp(iname);
												format = GetForm(iname);
										}
										len = format;
								}
						}
						else
						{
								errorflag = 1;	
						}
						if(oper[0] == '@')
						{
								niflag = 2;
								oper = oper + 1;
						}
						else if(oper[0] == '#')
						{
								niflag = 1;
								oper = oper + 1;
						}
						else
						{
								niflag = 3;
						}
						//@@@@@@@@@@@@@@niflag=0 case consider???
						//if base register changed, save its value
						if(!strcmp(iname,"LDB")||!strcmp(iname+1,"LDB"))
						{
								bloc = GetSymLoc(oper);
						}
						//starting disp calculation
						loc += len;
						if(!strcmp(iname,"RSUB"))
						{
								fprintf(lstfp,"4F0000\n");
								continue;
						}
						switch (format)
						{
								//fprint according to the format
								case 1:
										//format1
										fprintf(lstfp,"\n");
										break;
								case 2:
										//format2
										if(!strcmp(iname,"SVC"))
										{
											fprintf(lstfp,"%02X%01X00\n",opcode,StrToDec(oper));
											break;
										}
										else if(!strcmp(iname,"SHIFTL"))
										{
											fprintf(lstfp,"%02X%01X%01X\n",opcode,GetReg(oper),StrToDec(oper2));
										}
										fprintf(lstfp,"%02X%01X",opcode,GetReg(oper));
										if(GetReg(oper2)>0)
										{
												fprintf(lstfp,"%1X\n",GetReg(oper2));
										}
										else
										{
												fprintf(lstfp,"0\n");
										}
										break;
								case 3:
										//format3
										//flag set
										if(!strcmp(oper2,"X"))
												//b flag set ex) SBUF	STCH	BUFFER, X
										{
												xbpeflag += FlagX;
										}
										if(IsNum(oper[0]))
										{
												disp = StrToDec(oper);
										}
										else
										{
												disp = GetSymLoc(oper);
												{
														if((disp - loc) > 2048 || (disp-loc) < -2048)
																//out of pc relative range. use base register
														{
																xbpeflag += FlagB;
																disp -= bloc;
														}
														else
														{
																//in pc relative range
																xbpeflag += FlagP;
																disp -= loc;
														}

												}
										}
										disp = (disp&4095);
										//cut unwanted bits
										fprintf(lstfp,"%02X%01X%03X\n",opcode+niflag,xbpeflag,disp);
										break;
								case 4:
										//format(4
										opcode = GetOp(iname+1);
										xbpeflag += FlagE;
										//set e flag
										if(IsNum(oper[0]))
										{
												disp = StrToDec(oper);
										}
										else
										{
												if((disp = GetSymLoc(oper)) == -1)
												{
														errorflag=1;
												}
										}
										fprintf(lstfp,"%02X%01X%05X\n",opcode+niflag,xbpeflag,disp);
										break;
						}
						niflag = 0;
						xbpeflag = 0;
						format=0;
						len=0;
						memset(oper,0,strlen(oper));
						memset(oper2,0,strlen(oper2));
				}
				if(errorflag)
				{
						printf("error occured in %d line!\n",line);
						symboltable = temp;
						fclose(lstfp);
						fclose(objfp);
						fclose(fp);
						strcat(fname,".obj");
						remove(fname);
						fname[i] = '\0';
						strcat(fname,".lst");
						remove(fname);
						return 0;
				}
		}
		ResetSym();
		fclose(lstfp);
		fclose(objfp);
		fclose(fp);
		Pass2(fname,loc);
		return 1;
}

void PrintSymbol()
{
		SYM* currnode = symboltable;
		if(currnode == NULL)
		{
				printf("Empty symbol node!\n");
				return;
		}
		while(currnode != NULL)
		{
				printf("\t%s\t%04X\n",currnode->subr,currnode->loc);
				currnode = currnode -> link;
		}
		return;

}

int PrintFile(char* input)
{
		FILE* fp;
		char* buf1 = malloc(sizeof(char)*MAX);
		char* fname = malloc(sizeof(char)*MAX);
		int wnum=0;
		memset(buf1,0,MAX);
		sscanf(input,"%s %s",buf1,fname);
		if(fp = fopen(fname,"r"))
		{
				while(fgets(buf1,MAX-1,fp))
				{
						printf("%s",buf1);
				}
		}
		else
		{
				printf("File %s unavailable access for reading!\n",input);
				free(buf1);
				free(fname);
				return 0;
		}
		printf("\n");
		free(buf1);
		free(fname);
		fclose(fp);
		return 1;
}

void PrintDir()
		//Prints all the files in the current directory. prints different characters for different types of files.
{
		DIR* dname=opendir(".");
		//directory pointer is used to save names and types of the files in the directory
		struct dirent* dir;
		//dirent* structure is used to check whether there is files left to check or not
		struct stat filestat;
		//file's type is saved in filestat variable
		while((dir=readdir(dname)) != NULL)
				//When dir is NULL, there is no more file to check.
		{
				printf("%s",dir->d_name);
				//print file's name
				stat(dir->d_name,&filestat);
				//stat saves file's name in variable dir, and saves file type in filestat
				if(S_ISDIR(filestat.st_mode))
						//Checks if the current file is a directory
				{
						printf("/");
				}
				else if(filestat.st_mode&S_IEXEC)
						//Checks if the file is executable.
				{
						printf("*");
				}
				printf("\t");
		}
		printf("\n");
		closedir(dname);
		return;
}

void InputHistory(NODE** head, char* Input)
		//Saves the input in a linked list. Validity of the input will be checked before operating this function
{
		NODE* newnode;
		NODE* currnode=*head;
		newnode = (NODE*)malloc(sizeof(NODE));
		//Allocate newnode a size of NODE
		if(newnode == NULL)
				//When malloc was unsuccessful print error message and return
		{
				printf("Error creating new node!\n");
				return;
		}
		strcpy(newnode->InputString,Input);
		//Copy the input to the newnode with strcpy
		newnode->link = NULL;
		//initializes the next link to NULL
		if(*head == NULL)
		{
				*head = newnode;
				//When the linked list is empty, set newnode as the first node. therefore, newnode becomes the head of the list.
				return;
		}
		while(currnode->link != NULL)
		{
				currnode = currnode -> link;
				//Find the last node of the linked list.
		}
		currnode->link = newnode;
		//Set the last node as newnode by setting the last node's link as the newnode
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
				//Print the current node's string until you find the last node
		}
		while(currnode != NULL);
		//when you find the NULL pointer, it means there is no more nodes to go through
		return;
}

int IsIname(char* input)
{
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		int index=0;
		HASH* currnode=NULL;
		index = HashFunc(input);
		//identical instruction will have identical hash value
		currnode = hashtable[index];
		while(currnode != NULL)
		{
				if(!strcmp(input,currnode->opinstruction))
						break;
				//check if such instruction exists in this index of hashtable
				currnode = currnode->link;
		}
		if(currnode == NULL)
		{
				printf("instruction %s not found!\n",input);
				//when input does not match any instruction in the linked list print error message
				return 0;
		}
		printf("opcode for %s is %X\n",currnode->opinstruction,currnode->opcode);
		//print the right opcode for the instruction
		return 1;

}

int IsOper(char* input)
{
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		return 1;
}

int IsNum(char inputchar)
		//Returns 1 if a character is a ascii code for number 0 to 9
{
		if('/'<inputchar && inputchar<':')
		{
				return 1;
		}
		return 0;
}

int IsCap(char inputchar)
		//Returns 1 if a character is a Ascii code for capital letter
{
		if('@'<inputchar && inputchar<'[')
		{
				return 1;
		}
		return 0;
}
int IsCapHex(char inputchar)
		//Returns 1 if a character is a Ascii code for capital letter for Hexadecimal A to F
{
		if('@'<inputchar && inputchar<'G')
		{
				return 1;
		}
		return 0;
}

int IsLow(char inputchar)
		//Returns 1 if a character is a Ascii code for lowercase letter
{
		if('`'<inputchar && inputchar<'{')
		{
				return 1;
		}
		return 0;
}
int IsHex(char* input)
{
		int i=0;
		for(;input[i]!='\0';++i)
		{
				if('0' > input[i] || (input[i] >'9' && input[i] < 'A') || (input[i] > 'F')&& (input[i] < 'a') || input[i] > 'f')
						return 0;
		}
		return 1;
}

int StrToDec(char* input)
{
		int i=0;
		int val=0;
		while(input[i] != '\0')
		{
				val *= 10;
				val += (input[i]-'0');
				++i;
		}
		return val;
}

int HexToDec(unsigned char* input)
{
	int i=0;
	int val=0;
	while(input[i] != '\0')
	{
		val *= 16;
		if((64< input[i]) && input[i] < 71)
		{
			val += (input[i]-55);
		}
		else
		{
			val += (input[i]-'0');
		}
		++i;
	}
	return val;
}
int GetCom(char* input)
		//Returns the number of ','(commas) in a string
{
		int i=0;
		int count=0;
		for(i=0;input[i]!='\0';++i)
		{
				if(input[i]==',')
				{
						++count;
				}
		}
		return count;
}

int GetReg(char* oper)
{
		if(!strcmp(oper,"A"))
		{
				return RegA;
		}
		else if(!strcmp(oper,"X"))
		{
				return RegX;
		}
		else if(!strcmp(oper,"L"))
		{
				return RegL;
		}
		else if(!strcmp(oper,"B"))
		{
				return RegB;
		}
		else if(!strcmp(oper,"S"))
		{
				return RegS;
		}
		else if(!strcmp(oper,"T"))
		{
				return RegT;
		}
		else if(!strcmp(oper,"F"))
		{
				return RegF;
		}
		else if(!strncmp(oper,"PC",2))
		{
				return RegPC;
		}
		else if(!strncmp(oper,"SW",2))
		{
				return RegSW;
		}
		return -1;
}

int IsFormal(int* num1, int* num2, int* addr, char* input)
{
		char buf1[6],buf2[6],buf3[6],buf4[6],buf5[6];
		//buf arrays are used to take strings to check errors
		int n1=0,n2=0;
		//n1 and n2 saves the number parts of the input
		int wnum=0;
		wnum=sscanf(input,"%s%X ,%X",buf1,&n1,&n2);
		//wnum(wordnum) saves the number of words taken by sscanf. sscanf returns the number of words that are successfully saved
		if( (n1<0) || (n2<0))
		{
				//negative input error check
				return 0;
		}
		if((wnum == 3)&&(GetCom(input)==1))
				//when 3 parts of arguments were passed, there should only be one comma
		{
				if(sscanf(input,"%s%s%s%s%s",buf1,buf2,buf3,buf4,buf5)>4)
						//if the input contains unwanted string parts, return 0. Reason for 5 buffers is because hexadecimals can be considered as strings too
				{
						return 0;
				}
				if((n1 > n2) || (n2>1048575))
						//When n1 is bigger than n2 or n2 is bigger than maximum index fail.
				{
						return 0;
				}
				else
				{
						PrintMem(n1,n2,addr);
						//PrintMem will print from start(num1) to end(num2)
				}
		}
		else if(wnum == 2&&(GetCom(input)==0))
				//when only one number input present
		{
				if(sscanf(input,"%s%s%s",buf1,buf2,buf3)>2)
				{
						return 0;
				}
				if(!IsHex(buf2))
				{
						return 0;
				}
				if(n1>1048575)
				{
						//when n1 is bigger than max index fail.
						return 0;
				}
				else
				{
						PrintMem(n1,n1+159,addr);
						//No need to check n1+ 160 since printmem checks it
				}
		}
		else if(wnum == 1&&(strchr(input,' ')==NULL))
				//When only one word was input. 'dump' or 'du'. Then there should be no other words, therefore no spaces should exist.
		{
				PrintMem(*addr+1,*addr+160,addr);
		}
		else
				//cases where no input was existent or 4 or more words were contained
		{
				return 0;
		}
		return 1;
}

int IsFormalE(int* num1, int* num2, char* input)
		//Operation for edit
{
		int i=0;
		int val=0;
		if(input[1] == '\0')
		{
				//when user typed 'e' only
				return 0;
		}
		if(!strncmp(input,instruction[5],4) && input[4] == '\0')
		{
				//when user typed 'edit' only
				return 0;
		}
		if(!strncmp(input,instruction[5],4))
		{
				//when user typed 'edit' + sth
				i=4;
		}
		else if(!strncmp(input,instruction[15],1))
		{
				//when user typed 'e' + sth
				i=1;
		}
		else
		{
				return 0;
		}
		//error check for types like 'east' or 'edin'
		if(input[i] != ' ')
		{
				return 0;
		}
		//input's index i is currently on index of ' ' after 'edit' or 'e'
		while(input[i+1] == ' ')
		{
				//when you type multiple spaces after edit like 'e   ' or 'edit   '
				++i;
		}
		if(!(IsNum(input[i+1])||IsCapHex(input[i+1])))
		{
				//after 'edit ' or 'e ' must come a number or a capital
				return 0;
		}
		++i;
		while(IsNum(input[i])||IsCapHex(input[i]))
		{
				val*=16;
				if(IsNum(input[i]))
						val += (input[i]-48);
				else
						val += (input[i]-55);
				++i;
		}
		*num1 = val;
		val =0;
		//end of num1 input

		while(input[i] == ' ')
		{
				//when you type multiple spaces after num1 like 'e %d   ' or 'edit %d  '
				++i;
		}

		if(input[i] != ',')
		{
				return 0;
		}
		while(input[i+1] == ' ')
		{
				//when you type multiple spaces after comma like 'e %d,   ' or 'edit %d,   '
				++i;
		}

		if(!(IsNum(input[i+1])||IsCapHex(input[i+1])))
		{
				//when the first letter after 'edis %d, ' is not a number or capital letter, exception error
				return 0;
		}
		++i;
		while(IsNum(input[i])||IsCapHex(input[i]))
		{
				val*=16;
				if(IsNum(input[i]))
						val += (input[i]-48);
				else
						val += (input[i]-55);
				++i;
		}
		*num2 = val;
		val=0;
		//end of num2 input
		if(input[i] == '\0')
		{
				//when user typed 'edit %d, %d' or 'e %d, %d'
				if(*num1 > *num2)
				{
						return 0;
				}
				EditMem(*num1, *num2);
				//EditMem will Edit value of address in num1 into value of num2
				return 1;
		}
		return 0;

}

int IsFormalF(int* num1, int* num2, int* num3, char* input)
{
		int i=0;
		int val=0;
		if(input[1] == '\0')
		{
				//when user typed 'f' only
				return 0;
		}
		if(!strncmp(input,instruction[6],4) && input[4] == '\0')
		{
				//when user typed 'fill' only
				return 0;
		}
		if(!strncmp(input,instruction[6],4))
		{
				//when user typed 'fill' + sth
				i=4;
		}
		else if(!strncmp(input,instruction[16],1))
		{
				//when user typed 'f' + sth
				i=1;
		}
		else
		{
				return 0;
		}
		//error check for types like 'finn' or 'fire'
		if(input[i] != ' ')
		{
				return 0;
		}
		//input's index i is currently on index of ' ' after 'fill' or 'f'
		while(input[i+1] == ' ')
		{
				//when you type multiple spaces after fill like 'f   ' or 'fill   '
				++i;
		}
		if(!(IsNum(input[i+1])||IsCapHex(input[i+1])))
		{
				//after 'fill ' or 'f ' must come a number or a capital
				return 0;
		}
		++i;
		while(IsNum(input[i])||IsCapHex(input[i]))
		{
				val*=16;
				if(IsNum(input[i]))
						val += (input[i]-48);
				else
						val += (input[i]-55);
				++i;
		}
		*num1 = val;
		val =0;
		//end of num1 input

		while(input[i] == ' ')
		{
				//when you type multiple spaces after num1 like 'f %d   ' or 'fill %d  '
				++i;
		}

		if(input[i] != ',')
		{
				return 0;
		}
		while(input[i+1] == ' ')
		{
				//when you type multiple spaces after comma like 'f %d,   ' or 'fill %d,   '
				++i;
		}

		if(!(IsNum(input[i+1])||IsCapHex(input[i+1])))
		{
				//when the first letter after 'fill %d, ' is not a number or capital letter, exception error
				return 0;
		}
		++i;
		while(IsNum(input[i])||IsCapHex(input[i]))
		{
				val*=16;
				if(IsNum(input[i]))
						val += (input[i]-48);
				else
						val += (input[i]-55);
				++i;
		}
		*num2 = val;
		val=0;
		//end of num2 input

		while(input[i] == ' ')
		{
				//when you type multiple spaces after num1 like 'f %d, %d   ' or 'fill %d, %d  '
				++i;
		}

		if(input[i] != ',')
		{
				return 0;
		}
		while(input[i+1] == ' ')
		{
				//when you type multiple spaces after comma like 'f %d, %d,  ' or 'fill %d, %d,  '
				++i;
		}

		if(!(IsNum(input[i+1])||IsCapHex(input[i+1])))
		{
				//when the first letter after 'fill %d, %d, ' is not a number or capital letter, exception error
				return 0;
		}
		++i;
		while(IsNum(input[i])||IsCapHex(input[i]))
		{
				val*=16;
				if(IsNum(input[i]))
						val += (input[i]-48);
				else
						val += (input[i]-55);
				++i;
		}
		*num3 = val;
		val=0;
		//end of num3 input

		if(input[i] == '\0')
		{
				//when user typed 'fill %d, %d, %d' or 'f %d, %d, %d'
				if(*num1 > *num2)
				{
						return 0;
				}
				FillMem(*num1, *num2, *num3);
				//FillMem will Fill values of address in num1 to num2 into value of num3
				return 1;
		}
		return 0;

}

void PrintMem(int start, int end, int* addr)
		//Prints the memory in range of start to end
{
		int curridx=start;
		int temp=start/16*16;
		//temp saves the starting address of the line
		int endidx=0;
		if(end > 1048575)
		{
				//when the print range exceeds the maximum memory index
				end = 1048575;
		}
		for(curridx=start;curridx<=end;curridx+=16,temp+=16)
		{
				endidx = temp+15;
				if(endidx > end)
						endidx = end;
				//print the memory addr of the start of the line
				PrintHex(curridx, endidx);
				//print the hexadecimal numbers
				printf("; ");
				PrintAsc(curridx, endidx);
				//print the ascii codes after the hexadecimal part
				printf("\n");
				curridx = curridx/16*16;
				//at the end of the line 
		}
		*addr = end;
		return;
}

void PrintHex(int start, int end)
		//prints the hexadecimal parts of the line
{
		int curridx=0;
		int hchk=65536;
		int temp=start/16*16;
		printf("%05X ",temp);
		if(start%16)
				//when start is not the first index of the line
		{
				//when start is not divided by 16
				for(curridx = start/16*16; curridx < start; ++curridx)
						//printing from the start of the line, if it is not in the range of start to end, print spaces
				{
						printf("   ");
				}
				for(; curridx<=end; ++curridx)
				{
						printf("%02X ",mem[curridx]);
				}
				for(;curridx<temp+16;curridx++)
						//when the end index is not the end of the line, print spaces until the end of the line
				{
						printf("   ");
				}
				return;
		}
		else
				//when start is the first index of the line
		{
				for(curridx = start; curridx<=end;++curridx)
						//print hexadecimals until the end index
				{
						printf("%02X ",mem[curridx]);
				}
				if(curridx%16)
				{
						//when end is not the end of the line
						for(temp=curridx/16*16+16;curridx<temp;++curridx)
						{
								printf("   ");
						}
				}
				return;
		}
		return;
}

void PrintAsc(int start, int end)
{
		int curridx=0;
		int temp=start/16*16;
		if(start%16)
		{
				//when start is not divided by 16
				for(curridx = start/16*16; curridx < start; ++curridx)
						//print '.' for memory not in range
				{
						printf(".");
				}
				for(; curridx<=end; ++curridx)
						//print the ascii codes from start to end
				{
					if((mem[curridx] < 32) || (mem[curridx] > 126))
					{
						printf(".");
					}
					else
					{
						printf("%c",mem[curridx]);
					}
				}
				for(;curridx<temp+16;curridx++)
				{
						printf(".");
				}
				return;
		}
		else
		{
				//when start is the first index of the line
				for(curridx = start; curridx<=end;++curridx)
				{
					if((mem[curridx] < 32) || (mem[curridx] > 126))
					{
						printf(".");
					}
					else
					{
						printf("%c",mem[curridx]);
					}
				}
				if(curridx%16)
				{
						//when end is not the end of the line
						for(temp=curridx/16*16+16;curridx<temp;++curridx)
						{
								printf(".");
						}
				}
				return;
		}
		return;
}

void EditMem(int idx, int val)
{
		if((idx<0)||(idx>1048575))
		{
				printf("Wrong range of memory acces detected!\n");
				return;
		}
		mem[idx] = val;
		//error is checked before entering this function, so set the memory at the index to val
		return;
}

void FillMem(int idxs, int idxe, int val)
		//similar to function EditMem but edits several index of memory
{
		int i=0;
		if((idxs<0)||(idxs>1048575))
		{
				printf("Wrong range of memory access detected!\n");
				return;
		}
		if((idxe<0)||(idxe>1048575))
		{
				printf("Wrong range of memory access detected!\n");
				return;
		}
		for(i=idxs;i<=idxe;++i)
		{
				mem[i] = val;
		}
		return;
}

void CreateSym(char* newsubr, int newline)
{
		SYM* currnode = symboltable;
		SYM* newnode = (SYM*)malloc(sizeof(SYM));
		newnode->loc = newline;
		strcpy(newnode->subr,newsubr);
		newnode->link = NULL;
		if(currnode == NULL)
		{
				symboltable = newnode;
				return;
		}
		if(strcmp(currnode->subr,newsubr)>0)
		{
				symboltable = newnode;
				newnode->link = currnode;
				return;
		}
		while((currnode->link != NULL)&&(strcmp(currnode->link->subr,newsubr)<0))
		{
				currnode = currnode -> link;
		}
		newnode->link = currnode->link;
		currnode->link = newnode;
		return;
}

void ResetSym()
{
		SYM* deletenode = temp;
		while(temp != NULL)
		{
				deletenode = temp;
				temp = temp->link;
				deletenode->link = NULL;
				free(deletenode);
		}
		return;

}

void CreateHash()
{
		int idx=0, code=0;
		char buf[100];
		//buf saves the opcode table line by line
		//saves opcode of the line
		char iname[6];
		//saves instruction name of the line
		int format;
		//saves format of the line
		HASH* newnode = malloc(sizeof(HASH));
		HASH* currnode=NULL;
		hashtable = (HASH**)malloc(sizeof(HASH*)*20);
		//create HASH* array size of 20
		FILE* fp=fopen("opcode.txt", "r");
		while(1)
		{
				newnode = malloc(sizeof(HASH));
				if(fgets(buf,100,fp) != NULL)
				{
						sscanf(buf,"%X%s%x",&code,iname,&format);
						//completed input
				}
				else
				{
						fclose(fp);
						return;
				}
				strcpy(newnode->opinstruction,iname);
				newnode->opcode=code;
				newnode->format=format;
				newnode->link = NULL;
				//newnode initialization complete

				idx = HashFunc(iname);
				currnode = hashtable[idx];
				if(currnode == NULL)
				{
						//when certain hashtable index is an empty linked list. newnode becomes the head of the list
						hashtable[idx] = newnode;
						continue;
				}
				while(currnode->link != NULL)
				{
						currnode = currnode->link;
				}
				currnode->link = newnode;
				//link the new node at the end of the linked list

		}
		fclose(fp);
		return;
}

int HashFunc(char* input)
		//returns the hash value of the string
{
		int hash=0;
		int i=0;
		for(i=0;input[i]!='\0';++i)
		{
				hash += (int)input[i];
				hash+=2;
				//this functions's hash value is the sum of all the ascii codes in the string plus 2*length of the string
		}
		hash%=20;
		//hash value should not exceed 20 which is the maximum index of the hashtable
		return hash;
}

int GetOp(char* input)
		//takes instruction name as a parameter and returns its opcode in integer
{
		HASH* currnode = NULL;
		int index = HashFunc(input);
		int opcode=0;
		if(!strcmp(input,"RESW")||!strcmp(input,"RESB")||!strcmp(input,"START")||!strcmp(input,"BYTE")||!strcmp(input,"WORD")||!strcmp(input,"BASE")||!strcmp(input,"FIRST"))
				//Cases where the line doesn't have an object code
		{
				return -1;
		}
		currnode = hashtable[index];
		while(currnode!=NULL)
		{
				if(!strcmp(input,currnode->opinstruction))
						break;
				currnode = currnode->link;
		}
		if(currnode == NULL)
		{
				return -1;
		}
		opcode = currnode->opcode;
		return opcode;
}

int GetForm(char* input)
		//takes instruction name as a parameter and returns its opcode in integer
{
		HASH* currnode = NULL;
		int index = HashFunc(input);
		int format=0;
		if(input[0] == '+')
		{
				return 4;
		}
		currnode = hashtable[index];
		while(currnode!=NULL)
		{
				if(!strcmp(input,currnode->opinstruction))
						break;
				currnode = currnode->link;
		}
		if(currnode == NULL)
		{
				return 0;
		}
		format = currnode->format;
		return format;
}

int GetSymLoc(char* input)
{
		SYM* currnode=symboltable;
		if((strlen(input) == 0)||(input[0] == 0))
		{
				return -1;
		}
		if(currnode == NULL)
		{
				return -1;
		}
		while(currnode != NULL)
		{
				if(!strcmp(currnode->subr,input))
				{
						return currnode->loc;
				}
				currnode = currnode->link;
		}
		return -1;
}

int PrintOp(char* input)
{
		HASH* currnode = NULL;
		int index = 0;
		char iname[6];
		sscanf(input,"opcode %s", iname);
		//input an instruction name and save it on iname
		index = HashFunc(iname);
		//identical instruction will have identical hash value
		currnode = hashtable[index];
		while(currnode != NULL)
		{
				if(!strcmp(iname,currnode->opinstruction))
						break;
				//check if such instruction exists in this index of hashtable
				currnode = currnode->link;
		}
		if(currnode == NULL)
		{
				printf("instruction not found!\n");
				//when input does not match any instruction in the linked list print error message
				return 0;
		}
		printf("opcode is %X\n",currnode->opcode);
		//print the right opcode for the instruction
		return 1;
}

void PrintOpList()
{
		int i=0;
		HASH* currnode=NULL;
		for(;i<20;++i)
		{
				printf("%d : ",i);
				//print the index of the hashtable 
				for(currnode=hashtable[i];currnode != NULL;currnode = currnode->link)
				{
						if(currnode->link != NULL)
						{
								printf("[%s,%X] -> ",currnode->opinstruction,currnode->opcode);
								//when current node is not the end node print the node with arrow
						}
						else
						{
								printf("[%s,%X]",currnode->opinstruction,currnode->opcode);
								//when current node is the end of the node print the node without an arrow
						}
				}
				printf("\n");
		}
}

void Pass1(char* input)
{
		FILE* fp;
		char* buf1 = (char*)malloc(sizeof(char)*MAX);
		int loc=0,len=0,format=0,wnum=0,opcode=0,errorflag=0;
		char* sub = malloc(sizeof(char)*10);
		char* iname = malloc(sizeof(char)*10);
		char* oper = malloc(sizeof(char)*10);
		char* oper2 = malloc(sizeof(char)*10);
		fp = fopen(input,"r");
		temp=symboltable;
		symboltable = NULL;
		while(fgets(buf1,MAX,fp))
		{
				if(buf1[0] == '.')
				{
					continue;
				}
				if(IsCap(buf1[0]))
						//First letter is a capital. Which means the line is a subroutine
				{
						wnum=sscanf(buf1,"%s %s %s %s",sub,iname,oper,oper2);
						buf1[strlen(buf1)-1]='\t';
						//directives@@@
						if((GetOp(iname)<0)&&(GetForm(iname)!=4))
						{
								format=0;
								if(!strcmp(iname,"START"))
								{
									sscanf(oper,"%X",&loc);
								}
								else if(!strcmp(iname,"END"))
								{
								}
								else if(!strcmp(iname,"RESW"))
								{
										format = (StrToDec(oper)*3);
								}
								else if(!strcmp(iname,"RESB"))
								{
										format = StrToDec(oper);
								}
								else if(!strcmp(iname,"WORD"))
								{
										format = 3;
								}
								else if(!strcmp(iname,"BYTE"))
								{
										if(oper[0] == 'X')
										{
												format=1;
										}
										if(oper[0] == 'C')
										{
												format = strlen(oper)-3;
										}
								}
								else if(!strcmp(iname,"BASE"))
								{
								}
								len = format;
								format =5;
						}
						//end of directives
						else
						{
								if(wnum == 4)
										//When input was a subroutine and 2 operands
								{
										opcode = GetOp(iname);
										format = GetForm(iname);
								}
								else if(wnum == 3)
										//a subroutine command with one operand
								{
										opcode = GetOp(iname);
										format = GetForm(iname);
								}
								else if(wnum == 2)
								{
										opcode = GetOp(iname);
										format = GetForm(iname);
								}
								else if(wnum == 1)
								{
								}
								//format 4 disp calculation

								//array x flag control
								len = format;
						}
						CreateSym(sub,loc);
				}
				else if(buf1[0] == ' ')
				{
						wnum = sscanf(buf1,"%s %s %s",iname,oper,oper2);
						if((GetOp(iname)<0)&&(GetForm(iname)!=4))
						{
								format=0;
								if(!strcmp(iname,"START"))
								{
									sscanf(oper,"%X",&loc);
								}
								else if(!strcmp(iname,"END"))
								{
								}
								else if(!strcmp(iname,"RESW"))
								{
										format = (StrToDec(oper)*3);
								}
								else if(!strcmp(iname,"RESB"))
								{
										format = StrToDec(oper);
								}
								else if(!strcmp(iname,"WORD"))
								{
										format = 3;
								}
								else if(!strcmp(iname,"BYTE"))
								{
										if(oper[0] == 'X')
										{
												format=1;
										}
										if(oper[0] == 'C')
										{
												format = strlen(oper)-3;
										}
								}
								else if(!strcmp(iname,"BASE"))
								{
								}
								len = format;
								format =5;
						}
						else
						{
								if(wnum == 3)
								{
										opcode = GetOp(iname);
										format = GetForm(iname);
								}
								else if(wnum == 2)
								{
										opcode = GetOp(iname);
										format = GetForm(iname);
								}
								else if(wnum == 1)
								{
										opcode = GetOp(iname);
										format = GetForm(iname);
								}
								len = format;
						}
				}
				loc += len;
		}
		fclose(fp);
		return;
}

void Pass2(char *input,int endloc)
{
		FILE *lstfp, *objfp;
		int wnum=0,loc=0,line=0,start=startaddr,cflag=0,len=0,modcnt=0,endrecord=0,endflag=0,i=0,addrtemp=0;
		char* subr = malloc(sizeof(char)*100);
		char* iname = malloc(sizeof(char)*100);
		char* oper = malloc(sizeof(char)*100);
		char* oper2 = malloc(sizeof(char)*100);
		char* buf1 = malloc(sizeof(char)*300);
		char* objcode = malloc(sizeof(char)*100);
		char* trecord = malloc(sizeof(char)*100);
		int modlist[200];
		strcat(input,".obj");
		objfp = fopen(input,"w");
		strchr(input,'.')[0] = '\0';
		strcat(input,".lst");
		lstfp = fopen(input,"r");
		fgets(buf1,MAX,lstfp);
		sscanf(buf1,"%d %X %s %s %X",&line,&loc,subr,iname,&addrtemp);
		start += addrtemp;
		rewind(lstfp);
		if(!strcmp(iname,"START"))
		{
				fprintf(objfp,"H%-6s%06X%06X\nT%06X",subr,start,endloc-start,start);
		}
		else
		{
				fprintf(objfp,"H      %06X%X\nT%06X",start,endloc-start,start);
		}
		cflag=0;
		//set cflag to 1 to start from next line after header record
		memset(trecord,'\0',100);
		while(fgets(buf1,2*MAX,lstfp))
		{
				memset(objcode,'\0',100);
				memset(iname,'\0',100);
				memset(oper,'\0',100);
				memset(oper2,'\0',100);
				wnum = sscanf(buf1,"%d %X\t",&line,&loc);
				if(wnum < 2)
						//line without loc. has no objectcode nor length. ex)110	.    subroutine to read
				{
						continue;
				}
				//check first line @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
				i=0;
				while(buf1[i+11]!='\0')
				{
						buf1[i] = buf1[i+11];
						i++;
				}
				buf1[i]='\0';
				//scanf source statement and object code to remove loc and line
				if(IsCap(buf1[0]))
				{
						wnum = sscanf(buf1,"%s %s %s %s %s",subr,iname,oper,oper2,objcode);
						//check rsub here first
						if(!strcmp(iname,"RSUB"))
						{
								strcpy(objcode,oper);
						}
						if(wnum == 3)
								//a subroutine directive without objectcode
						{
						}
						else if(wnum == 4)
								//a subroutine with one operand and objectcode
						{
								strcpy(objcode,oper2);
						}
						else if(wnum == 5)
								//a subroutine with two operand and objectcode
						{
						}
				}
				else if(buf1[0] == ' ')
				{
						wnum = sscanf(buf1,"%s %s %s %s",iname,oper,oper2,objcode);
						//check rsub here first
						if(!strcmp(iname,"RSUB"))
						{
								strcpy(objcode,oper);
						}
						if(wnum == 2)
								//instruction with operand without opcde ex) resw 1
						{
						}
						else if(wnum == 3)
								//instruction with operand and objcode but not a subroutine
						{
								strcpy(objcode,oper2);
						}
						else if(wnum == 4)
								//instruction with 2 operands and an objcode but not a subroutine
						{
						}
				}
				if((iname[0] == '+')&&(oper[0] != '#'))
				{
						modlist[modcnt] = loc;
						modcnt++;
				}
				if(strlen(objcode)!=0)
				{
						if((len+strlen(objcode)>60)||cflag==1)
						{
								fprintf(objfp,"%02X%s\nT%06X",len/2,trecord,loc);
								strcpy(trecord,objcode);
								len=strlen(objcode);
						}
						else
						{
								len += strlen(objcode);
								strcat(trecord,objcode);
						}
						if(!endflag)
						{
								endflag=1;
								endrecord = loc;
						}
						cflag=0;
				}
				loc+= strlen(objcode)/2;
				if((!strcmp(iname,"RESW")||!strcmp(iname,"RESB"))&&cflag==0)
				{
						cflag=1;
				}
		}
		fprintf(objfp,"%02X%s",len/2,trecord);
		if(modcnt>0)
		{
				i=0;
				while(i<modcnt)
				{
						fprintf(objfp,"\nM%06X05",modlist[i]+1);
						i++;
				}
		}
		fprintf(objfp,"\nE%06X",endrecord);
		fclose(lstfp);
		fclose(objfp);
}

int Progaddr(char* input)
{
	char buf1[MAX];
	int tempnum=0;
	int wnum=0;
	wnum=sscanf(input, "%s %X",buf1,&tempnum);
	if(wnum != 2)
	{
		return 0;
	}
	else
	{
		startaddr = tempnum;
		return 1;
	}
}

int Loader(char* input)
{
	char fname[MAX];
	int index=7;
	int maxindex=strlen(input)-1;
	while(index<maxindex)
	{
		if(input[index] == ' ')
		{
			++index;
			continue;
		}
		sscanf(input+index,"%s",fname);
		index+=strlen(fname);
		//function to open specific file and read length from first line
		if(!Extsymtab(fname))
		{
			return 0;
		}
		//
		++index;
	}
	index=7;
	while(index<maxindex)
	{
		if(input[index] == ' ')
		{
			++index;
			continue;
		}
		sscanf(input+index,"%s",fname);
		index+=strlen(fname);
		//function to open specific file and read length from first line
		LoadMem(fname);
	//loading it on the memory@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		ModMem(fname);
		//
		++index;
	}//completed making external symbol table

	return 1;
}

int Extsymtab(char* filename)
//creates a external symbol table nodes for a single object file
{
	char buf[MAX];
	char csectname[10];
	char symname[10];
	int addrtemp=0;
	int length=0;
	int index=1;
	int wnum=0;
	FILE* fp;
	memset(buf,0,sizeof(char)*MAX);
	if(!(fp = fopen(filename,"r")))
	{
		return 0;
	}
	if(!fgets(buf,MAX,fp))
	//read first line for recognizing csect name and head record
	{
		printf("file head record not in format!\n");
	}
	sscanf(buf+1,"%s %06X%06X",csectname,&addrtemp,&length);
	AddCsect(csectname,addrtemp,length);
	totallength += length;
	fgets(buf,MAX,fp);
	while(sscanf(buf+index,"%s %06X",symname,&addrtemp)!= -1)
	{
		if(!AddEsym(symname, addrtemp))
		{
			return 0;
		}
		index += 12;
	}
	startaddr += length;
	//at the end of the external symboltable creation of the obj file, increase the starting address
}

void AddCsect(char* csectname, int addrtemp, int length)
{
	ESYM* newnode = (ESYM*)malloc(sizeof(ESYM));
	ESYM* currnode = esymboltable;
	strcpy(newnode->csect, csectname);
	memset(newnode->symname,0,sizeof(char)*10);
	newnode->address = (startaddr+addrtemp);
	newnode->length = length;
	newnode->link = NULL;


//newnode initialization
	if(currnode == NULL)
	{
		esymboltable = newnode;
		return;
	}
	while(currnode->link != NULL)
	{
		currnode = currnode->link;
	}
	currnode->link = newnode;
	return;
}

int AddEsym(char* symname, int addrtemp)
{
	ESYM* newnode = (ESYM*)malloc(sizeof(ESYM));
	ESYM* currnode=esymboltable;
	memset(newnode->csect,0,sizeof(char)*10);
	strcpy(newnode->symname,symname);
	newnode->address = (startaddr+addrtemp);
	newnode->length = -1;
	newnode->link = NULL;
	if(currnode == NULL)
	{
		printf("objectfile has no preceding program label!\n");
		return 0;
	}
	while(currnode->link !=NULL)
	{
		currnode = currnode -> link;
	}
	currnode->link = newnode;
	return 1;
}

int AddLsymtab(int index, unsigned int address)
{
	LSYM* newnode = (LSYM*)malloc(sizeof(LSYM));
	LSYM* currnode = lsymboltable;
	int curridx=1;
	newnode->laddress = address;
	if(lsymboltable == NULL)
	{
		lsymboltable = newnode;
		return 1;
	}
	while(++curridx < index)
	{
		if(currnode->link == NULL)
		{
			printf("error : node number not right!\n");
			return 0;
		}
		currnode = currnode->link;
	}
	currnode->link = newnode;
	return 1;
}

void PrintExtsymtab()
{
		ESYM* currnode = esymboltable;
		if(currnode == NULL)
		{
				printf("Empty external symbol node!\n");
				return;
		}
		printf("\tcontrol\t\tsymbol\t\taddress\t\tlength\n");
		printf("\tsection\t\tname\n");
		printf("\t--------------------------------------------------\n");
		while(currnode != NULL)
		{
				printf("\t%s\t\t%s\t\t%04X",currnode->csect,currnode->symname,currnode->address);
				if(currnode->length != -1)
				{
					printf("\t\t%04X",currnode->length);
				}
				printf("\n");
				currnode = currnode -> link;
		}
		printf("\t--------------------------------------------------\n");
		printf("\t\t\t\t\ttotal length\t%04X\n",totallength);
		return;

}

void PrintLsymtab()
{
		LSYM* currnode = lsymboltable;
		if(currnode == NULL)
		{
				printf("Empty local symbol node!\n");
				return;
		}
		printf("\tladdress\n");
		printf("\t--------------------------------------------------\n");
		while(currnode != NULL)
		{
				printf("\t%04X",currnode->laddress);
				printf("\n");
				currnode = currnode -> link;
		}
		printf("\t--------------------------------------------------\n");
		return;

}

void ResetEsym()
{
		ESYM* deletenode = etemp;
		while(etemp != NULL)
		{
				deletenode = etemp;
				etemp = etemp->link;
				deletenode->link = NULL;
				free(deletenode);
		}
		return;

}

void SetBp(char* input)
{
	int address=0;
	BP* newnode = (BP*)malloc(sizeof(BP));
	BP* currnode = bphead;
	BP* tempnode=NULL;
	if(sscanf(input+3,"%X",&address)==-1)
	{
		printf("input not a decimal number!\n");
		return;
	}
	//tokenize input and get address
	newnode->bp = address;
	if(currnode == NULL)
	{
		bphead = newnode;
		return;
	}
	if(currnode->bp > address)
	{
		newnode->link = currnode;
		bphead = newnode;
		return;
	}
	while(currnode->bp < address)
	{
		if(currnode->bp == address)
		{
			return;
		}
		if(currnode->link == NULL)
		{
			currnode->link = newnode;
			return;
		}
		tempnode = currnode;
		currnode = currnode->link;
	}
	//when address is between two nodes
	newnode->link = tempnode->link;
	tempnode->link = newnode;
	return;
}
	
void PrintBp()
{
		BP* currnode = bphead;
		if(currnode == NULL)
		{
				printf("Empty breakpoint node!\n");
				return;
		}
		printf("\t\tbreakpoint\n");
		printf("\t\t----------\n");
		while(currnode != NULL)
		{
				printf("\t\t%04X\n",currnode->bp);
				currnode = currnode -> link;
		}
		return;

}

void LoadMem(char* fname)
//fname contains the name of the obj file ex) "proga.obj"
{
	FILE* fp;
	char buf[MAX];
	char pname[MAX];
	unsigned int index=0, maxindex=0, paddress=0, taddress=0;
	int hb=0;
	memset(buf,0,sizeof(char)*MAX);
	memset(pname,0,sizeof(char)*MAX);
	fp = fopen(fname,"r");
	while(fgets(buf,MAX,fp))
	{
		index=0;
		if((buf[0] == '.'))
		{
			continue;
		}
		else if(buf[0] == 'H')
		{
			sscanf(buf+1,"%s",pname);
			paddress = GetCsect(pname);
		}
		else if(buf[0] == 'E')
		{
			break;
		}
		else if(buf[0] == 'T')
		{
			sscanf(buf+1,"%06X%02X",&taddress,&maxindex);
			index+=4;
			while(sscanf(buf+1+(index*2),"%02X",&hb) != -1)
			{
				EditMem(paddress+taddress+index-4,hb);
				index++;
			}
		}
	}
	return;
}

int GetCsect(char* csectname)
{
	ESYM* currnode = esymboltable;
	while(currnode!=NULL)
	{
		if(!strcmp(currnode->csect,csectname))
		{
			return currnode->address;
		}
		if(currnode->link ==NULL)
		{
			printf("error : program name not found!\n");
			return -1;
		}
		currnode = currnode->link;
	}
	return 0;
}

int GetEsymtab(char* input)
{
	ESYM* currnode = esymboltable;
	if(currnode == NULL)
	{
		printf("error : empty external symbol table!\n");
		return -1;
	}
	while(currnode->link != NULL)
	{
		if(!strcmp(currnode->symname,input))
		{
			return currnode->address;
		}
		currnode = currnode->link;
	}
	if(!strcmp(currnode->symname,input))
	{
		return currnode->address;
	}
	else
	{
		printf("error : node not found!\n");
		return -1;
	}
}

int GetLsymtab(int index)
{
	LSYM* currnode = lsymboltable;
	int curridx=1;
	if(currnode == NULL)
	{
		printf("error : local symbol table empty!\n");
		return -1;
	}
	while(curridx < index)
	{
		currnode = currnode->link;
		++curridx;
	}
	return currnode->laddress;
}

void ModMem(char* fname)
{
	FILE* fp;
	char buf[MAX];
	char pname[MAX];
	char refname[10];
	unsigned int index=0, modlength=0, paddress=0, taddress=0, sindex=1, disp=0;
	int hb=0;
	memset(buf,0,sizeof(char)*MAX);
	memset(pname,0,sizeof(char)*MAX);
	memset(refname,0,sizeof(char)*10);
	fp = fopen(fname,"r");
	while(fgets(buf,MAX,fp))
	{
		index=0;
		if((buf[0] == '.'))
		{
			continue;
		}
		else if(buf[0] == 'H')
		{
			sscanf(buf+1,"%s",pname);
			paddress = GetCsect(pname);
			AddLsymtab(sindex,paddress);
			//00 local symtable node becomes the head node
		}
		else if(buf[0] == 'D')
		{
			while(sscanf(buf+1+(index*12),"%s %06X",refname,&taddress)!=-1)
			{
				index++;
				//AddDsymtab(sindex,taddress);
				//move to link index-1 times to add local symtable.
			}
		}
		else if(buf[0] == 'R')
		{
			while(sscanf(buf+1+(index*8),"%02X%s",&sindex,refname)!=-1)
			{
				index++;
				AddLsymtab(sindex,GetEsymtab(refname));
				//move to link index-1 times to add local symtable.
			}
		}
		else if(buf[0] == 'M')
		{
		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@change needed!
			sscanf(buf+1,"%06X%02X",&taddress,&modlength);
			/*while(sscanf(buf+1+(index*2),"%02X",&hb) != -1)
			{
				EditMem(paddress+taddress+index-4,hb);
				index++;
			}*/
			if(modlength == 5)
			//format4 modification
			{
				disp = (mem[taddress]%16)*65536+mem[taddress+1]*256+mem[taddress+2];
			}
			else
			//format3 modification
			{
				disp = mem[taddress]*65536+mem[taddress+1]*256+mem[taddress+2];
			}
			sscanf(buf+10,"%02X",&sindex);
			if(buf[9] == '+')
			{
				disp += GetLsymtab(sindex);
			}
			else
			{
				disp -= GetLsymtab(sindex);
			}
			if(modlength == 5)
			{
				mem[taddress]/= 16;
				mem[taddress] += disp%1048576/65536;
				mem[taddress+1] = disp%65536/256;
				mem[taddress+2] = disp%256;
			}
			else
			{
				mem[taddress] = disp%16777216/65536;
				mem[taddress+1] = disp%65536/256;
				mem[taddress+2] = disp%256;
			}
		}
	}
	return;
}
