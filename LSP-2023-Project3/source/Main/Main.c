

#include "Main.h"


int main(void)
{
    char prompt[MAX_BUFFER];
    char argument[MAX_BUFFER];

    int getResult;

    E_Command_List commandType;


    while(1)
    {
        memset(prompt, 0, MAX_BUFFER);
        
        for(int i = 0; i < MAX_ARG_CNT; i++)
        {
            memset(arguments[i], 0, MAX_BUFFER);
        }

        memset(argument, 0, MAX_BUFFER);

        printf("%s> ", Student_Number);
        fgets(prompt, MAX_BUFFER, stdin);

        fflush(stdin);

        if(prompt[strlen(prompt) - 1] != '\n')
        {
            fprintf(stderr, "Too much long input\n");
            
            exit(1);
        }

        prompt[strlen(prompt) - 1] = '\0';

        if(strcmp(prompt, "") == 0)
        {
            continue;
        }

        // 입력 인자 획득
        getResult = Get_Argument(prompt, argument);
        if(getResult == false)
        {
            fprintf(stderr, "Fail to get argument\n");

            exit(1);
        }

        // 명령어 확인
        commandType = Get_Command(prompt);

        // 명령어 실행
        Run_Command(commandType, argument, prompt);
    }
}


int Get_Argument(char *prompt, char *argument)
{
    char buf[MAX_BUFFER];

    char *strPtr;


    memset(buf, 0, MAX_BUFFER);

    strcpy(buf, prompt);

    strPtr = strtok(buf, " ");
    if(strPtr == NULL)
    {
        return false;
    }

    strPtr = strtok(NULL, " ");
    if(strPtr == NULL)
    {
        strcpy(argument, "");
    }
    else
    {
        strcpy(argument, strPtr);
    }

    return true;
}


int Get_Command(char *prompt)
{
    char buf[MAX_BUFFER];

    char *strPtr;

    
    memset(buf, 0, MAX_BUFFER);

    strncpy(buf, prompt, MAX_BUFFER);

    strPtr = strtok(buf, " ");

    for(int i = 0; i < E_Command_Cnt; i++)
    {
        if(strcmp(strPtr, CommandList[i]) == 0)
        {
            return i;
        }
    }

    return E_Invalid_Command;
}


void Run_Command(E_Command_List commandType, char *argument, char *prompt)
{
    switch(commandType)
    {
        case E_add:
        {
            Add(argument, prompt);
        }
        break;

        case E_delete:
        {
            Delete(argument);
        }
        break;

        case E_tree:
        {
            Tree(argument);
        }
        break;

        case E_help:
        {
            Help();
        }
        break;

        case E_exit:
        {
            exit(0);
        }
        break;

        default:
        {
            Help();
        }
        break;
    }
}