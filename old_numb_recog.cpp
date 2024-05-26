#include "neural_web.h"
#include "traintestsets.h"
#include "readbindataset.h"
#include "read_mnist.h"

int main() {
  Read_MNIST mnist_data("train-images-idx3-ubyte", "train-labels-idx1-ubyte", false);
  TrainTestSets<Read_MNIST> sets(mnist_data, 10000);
  //TrainTestSets sets(mnist_data, 60000);
  NeuralWeb web({784, 100, 10}, sigmoid, der_sigmoid);
  //web.get_weights("best_weights.txt");

  while(std::cin.get()) {
    double gradient = train_from_set(web, sets);
    double perc = test_result(web, sets);
    //save_weights_sets(web, sets, 1, perc);
    std::cout << "Final result: " << perc << "%" << std::endl;
    std::cout << "Gradient: " << gradient << std::endl;
  }
  
  return 0;
}
