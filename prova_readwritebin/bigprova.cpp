// Copyright

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <cstdlib>
#include "../readwritebin.h"

using std::cout;
using std::endl;

int myrand(int n) {
  if (n <= 0 || n > RAND_MAX)
    throw std::domain_error("Argument to nrand is out of range");

  const int bucket_size = RAND_MAX / n;
  int r;

  do {
    r = std::rand() / bucket_size;
  } while (r >= n);

  return r;
}

int main() {
  Bin wr("big.txt", true);
  auto iter = wr.begin<double>();
  for (int i = 0; i != 1 << 17; ++i) {  // 27 = 1 GB, 17 = 1 MB
    // *iter++ = i;  // E' TROOOOOOPPO PIU' LENTOOO
    wr = static_cast<double>(i);
  }
  cout << "Size is " << wr.size() << endl;
  // std::random_shuffle(wr.begin<double>(), wr.end<double>(), myrand);
  cout << "Done 1. Size is " << wr.size() << endl;
  wr.close();

  Bin wr2("big.txt", false);
  auto strmsz = cout.precision(10);
  cout << "Size will be " << wr2.size() << endl;
  std::copy(wr2.begin<double>() + 0, wr2.begin<double>() + 100, std::ostream_iterator<double>(cout, "\n"));
  cout.precision(strmsz);
  wr2.close();
  
  Bin wr3("big.txt", false);
  std::vector<int> h = std::vector<int>(1 << 17, 2);
  cout << "Start... ";
  cout.flush();
  wr3.write_many(h.begin(), h.end(), 0);
  cout << "end. Size is " << wr3.size() << endl;
  auto h2 = wr3.get_values<char>(4, 0);
  for (auto a : h2) cout << static_cast<int>(a) << " ";
  cout << endl;

  return 0;
}
