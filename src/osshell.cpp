#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <fstream>

void splitString(std::string text, char d, std::vector<std::string>& result);
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result);
void freeArrayOfCharArrays(char **array, size_t array_length);
void printHistory(std::ofstream history);

int main (int argc, char **argv)
{
    std::ofstream historyOF;
    std::ifstream historyIF;

    // Get list of paths to binary executables
    std::vector<std::string> os_path_list;
    char* os_path = getenv("PATH");
    splitString(os_path, ':', os_path_list);

    ///////////////////////DONT INCLUDE/////////////////////////////////////////////////////
    //Shows how to loop over the directories in the PATH environment vairable
    int i;
    for(i = 0; i < os_path_list.size(); i++)
    {
        printf("PATH[%2d]: %s\n", i, os_path_list[i].c_str());
    }
    /////////////////////////////////DONT INCLUDE THIS IN FINAL/////////////////////////////

    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");


    std::vector<std::string> command_list; //to store command user types in, split into its variour parameters
    char **command_list_exec; //command_list convered to an array of character arrays

    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //   If yes, execute it
    //   If no, print error statement: "<command_name>: Error command not found" (do include newline)
    while(true){
        std::cout << "osshell> ";

        std::string input;
        std::getline(std::cin, input);
    
        if(input == "exit"){
            historyOF.open("history.txt",std::ios_base::app);
            history << "exit\n";
            historyOF.close();
            // Free allocated memory
            //freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
            break;
        }
        else if(input == "history"){
            //vectorOfStringsToArrayOfCharArrays(command_list, &command_list_exec);
            historyIF.open("history.txt",std::ios::in);
            std::string currLine;
            while(getline(history,currLine))
            {
                std::cout << currLine;
            }
            history.close();
            history.open("history.txt",std::ios::app);
            history << "history\n";
            history.close();
        }
        else if(input != ""){
            //split the input from the user
            //write a method to check if there is an executable by that name in Path directory
            //if so, execute it
            //else, print error statement
        }
    }

    return 0;
}

/*
   array_ptr: pointer to list of strings to be allocated
   array_length: number of strings to allocate space for in the list
   item_size: length of each string to allocate space for
*/
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result)
{
    int i;
    int result_length = list.size() + 1;
    *result = new char*[result_length];
    for(i = 0; i < list.size();i++)
    {
        (*result)[i] = new char[list[i].length() + 1];
        strcpy((*result)[i], list[i].c_str());
    }
    (*result)[list.size()] = NULL;
}

/*
   array: list of strings to be freed
   array_length: number of strings in the list to free
*/
void freeArrayOfCharArrays(char **array, size_t array_length)
{
    int i;
    for (i = 0; i < array_length; i++)
    {
        if(array[i] != NULL)
        {
            delete[] array[i];
        }
    }
    delete[] array;
}

/*
   text: string to split
   d: character delimiter to split `text` on
   result: NULL terminated list of strings (char **) - result will be stored here
*/
void splitString(std::string text, char d, std::vector<std::string>& result)
{
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::string token;
    result.clear();
    for(i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch(state){
            case NONE:
                if(c != d){
                    if(c == '\"'){
                        state = IN_STRING;
                        token = "";
                    }
                    else{
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d){
                    result.push_back(token);
                    state = NONE;
                }
                else{
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"'){
                    result.push_back(token);
                    state = NONE;
                }
                else{
                    token += c;
                }
                break;
        }
    }
    if(state != NONE)
    {
        result.push_back(token);
    }
}

//function to find executable
bool executableExists(std::string pathName){
    //either stat library or filesystem
}