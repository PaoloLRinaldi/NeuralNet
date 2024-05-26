#include <iostream>
#include <vector>
#include <algorithm>
#include <initializer_list>
#include "../readwritebin.h"

int main() {
  // Create the file
  Bin binfile("binfile.bin", true);

  // Write 4 integers
  binfile.write_many<int>({0, 2, 5, 9});
  // binfile.write_many<int>({0.0, 2.0, 5.0, 9.0});
  // binfile.write_many<std::initializer_list<int>>({0, 2, 5, 9});
  // binfile.write_many({0, 2, 5, 9}, 0);
  // auto myinit = {0, 2, 5, 9};
  // binfile.write_many(myinit);

  // std::vector<double> vals{0.2, 2.7, 5.5, 9.0};
  // std::vector<int> vals{0, 2, 5, 9};
  // binfile.write_many<int>(vals);

  // Place the cursor 3 integers behind
  binfile.rmove_by<int>(-3);

  // Read the following 3 integers
  std::vector<int> values = binfile.get_values<int>(3);

  // Print them on the screen
  std::cout << "The first time the numbers are: ";
  for (auto a : values) std::cout << a << " ";
  std::cout << std::endl;

  // Now let's reverse this three numbers in the file
  // The number 2 is the second integer number, so
  // its position, in terms of integers, is 1
  std::reverse(binfile.begin<int>() + 1, binfile.end<int>());

  // Read all 4 numbers from the beginning (position 0)
  values = binfile.get_values<int>(4, 0);

  // Print them on the screen
  std::cout << "The second time the numbers are: ";
  for (auto a : values) std::cout << a << " ";
  std::cout << std::endl;

  return 0;
}
