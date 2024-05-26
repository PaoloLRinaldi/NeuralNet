#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <utility>


int myrand(int n);

class PhasesSets {
public:
  PhasesSets(int sz);
  
  size_t tot_train_size() const { return (total_sz - test_sz) * n_output(); }
  size_t tot_test_size() const { return test_sz * n_output(); }
  int n_input() const { return 1372 * 3 + 1; }
  int n_output() const { return phase.size(); }
  
  void new_train() {
    /*
    std::cout << std::endl << std::endl;
    for (auto a : train_order)
      std::cout << a << " ";
    std::cout << std::endl;
    */
    std::random_shuffle(train_order.begin(), train_order.end(), myrand);
    /*
    for (auto a : train_order)
      std::cout << a << " ";
    std::cout << std::endl;
    std::cout << std::endl << std::endl;
    */
    train_index = 0;
  }
  
  void new_test() { test_index = 0; }
  
  std::pair<std::vector<double>, int> train_input(int index);
  std::pair<std::vector<double>, int> test_input(int index);
  
private:

  int test_sz;
  int total_sz;
  int train_index;
  int test_index;
  
  std::vector<std::string> in_foldier;
  std::vector<std::string> phase;
  
  std::vector<int> test_order;
  std::vector<int> train_order;
  
  std::vector<double> read_input(int n);
  
  
};
