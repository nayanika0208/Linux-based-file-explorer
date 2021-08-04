## Linux File Explorer-


### About the project-
Building a fully functional file explorer application, albeit with restricted features.
The application supports two modes:
* NORMAL MODE
* COMMAND MODE
### Normal Mode-
* Displays list of directories and files present in current folder.
* Informations displayed: file name, file size, ownership, last modified (in human readable format)
* Key presses and their functionalities:
    * `ENTER KEY`: When user presses it, the file/directory the cursor is at, opens.
    * `LEFT ARROW KEY`: Goes back to the previously visited directory
    * `RIGHT ARROW KEY`: Goes to next directory
    * `BACKSPACE KEY`: Takes user up to one level
    * `HOME KEY`: Takes user back to root directory
    * `UP and DOWN keys`: used for moving cursor up and down to desired file or directory

### Command Mode-
This mode is entered from Normal mode whenever `:` is pressed.
##### Following commands are supported:
* COPY: `copy <file_name(s)> <target_directory_path>`
* MOVE: `move <file_name(s)> <target_directory_path>`
* RENAME: `rename <old_file_name> <new_file_name>`
* CREATE FILE: `create_file <file_name> <destination_path>`
* CREATE DIRECTORY: `create_dir <diectory_name> <destination_path>`
* DELETE FILE: `delete_file <file_path>`
* DELETE DIRECTORY: `delete_dir <directory_path>`
* SEARCH: `search <file_name / directory_name>`
* GOTO: `goto <directory_path>`
Pressing ESC KEY takes user back to Normal Mode.

#### How to execute the program-
* Open the terminal with root of the application folder as the present working directory
* Compile the cpp file: `g++ main.cpp -o main`
* Run the executable file: `./main`
#### Language used-
C++

