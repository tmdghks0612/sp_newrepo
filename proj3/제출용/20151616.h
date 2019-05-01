#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX 1000

char* instruction[] = {"help", "dir", "quit", "history", "dump", "edit", "fill", "reset", "opcode ", "opcodelist", "h", "d", "q", "hi", "du", "e", "f", "assemble filename", "type filename", "symbol", "progaddr","loader ","bp ","bp"};

typedef struct __node
{
	char InputString[MAX];
	struct __node* link;
} NODE;

typedef struct __bpnode
{
	unsigned int bp;
	struct __bpnode* link;
} BP;

typedef struct HASH__node
{
	unsigned int opcode;
	char opinstruction[6];
	int format;
	struct HASH__node* link;
} HASH;

typedef struct SYM__node
{
	unsigned int loc;
	char subr[10];
	struct SYM__node* link;
} SYM;

typedef struct ESYM__node
{
	char csect[10];
	char symname[10];
	unsigned int address;
	unsigned int length;
	struct ESYM__node* link;
} ESYM;

typedef struct LSYM__node
{
	int laddress;
	struct LSYM__node* link;
} LSYM;

typedef struct DSYM__node
{
	int daddress;
	char defname[10];
	struct DSYM__node* link;
} DSYM;

enum Reg{RegA=0,RegX=1,RegL=2,RegB=3,RegS=4,RegT=5,RegF=6,RegPC=8,RegSW=9};
enum Flags{FlagX=8,FlagB=4,FlagP=2,FlagE=1};

int IsAssemble(char* input);
void PrintInstruction();
void PrintDir();
void PrintHistory(NODE*);
void PrintSymbol();
void InputHistory(NODE**, char*);
int IsCap(char inputchar);
int IsNum(char inputchar);
int IsIname(char* iname);
//returns 0 if iname is not an instruction. references opcode.txt
int IsOper(char* oper);
//returns 0 if oper is not an operand. references symbol table
int IsFormal(int* num1, int* num2, int* addr, char* input);
//Checks if the input is in right form. If the form is not right, return 0. Depending on the form, calls the PrintMem in correct index
int IsFormalE(int* num1, int* num2, char* input);
int IsFormalF(int* num1, int* num2, int* num3, char* input);
int PrintFile(char* input);
void PrintMem(int start, int end, int* addr);
//Prints the specific memory addr from start to end. Sets the addr to end index
void PrintHex(int start, int end);
void PrintAsc(int start, int end);
void EditMem(int idx, int val);
void FillMem(int idxs, int idxe, int val);
void CreateHash();
void CreateSym(char* newsubr,int newline);
void ResetSym();
void ResetEsym();
int HashFunc(char* input);
int PrintOp(char* input);
void PrintOpList();
void PrintTable();
int GetCom(char* input);
int GetOp(char* input);
int GetForm(char *input);
int GetReg(char* input);
int GetSymLoc(char* input);
//returns the location of the symbol. if input is not in the symboltable return -1
int StrToDec(char* input);
void Pass1(char* input);
void Pass2(char* input,int endloc);
int Progaddr(char* input);
int Loader(char* input);
int Extsymtab(char* input);
void AddCsect(char* csectname, int addrtemp, int length);
int AddEsym(char* symname, int addrtemp);
int AddLsym(int index, unsigned int address);
int GetCsect(char* csectname);
int GetEsymtab(char* symname);
int GetLsymtab(int index);
void PrintExtsymtab();
void SetBp(char* input);
void PrintBp();
void LoadMem(char* fname);
void ModMem(char* fname);

unsigned char mem[1048576];
HASH** hashtable;
BP* bphead=NULL;
SYM* symboltable = NULL;
SYM* temp = NULL;
ESYM* esymboltable=NULL;
ESYM* etemp = NULL;
LSYM* lsymboltable=NULL;
int startaddr=0;
int totallength=0;



