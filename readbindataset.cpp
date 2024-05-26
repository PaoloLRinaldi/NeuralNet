#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include "readbindataset.h"

using std::vector;
using std::cout;
using std::cin;
using std::endl;

using namespace std::placeholders;

ReadBinDataset::ReadBinDataset(const std::string& file_name,
                               double mn,
                               double mx,
                               bool little_end) :
    fis(file_name),
    is_little_endian(little_end),
    min_norm(mn),
    max_norm(mx) {
  if (max_norm < min_norm)
    throw std::domain_error("Max element < Min element, cannot normalize!");
  read_magic_number();
  switch (data_type) {
  case 0x08:
    data_size = sizeof(unsigned char);
    break;
  case 0x09:
    data_size = sizeof(char);
    break;
  case 0x0B:
    data_size = sizeof(short);
    break;
  case 0x0C:
    data_size = sizeof(int);
    break;
  case 0x0D:
    data_size = sizeof(float);
    break;
  case 0x0E:
    data_size = sizeof(double);
    break;
  default:
    throw std::domain_error("Can't detect data type!");
  }

  single_input_size = data_size;
  for (unsigned int i = 1; i != dimentions.size(); ++i)
    single_input_size *= dimentions[i];

  begin_data_pos = 4 + sizeof(int) * n_dimentions; //il primo 4 perché il numero magico è da 4 byte, il secondo perché le dimensioni sono salvate come interi
}


std::istream& ReadBinDataset::read_magic_number() {
  unsigned char byte1 = get_binary_integer<unsigned char>();
  unsigned char byte2 = get_binary_integer<unsigned char>();
  if (byte1 != 0 || byte2 != 0)
     std::cerr << "Something went wrong: first two bytes = " << byte1 << "    " << byte2 << endl;

  data_type = get_binary_integer<unsigned char>();
  n_dimentions = get_binary_integer<unsigned char>();

  dimentions.clear();
  for (size_t i = 0; i != n_dimentions; ++i)
    dimentions.push_back(get_binary_integer<int>());

  return fis;
}

// Returns an entire input, without differentiating between input and solution
std::vector<double> ReadBinDataset::get_hole_input(size_t index) const {
  jump_to(index);
  unsigned int elems_in_input = single_input_size / data_size;
  vector<double> ret_vec(elems_in_input);
  for (unsigned int i = 0; i != elems_in_input; ++i)
    ret_vec[i] = switch_datatype();
  return ret_vec;
}

// Returns an entire input normalized,
// without differentiating between input and solution
std::vector<double> ReadBinDataset::get_hole_input_norm(size_t index) const {
  jump_to(index);
  unsigned int elems_in_input = single_input_size / data_size;
  vector<double> ret_vec(elems_in_input);
  for (unsigned int i = 0; i != elems_in_input; ++i)
    ret_vec[i] = switch_datatype();
  if (max_norm || min_norm) {
    std::transform(ret_vec.begin(),
                   ret_vec.end(),
                   ret_vec.begin(),
                   std::bind(normalize, _1, min_norm, max_norm));
  }
  return ret_vec;
}

//Retrns a pair with the input and it's relative solution
std::pair<std::vector<double>, unsigned int> ReadBinDataset::operator[] (size_t index) const {  //The solution is the last element
  auto ret_vec = get_hole_input(index);
  unsigned int solution = ret_vec.back();
  ret_vec.erase(ret_vec.end() - 1);
  if (max_norm || min_norm) {
    std::transform(ret_vec.begin(),
                   ret_vec.end(),
                   ret_vec.begin(),
                   std::bind(normalize, _1, min_norm, max_norm));
  }
  return {ret_vec, solution};
}

double ReadBinDataset::switch_datatype() const {
  switch(data_type) {
  case 0x08:
    return get_binary_integer<unsigned char>();
    break;
  case 0x09:
    return get_binary_integer<char>();
    break;
  case 0x0B:  //Lo so passa da 0x09 a 0x0B, è strano ma è così
    return get_binary_integer<short>();
    break;
  case 0x0C:
    return get_binary_integer<int>();
    break;
  case 0x0D:
    return get_binary_float<float>();
    break;
  case 0x0E:
    return get_binary_float<double>();
    break;
  }
  return 0;
}
