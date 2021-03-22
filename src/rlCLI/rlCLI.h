

#ifndef __RL_CLI_H__
#define __RL_CLI_H__

void cli_init();
void my_cli();

void setCliCmd(int *hashList,int (**arr)(char (*)[64]),int count);
void suspendCli();
int genHash( const char *cmd );


#endif //RL_CLI_H