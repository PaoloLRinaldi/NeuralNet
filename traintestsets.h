#ifndef TRAINTESTSETS_H
#define TRAINTESTSETS_H

#include <vector>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include "neural_web.h"


inline int myrand(int n) {
  if (n <= 0 || n > RAND_MAX)
    throw std::domain_error("Argument to nrand is out of range");
  const int bucket_size = RAND_MAX / n;
  int r;
  do r = std::rand() / bucket_size;
  while (r >= n);
  return r;
}

inline std::vector<int> ordered_vec(size_t n) {
  std::vector<int> ret(n);
  for (size_t i = 0; i != n; ++i)
    ret[i] = i;
  return ret;
}

template <typename T>
class TrainTestSets {
 public:

  //Constructors
 TrainTestSets(T *to_manage, size_t test_size, bool equal_sets = false, size_t n_output_layer = 0) : raw_set_(to_manage), test_sz_(test_size) {
    if (equal_sets && n_output_layer == 0)
      throw std::domain_error("You must provide the number of neurons in the output layer!");
     std::srand(std::time(0));
    if (size() < test_sz_)
      throw std::domain_error("Test size given is bigger than the total size!");
    if (equal_sets) {
      if (!n_output_layer)
	throw std::domain_error("Number of different classes not given!");
      equal_set_init(n_output_layer);
    } else {
      unequal_set_init();
    }
    begin_new_train();
    begin_new_test();
  }
  // Non credo vada bene questo copy constructor perché
  // se sto usando una derived class di T non viene
  // chiamato il rispettico virtual copy constructor
  /*
  TrainTestSets(const TrainTestSets &to_copy) :
      TrainTestSets(new T(*to_copy.raw_set_), to_copy.test_sz_) {
    train_elems_ = to_copy.train_elems_;
    train_elems_index_ = to_copy.train_elems_index_;
    test_elems_ = to_copy.test_elems_;
    test_elems_index_ = to_copy.test_elems_index_;
  }
  */
  /*
  TrainTestSets(const TrainTestSets &to_copy, T *ptr) : TrainTestSets(to_copy) {
    raw_set_ = ptr;
  }
  */

  void begin_new_train() {
    std::random_shuffle(train_elems_.begin(), train_elems_.end(), myrand);
    train_elems_index_ = 0;
  }
  void begin_new_test() { test_elems_index_ = 0; }

  //Check if you can aska anew training input
  bool train_not_finished() const { return train_elems_index_ < train_elems_.size(); }
  //Check if you can aska anew test input
  bool test_not_finished() const { return test_elems_index_ < test_elems_.size(); }
  //Ask a training input
  std::pair<std::vector<double>, int> next_train_elem() {
    if (!train_not_finished())
      throw std::domain_error("Train set finished, can't go further!");
    return (*raw_set_)[train_elems_[train_elems_index_++]];
  }
  //Ask a test input
  std::pair<std::vector<double>, unsigned int> next_test_elem() {
    if (!test_not_finished())
      throw std::domain_error("Test set finished, can't go further!");
    return (*raw_set_)[test_elems_[test_elems_index_++]];
  }

  //Getters
  size_t size() const { return raw_set_->size(); }
  size_t test_size() const { return test_sz_; }
  size_t train_size() const { return size() - test_size(); }
  std::pair<std::vector<double>, unsigned int> operator[](size_t index) const {
    if (size() <= index)
      throw std::out_of_range("Index out of range!");
    if (index < train_size())
      return (*raw_set_)[train_elems_[index]];
    return (*raw_set_)[test_elems_[index - train_size()]];
  }
  std::vector<int>::size_type train_index() { return train_elems_index_; }
  std::vector<int>::size_type test_index() { return test_elems_index_; }

  //Saving and loading set
  void save_sets(const std::string &file_name) const {
    std::ofstream fout(file_name);
    for (auto a : train_elems_)
      fout << a << " ";
    fout << std::endl;
    for (auto a : test_elems_)
      fout << a << " ";
  }
  void load_sets(const std::string &file_name) {
    train_elems_.clear();
    test_elems_.clear();

    std::ifstream is(file_name);
    if (!is.is_open())
      throw std::domain_error(("Could not open file " + file_name + "!").c_str());
    std::string line;
    std::getline(is, line);
    if (!is || line.empty())
      throw std::domain_error("Cannot read file for sets!");
    std::istringstream sline(line);
    std::copy(std::istream_iterator<int>(sline), std::istream_iterator<int>(), std::back_inserter(train_elems_));
    std::getline(is, line);
    if (!is || line.empty())
      throw std::domain_error("Cannot read file for sets!");
    sline.clear();
    sline.str(line);
    std::copy(std::istream_iterator<int>(sline), std::istream_iterator<int>(), std::back_inserter(test_elems_));

    test_sz_ = test_elems_.size();
    train_elems_index_ = 0;
    test_elems_index_ = 0;
  }

  /*
  ~TrainTestSets() {
    if (raw_set_) {
      delete raw_set_;
      raw_set_ = nullptr;
    }
  }
  */

 private:
  std::shared_ptr<const T> raw_set_;  // it must have a subscript operator that retruns a pair<vector<double>, int>
  size_t test_sz_;  // Size of the test set
  // Training data members
  std::vector<int> train_elems_;  // Vector to manage the order with which accessing raw_set_
  std::vector<int>::size_type train_elems_index_ = 0;
  //Test data memberd
  std::vector<int> test_elems_;
  std::vector<int>::size_type test_elems_index_ = 0;


  //Initialization in case you don't care to have balanced train and test set, with the same number ov elements for each class
  void unequal_set_init() {
    auto tmp_vec = ordered_vec(size());
    std::random_shuffle(tmp_vec.begin(), tmp_vec.end(), myrand);
    std::copy(tmp_vec.begin(), tmp_vec.begin() + train_size(), std::back_inserter(train_elems_));
    std::copy(tmp_vec.begin() + train_size(), tmp_vec.end(), std::back_inserter(test_elems_));
  }
  //Initialization in case you want the same number of elements for each class in both training and test set
  void equal_set_init(size_t nol) {
    size_t part_sz = size() / nol;
    size_t train_part_sz = train_size() / nol;
    if (part_sz != double(size()) / nol)
      throw std::domain_error("The number of different classes given is wrong! Make sure the total number of element is a multiple of the number of different classes");
    else if (train_part_sz != double(train_size()) / nol)
      throw std::domain_error("The number of different clases given is wrong! Make sure the total number of element in the train/test set is a multiple of the number of different classes");
    
    auto tmp_vec = ordered_vec(size());
    for (size_t i = 0; i != nol; ++i) {
      std::vector<int>::size_type beg_index = i * part_sz, end_index = (i + 1) * part_sz;
      std::random_shuffle(tmp_vec.begin() + beg_index, tmp_vec.begin() + end_index, myrand);
      std::copy(tmp_vec.begin() + beg_index, tmp_vec.begin() + beg_index + train_part_sz, std::back_inserter(train_elems_));
      std::copy(tmp_vec.begin() + beg_index + train_part_sz, tmp_vec.begin() + end_index, std::back_inserter(test_elems_));
    }
  }
};

/**********************************************
 *                                            *
 *                                            *
 *          All about neural networks         *
 *                                            *
 *                                            *
 **********************************************/

//DECLARATION

template <typename T>
double test_result (NeuralWeb &web, TrainTestSets<T> &sets, bool print_progress = true, std::ostream &os = std::cout);
template <typename T>
double test_from_train_set (NeuralWeb &web, TrainTestSets<T> &sets, bool print_progress = true, std::ostream &os = std::cout);  //to check if it's overfitting
template <typename T>
double train_from_set (NeuralWeb &web, TrainTestSets<T> &sets, bool print_progress = true, std::ostream &os = std::cout);  //Returns gradient average
template <typename T>
std::string gen_file_name(const NeuralWeb &web, const TrainTestSets<T> &sets, int training_round, double perc = -1);
template <typename T>
void save_web_sets (const NeuralWeb &web, const TrainTestSets<T> &sets, int training_round, double perc = -1);
template <typename T>
void load_web_sets (NeuralWeb &web, TrainTestSets<T> &sets, const std::string &file_name);  //Requires the file name except the "_sets.txt" or "_weights.txt" part. Per generare il file_name che riguarda altre web o sets si può usare gen_file_name
template <typename T>
void load_web_sets (NeuralWeb &web, TrainTestSets<T> &sets, int training_round, double perc = -1);  //In questa versione le informazioni le prende proprio da web e sets, in quella prima solo da file_name

//DEFINITION

template <typename T>
double test_result (NeuralWeb &web, TrainTestSets<T> &sets, bool print_progress, std::ostream &os) {
  sets.begin_new_test();
  int counter = 0;
  int percentage = 0;
  std::vector<unsigned int> answ_count(web.layers_info_vec().back(), 0);
  std::vector<unsigned int> correct_answ_count(web.layers_info_vec().back(), 0);
  std::vector<unsigned int> correct_count(web.layers_info_vec().back(), 0);
  while (sets.test_not_finished()) {
    auto input = sets.next_test_elem();
    unsigned int answ;
    if ((answ = web.ask(input.first)) == input.second)
      ++counter;
    if (print_progress) {
      int current_percentage = sets.test_index() * 100 / sets.test_size();
      if (current_percentage != percentage) {
	percentage = current_percentage;
	os << "Test progress: " << percentage << "%..." << "\r" << std::flush;
      }
      ++answ_count[answ];
      ++correct_count[input.second];
      if (answ == input.second)
        ++correct_answ_count[answ];
    }
  }
  if (print_progress) {
    std::cout << std::endl;
    for (unsigned int i = 0; i != answ_count.size(); ++i) {
      std::cout << i << ")" << "\tTimes answered: " << answ_count[i] << std::endl
                << "\tTimes answered properly: " << correct_answ_count[i] << std::endl
                << "\tTimes not answered when should: " << correct_count[i] - correct_answ_count[i] << std::endl
                << "\tTimes answered when should not: " << answ_count[i] - correct_answ_count[i] << std::endl
                << "\tTotal (" << i << ") testing inputs: " << correct_count[i] << std::endl;
    }
  }
  return static_cast<double>(counter) * 100 / sets.test_size();
}


template <typename T>
double test_from_train_set (NeuralWeb &web, TrainTestSets<T> &sets, bool print_progress, std::ostream &os) {
  sets.begin_new_train();
  int counter = 0;
  int percentage = 0;
  while (sets.train_not_finished()) {
    auto input = sets.next_train_elem();
    if (web.ask(input.first) == static_cast<unsigned int>(input.second))  //Non ti confondere, è giusto ask
      ++counter;
    if (print_progress) {
      int current_percentage = sets.train_index() * 100 / sets.train_size();
      if (current_percentage != percentage) {
	percentage = current_percentage;
	os << "Test progress (on training set): " << percentage << "%..." << "\r" << std::flush;
      }
    }
  }
  if (print_progress)
    os << std::endl;
  return static_cast<double>(counter) * 100 / sets.train_size();
}


template <typename T>
double train_from_set (NeuralWeb &web, TrainTestSets<T> &sets, bool print_progress, std::ostream &os) {
  sets.begin_new_train();
  int percentage = 0;
  double gradient_sum = 0;
  while (sets.train_not_finished()) {
    auto input = sets.next_train_elem();
    web.train(input.first, input.second);
    gradient_sum += web.grad_mod();
    if (print_progress) {
      int current_percentage = sets.train_index() * 100 / sets.train_size();
      if (current_percentage != percentage) {
	percentage = current_percentage;
	os << "Training progress: " << percentage << "%..." << "\r" <<  std::flush;
      }
    }
  }
  if (print_progress)
    os << std::endl;
  return gradient_sum / sets.train_size();
}


template <typename T>
std::string gen_file_name(const NeuralWeb &web, const TrainTestSets<T> &sets, int training_round, double perc) {
  std::string file_name = "perc_";
  if (perc == -1)
    file_name += "NULL";
  else {
    std::string sperc = std::to_string(perc);
    sperc[sperc.find('.')] = '_';
    file_name += sperc;
  }

  file_name += "_layersinfo_" + web.layers_info() +
    "trainsz_" + std::to_string(sets.train_size()) +
    "_testsz_" + std::to_string(sets.test_size()) +
    "_traininground_" + std::to_string(training_round);

  return file_name;
}

template <typename T>
void save_web_sets (const NeuralWeb &web, const TrainTestSets<T> &sets, int training_round, double perc) {
  using std::to_string;
  
  std::string file_name = gen_file_name(web, sets, training_round, perc);
  sets.save_sets(file_name + "_sets.txt");
  web.save_web(file_name + "_web.txt");
  
}

template <typename T>
void load_web_sets (NeuralWeb &web, TrainTestSets<T> &sets, const std::string &file_name){ 
  sets.load_sets(file_name + "_sets.txt");
  web.load_web(file_name + "_web.txt");  
}

template <typename T>
void load_web_sets (NeuralWeb &web, TrainTestSets<T> &sets, int training_round, double perc) {
  load_web_sets(web, sets, gen_file_name(web, sets, training_round, perc));
}


#endif //TRAINTESTSETS_H
