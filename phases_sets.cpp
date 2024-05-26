#include "phases_sets.h"
#include "big_lib.h"
#include <iterator>
#include <stdexcept>
#include <cmath>

using std::vector;
using std::random_shuffle;
using std::copy;
using std::back_inserter;
using std::string;
using std::vector;


PhasesSets::PhasesSets(int sz) : test_sz(sz) {
  std::srand( unsigned( std::time(0)));
  
  phase.emplace_back("phase_liquid/configurations/");
  phase.emplace_back("phase_hcp/configurations/");
  phase.emplace_back("phase_fcc/configurations/");

  auto list_files = getls_str(phase[0]);
  
  /*
  in_foldier.emplace_back("/pos_10000000");
  in_foldier.emplace_back("/pos_1010000");
  in_foldier.emplace_back("/pos_2010000");
  in_foldier.emplace_back("/pos_3010000");
  in_foldier.emplace_back("/pos_4010000");
  in_foldier.emplace_back("/pos_5000000");
  in_foldier.emplace_back("/pos_6000000");
  in_foldier.emplace_back("/pos_7000000");
  in_foldier.emplace_back("/pos_8000000");
  in_foldier.emplace_back("/pos_9000000");
  */

  copy(list_files.begin() + 2, list_files.end(), back_inserter(in_foldier));  //"+ 2" perché i primi 2 sono "." e ".."
  total_sz = in_foldier.size();
  //std::cout << total_sz << std::endl;
  
  vector<int> fluid_order, hcp_order, fcc_order;
  
  for (int i = 0; i != total_sz; ++i) fluid_order.push_back(i);
  for (int i = total_sz; i != 2 * total_sz; ++i) hcp_order.push_back(i);
  for (int i = 2 * total_sz; i != 3 * total_sz; ++i) fcc_order.push_back(i);
  
  random_shuffle(fluid_order.begin(), fluid_order.end(), myrand);
  random_shuffle(hcp_order.begin(), hcp_order.end(), myrand);
  random_shuffle(fcc_order.begin(), fcc_order.end(), myrand);
  
  copy(fluid_order.begin() + test_sz, fluid_order.end(), back_inserter(train_order));
  fluid_order.erase(fluid_order.begin() + test_sz, fluid_order.end());
  copy(hcp_order.begin() + test_sz, hcp_order.end(), back_inserter(train_order));
  hcp_order.erase(hcp_order.begin() + test_sz, hcp_order.end());
  copy(fcc_order.begin() + test_sz, fcc_order.end(), back_inserter(train_order));
  fcc_order.erase(fcc_order.begin() + test_sz, fcc_order.end());
  
  copy(fluid_order.begin(), fluid_order.end(), back_inserter(test_order));
  copy(hcp_order.begin(), hcp_order.end(), back_inserter(test_order));
  copy(fcc_order.begin(), fcc_order.end(), back_inserter(test_order));
  
  new_train();
  new_test();
  
}


std::pair<std::vector<double>, int> PhasesSets::train_input(int index) {
  if (index < 0 || index >= tot_train_size())
    throw std::out_of_range("Training input out of range!");
  return { read_input(train_order[index]), train_order[index] / total_sz };
}

std::pair<std::vector<double>, int> PhasesSets::test_input(int index) {
  if (index < 0 || index >= tot_test_size())
    throw std::out_of_range("Test input out of range!");
  return { read_input(test_order[index]), test_order[index] / total_sz };
}

std::vector<double> PhasesSets::read_input(int n) {
  //std::cout << n << std::endl;
  
  std::ifstream fin(phase[n / total_sz] + in_foldier[n % total_sz]);
  double tmp;
  fin >> tmp;
  int n_sphere;
  fin >> n_sphere;
  double x_tot, y_tot, z_tot;
  fin >> x_tot >> y_tot >> z_tot;
  
  //std::cout << x_tot << " " << y_tot << " " << z_tot << std::endl;
  
  std::vector<double> ret;
  for (int i = 0; i != n_sphere; ++i) {
    fin >> tmp;
    ret.push_back(tmp / x_tot * 0.99 + 0.01);
    fin >> tmp;
    ret.push_back(tmp / y_tot * 0.99  + 0.01);
    fin >> tmp;
    ret.push_back(tmp / z_tot * 0.99 + 0.01);
    
    fin >> tmp;
    if (i == n_sphere - 1) {
      //ret.push_back(tmp * 3 / (x_tot + y_tot + z_tot));
      ret.push_back(tmp * 3 / std::cbrt(x_tot * y_tot * z_tot));
    }
  }
  
  return ret;
  
}

int myrand(int n) {
  if (n <= 0 || n > RAND_MAX)
    throw std::domain_error("Argument to nrand is out of range");

  const int bucket_size = RAND_MAX / n;
  int r;

  do r = std::rand() / bucket_size;
  while (r >= n);

  return r;
}
