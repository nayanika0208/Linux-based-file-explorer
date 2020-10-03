
#include <unistd.h>
#include<bits/stdc++.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
using namespace std;


#define MOVE_CURSER printf("%c[%d;%dH", 27, posx, posy)

extern int g_argc; //global argc.
extern char** g_argv; //global argv.
extern char root[4096]; //global root variable.
extern char cur_directory[4096]; //to maintain current dir.
extern vector<string> directoryList; //global vector for dirctory names.
extern stack<string> back_stack; //to store previously visited dir.
extern stack<string> forw_stack; //opposite of back_stack.
extern int totalFiles;
extern vector<char> command_string; //to store char by char info in command mode.
extern char homepath[4096];
extern unsigned int cx; //curser position x.
extern unsigned int cy; //curser position y.
extern unsigned int term_row_num; //number of row in terminal.
extern unsigned int term_col_num; //number of col in terminal.
extern int cur_window; //helper variable in scrolling.
extern struct winsize terminal; //to get terminal window config.
extern struct termios raw, newraw; //to get terminal configuration.
extern vector<string> my_command; //store command tokens in command mode.
extern ofstream fout; //for storing snapshot, to open file.


// void openDirectory(const char *path);
// // void display(const char *dir_name, const char *root);
// int getFilePrintingcount();
// void clearStack(stack<string> &s);
// void navigate();
// int isdirectory(char *newpath);
// string getFileNameFromPath(string newDataw);
// int listdir(const char*);
// void display(const char*);

int directory_Listing(const char*);
void display(const char*);
void enableNCanon();
string SplitFilename(string);
void UpArrow();
void DownArrow();
void LeftArrow();
void RightArrow();
void HomeKey();
void EnterKey();
void BackspaceKey();
int command_mode();
void split_command();
void function_call();
int my_copy();
void file_copy(string, string);
void directory_copy(string, string);
void my_move();
void my_rename();
void create_file();
void create_dir();
void delete_file();
void delete_recursive(string);
void delete_dir();
void my_goto();
void my_search();
void snapshot_file(string, string);
void snapshot();
string create_absolute_path(string);
void reset_curser_top();
void update_list();
bool isDirectory(string);
int isFileExist(string);

void DeleteSingleFile(string path);
 void removeFiles(vector<string> list);
 void DeleteSingleDir(string dirToDel);
 void removeDirectories(vector<string> cmdList);
 void copySingleFile(string fromFile, string toFile);
 void CopySingleDirectory(string from, string to);
 bool my_copy(vector<string> cmdList);
 void createFile(vector<string> cmdList);
 void createDirectory(vector<string> cmdList);
void renameFiles(vector<string> list);

void movecommand(vector<string> list);
void enableRawMode();
