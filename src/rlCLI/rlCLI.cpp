#include <Arduino.h>
#include "rlCLI.h"

#define LINE_BUF_SIZE 128   //Maximum input string length
#define ARG_BUF_SIZE 64     //Maximum argument string length
#define MAX_NUM_ARGS 4      //Maximum number of arguments
bool error_flag = false;
char line[LINE_BUF_SIZE];
int lineLength=0;
bool newLine=false;
bool prompt=true;
char args[MAX_NUM_ARGS][ARG_BUF_SIZE];
//List of command names

int (**cmd)(char (*)[64]);
int *hashCmd;
int numCommands = 0;
bool suspendCLI=false;


void read_line();
void parse_line();
int execute();  

void cli_init() {
    //viewFifo = new CliFifo();
    Serial.println("Welcome to R-Link command line interface.");
    Serial.println("Type \"help\" to see a list of commands.");
}

void my_cli() {
    if (prompt){
        Serial.print("> ");
        prompt=false;
    }
    if (suspendCLI){
        if (Serial.available()){
            if (Serial.read()==27){
                suspendCLI=false;
                return;
            }

        }
        return;
    }
    read_line();
    if (newLine) {
        if(!error_flag) {
            parse_line();
        }
        if(!error_flag) {
            execute();
        }
    
        memset(line, 0, LINE_BUF_SIZE);
        memset(args, 0, sizeof(args[0][0]) * MAX_NUM_ARGS * ARG_BUF_SIZE);
        newLine = false;
        prompt=true;
        error_flag = false;
    }
}

void setCliCmd(int *hashList,int (**funcList)( char (*)[64]),int count){
    hashCmd=hashList;
    cmd = funcList;
    numCommands = count;
}

void read_line(){
 
 
    while(Serial.available()){
        line[lineLength] = (char)Serial.read();
        if (line[lineLength] == 13)
            break;
        lineLength++;
        if (line[lineLength-1] == 10){
            line[lineLength-1] = 0;
            lineLength=0;
            newLine=true;
            break;
        }
        
    }
    if(lineLength == LINE_BUF_SIZE && !newLine){
        Serial.println("Input string too long.");
        error_flag = true;
    }
    if (newLine){
        Serial.println(line);
    }
}
 
void parse_line(){
    char *argument;
    int counter = 0;
 
    argument = strtok(line, " ");
 
    while((argument != NULL)){
        if(counter < MAX_NUM_ARGS){
            if(strlen(argument) < ARG_BUF_SIZE){
                strcpy(args[counter],argument);
                argument = strtok(NULL, " ");
                counter++;
            }
            else{
                Serial.println("Input string too long.");
                error_flag = true;
                break;
            }
        }
        else{
            break;
        }
    }
}
 
int execute(){  
    for(int i=0; i<numCommands; i++){
        if(genHash(args[0])==hashCmd[i]){
            return (*cmd[i])(args);
        }
    }
 
    Serial.println("Invalid command. Type \"help\" for more.");
    return 0;
}



int genHash( const char *cmd ){
    int ch;
    int hash;

    ch = 0;
    hash = 0x05;
    while((ch = *cmd++))
        hash += (ch << 1);

    return hash;
}


void suspendCli(){
    suspendCLI=true;
}
