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
    term_row_num= terminalWindow.ws_row - 1;
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
            unsigned int lastPos = terminalWindow.ws_row+1;
            printf("%c[%d;%dH", 27, lastPos ,1);
            cout << "-----NORMAL MODE-----";
            
            printf("%c[%d;%dH", 27, posx, posy);
            fflush(0);
            if (read(STDIN_FILENO, inputKey, 3) == 0)
                //0 bytes are read
                continue;
            else if (inputKey[0] == 27 && inputKey[1] == '[' && inputKey[2] == 'A'){
                  
                  if (posx + cur_window > 1)
                    {
                        posx--;
                        if (posx > 0)
                        {
                            printf("%c[%d;%dH", 27, posx, posy);
                        }
                        else if (posx <= 0 && posx + cur_window >= 1)
                        {
                           printf("%c[2J", 27);
                            if (cur_window > 0)
                            {
                                cur_window--;
                            }
                           
                            printf("%c[%d;%dH", 27, 1, 1);
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
            else if (inputKey[0] == 27 && inputKey[1] == '[' && inputKey[2] == 'B'){
               int lenRecord;
                    if (posx + cur_window < (totalFiles))
                    {
                        posx++;
                        if (posx <= term_row_num)
                        {
                            printf("%c[%d;%dH", 27, posx, posy);
                        }
                        else if (posx > term_row_num && posx + cur_window <= totalFiles)
                        {
                            printf("%c[2J", 27);
                            lenRecord = FilesToPrint() - 1;
                            if (totalFiles > term_row_num)
                            {
                                cur_window++;
                            }
                            //cout<<"cur_window : "<<cur_window<<"***********";
                             printf("%c[%d;%dH", 27, 1, 1);
                            for (int i = cur_window; i <= lenRecord + cur_window; i++)
                            {
                                string fName=directoryList[i];
                                display(fName.c_str());
                            }
                            posx--;
                        }
                         printf("%c[%d;%dH", 27, posx, posy);
                    }
            }
            else if (inputKey[0] == 27 && inputKey[1] == '[' && inputKey[2] == 'C'){
                cout<< "RightArrow"<<endl;
            }
            else if (inputKey[0] == 27 && inputKey[1] == '[' && inputKey[2] == 'D'){
                cout<< "LeftArrow"<<endl;
            }
            else if (inputKey[0] == 'H' || inputKey[0] == 'h'){
                cout<< "HomeKey"<<endl;
            }
            else if (inputKey[0] == 127){
                cout<<" BackspaceKey"<<endl;
            }
            else if (inputKey[0] == 10){
                cout<< "EnterKey"<<endl;
            }
            else if (inputKey[0] == ':') {
                // int ret = command_mode();
                // posy = 1;
                // CURSER;
                // if (ret == 2) {
                //     search_flag = true;
                //     continue;
                // }
                // else
                //     listdir(cur_dir);
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
        strcpy(root, str.c_str());  
         strcat(cur_directory, root);
        back_stack.push(root); //back_stack saves history of visited dir.      
        directory_Listing(root);

        
        
    }
    else if (argc == 2)
    {
        strcpy(root, g_argv[1]);
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