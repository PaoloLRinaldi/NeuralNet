// COpyright
#ifndef SETTINGS_H
#define SETTINGS_H

#include <utility>
#include <string>
#include <vector>
#include <iostream>

class Settings {
 public:
  // The first part of the pair
  // indicates if that value was
  // left blank
  // Part 1
  std::pair <bool, std::string> web_file = {false, std::string()};
  std::pair <bool, std::vector<unsigned int>> web_structure = {false, std::vector<unsigned int>()};
  std::pair <bool, std::string> hlact = {true, std::string("sigmoid")};;
  std::pair <bool, std::string> olact = {true, std::string("sigmoid")};;
  std::pair <bool, int> cost_function = {true, 0};
  std::pair <bool, int> batch = {true, 1};
  std::pair <bool, double> tau = {true, 0};
  std::pair <bool, double> gradient_step = {true, 0.01};
  std::pair <bool, bool> bias = {true, true};

  // PART 2
  std::pair <bool, bool> is_little_endian = {true, true};
  std::pair <bool, bool> data_stored_tadily = {true, false};
  std::pair <bool, double> min_norm = {true, 0};
  std::pair <bool, double> max_norm = {true, 0};

  // PART 3
  std::pair <bool, std::string> sets_file = {false, std::string()};;
  std::pair <bool, size_t> test_sz = {false, size_t()};
  std::pair <bool, bool> test_only = {true, false};
  std::pair <bool, bool> test_entire = {true, true};
  std::pair <bool, bool> train_until_overfitted = {true, true};
  std::pair <bool, size_t> training_cycles = {false, 1};
  std::pair <bool, bool> train_until_better = {true, false};
  std::pair <bool, double> train_until_better_than = {false, double()};

  // PART 4
  std::pair <bool, bool> save_when_finishes = {true, true};
  std::pair <bool, bool> overwrite_if_better = {true, true};

  // PART 5
  std::pair <bool, bool> update_this = {true, true};

  std::ostream &settings_file_content(std::ostream &os);
  static void instructions(std::ostream &os = std::cout);
  void read_settings_file(const std::string &setts_file);
};

struct FileNameInfo {
  double perc = 0;
  std::vector<unsigned int> layersinfo;
  unsigned int train_sz = 0;
  unsigned int test_sz = 0;
  unsigned int traininground = 0;
  bool is_set = false;
  bool is_web = false;
  bool is_good = false;
};



FileNameInfo get_file_name_infos(const std::string &file_name);












#endif  //SETTINGS_H
