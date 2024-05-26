#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdexcept>
#include "big_lib.h"


//void rmEcho();
//void setEcho();

//int kbhit();
//int getch();

//int getls (file_info **list, const char *dir);
//void free_str_ltrl_array(file_info *list, int size);

//unsigned int default_menu(const std::vector<std::string>& choices);
//std::string exec(const char* cmd);
//std::string my_dir();
//int nrand(int n);
//int users_choice(const int a, const int b);

void rmEcho() {
	
	struct termios t;
	int fd=0;
		tcgetattr(fd, &t);
		t.c_lflag |= ~ECHO;
		tcsetattr(fd, TCSANOW, &t);
	
	return;
}

/************************/

void setEcho() {
	
	struct termios t;
	int fd=0;
		tcgetattr(fd, &t);
		t.c_lflag &= ECHO;
		tcsetattr(fd, TCSANOW, &t);
	
	return;
}

/**************************************************/


int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

/**************************************************/


int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

/**************************************************/


std::vector<file_info> getls_str(const std::string& dir) {
  DIR *dp;
  struct dirent *ep;  
  std::string precise("./" + dir);   
//  int i=0;

  std::vector<file_info> ret;
  
  //(*list) = (file_info*) calloc (1, sizeof (file_info));
  //precise = (char*) calloc (1000, sizeof(char));
  //sprintf (precise, "./%s", dir);
  
  dp = opendir (precise.c_str());
  if (dp != NULL)
  {
    while ((ep = readdir (dp))) {
      //(*list) = (file_info*) realloc ((*list), (++i)*sizeof(file_info));
      //(*list)[i-1].name = (char*) calloc (100, sizeof(char));
      //sprintf ((*list)[i-1].name, ep->d_name);
      if (ep->d_type == DT_REG)
        //(*list)[i - 1].is_dir = false;
	ret.emplace_back(ep->d_name, false);
      else if (ep->d_type == DT_DIR)
        //(*list)[i - 1].is_dir = true;
	ret.emplace_back(ep->d_name, true);
      else
        perror ("Unknown file type");
    }

    (void) closedir (dp);
  }
  else
    perror ("Couldn't open the directory");
  
  //free(precise);
  return ret;
}


/**************************************************/


unsigned int default_menu(const std::vector<std::string>& choices) {
  std::cout << "0: Back" << std::endl;
  for (unsigned int i = 0; i != choices.size(); ++i) {
    std::cout << i + 1 << ": " << choices[i] << std::endl;
  }
  std::cout << std::endl;

  return users_choice(0, choices.size());
}


/**************************************************/

std::string exec(const std::string& cmd) {
  char buffer[128];
  std::string result = "";
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    throw std::runtime_error("popen() failed!");
  try {
    while (!feof(pipe)) {
      if (fgets(buffer, 128, pipe) != NULL)
        result += buffer;
      }
    } catch (...) {
      pclose(pipe);
      throw;
    }
  pclose(pipe);
  return result;
}

/**************************************************/

std::string my_dir() {
  static std::string directory;
  if (directory.empty()) {
    std::vector<file_info> this_dir_ls = getls_str("");
    std::vector<std::string> is_ubuntu;
    for (const auto& a : this_dir_ls)
      is_ubuntu.push_back(a.name());

    if (std::find(is_ubuntu.cbegin(), is_ubuntu.cend(), "ubuntu.txt") == is_ubuntu.cend()) {
      //directory = "../../../../../../../../../../../../sdcard/Android/data/com.dropbox.android/files/u612100211/scratch/ProgrammiCPaolo/ProgrammiCppPaolo/";
      directory = "/storage/emulated/0/Android/data/com.dropbox.android/files/u612100211/scratch/ProgrammiCPaolo/ProgrammiCppPaolo/";
    } else {
      directory = "~/Dropbox/ProgrammiCPaolo/ProgrammiCppPaolo/";
    }
  }
  return directory;
}

/**************************************************/

int nrand(int n) {
  if (n <= 0 || n > RAND_MAX)
    throw std::domain_error("Argument to nrand is out of range");

  const int bucket_size = RAND_MAX / n;
  int r;

  do r = std::rand() / bucket_size;
  while (r >= n);

  return r;
}

/**************************************************/

int users_choice(const int a, const int b) {
	int ret;
	bool inrange = false, isnumber = false;

	do {
		std::cout << "User's choice: ";
		if (b >= 10) {
		  if (!(std::cin >> ret)) {
		    isnumber = false;
		    std::cin.clear();
		    while (kbhit()) getch();
  			std::cout << std::endl <<"An integer number is expected." << std::endl;
		  } else
		    isnumber = true;
		} else {
		  char input = getch();
		  if ((isnumber = std::isdigit(input))) {
			  ret = input - 48;
		  } else
		    std::cout << std::endl <<"An integer number is expected." << std::endl;
		}
		if (isnumber) {
		  inrange = ret >= a && ret <= b;
		  if (!inrange)
		  std::cout << std::endl << "A number in the range [" << a << ", " << b << "] is expected." << std::endl;
		}
	} while (!isnumber || !inrange);
	std::cout << ret << std::endl;

	return ret;
}

/**************************************************/
