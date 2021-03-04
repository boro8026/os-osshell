#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <filesystem>
#include <sys/wait.h>

void splitString(std::string text, char d, std::vector<std::string>& result);
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result);
void freeArrayOfCharArrays(char **array, size_t array_length);
std::string executableExists(std::string command, std::vector<std::string> pathEnv);
void deleteTopLinesIfNeeded();
bool isNumber(std::string input);
int countHistoryLines();
void printHistoryToInt(int totalLines, int numLines);
void printTotalHistory();

int main (int argc, char **argv)
{
    std::ofstream historyOF;
    std::ifstream historyIF;
    int pid;
    int parentPid;

    // Get list of paths to binary executables
    std::vector<std::string> os_path_list;
    char* os_path = getenv("PATH");
    splitString(os_path, ':', os_path_list);

    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");


    std::vector<std::string> command_list; //to store command user types in, split into its variour parameters
    char **command_list_exec; //command_list converted to an array of character arrays

    while(true){
        std::cout << "osshell> ";

        std::string input;
        std::getline(std::cin, input);
        splitString(input,' ',command_list);
    
        if(input == "exit"){ //if exit is entered
            deleteTopLinesIfNeeded();
            historyOF.open("history.txt",std::ios_base::app);
            historyOF << "exit\n";
            historyOF.close();
            freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
            break;
        }
        else if(command_list[0] == "history"){
            if(command_list.size() == 2){
                if(command_list[1] == "clear"){ //clearing history
                    remove("history.txt");
                }
                else if(isNumber(command_list[1])){ //if there is a select number of history
                    int num = std::stoi(command_list[1]);
                    std::string currLine;
                    int numLines = countHistoryLines();
                    printHistoryToInt(numLines,num);
                    deleteTopLinesIfNeeded();
                    historyOF.open("history.txt",std::ios::app);
                    historyOF << "history " << num << "\n";
                    historyOF.close();
                }
                else{ //if an invalid history call
                    historyOF.open("history.txt",std::ios::app);
                    historyOF << input << "\n";
                    historyOF.close();
                    std::cout << "Error: history expects an integer > 0 (or clear)" << std::endl;
                }
            }
            else if(command_list.size() == 1){ //if a normal history
                printTotalHistory();
                deleteTopLinesIfNeeded();
                historyOF.open("history.txt",std::ios::app);
                historyOF << "history\n";
                historyOF.close();
            }
            else{ //if an invalid history call
                historyOF.open("history.txt",std::ios::app);
                historyOF << input << "\n";
                historyOF.close();
                std::cout << "Error: history expects an integer > 0 (or clear)\n";
            }
        }
        else if(input != ""){ //any other call
            deleteTopLinesIfNeeded();
            historyOF.open("history.txt",std::ios::app);
            historyOF << input << "\n";
            historyOF.close();

            vectorOfStringsToArrayOfCharArrays(command_list,&command_list_exec);
            std::string path = executableExists(command_list[0],os_path_list); //gets path if exectuable
            if(path != ""){
                pid = fork();
                if(pid == 0){
                    execv(path.c_str(),&command_list_exec[0]);
                }
                else{
                    while(wait(&parentPid) != pid); //while the parent pid is not equal to the childs pid
                }
            }
            else{
                std::cout << input << ": Error command not found" << std::endl;
            }
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
   result: vector of strings - result will be stored here
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
std::string executableExists(std::string command, std::vector<std::string> pathEnv){
    for(int index = 0; index < pathEnv.size(); index++){
        std::filesystem::path thisPath(pathEnv[index].append("/").append(command));
        if(std::filesystem::exists(thisPath) == 1){
            return pathEnv[index];
        }
    }
    return "";
}

//Deletes the oldest lines in history.txt down to 127 total lines
void deleteTopLinesIfNeeded(){
    std::ifstream file;
    std::ofstream writingFile;
    std::string currLine;
    std::vector<std::string> holdLines;

    int numLines = countHistoryLines();
    if(numLines >= 128)
    {
        remove("history.txt");
        writingFile.open("history.txt",std::ios::app);
        for(int index = 0; index < numLines; index++)
        {
            if(index > numLines - 128)
            {
                writingFile << holdLines[index] << "\n";
            }
        }
        writingFile.close();
    }
}

//returns the if inputted string is a number > 0
bool isNumber(std::string input){
    for(int index = 0; index < input.size(); index++){
        if(!isdigit(input[index])){
            return false;
        }
    }
    return true;
}

//returns the total  number of lines in history.txt
int countHistoryLines(){
    std::string currLine;
    std::ifstream historyIF;
    int numLines = 0;
    historyIF.open("history.txt",std::ios::in);
    getline(historyIF,currLine);
    while(currLine != "")
    {
        getline(historyIF,currLine);
        numLines++;
    }
    historyIF.close();
    return numLines;
}

//Print the last lines of history.txt equal to the differnece of totalLines - numLines
void printHistoryToInt(int totalLines, int numLines){
    std::ifstream historyIF;
    std::string currLine;
    historyIF.open("history.txt",std::ios::in);
    for(int index = 0; index < totalLines; index++){
        getline(historyIF,currLine);
        if(index >= totalLines - numLines){
            std::cout << "  " << index+1 << ": " << currLine << std::endl;
        }
    }
    historyIF.close();
}

//Prints all lines of history.txt
void printTotalHistory(){
    std::ifstream historyIF;
    std::string currLine;
    int lineNum = 1;
    historyIF.open("history.txt",std::ios::in);
    while(getline(historyIF,currLine))
    {
        std::cout << "  " << lineNum << ": " << currLine << std::endl;
        lineNum++;
    }
    historyIF.close();
}