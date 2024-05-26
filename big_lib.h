#ifndef GUARD_big_lib_h
#define GUARD_big_lib_h

#include <vector>
#include <string>
#include <ctime>

class file_info {
public:
  file_info() = default;
 file_info(const std::string& s, bool d) : file_name(s), dir(d) { }
  bool is_dir() const { return dir; }
  const std::string& name() const { return file_name; }
  operator std::string() const { return name(); }
private:
  std::string file_name;
  bool dir;
};

class Difftime {
 public:
  void set_begin() { begin = std::clock(); }
  void set_end() { end = std::clock(); }
  double ms_till_now() const {
    return (std::clock() - begin) / (CLOCKS_PER_SEC/1000);
  }
  double ms_till_end() const {
    return (end - begin) / (CLOCKS_PER_SEC/1000);
  }
  double s_till_now() const {
    return ms_till_now() / 1000;
  }
  double s_till_end() const {
    return ms_till_end() / 1000;
  }
  
 private:
  std::clock_t begin;
  std::clock_t end;
};





void rmEcho();
void setEcho();

int kbhit();
int getch();

unsigned int default_menu(const std::vector<std::string>& choices);
std::string exec(const std::string& cmd);
std::vector<file_info> getls_str(const std::string& dir);
std::string my_dir();
int nrand(int n);
int users_choice(const int a, const int b);

#endif
