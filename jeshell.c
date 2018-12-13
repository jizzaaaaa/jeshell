
/*CMSC 125 PROJECT: JESHELL (a c based shell)
Jezza M. Vinalon

Reference: 	https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
			https://github.com/zhouheidz?tab=repositories&fbclid=IwAR3ee1VhWEOV76sVSke9sTByyOHeWF2HVRekPG_MmcDAxSghAJHZq3IQJEA4
			https://github.com/agrocks23/shell
			https://brennan.io/2015/01/16/write-a-shell-in-c/

Note: all user input should be lowercase letters. 
			
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <winbase.h>
#define bool int
#define true 1
#define false 0

void cd_cmd(char *cmd, char *cmdtok);
void chdir_cmd(char *cmd, char *cmdtok);
void cls_cmd();
void copy_cmd(char *cmd);
void date_cmd(char *cmd);
void dir_cmd(DIR *dir);
void del_cmd(char *cmd);
void move_cmd(char *cmd);
void mkdir_cmd(char *folder);
void rename_cmd(char *cmd);
void rmdir_cmd(char *cmd);
void time_cmd(char* cmd);
void type_cmd(char *cmd);
void replace_cmd(char *cmd);
void exit_cmd();
void read_cmd(DIR *dir, char *cmd);
char *trim(char *);
void formatInteger(unsigned n);
void formatIntegerFigures(unsigned n);
int is_file_specific (char *cmd);
char *extract_filename(char *cmd);
int is_date_valid(char *cmd);
int change_local_date(int month, int day, int year);
void print_time_results (int time_result);
int is_time_valid(char *cmd);
int change_local_time(int hour, int minute, int second);

int main(void) {
  char cwd[1024];
  char input[1024];

  DIR *dir;
  
  printf("Microsoft Windows [Version 10.0.17134.407]\n(c) 2018 Microsoft Corporation. All rights reserved.\n\n");

  while (1) {
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      input[0] != '\0';
      printf("%s>", cwd);
      fflush(stdin);
      scanf("%[^\n]", input);

      if(input[0] != '\0'){
        read_cmd(dir, input);
      }

      printf("\n");
    } else {
      perror("Error: ");
      break;
    }
  }

  return(0);
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Function to read user input and redirect them to the proper functions
	Parameters: Dir, char*
 */
void read_cmd(DIR *dir, char *cmd) {

  cmd = trim(cmd);                    // Clean user input
  char *cmdtok = strtok (cmd, " ");   // Tokenize user input

  // If-else block for appropriate function invokation per user input
  if (strcmp(cmdtok, "cmd") == 0) {
    printf("Microsoft Windows [Version 10.0.17134.407]\n(c) 2018 Microsoft Corporation. All rights reserved.\n\n");
  } else if (strcmp(cmdtok, "cls") == 0) {
    cls_cmd();
  } else if (strcmp(cmdtok, "cd") == 0) {
    cd_cmd(cmd, cmdtok);
  } else if (strcmp(cmdtok, "cd..") == 0) {
    chdir("..");
  } else if (strcmp(cmdtok, "dir") == 0) {
    dir_cmd(dir);
  } else if (strcmp(cmdtok, "chdir") == 0) {
    chdir_cmd(cmd, cmdtok);
  } else if (strcmp(cmdtok, "chdir..") == 0) {
    chdir("..");
  } else if (strcmp(cmdtok, "mkdir") == 0) {
    char *folder = cmd + strlen(cmdtok) + 1;
    mkdir_cmd(folder);
  } else if (strcmp(cmdtok, "time") == 0) {
    time_cmd(cmd);
  } else if (strcmp(cmdtok, "date") == 0) {
    date_cmd(cmd);
  } else if (strcmp(cmdtok, "copy") == 0) {
    copy_cmd(cmd);
  } else if (strcmp(cmdtok, "type") == 0) {
    type_cmd(cmd);
  } else if (strcmp(cmdtok, "rename") == 0) {
    rename_cmd(cmd);
  } else if (strcmp(cmdtok, "del") == 0) {
    del_cmd(cmd);
  } else if (strcmp(cmdtok, "move") == 0) {
    move_cmd(cmd);
  } else if (strcmp(cmdtok, "rmdir") == 0) {
    rmdir_cmd(cmd);
  } else if (strcmp(cmdtok, "exit") == 0) {
    exit_cmd(cmd);
  } else if (strcmp(cmdtok, "replace") == 0) {
    replace_cmd(cmd);
  } else {
    printf("'%s' is not recognized as an internal or external command,\noperable program or batch file.\n", cmd);
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Function to change directory command or 'cd'
  	Parameters: char*, char*
 */
void cd_cmd(char *cmd, char *cmdtok) {

  char *cmd_dest = cmd + strlen(cmdtok) + 1;  // Extract destination string token
  char *is_args_null = strtok(NULL, " ");     // NULL pointer checkpoint

  // If destination directory specified or not
  if (is_args_null == NULL) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));                 // get current working directory
    printf("%s\n", cwd);                      // print current working directory
  } else if (chdir(trim(cmd_dest)) != 0) {
      printf("The system cannot find the path specified.\n");   // System error on unlocatable path
  }

}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Function to change directory command or 'chdir'
  	Parameters: char*, char*
 */
void chdir_cmd(char *cmd, char *cmdtok) {
  char *cmd_dest = cmd + strlen(cmdtok) + 1;  // Extract destination string token
  char *is_args_null = strtok(NULL, " ");     // NULL pointer checkpoint

  // If destination directory specified or not
  if (is_args_null == NULL) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));                 // get current working directory
    printf("%s\n", cwd);                      // print current working directory
  } else if (chdir(trim(cmd_dest)) != 0) {
    printf("The system cannot find the path specified.\n");     // System error on unlocatable path
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Clear screen command or 'cls'
	Parameters: ()
 */
void cls_cmd() {

  // Assembly variables declaration
  HANDLE                     hStdOut;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD                      count;
  DWORD                      cellCount;
  COORD                      homeCoords = { 0, 0 };

  hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
  if (hStdOut == INVALID_HANDLE_VALUE) return;

  // Get number of cells in the current buffer
  if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
  cellCount = csbi.dwSize.X *csbi.dwSize.Y;

  // Fill the entire buffer with spaces
  if (!FillConsoleOutputCharacter(
    hStdOut,
    (TCHAR) ' ',
    cellCount,
    homeCoords,
    &count
    )) return;

  // Fill the entire buffer with the current colors and attributes
  if (!FillConsoleOutputAttribute(
    hStdOut,
    csbi.wAttributes,
    cellCount,
    homeCoords,
    &count
    )) return;

  // Move the cursor to home coordinates
  SetConsoleCursorPosition( hStdOut, homeCoords );
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* Copy file command or 'copy'
 Parameters: copy
 */
void copy_cmd(char *cmd) {
  char *cmd_src = strtok(NULL, " ");
  char *cmd_dest = cmd_src + strlen(cmd_src) + 1;

  cmd_dest = strtok(NULL, " ");

  // Check if args exist
  if(cmd_src == NULL) {
    printf("The syntax of the command is incorrect.\n");

  } else if (cmd_dest == NULL){
    printf("The file cannot be copied onto itself.\n");

  } else {
    char ch;
    FILE *source_file, *dest_file;

    // Open source file and copy the contents
    source_file = fopen(cmd_src, "r");

    if(source_file == NULL) {
      printf("The system cannot find the file specified.\n");
    } else {

      if(strcmp(cmd_src, cmd_dest) == 0) {
        printf("The file cannot be copied onto itself.\n");
      } else {

        // Open destination file or create destination file
        if (!is_file_specific(cmd_dest)) {
          char *new_file = extract_filename(cmd_src);

          if (cmd_dest[strlen(cmd_dest) - 1] == '/') {
            strcat(cmd_dest, new_file);
          } else {
            strcat(cmd_dest, "/");
            strcat(cmd_dest, new_file);
          }
        }

        // Copy the contents of source file and
        // paste onto the destination file
        dest_file = fopen(cmd_dest, "w");
        ch = fgetc(source_file);
        while (ch != EOF) {
          fputc(ch, dest_file);
          ch = fgetc(source_file);
        }
        printf("1 file(s) copied.\n");
        fclose(dest_file);
      }
      fclose(source_file);
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Date display and change command or 'date'
 	Parameters: char*
 */
void date_cmd(char *cmd){
  char buffer[32];
  char input[1024];

  cmd = strtok(NULL, " ");

  // Check if any proceeding args
  if(cmd != NULL) {
    // Change date if user provided date args
    int time_result = is_date_valid(cmd);
    print_time_results(time_result);
    return;
  } else {

    // Display current date
    time_t raw_time = time(NULL);
    struct tm * tm = localtime(&raw_time);
    strftime(buffer, sizeof(buffer),  "%a %m/%d/%Y", tm);
    printf("The current date is: %s\nEnter new date (mm-dd-yy):", buffer);

    // Prompt user to enter a new date
    fflush(stdin);
    scanf("%[^\n]", input);
    if(input[0] != '\0'){
      int time_result = is_date_valid(input);
      print_time_results(time_result);

    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Delete files command or 'del'
 	Params: char*
 */
void del_cmd(char *cmd) {
  char *cmd_filename = strtok(NULL, " ");

  // Check if user specified a file to be deleted
  if(cmd_filename == NULL) {
    printf("The syntax of the command is incorrect.\n");
  } else {

    char *temp_fn = cmd_filename;
    int count = 1;
    int length = strlen(temp_fn);
    int running = 1;

    int delete_count = 0;

    // Count how many files are to be deleted
    while(running) {
      temp_fn = temp_fn + length + 1;
      length = length + strlen(temp_fn);

      if ((temp_fn = strtok(NULL, " ")) == NULL) {
        running = 0;
      } else {
        ++count;
      }
    }

    // Traverse through the files to be deleted
    while (count) {
      FILE *file;
      file = fopen(cmd_filename, "r");

      // Remove each file
      if(file) {
        fclose(file);
        int remove_success = remove(cmd_filename);

        if(remove_success == 0) {
          ++delete_count;
        } else {
          printf("Could not delete file %s\n", cmd_filename);
        }
      } else {
        printf("Could Not Find %s \n", cmd_filename);
      }

      // Go to the next file
      cmd_filename = cmd_filename + strlen(cmd_filename) + 1;
      --count;
    }

    // Inform user how many files deleted
    printf("%d file(s) deleted.\n", delete_count);
  }

}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	List directory command or 'dir'
 	Parameters: Dir*
 */
void dir_cmd(DIR *dir) {
  struct dirent *dirnt;
  dir = opendir(".");

  struct stat attr;
  char buff[20];
  struct tm *time_info;

  int file_count = 0;
  int dir_count = 0;

  // If directory exists
  if (dir) {
    dirnt = readdir(dir);

    // Traverse directory entry
    while (dirnt != NULL) {
      stat(dirnt->d_name, &attr);
      time_info = localtime(&attr.st_mtime);
      strftime(buff, sizeof(buff), "%m/%d/%Y %I:%M %p", time_info);
      printf("%s", buff);

      // If directory element is a directory
      if(S_ISDIR(attr.st_mode)) {
        printf("\t\t  <DIR>\t");
      } else {
        int count = (int)log10(attr.st_size) + 1;
        count += (count-1)/3;

        // Routine to adjust spaces and tabs and to keep directory view neat
        int i = 0;
        for (; i < 20-count; i++) {
            printf(" ");
        }

        // Call misc function to properly print integers
        formatInteger((unsigned)attr.st_size);
        printf("\t");
      }

      // Move to the next directory entry and print name
      printf("\t%s\n", dirnt->d_name);

      dirnt = readdir(dir);
    }
    closedir(dir);
  } else {
    perror("Error: ");
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Create directory command or 'mkdir'
 	Params: char*
*/
void mkdir_cmd(char *folder) {

  // If no directory was passed
  if (folder == NULL) {
    printf("usage: mkdir [-pv] [-m mode] directory ...\n");
  } else {

    // Invoke mkdir() function
    int mkdir_status = mkdir(folder);           /// Function format for Windows

    // Check function success
    if(mkdir_status != 0) {
      printf("Directory already exists.\n");
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* Move file command or 'move'
 * Params: char*
 */
void move_cmd(char *cmd) {
  char *cmd_src = strtok(NULL, " ");
  char *cmd_dest = cmd_src + strlen(cmd_src) + 1;

  cmd_dest = strtok(NULL, " ");

  // If user provided source file and destination path
  if (cmd_src == NULL && cmd_dest == NULL) {
    printf("The syntax of the command is incorrect.\n");
  } else {
    char ch;
    FILE *source_file, *dest_file;

    source_file = fopen(cmd_src, "r");

    if(source_file == NULL) {
      printf("The system cannot find the file specified.\n");
    } else {

      // Copy the file to another directory
      char *new_file = extract_filename(cmd_src);

      // Concatenate file name to the destination path
      if (cmd_dest[strlen(cmd_dest) - 1] == '/') {
        strcat(cmd_dest, new_file);
      } else {
        strcat(cmd_dest, "/");
        strcat(cmd_dest, new_file);
      }

      // Create destination file
      dest_file = fopen(cmd_dest, "w");

      // Copy contents of source file to destination file
      ch = fgetc(source_file);
      while (ch != EOF) {
        fputc(ch, dest_file);
        ch = fgetc(source_file);
      }

      fclose(dest_file);
      fclose(source_file);

      // Delete the file in original directory
      int remove_success = remove(cmd_src);

      if(remove_success == 0) {
        printf("1 file(s) moved.\n");
      } else {
        printf("Could not delete file %s\n", source_file);
      }
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Rename files command or 'rename' 
 	Params: char*
 */
void rename_cmd(char *cmd) {
  char *cmd_src = strtok(NULL, " ");
  char *cmd_new = cmd_src + strlen(cmd_src) + 1;

  cmd_new = strtok(NULL, " ");

  // Check if names were provided
  if(cmd_src == NULL && cmd_new == NULL) {
    printf("The syntax of the command is incorrect.\n");
  } else {
    // Rename files
    struct stat buffer;
    if(stat (cmd_new, &buffer) != 0) {
      int rename_success = rename(cmd_src, cmd_new);

      if(rename_success == 0) {
        printf("1 file(s) renamed.\n");
      } else {
        printf("The syntax of the command is incorrect.\n");
      }
    } else {
      printf("Error renaming file, please try again.\n");
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* Remove directory command or 'rmdir'
 * Params: char*
 */
void rmdir_cmd(char *cmd) {
  char *cmd_dest = cmd + strlen(cmd) + 1;
  DIR *dir = opendir(cmd_dest);

  // check if directory to be deleted exists
  if (dir != NULL) {
    closedir(dir);
    int response = rmdir(cmd_dest);

    // if nonzero value was returned,
    // rmdir was not successfully executed
    if (response) {
      printf("The directory is not empty.\n\n");
    } else {
      printf("Successfully removed directory.\n");
    }
  } else {
    printf("The system cannot find the file specified.\n");
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Time display and change command or 'time'
 	Parameters: char*
 */
void time_cmd(char *cmd){
  char buffer[32];
  char input[1024];

  cmd = strtok(NULL, " ");

  // Check if any proceeding args
  if(cmd != NULL) {
    // Change time if user provided time args
    int time_result = is_time_valid(cmd);
    print_time_results(time_result);
  } else {

    // Display current time
    time_t raw_time = time(NULL);
    struct tm * tm = localtime(&raw_time);
    strftime(buffer, sizeof(buffer), "%X", tm);
    printf("The current time is: %s\nEnter the new time: ", buffer);

    // Prompt user to enter a new time
    fflush(stdin);
    scanf("%[^\n]", input);
    if(input[0] != '\0'){
      int time_result = is_time_valid(input);
      print_time_results(time_result);
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/*	Display file command or 'type'
	Parameters: char*
 */
void type_cmd(char *cmd) {
  char *cmd_filename = strtok(NULL, " ");

  // Check filename(s) specified
  if(cmd_filename == NULL) {
    printf("The syntax of the command is incorrect.\n");
  } else {
    char *temp = cmd_filename;
    int count = 1;

    int length = strlen(temp);
    int running = 1;

    // Count the files needed to be opened
    while(running) {
      temp = temp + length + 1;
      length = length + strlen(temp);

      if ((temp = strtok(NULL, " ")) == NULL) {
        running = 0;
      } else {
        ++count;
      }
    }

    // Open the file(s) specified by the user
    while (count) {
      FILE *file;
      file = fopen(cmd_filename, "r");
      if(!file) {
        printf("Error occurred while processing: %s\n", cmd_filename);
      }

      char buffer[10000];

      // Print name and contents of the file
      printf("%s\n", cmd_filename);
      while(fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
      }
      printf("\n\n");
      fclose(file);

      cmd_filename = cmd_filename + strlen(cmd_filename) + 1;
      --count;
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* Replace file command or 'copy'
 * Parameters: copy
 */
void replace_cmd(char *cmd) {
  char *cmd_src = strtok(NULL, " ");
  char *cmd_dest = cmd_src + strlen(cmd_src) + 1;

  cmd_dest = strtok(NULL, " ");

  // Check if args exist
  if(cmd_src == NULL) {
    printf("The syntax of the command is incorrect.\n");

  } else if (cmd_dest == NULL){
    printf("The file cannot be copied onto itself.\n");

  } else {
    char ch;
    FILE *source_file, *dest_file;

    // Open source file and copy the contents
    source_file = fopen(cmd_src, "r");

    if(source_file == NULL) {
      printf("The system cannot find the file specified.\n");
    } else {

      if(strcmp(cmd_src, cmd_dest) == 0) {
        printf("The file cannot be copied onto itself.\n");
      } else {

        char *existing_file = extract_filename(cmd_src);

        // Find the existing file to be replaced
        if (cmd_dest[strlen(cmd_dest) - 1] == '/') {
          strcat(cmd_dest, existing_file);
        } else {
          strcat(cmd_dest, "/");
          strcat(cmd_dest, existing_file);
        }

        // Copy the contents of source file and
        // paste onto the destination file
        dest_file = fopen(cmd_dest, "w");
        ch = fgetc(source_file);
        while (ch != EOF) {
          fputc(ch, dest_file);
          ch = fgetc(source_file);
        }
        printf("1 file(s) replaced.\n");
        fclose(dest_file);
      }
      fclose(source_file);
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* Extra recursive function for formatting integer printing
 * Base case: n < 1000
 * Parameters: unsigned int
 */
void formatIntegerFigures (unsigned n) {

  // Format >999 integers in recursive manner
  if (n < 1000) {
    printf("%d", n);
    return;
  }

  // Print appropriate commas
  formatIntegerFigures(n/1000);
  printf(",%03d", n%1000);
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Extra function for changing local time
 	Parameters: int, int, int
 */
int change_local_time(int hour, int minute, int second) {

  // Get current system time
  SYSTEMTIME st;
  GetSystemTime(&st);

  // Format new SystemTime object
  SYSTEMTIME new_time;
  new_time.wYear = st.wYear;
  new_time.wMonth = st.wMonth;
  new_time.wDayOfWeek = st.wDayOfWeek;
  new_time.wDay = st.wDay;
  new_time.wHour = hour;
  new_time.wMinute = minute;
  new_time.wSecond = second;
  new_time.wMilliseconds = st.wMilliseconds ;

  // Change PC time
  SetLocalTime(&new_time);

  // Return result code
  DWORD dword = GetLastError();
  return (int) dword;
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Extra function for checking validity of time input
 	Parameters: char*
 */
int is_time_valid(char *cmd) {
  // Get user input
  char *hour = strtok(cmd, ":");
  char *minute = strtok(hour + strlen(hour) + 1, ":");
  char *second = minute + strlen(minute) + 1;

  // Convert user input to int
  int hour_int = atoi(hour);
  int minute_int = atoi(minute);
  int second_int = atoi(second);

  // Check input length, as well as values
  if (strlen(hour) != 2 || strlen(minute) != 2 || strlen(second) != 2) {
    return -1;
  } else if ((hour_int < 0 || hour_int > 23) || (minute_int < 0 || minute_int > 59) || (second_int < 0 || second_int > 59)) {
    return -1;
  } else {
    return change_local_time(hour_int, minute_int, second_int);
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Extra function for printing date/time results
 	Parameters: int
 */
void print_time_results (int time_result) {

  // Print error or success messages
  if (time_result == -1) {
    printf("The system cannot accept the time/date entered.\n");
    return;
  } else if (time_result == 1314) {
    printf("A required privilege is not held by the client.\n");
  } else if (time_result == 0) {
    printf("Time/date updated successfully.");
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Extra function for changing date
 	Params: int, int, int
 */
int change_local_date(int month, int day, int year) {
  // Get current system time
  SYSTEMTIME st;
  GetSystemTime(&st);

  // Configure new date
  SYSTEMTIME new_date;
  new_date.wYear = year;
  new_date.wMonth = month;
  new_date.wDayOfWeek = day;
  new_date.wDay = st.wDay;
  new_date.wHour = st.wHour;
  new_date.wMinute = st.wMinute;
  new_date.wSecond = st.wSecond;
  new_date.wMilliseconds = st.wMilliseconds ;

  // Change system date
  SetLocalTime(&new_date);

  // Return result code
  DWORD dword = GetLastError();
  return (int) dword;

}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Extra function for checking validity of date input
 	Params: char*
 */
int is_date_valid(char *cmd) {

  // Get user input
  char *month = strtok(cmd, "/");
  char *day = strtok(month + strlen(month) + 1, "/");
  char *year = day + strlen(day) + 1;

  // Format input into int
  int month_int = atoi(month);
  int day_int = atoi(day);
  int year_int = atoi(year);

  // Check input length, as well as input values
  if (strlen(month) != 2 || strlen(day) != 2 || strlen(year) != 4) {
    return -1;
  } else if ((month_int < 0 && month_int > 12) || (day_int < 0 && day_int > 31) || (year_int < 1970)) {
    return -1;
  } else {
    return change_local_date(month_int, day_int, year_int);
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Extra function in extracting the file name from a file path
  	Parameters: char*
 */
char *extract_filename(char *cmd) {
  int filename_idx = 0;
  int i = strlen(cmd) - 1;
  char filename[128];

  // Extract index to the last slash
  for (; i >= 0; i--){
    if(cmd[i] == '/') {
      filename_idx = i;
      break;
    }
  }

  // Retrieve filename of the input file path
  // starting at the identified starting index
  i = filename_idx;
  int j = 0;
  for(; i < strlen(cmd); i++, j++) {
    filename[j] = cmd[i];
  }

  // Add null character terminator
  filename[j] = '\0';

  return filename;
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* Extra function to check if file name exists in a path
 Parameters: char*
 */
int is_file_specific (char *cmd) {
  int i = 0;

  // Traverse through file path and determine if file name is included
  for (; i < strlen(cmd); i++) {
    if(cmd[i] == '.') {
      return 1;
    }
  }
  return 0;
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Exit command or 'exit'
 	Parameters: ()
 */
void exit_cmd() {
  exit(0);
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Extra function for trimming unnecessary spaces
 	Parameters: char*
 */
char *trim(char *str) {
  char *lastChar;         					// Declare pointer for the last character

  // Trim preceding spaces
  while(isspace((unsigned char) *str)) {
    str++;
  }

  if (*str == 0) {
    return str;
  }

  // Trim proceeding spaces
  lastChar = str + strlen(str) - 1;
  while(lastChar > str && isspace((unsigned char)*lastChar)) {
    lastChar--;
  }

  // Setting a new null char terminator at the last character
  *(lastChar+1) = 0;

  return str;
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* 	Extre function for formatting integer printing
	Parameters: unsigned int
 */
void formatInteger (unsigned n) {

  // Format negative numbers accordingly
  if (n < 0) {
    printf ("-");
    n = -n;
  }
  // Call child misc function
  formatIntegerFigures(n);

}

