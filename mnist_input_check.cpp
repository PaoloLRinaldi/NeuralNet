#include <iostream>
#include <string>
#include "readbindataset.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

template <typename T>
void print_elem(const T &v, int l) {
  cout << " " << string(28 * 2, '-') << endl;
  for (int i = 0; i != l; ++i) {
    cout << "|";
    for (int j = 0; j != l; ++j) {
      auto val = v[(i * l) + j];
      if (val < 64) cout << " ";
      else if (val < 128) cout << ".";
      else if (val < 192) cout << "*";
      else cout << "O";
      cout << " ";
    }
    cout << "|" << endl;
  }
  cout << " " << string(28 * 2, '-') << endl;
  cout << endl << endl;
  
}

int main(int argc, char *argv[]) {
  if (argc != 2) std::cerr << "Syntax error" << endl;
  
  ReadBinDataset in(argv[1], 0, 0, false);
  int sz = in.size();
  cout << "Size: " << sz << endl;
  cin.get();
  int count = 0;
  while (cin && count < sz) {
    print_elem(in.get_hole_input(count++), 28);
    cin.get();
  }
  cout << "Bye" << endl;
  return 0;
}