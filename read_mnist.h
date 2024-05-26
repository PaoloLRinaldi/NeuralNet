// Copyright
#ifndef READ_MNIST_H
#define READ_MNIST_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "./readbindataset.h"

class Read_MNIST : public ReadBinDataset {
 public:
  // Constructors
  Read_MNIST(const std::string& filename,
             double mn,
             double mx,
             bool little_end) = delete;
  explicit Read_MNIST(const std::string &input_file,
                      const std::string &solution_file,
                      double mn = 0,
                      double mx = 0,
                      bool is_little_endian = true) :
      ReadBinDataset(input_file, mn, mx, is_little_endian),
      sol_data(new ReadBinDataset(solution_file, 0, 0, is_little_endian)) {
    if (size() != sol_data->size())
      throw std::domain_error("Input file and solution file have "
                              "different numbers of data!");
  }


  // Getters
  size_t size() const override { return get_dimentions()[0]; }
  std::vector<unsigned int> get_input_dimentions() const {
    return get_dimentions();
  }
  unsigned int get_sol(size_t index) const {
    return sol_data->get_hole_input(index)[0];
  }
  std::pair<std::vector<double>, unsigned int> operator[] (size_t index) const override { return {get_hole_input_norm(index), get_sol(index)}; }

  // Printers
  void print_image_and_sol(size_t index, std::ostream &os = std::cout);

  ~Read_MNIST() override { delete sol_data; }

 private:
  // Data members
  ReadBinDataset *sol_data;
};



#endif  // READ_MNIST_H
