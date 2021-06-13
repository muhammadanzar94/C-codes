/*
This Code is written By Muhammad Anzar
email me at: muhammadanzar94@gmail.com
for C, CPP, Java Related services or more.


Linux Shell replica using execvp commands in C language
works almost the same like a shell 
Does all the commands like 
exit, cd, pwd, ls, echo
*/
#define _GNU_SOURCE
#include<sys/wait.h> 
#include<stdlib.h>
#include<string.h> 
#include<unistd.h> 
#include<stdio.h> 
#include <fcntl.h>
#include<sys/types.h> 
#include <sys/time.h>
#include <sys/resource.h>

  
#define MAXLETTERS 1000 // max number of letters
// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

#define LISTSIZE 100 // max number of commands

  
  int fd;
// clear screen before start
void init_program() 
{ 
    clear(); 
} 

//to read a line from the console

char* lsh_read_line(char* line,ssize_t *size)
{
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  
  *size=getline(&line, &bufsize, stdin);
  if (*size == -1){
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }
  return line;
}
//take input from user
 void takeInput(char * str) 
{  
     size_t len = 0;
     printf("\n>>> ");
     strcpy(str,lsh_read_line(str,&len));
    str[len-1]='\0';
   
} 
  
// Function to print Curr Dir 
void printDir() 
{ 
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("\nDir: %s", cwd); 
} 
  
  //check if a command includes '&' sign
int hasParallelcmds(char **parsed)
{
int i;
for(i=0;parsed[i]!=NULL;i++)
{
if(strcmp(parsed[i],"&")==0)
{
 if(parsed[i+1]!=NULL)
	return 1; 
 else 
	return 0;
}
}
return 0;
}  

//to count the number of commands to be executed in parallel
int countParallelStatements(char **parsed){
int i;

int cmds=1;
for(i=0;parsed[i]!=NULL;i++)
{
if(strcmp(parsed[i],"&")==0)
{
cmds++;
}
}
return cmds;
}

//to run commands in parallel
void execvpParallel(char** parsedStr){

    //forking a child 
    pid_t pid = fork();  
    
    if (pid == -1) { 
        char error_message[30] = "An error has occurred\n"; 
        write(STDERR_FILENO, error_message, strlen(error_message));
        return; 
    } else if (pid == 0) { 

        if (execvp(parsedStr[0],parsedStr) < 0) {
        char error_message[30] = "An error has occurred\n"; 
        write(STDERR_FILENO, error_message, strlen(error_message));
        }

        exit(0); 
    } else {

        //Waiting for the child thread to terminate 
        wait(NULL);  
        return; 
    }
    
}

    
// Function where the system command is executed using execvp
void execvpFunction(char** parsedStr) 
{ 
if(hasParallelcmds(parsedStr)==0){
    //forking a child 
    pid_t pid1 = fork();  
    
    if (pid1 == -1) { 
        char error_message[30] = "An error has occurred\n"; 
        write(STDERR_FILENO, error_message, strlen(error_message));
        return; 
    } else if (pid1 == 0) { 

        if (execvp(parsedStr[0], parsedStr) < 0) { 
        char error_message[30] = "An error has occurred\n"; 
        write(STDERR_FILENO, error_message, strlen(error_message));
           
        }

        exit(0); 
    } else { 
        //Waiting for the child thread to terminate 
        wait(NULL);  
        return; 
    } 
}

//if has more than one commands

else
{

//number of commands
countParallelStatements(parsedStr);

//int i;
char * temp[LISTSIZE];
//char * temp2[LISTSIZE];
strcpy(temp[0],parsedStr[0]);
temp[1]=(char *)0;

execvpParallel(temp);

}}

// Help command builtin 
void openHelp() 
{ 
    puts("\n|*|*|*|WELCOME TO MY SHELL HELP|*|*|*|\n"
        "\nList of Commands supported:"
        "\n=>Our self made shell can do many things"
        "\n=>it has two modes\n"
        "\n1-interactive mode\n2- batch mode for input"
        "\n=>it both modes it can run built in commands like(cd, exit, help)"
        "\n=>more over it can also run all system commands"
        "\n=>it can also output any command into a file"
        ); 
  
    return; 
} 
  
// Function to execute builtin commands 
int BuiltInCMDHandler(char** parsed) 
{ 
    int NoOfOwnCmds = 4, i, switchOwnArg = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
    
    ListOfOwnCmds[0] = "exit"; 
    ListOfOwnCmds[1] = "cd"; 
    ListOfOwnCmds[2] = "help"; 
    ListOfOwnCmds[3] = "path";
  
    for (i = 0; i < NoOfOwnCmds; i++) { 
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) { 
            switchOwnArg = i + 1; 
            break; 
        } 
    } 
  
    switch (switchOwnArg) { 
    case 1: 
        exit(0); 
    case 2: 
        chdir(parsed[1]); 
        return 1;
    case 3: 
        openHelp(); 
        return 1; 
    case 4: 
        chdir(parsed[1]); 
        return 1;
    default: 
        break; 
    } 
  
    return 0; 
} 
  
  

//to store stdout reference
  int current_out;
    
    
// function to output IN a FILE
void OutputInFile(char** parsed)
{
	int i=0;
	while(parsed[i]!=NULL)
	{
	if(strcmp(parsed[i],">")==0)
	{
	
	//getting the file name to be printed in
	char* filename=parsed[i+1];
	
	//now decreasing the char** parsed upto '>' sign
	parsed[i]=NULL;
	
	
	//create or truncate both are used
	fd =open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

	current_out = dup(1);

        dup2(fd, 1);   // make stdout go to file

	close(fd);
        break;
    	
    	}
    
  
	i++;
	}
	}


  

  
//helper function of parseStringFunction.
//it parses the commands
void parseSpace(char* str, char** parsedStr) 
{ 
    int i; 
  
    for (i = 0; i < LISTSIZE; i++) { 
        parsedStr[i] = strsep(&str, " "); 
  
  	// At the end of the string
        if (parsedStr[i] == NULL) 
            break; 
            
            //for ignoring extra spaces
        if (strlen(parsedStr[i]) == 0) 
            i--; 
    }
    OutputInFile(parsedStr);
    
} 
  
  //function to parse string and process it
int parseStringFunc(char* str, char** parsed) 
{ 
  
        parseSpace(str, parsed); 
  
    if (BuiltInCMDHandler(parsed)) 
        return 0; 
    else
  	return 1;
} 
 

    #define MAXCHAR 1000
int main(int argc, char **argv)
{ 
    char inStr[MAXLETTERS];
    char *prsedStr[LISTSIZE]; 
    int execFlag = 0; 
    init_program(); 
    
    
    // for batch more
    if(argc==2){
    FILE *fp = fopen(argv[1], "r"); //opening file for reading 
   
    //returns NULL if file is not present
    if(fp==NULL){
    printf("No file found of given name");
        char error_message[30] = "An error has occurred\n"; 
        write(STDERR_FILENO, error_message, strlen(error_message));
    return 1;}
    //if file is opened succesfully
    else
    {
    char str[MAXCHAR];
 
    while (fgets(str, MAXCHAR, fp) != NULL)
     {   
        printf("%s", str);
    
    const char ch[] = "\n";  
    char* token;
    token = strtok(str, ch); 
        
        execFlag = parseStringFunc(str,prsedStr); 
  
        // execute command 
        if (execFlag == 1) {
        //execution of commands function
            execvpFunction(prsedStr); 
        }
    }
   
   
    fclose(fp);
    }
  }  
  
  //for Interactive more
  
  else if(argc==1){
    while (1) { 
         
        printDir(); 
        
        // input string 
        takeInput(inStr);
        
        // process the string 
        execFlag = parseStringFunc(inStr, 
        prsedStr); 
  
  	// execute command 
        if (execFlag == 1) 
        //execution of commands function
            execvpFunction(prsedStr); 

//return reference back to stout from file
dup2(current_out, 1);
    

 } 
    } 
    return 0; 
} 
