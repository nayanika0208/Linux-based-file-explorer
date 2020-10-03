#include "headers.h"

 int g_argc; //global argc.
 char** g_argv; //global argv.
 char root[4096]; //global root variable.
 char cur_directory[4096]; //to maintain current dir.
 vector<string> directoryList; //global vector for dirctory names.
 stack<string> back_stack; //to store previously visited dir.
 stack<string> forw_stack; //opposite of back_stack.
 int totalFiles;
 vector<char> command_string; //to store char by char info in command mode.
char homepath[4096];
 unsigned int term_row_num; //number of row in terminal.
 unsigned int term_col_num; //number of col in terminal.
  unsigned int posx; //curser position x.
 unsigned int posy; //curser position y.
 int cur_window; //helper variable in scrolling.
 struct winsize terminalWindow; //to get terminal window config.
 struct termios raw, newraw; //to get terminal configuration.
 vector<string> my_command; //store command tockens in command mode.
 ofstream fout; //for storing snapshot, to open file.
 

int FilesToPrint()
{
    int lenRecord;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminalWindow);
    term_row_num= terminalWindow.ws_row - 2;
    term_col_num=terminalWindow.ws_col;
    if (totalFiles <= term_row_num)
    {
        lenRecord = totalFiles;
    }
    else
    {
        lenRecord = term_row_num;
    }
    return lenRecord;
}

//checking if the path denotes a directory or not
bool isDirectory(const char*path){
   

    struct stat Stat;
     stat(path, &Stat);

     return ((Stat.st_mode & S_IFMT) == S_IFDIR)?true:false;
}
//checking if the path denotes a directory or not
bool isRegularFile(const char*path){
   

    struct stat Stat;
     stat(path, &Stat);

     return ((Stat.st_mode & S_IFMT) == S_IFREG)?true:false;
}



   //************************************************************************* command mode******************
//*******************************************************************************************************
//********************************************************************************************

   //*******************************************************************************************
//*******************************************************************************************************
//**

void split_chat_to_string()
{

    string temp = "";
    unsigned int i = 0;
    unsigned int siz=command_string.size();
    cout << endl;

    for (; i < siz ; i++) {
        //spliting the
        if (command_string[i] == ' ' || command_string[i] == '\n' ) {

            if(temp.size() > 0) {
                //push in command string 
                my_command.push_back(temp);
            }

            temp = "";
        }
        else if (command_string[i] == '\\'){
            temp =  temp+ command_string[++i];
        }
        else{
            temp = temp + command_string[i];
        }
    }
    
}
string SplitFilename(string str)
{
    size_t found;
    found = str.find_last_of("/\\");
    return str.substr(0, found);
}

/*============================================================
take relative path and convert to absolute path for internal
system calls.
=============================================================*/


string create_absolute_path( string str)
{
    string abs="";
    char firstchar = str[0];
    
    string basepath = string(root);
    if(firstchar =='/')
    {
        abs = basepath + str;
    }
    else if(firstchar=='~')
    {
        abs = basepath + str.substr(1,str.length());
    }
    else if(firstchar=='.')
    {
        abs = string(cur_directory) + str.substr(1,str.length());    
    }
    else
    {
        abs= string(cur_directory)+ "/" + str;
    }

    return abs;
}

void clearCommand()
{
    unsigned int lastLine = term_row_num -1;
    printf("%c[%d;%dH",27,lastLine,1);
    printf("%c[2K", 27);
    cout<<":";
}

int commandMode(){

    // goto_flag = false;
    //     search_flag_c = false;
        posx = terminalWindow.ws_row-1;
        posy = 1;
        MOVE_CURSER;
        printf("\x1b[0K");
        printf(":");
        fflush(0);
        posy++;
        char ch[3] = { 0 };
        command_string.clear();
        my_command.clear();
        while (1) {
            if (read(STDIN_FILENO, ch, 3) == 0)
                continue;
            if (ch[0] == 27 && ch[1] == 0 && ch[2] == 0) {
                posx = 1;
                posy = 1;
                MOVE_CURSER;
                return 0;
            }
            else if (ch[0] == 27 && ch[1] == '[' && (ch[2] == 'A' || ch[2] == 'B')) {
                continue;
            }
            else if (ch[0] == 27 && ch[1] == '[' && (ch[2] == 'C' || ch[2] == 'D')) {
                continue;
            }
            else if (ch[0] == 10) {
                command_string.push_back('\n');

                split_chat_to_string();

               string s = my_command[0];
                if (s == "copy"){


                    cout<<"my_copy()"<<endl;
                    clearCommand();
                }
                else if (s == "move"){
                     cout<<"my_move()"<<endl;

                 }
                else if (s == "rename"){

                    cout<<"my_rename();"<<endl;
                    clearCommand();
                }
                else if (s == "create_file"){
                   
                      cout<<"create_file();;"<<endl;
                      clearCommand();
                }
                else if (s == "create_dir"){
                    cout<<"create_dir();"<<endl;
                    clearCommand();
                }
                else if (s == "delete_file"){
                    cout<<"delete_file();"<<endl;
                    clearCommand();
                }
                else if (s == "delete_dir"){
                    cout<<"delete_dir();"<<endl;
                    clearCommand();
                }
                else if (s == "goto"){

          
                    string my_path ="";

                    if(my_command.size() != 2){
                         cout<<"Invalid Argument in Goto";;
                     }else{
                        my_path = create_absolute_path(my_command[1]);

                     }


                    back_stack.push(string(cur_directory));
                    while(!forw_stack.empty()){
                        forw_stack.pop();
                    }
                    strcpy(cur_directory, my_path.c_str());
                    
                    clearCommand();
                    return 1;
                }
                else if (s == "search"){
                   cout<<" my_search();;"<<endl;
                   clearCommand();
                }
                else if (s == "snapshot"){
                    cout<<"snapshot();;"<<endl;
                    clearCommand();
                }
                else {
                    
                    cout << "command not found." << endl;
                    clearCommand();
                }
                // if (goto_flag)
                //     return 1;
                // if (search_flag_c)
                //     return 2;
                // break;
            }
            else if (ch[0] == 127) {
                if (posy > 2) {
                    posy--;
                    MOVE_CURSER;
                    printf("\x1b[0K");
                    command_string.pop_back();
                }
            }
            else {
                cout << ch[0];
                posy++;
                MOVE_CURSER;
                command_string.push_back(ch[0]);
            }
            fflush(0);
            memset(ch, 0, 3 * sizeof(ch[0]));
        }
    
    return 0;
}
 


   //************************************************************************* enabling raw mode*******************

//*******************************************************************************************************
//********************************************************************************************

void enableRawMode(){
    //getting the initial terminal settings
    tcgetattr(STDIN_FILENO, &raw);
    newraw = raw;
    //changing the flags for entering Non Canonical mode.
    newraw.c_lflag &= ~(ICANON | ECHO | IEXTEN | ISIG);
    newraw.c_iflag &= ~(BRKINT);
    //setting the  new terminal settings and checing if it is successfull.

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &newraw) != 0)
        fprintf(stderr, "Failed in  setting  attributes\n");
    else {
        //Now we are in non canonical mode and have to check which key is pressed and work according to its functionality
        char inputKey[3];

        memset(inputKey, 0, 3 * sizeof(inputKey[0]));

        posx=1;
        posy=80;
        while (true) {
            unsigned int lastPos = terminalWindow.ws_row-1;
            printf("%c[%d;%dH", 27, lastPos ,1);
            cout << "-----NORMAL MODE-----";
            
            printf("%c[%d;%dH", 27, posx, posy);
            // printf("%c[%d;%dH", 27, 1, 80);
            unsigned int x=posx + cur_window ;
            fflush(0);
            if (read(STDIN_FILENO, inputKey, 3) == 0)
                //0 bytes are read
                continue;
            else if (inputKey[2] == 'A'){
                  

                        // if (posx > 1) {
                        //     posx--;
                        //     MOVE_CURSER;
                        // }
                        // else if (posx == 1 && posx + cur_window > 1) {
                        //    if (cur_window > 0)
                        //     {
                        //         cur_window--;
                        //     }
                        //     //listing the files window
                        //      printf("%c[2J", 27);
                        //     for (unsigned int i = cur_window; i <= term_row_num + cur_window - 1; i++)
                        //     {
                        //         string fName=directoryList[i];
                        //         display(fName.c_str());
                        //     }
                        //     MOVE_CURSER;
                        // }
                  if (posx + cur_window > 1)
                    {
                        posx--;
                        if (posx >= 1)
                        {

                            printf("%c[%d;%dH", 27, posx, posy);

                        }
                        
                        else if ( x>= 1 && posx <= 0 )
                        {
                             printf("%c[2J", 27);
                            if (cur_window > 0)
                            {
                                cur_window--;
                            }
                           
                            printf("%c[%d;%dH", 27, 1, 1);

                            unsigned int i;
                            for (unsigned int i = cur_window; i <= term_row_num + cur_window - 1; i++)
                            {
                                string fName=directoryList[i];
                                display(fName.c_str());
                            }
                            posx++;
                            printf("%c[%d;%dH", 27, posx, posy);
                        }
                    }
                    

            }
            else if (inputKey[2] == 'B'){
               int lenRecord;
                    if (x < (totalFiles))
                    {
                        posx++;
                        if (posx <= term_row_num)
                        {
                            printf("%c[%d;%dH", 27, posx, posy);
                        }
                        else if (posx > term_row_num && x <= totalFiles)
                        {
                            printf("%c[2J", 27);
                            lenRecord = FilesToPrint() - 1;
                            if (totalFiles > term_row_num)
                            {
                                cur_window++;
                            }
                            
                             printf("%c[%d;%dH", 27, 1, 1);
                             unsigned int i;
                            for (i = cur_window; i <= lenRecord + cur_window; i++)
                            {
                                string fName=directoryList[i];
                                display(fName.c_str());
                            }
                            posx--;
                        }
                         printf("%c[%d;%dH", 27, posx, posy);
                    }
            }
            else if ( inputKey[2] == 'C'){

                // printf("%c[%d;%dH", 27, posx, posy);
                //if forward stack contains someting then we should go to top directory
                if(!forw_stack.empty()){
                    back_stack.push(cur_directory);
                    string gotoD=forw_stack.top();
                     strcpy(cur_directory,gotoD.c_str());
                    forw_stack.pop();
                    
                    directory_Listing(cur_directory);
                     // printf("%c[%d;%dH", 27, posx, posy);
                }
            }
            else if (inputKey[2] == 'D'){
               //if forward stack contains someting then we should go to top directory
                // printf("%c[%d;%dH", 27, 1, 80);
                int s=back_stack.size() ;
                if(s>=2 ){

                    string gotoD=back_stack.top();
                     
                    back_stack.pop();
                    forw_stack.push(string(cur_directory));
                    strcpy(cur_directory,gotoD.c_str());
                    directory_Listing(cur_directory);
                 
                 }
            }
            else if (inputKey[0] == 'H' || inputKey[0] == 'h'){
                back_stack.push(cur_directory);
                forw_stack.push(homepath);
                directory_Listing(homepath);
            }
            //backspace key
            else if (inputKey[0] == 127){
                string s=string(cur_directory);
                int posoflastslash=0;
                for(int i=0;i<s.length();i++){
                    if(s[i]=='/'){
                        posoflastslash=i;
                    }
                }

                string parent=s.substr(0,posoflastslash);
                
                back_stack.push(cur_directory);
                forw_stack.push(parent);
                strcpy(cur_directory,parent.c_str());
                directory_Listing(cur_directory);

                
            }
            else if (inputKey[0] == 10){
                string parent = "..";
                string current = ".";
                string myDir=directoryList[cur_window + posx - 1];
                if(myDir == current){
                    continue;
                }else if(myDir == parent){
                    if(!(cur_directory == homepath)){

                        string s=string(cur_directory);
                        int posoflastslash=0;
                        for(int i=0;i<s.length();i++){
                            if(s[i]=='/'){
                                posoflastslash=i;
                            }
                        }

                        string parent=s.substr(0,posoflastslash);
                        
                        back_stack.push(cur_directory);
                        forw_stack.push(parent);
                        strcpy(cur_directory,parent.c_str());
                directory_Listing(cur_directory);
                    }                        
                }else{
                    //get the full path 
                  string fullP=string(cur_directory)+"/"+myDir;
                char *fpath = new char[fullP.length() + 1];
                strcpy(fpath , fullP.c_str());

                if (isDirectory(fpath)) {
                    back_stack.push(string(cur_directory));
                    strcpy(cur_directory, fpath);
                    // back_stack.push(string(cur_directory));
                    while (!forw_stack.empty())
                     forw_stack.pop();
                      forw_stack.push(string(cur_directory));

                     printf("%c[%d;%dH", 27, 1, 1);
                     directory_Listing(cur_directory);
                    
                }else if(isRegularFile(fpath)){
                    
                    

                    pid_t pid = fork();
                    if (pid == 0) {
                        // close(2);
                        // char *const pat=fpath;
                         // execv("vi",fpath);
                        char* arguments[3] = { "vi", fpath, NULL };
                        execvp("vi", arguments);
                        
                        
                        
                    }else{
                        int *c;
                        wait(c);
                    }
                    // wait( 2 );
                          
                }

                }
                
            }
            else if (inputKey[0] == ':') {
               
                printf("%c[%d;%dH", 27, lastPos ,1);
                printf("%c[2K", 27);
                cout << ":";
                // cout<<"going into command mode :"<<endl;
                int result = commandMode();
                posx = 1;

                printf("%c[%d;%dH", 27, posx, posy);
                if (result == 1)
                {
                    directory_Listing(cur_directory);
                    //cout<<"goto out : ";
                }
                else if (result == 2)
                {
                    //cout<<"search out : ";
                }
                else
                {
                    //cout<<"Normal out : ";
                    //searchflag=0;
                    //openDirecoty(curPath);
                }
                
            }
            else if (inputKey[0] == 'q') {
                write(STDOUT_FILENO, "\x1b[2J", 4);
                tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
                posx = 1;
                posy = 1;
                MOVE_CURSER;
                exit(1);
            }
            fflush(0);
             memset(inputKey, 0, 3 * sizeof(inputKey[0]));
        }
    }
}

int directory_Listing(const char* path){
    
    
   //Defining the different components of the program
    //The directory: it's the folder we're browsing (we'll use te path argument in order to identify it)
   DIR  *pwDir ;//My present Working Directory
   struct dirent *theFile; //The file: when a file is found in the directory readdir loop, it's going to be called this way.
   pwDir=opendir(path);

    if(pwDir == NULL){
        perror("opendir");

        return -1;
    }
   
    directoryList.clear();//Now i will make the list of the current files or directories

    while ((theFile = readdir(pwDir))) {
        if ((string(theFile->d_name) == "..") && (strcmp(path, root) == 0))
            {
                continue;
            }
            else
            {
                directoryList.push_back(string(theFile->d_name));//pushing each file in the directory list
        
            }
        }
        
  
    // printf("\033[H\033[J");
    // printf("%c[%d;%dH", 27, 1, 1);
    posx = 1;
    MOVE_CURSER;
    sort(directoryList.begin(), directoryList.end());
    cout<<"\033c";
    cout<<"\u001b[33m";
    update_list();
    posy=80;
    closedir(pwDir);
    return 0;

    
}
void update_list()
{
    cout<<"\033c";
     posy = 1;
    MOVE_CURSER;
    totalFiles=directoryList.size();
    unsigned int len = FilesToPrint();
   for (unsigned int i = 0, itr = 1; i < totalFiles && itr <= len; i++, itr++){
        string fName=directoryList[i];
        display(fName.c_str());
    }

    return;
}

void display(const char* dirName){
   
    struct stat thestat;
    //will be used to determine the file owner & group
    struct passwd *tf; 
    struct group *gf;
   
    //Creating a placeholder for the string. 
    //We create this so later it can be properly adressed.
    //It's reasonnable here to consider a 512 maximum lenght, as we're just going to use it to display a path to a file, 
    //but we could have used a strlen/malloc combo and declared a simple buf[] at this moment
    
    stat(dirName, &thestat);


        switch (thestat.st_mode & S_IFMT) {
            case S_IFBLK:  cout<<"b"; break;
            case S_IFCHR:  cout<<"c"; break; 
            case S_IFDIR:  cout<<"d"; break; //It's a (sub)directory 
            case S_IFIFO:  cout<<"p";break; //fifo
            case S_IFLNK:  cout<<"l"; break; //Sym link
            case S_IFSOCK: cout<<"s"; break;
            //Filetype isn't identified
            default:      cout<<"-"; break;
                }
        //[permissions]
        //Same for the permissions, we have to test the different rights
        //READ http://linux.die.net/man/2/chmod 
       
            cout<<( (thestat.st_mode & S_IRUSR) ? "r" : "-");
            cout<<( (thestat.st_mode & S_IWUSR) ? "w" : "-");
            cout<<( (thestat.st_mode & S_IXUSR) ? "x" : "-");
            cout<<( (thestat.st_mode & S_IRGRP) ? "r" : "-");
            cout<<( (thestat.st_mode & S_IWGRP) ? "w" : "-");
            cout<<( (thestat.st_mode & S_IXGRP) ? "x" : "-");
            cout<<( (thestat.st_mode & S_IROTH) ? "r" : "-");
            cout<<( (thestat.st_mode & S_IWOTH) ? "w" : "-");
         

          cout<<string(1,'\t');


        // [number of hard links] 
        // Quoting: http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html
        // "This count keeps track of how many directories have entries for this file. 
        // If the count is ever decremented to zero, then the file itself is discarded as soon as no process still holds it open."
       
    
         tf = getpwuid(thestat.st_uid);
         printf("%10s ", tf->pw_name);
         
         cout<<string(1,'\t');
         gf = getgrgid(thestat.st_gid);
        printf("%10s ", gf->gr_name);

       //size of file
        long long size = thestat.st_size;
            if (size >= (1 << 30))
                 printf("%4lldG ", size / (1 << 30));
            else if (size >= (1 << 20))
                 printf("%4lldM ", size / (1 << 20));
            else if (size >= (1 << 10))
                 printf("%4lldK ", size / (1 << 10));
            else
                 printf("%4lldB ", size);

        

     char *tt = (ctime(&thestat.st_mtime));
    tt[strlen(tt) - 1] = '\0';
    printf("%-30s", tt);
    if(S_ISDIR(thestat.st_mode))
    {
        printf("\033[1;32m");
        printf("\t%-20s\n", dirName);
        printf("\033[0m");
    }
    else
        printf("\t%-20s\n", dirName);
        
        
  
   
}



int main(int argc, char* argv[]) 
{
    g_argc=argc;
    g_argv=argv;
    if (g_argc == 1)
    {
        string str = ".";
        
        strcpy(root,get_current_dir_name());  
         strcat(cur_directory, root);
        strcpy(homepath,root);
        back_stack.push(root);
         //back_stack saves history of visited dir.      
        directory_Listing(root);

        
        
    }
    else if (argc == 2)
    {
        strcpy(root, g_argv[1]);
        strcpy(homepath,root);
        strcat(cur_directory, root);
        
        back_stack.push(root);
        directory_Listing(root);
        
    }
    else
    {
        cout << "!!!!!!Invalid Argument!!!!!!" << endl;
    }
    enableRawMode();
    
}