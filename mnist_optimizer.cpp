#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "arr_scale.h"
#include "readbindataset.h"
#include "readwritebin.h"


using std::string;
using std::cerr;
using std::clog;
using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::vector;
using std::copy;

template <typename T>
vector<vector<T>> vec_to_vec2(const vector<T> &v, int l);

template <typename T>
vector<T> vec2_to_vec(const vector<vector<T>> &v);


vector<vector<double>> transpose(const vector<vector<double>> &v);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "SYNTAX ERROR: ./mnist_optimizer <input_file> <output_file>" << endl;
    return 0;
  }
  
  int l = 28;
  
  string input_file_name = argv[1];
  string output_file_name = argv[2];
  
  Bin tmpin(input_file_name, false, false);
  Bin out(output_file_name, true, false);
  auto first4bytes = tmpin.get_values<unsigned char>(4, 0);
  out.write_many(first4bytes.begin(), first4bytes.end());
  tmpin.close();

  ReadBinDataset in(input_file_name, 0, 0, false);
  auto dims = in.get_dimentions();
  out.write_many(dims.begin(), dims.end());

  int n = in.size();
  clog << "Size  = " << n << endl;
  vector<double> tmp;
  vector<vector<double>> old_d;
  vector<double> new_tmp;
  vector<unsigned char> fin(l * l);
  
  cout << "Transpose input? (y/n): ";
  char yn;
  while (!(cin >> yn) || (yn != 'y' && yn != 'n')) {
    cout << "Error, repeat: ";
    cin.clear();
  }
  
  int perc = -1;
  for (int i = 0; i != n; ++i) {
    vector<vector<double>> new_d(l, vector<double>(l));
    tmp = in.get_hole_input(i);
    old_d = vec_to_vec2(tmp, l);
    arr_stretch2(new_d, l, old_d, centered_mat_coords(old_d, l));
    if (yn == 'y') new_d = transpose(new_d);
    new_tmp = vec2_to_vec(new_d);
    copy(new_tmp.begin(), new_tmp.end(), fin.begin());
    out.write_many(fin.begin(), fin.end());
    
    if ((i + 1) / (n / 100) != perc) {
      perc = (i + 1) / (n / 100);
      cout << perc << "%" << endl;
    }
  }
  
  cout << "Done" << endl;

  return 0;
}

// ../number_recog_proj/t10k-images-idx3-ubyte opt-t10k-images-idx3-ubyte



template <typename T>
vector<vector<T>> vec_to_vec2(const vector<T> &v, int l) {
  vector<vector<T>> ret(l, vector<T>(l));
  for (int i = 0; i != l; ++i)
    copy(v.cbegin() + (i * l), v.cbegin() + ((i + 1) * l), ret[i]. begin());
  return ret;
}


template <typename T>
vector<T> vec2_to_vec(const vector<vector<T>> &v) {
  auto sz = v.size();
  vector<T> ret(sz * sz);
  for (int i = 0; i != sz; ++i)
    copy(v[i].begin(), v[i].end(), ret.begin() + (i * sz));
  return ret;
}


vector<vector<double>> transpose(const vector<vector<double>> &v) {
  int sz = v.size();
  vector<vector<double>> ret(sz, vector<double>(sz));
  for (int i = 0; i != sz; ++i)
    for (int j = 0; j != sz; ++j)
      ret[i][j] = v[j][i];
  return ret;
}

