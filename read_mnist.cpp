// Copyright
#include "./read_mnist.h"

void Read_MNIST::print_image_and_sol(size_t index, std::ostream &os) { {
    size_t x_dim = get_input_dimentions()[1];
    // Mi fermo a 2 perché so che è un'immagine di 2 dimensioni.
    // Lo 0 dice quante immagini ci sono, non mi interessa
    size_t y_dim = get_input_dimentions()[2];
    std::vector<double> vec = get_hole_input_norm(index);
    for (size_t i = 0; i != x_dim; ++i) {
      for (size_t j = 0; j != y_dim; ++j) {
        double b = vec[x_dim * i + j];
        if (b < 0.25)
          os << " ";
        else if (b < 0.5)
          os << ".";
        else if (b < 0.75)
          os << "o";
        else
          os << "*";
        os << " ";
      }
      os << std::endl;
    }
    os << std::endl << "Solution: " << get_sol(index) << std::endl << std::endl;
  }
}
