#include "headers.h"

 int g_argc; 
 char** g_argv;
 char root[4096]; 
 char cur_directory[4096]; 
 vector<string> directoryList; 
 stack<string> back_stack; 
 stack<string> forw_stack; 
 int totalFiles;
 vector<char> command_string; 
char homepath[4096];
 unsigned int term_row_num;
 unsigned int term_col_num; 
  unsigned int posx; 
 unsigned int posy; 
 int cur_window; 
 struct winsize terminalWindow; 
 struct termios raw, newraw;
 vector<string> my_command; 


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


//This creates single or multiple directories
void createDirectory(vector<string> cmdList)
{
    
    unsigned int len=cmdList.size();
    if(len <= 2)
    {
                           
         printf("too few arguments:\n");
         return;
    }
    else{
        string destDir= create_absolute_path(cmdList[len-1]);
       
         //verifies if destination is directory or not.
        if (!isDirectory(destDir)) {
                            cout<<endl;
                            cout<<"\033[0;31m"<<"In the command The destination is not valid directory "<<destDir<<endl;
                            cout<<"\033[0m";
                            cout<<":";
           
            return;
        }
        for (unsigned int i = 1; i < len -1; i++) {
          
             string Dir= destDir + "/"+cmdList[i];
          
            if (mkdir(Dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                 {      
                             cout<<endl;
                            cout<<"\033[0;31m"<<" Error in Creating Directory in path "<<destDir<<endl;
                            cout<<"\033[0m";
                            cout<<":";
                   
                }
          

        }

    }
    return ;
    

}


//*************************************//*********LISTING DIRECTORIES****************************//*************************************//*************************************


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
    string finalpath;
    string dir=string(cur_directory) ;
    string cur=string(dirName);
    finalpath = dir+ "/" + cur ;

    char *path = new char[finalpath.length() + 1];
    strcpy(path, finalpath.c_str());
    
   
    if (stat(path, &thestat) == -1)
    {
        perror("lstat");
    }
   
    
   
    //Creating a placeholder for the string. 
    //We create this so later it can be properly adressed.
    //It's reasonnable here to consider a 512 maximum lenght, as we're just going to use it to display a path to a file, 
    //but we could have used a strlen/malloc combo and declared a simple buf[] at this moment
    
    


       
            printf((S_ISDIR(thestat.st_mode)) ? "d" : "-");
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




//*************************************//*************************************//*************************************//*************************************

//*************************************//********ENABLE NON CANONICAL*****************************//*************************************//*************************************




void enableRawMode(){
    //getting the initial terminal settings
    tcgetattr(STDIN_FILENO, &raw);
    newraw = raw;
    //changing the flags for entering Non Canonical mode.
    newraw.c_lflag &= ~(ICANON | ECHO | IEXTEN | ISIG);
    newraw.c_iflag &= ~(BRKINT);
    

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &newraw) != 0)
        fprintf(stderr, "Failed in  setting  attributes\n");
    else {
       
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
                if(strcmp(cur_directory,homepath) == 0){
                    continue;
                }
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
            //enter key
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
                   directory_Listing(cur_directory);
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




//*************************************//*************************************//*************************************//*************************************


//*************************************//********COMMAND MODE*****************************//*************************************//*************************************

int commandMode(){

        posx = terminalWindow.ws_row-1;
        posy = 1;
        MOVE_CURSER;
        printf("\x1b[0K");
        printf(":");
        fflush(0);
        posy++;
         

       
        char inp[3];
         memset(inp, 0, 3 * sizeof(inp[0]));
        command_string.clear();
        my_command.clear();
        while (1) {
            
        
            if (read(STDIN_FILENO, inp, 3) == 0)
                continue;
            if (inp[0] == 27 && inp[1] == 0 && inp[2] == 0) {
                posx = 1;
                posy = 80;
                MOVE_CURSER;
                return 0;
            }
            else if (inp[0] == 27 && inp[1] == '[' && (inp[2] == 'A' || inp[2] == 'B'|| inp[2] == 'C' || inp[2] == 'D')) {
                continue;
            }
           
            else if (inp[0] == 10) {
                command_string.push_back('\n');

                split_char_to_string();
               if(my_command.size() >=1){
                string s = my_command[0];
                if (s == "copy"){

                     my_copy(my_command);
                    // cout<<"my_copy()"<<endl;
                      posy=2;
                    command_string.clear();
                    clearLastLine();
                }
                else if (s == "move"){
                     movecommand(my_command);
                      posy=2;
                    command_string.clear();
                    clearLastLine();

                 }
                else if (s == "rename"){

                    renameFiles(my_command);
                     posy=2;
                    command_string.clear();
                    clearLastLine();
                }
                else if (s == "create_file"){
                   
                     createFile(my_command);
                      posy=2;
                    command_string.clear();

                    clearLastLine();
                }
                else if (s == "create_dir"){
                    // cout<< "my command .size   "<<my_command.size()<<endl;

                    createDirectory(my_command);
                    posy=2;
                    command_string.clear();

                    clearLastLine();
                }
                else if (s == "delete_file"){
                    removeFiles(my_command);
                    posy=2;
                    command_string.clear();

                    clearLastLine();
                }
                else if (s == "delete_dir"){
                    removeDirectories(my_command);
                     posy=2;
                    command_string.clear();

                    clearLastLine();
                }
                else if (s == "goto"){

          
                    string my_path ="";

                    if(my_command.size() != 2){
                            cout<<endl;
                            cout<<"\033[0;31m"<<" Invalid command "<<endl;
                            cout<<"\033[0m";
                            cout<<":";
                     }else{
                        my_path = create_absolute_path(my_command[1]);

                     }


                    back_stack.push(string(cur_directory));
                    while(!forw_stack.empty()){
                        forw_stack.pop();
                    }
                    strcpy(cur_directory, my_path.c_str());
                    
                    clearLastLine();
                    return 1;
                }
                else if (s == "search"){
                   // cout<<"here  "<<endl;
                   bool found=searchCommand(my_command);
                   unsigned int last= term_row_num ;
                   
                   if(found == true){
                   printf("%c[%d;%dH",27,last,1);
                    printf("%c[2K", 27);
                    cout<<"Last Search Result :True";
                   }else{
                    printf("%c[%d;%dH",27,last,1);
                    printf("%c[2K", 27);
                     cout<<"Last Search Result : False "<<endl;
                   }
                   posy=2;
                   command_string.clear();
                   printf("%c[%d;%dH",27,last+1,1);
                    printf("%c[2K", 27);
                    cout<<":";
                    

                  
                }
                else {
                    
                            cout<<endl;
                            cout<<"\033[0;31m"<<" Command not found  " <<endl;
                            cout<<"\033[0m";
                            cout<<":";
                            clearLastLine();
                }
                // if (goto_flag)
                //     return 1;
                // if (search_flag_c)
                //     return 2;
                // break;
            }else{
                clearLastLine(); 
            }
               
            }
            else if (inp[0] == 127) {
                if (posy > 2) {
                    posy--;
                    MOVE_CURSER;
                    printf("\x1b[0K");
                    command_string.pop_back();
                }
            }
            else {
                cout << inp[0];
                posy++;
                MOVE_CURSER;
                command_string.push_back(inp[0]);
            }
            fflush(0);
            memset(inp, 0, 3 * sizeof(inp[0]));
        }
    
    return 0;
}
 




//*************************************//*************************************//*************************************//*************************************

//*************************************CREATE AND DELETE SECTON*******************************************************************

//This creates single or multiple files
void createFile(vector<string> cmdList)
{
    unsigned int len=cmdList.size();
    if(len < 3)
    {
                           // cout<<endl;
                            cout<<"\033[0;31m"<<" To Few arguments in the command given "<<endl;
                            cout<<"\033[0m";
                            cout<<":";
    }
    else{
        string destDir= create_absolute_path(cmdList[len-1]);
        //cout<<"\ndestDir : "<<destDir<<endl;
         //verifies if destination is directory or not.
        if (!isDirectory(destDir)) {
            cout << "In the command The destination is not valid directory " << endl;
            return;
        }
        
        for (unsigned int i = 1; i < cmdList.size() - 1; i++) {
          
             string filePath= destDir + "/"+cmdList[i];
             
             int status=open(filePath.c_str(),O_RDONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );   
            if (status == -1)
            {
                            cout<<endl;
                            cout<<"\033[0;31m"<<" Error creating file "<<endl;
                            cout<<"\033[0m";
                            cout<<":";          
            }
            
            
          

        }

    }
    return;
    
}

//This function is for deleting mutiple directories

void removeDirectories(vector<string> cmdList)
{
    if(cmdList.size() < 2)
    {
        cout<<endl;
        cout<<"\033[0;31m"<<" Too Few Arguments "<<endl;
        cout<<"\033[0m";
        cout<<":";  
    }
    for(unsigned int i=1;i<cmdList.size();i++)
    {
        
         
         DeleteSingleDir(create_absolute_path( cmdList[i]));
    }    
}


//This function is for deleting single directory
 
 void DeleteSingleFile(string path)
{
    //cout<<"path for deleting file : "<<path<<endl;
    int status= remove(path.c_str());
     if(status != 0)
     {
        perror("");
     }
    
}



//This function is for deleting mutiple files
void removeFiles(vector<string> list)
{   
    if(list.size()<2)
    {
          perror("");
    }
    for(unsigned int i=1;i<list.size();i++)
    {
          string fileToDelete = create_absolute_path(list[i]);
         DeleteSingleFile(fileToDelete);
    }    
}

//This function is for deleting single file
 void DeleteSingleDir(string dirToDel)
{
    struct dirent* d;
    DIR* folder;
    folder = opendir(dirToDel.c_str());
   
    if (folder == NULL) {
        perror("opendir");
        return;
    }
    
    string curr=".";
    string parent="..";

    while ((d = readdir(folder))) {
          string dname=string(d->d_name);
           string dirToDel_path = dirToDel + "/" + dname;
           
        if(dname == curr || dname == parent )
            continue;
        else {
           
            if (isDirectory(dirToDel_path)) {
                
                    //recursive call to creating directory
                    DeleteSingleDir(dirToDel_path);
                
            }
            else {
                DeleteSingleFile(dirToDel_path);
            }
        }
    }
    closedir(folder);
    remove(dirToDel.c_str());
   
 
 
}



//***********************************************************************************************************************





//*********************************MOVE AND COPY SECTION**************************************************************************************





 



void copySingleFile(string fromFile, string toFile)
{

    char ch;
    FILE *fromFile_f, *toFile_f;
    
    fromFile_f = fopen(fromFile.c_str(), "r");
    toFile_f = fopen(toFile.c_str(), "w");
    if (fromFile_f == NULL) {
        perror("");
        return;
    }
    if (toFile_f == NULL) {
        perror("");
        return;
    }
    while ((ch = getc(fromFile_f)) != EOF) {
        
        putc(ch, toFile_f);
    }


    struct stat fromFile_stat;
    stat(fromFile.c_str(), &fromFile_stat);
    chown(toFile.c_str(), fromFile_stat.st_uid, fromFile_stat.st_gid);
    chmod(toFile.c_str(), fromFile_stat.st_mode);
    fclose(fromFile_f);
    fclose(toFile_f);
 
}

void CopySingleDirectory(string from, string to)
{
    struct dirent* d;
    DIR* folder;
    folder = opendir(from.c_str());
   
    if (folder == NULL) {
        perror("opendir");
        return;
    }
    
    string curr=".";
    string parent="..";

    while ((d = readdir(folder))) {
          string dname=string(d->d_name);
           string from_path = from + "/" + dname;
            string to_path = to + "/" + dname;
        if(dname == curr || dname == parent )
            continue;
        else {
           
            if (isDirectory(from_path)) {
                if (mkdir(to_path.c_str(), 0755) == -1) {
                    perror("");
                    return;
                }
                else
                {
                    //recursive call to creating directory
                    CopySingleDirectory(from_path, to_path);
                }
            }
            else {
                copySingleFile(from_path, to_path);
            }
        }
    }
    closedir(folder);
    return;
}



bool my_copy(vector<string> cmdList)
{
    if (cmdList.size() < 3)
        {
                    cout<<"\033[0;31m"<<" To Few arguments in the command given "<<endl;
                     cout<<"\033[0m";
                     cout<<":";
                     return false;
        }
    else {
        unsigned int siz=cmdList.size();
        string destination = create_absolute_path(cmdList[ siz - 1]);
        if (!isDirectory(destination)) {
                  
            return false;
        }
        for (unsigned int i = 1; i < siz - 1; i++) {
            string from_path = create_absolute_path(cmdList[i]);
            int posoflastslash=0;
                for(int i=0;i<from_path.length();i++){
                    if(from_path[i]=='/'){
                        posoflastslash=i;
                    }
                }
            // size_t found = from_path.find_last_of("/\\");
            string to_path = destination + "/" + from_path.substr(posoflastslash + 1, from_path.length() - posoflastslash);
            if (isDirectory(from_path)) {
                if (mkdir(to_path.c_str(), 0755) != 0) {
                    perror("");
                    return false;
                }
                CopySingleDirectory(from_path, to_path);
            }
            else {
                copySingleFile(from_path, to_path);
            }
        }
    }
    return true;
}




void movecommand(vector<string> list)
{
    unsigned int len = list.size();
    if(len < 3)
    {
       perror("");
    }
    else{

        for(unsigned int i=1;i<len-1;i++)
        {           
            string newData = create_absolute_path( list[i] );
             string s=newData;
            int posoflastslash=0;
            //getting the position of the last slash

            for(int i=0;i<s.length();i++){
                   if(s[i]=='/'){
                    posoflastslash=i;
                            }
                        }
                        string name;

            
             name=s.substr(posoflastslash+1,s.length());
             
                        
           

            string destpath= create_absolute_path( list[len-1]);
            destpath =destpath + "/" + name;
           
            
            if(isDirectory(newData))
            {
                if (mkdir(destpath.c_str(), 0755) == -1) {
                    perror("");
                    return;
                }
                else
                {
                    //for moving  directory first copy directory then delete 
                    CopySingleDirectory(newData,destpath);
                     DeleteSingleDir(newData);
                }
                
               
            }
            else
            {
                //for moving  file first copy file then delete 
                copySingleFile(newData,destpath);
                DeleteSingleFile(newData);
            }
        }
    }
}

//***********************************************************************************************************************



//**************MISCELLANEOUS*********************************************************************************************************




//convets relative path to absolute path
string create_absolute_path( string str)
{
    string abs="";
    char firstchar = str[0];
    
    string basepath = string(root);
    if(firstchar=='.')
    {
        abs = string(cur_directory) + str.substr(1,str.length());    
    }
    
    else if(firstchar=='~')
    {
        abs = basepath + str.substr(1,str.length());
    }
    else if(firstchar =='/')
    {
        abs = basepath + str;
    }
    else
    {
        abs= string(cur_directory)+ "/" + str;
    }

    return abs;
}


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
bool isDirectory(string path){
   

    struct stat Stat;
     stat(path.c_str(), &Stat);

     return ((Stat.st_mode & S_IFMT) == S_IFDIR)?true:false;
}
//checking if the path denotes a regular file or not
bool isRegularFile(const char*path){
   

    struct stat Stat;
     stat(path, &Stat);

     return ((Stat.st_mode & S_IFMT) == S_IFREG)?true:false;
}


void split_char_to_string()
{
    

     my_command.clear();
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
            i++;
            temp =  temp+ command_string[i];
        }
        else{
            temp = temp + command_string[i];
        }
    }
    
}


void clearLastLine()
{
    unsigned int lastLine = term_row_num +1;
    printf("%c[%d;%dH",27,lastLine,1);
    printf("%c[2K", 27);
    cout<<":";
}


void renameFiles(vector<string> list)
{
    if(list.size()!=3)
    {
        // showError("Invalid Argument in Renaming !!!");
    }
    else{
        string initName = create_absolute_path(  list[1]);
        string finalName = create_absolute_path(list[2]);
        rename(initName.c_str(),finalName.c_str());
    }
    
}



//***********************************************************************************************************************


bool searchCommand(vector<string> cmdL){
    if(cmdL.size() != 2){
        cout<<" INVALID no of arguents "<<endl;
        return false;
    }
   string s=string(cur_directory);
   return searchFileorDir(s,cmdL[1]);
}
 

 bool searchFileorDir(string cur,string toFind){

    struct dirent* d;
    DIR* folder;
    folder = opendir(cur.c_str());
     if (folder == NULL) {
        perror("opendir");
        return false;
    }


    string curr=".";
    string parent="..";

    while ((d = readdir(folder))) {
          string dname=string(d->d_name);
            string nextDir=cur +"/"+dname;
        if(dname == curr || dname == parent )
            continue;
        else {
           
            if (isDirectory(nextDir)) {
                if(dname == toFind){
                    return true;
                 
                }else{
                    
                     bool found =searchFileorDir(nextDir,toFind);
                     if(found == true){
                        return true;
                     }
                }
            }else{
                 if(dname == toFind){
                    return true;
                 
                }
            }
           
        }
    }
    closedir(folder);
    return false;



 }