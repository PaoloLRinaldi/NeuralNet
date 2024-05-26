// Copyright
#ifndef GUARD_NEURAL_WEB_H
#define GUARD_NEURAL_WEB_H

#include <vector>
#include <initializer_list>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>
// #include <cmath>



// ***Inizio varie funzioni di attivazione
inline double leaky_ReLU(double x) {
  return x > 0 ? x : 0.01 * x;
}
inline double der_leaky_ReLU(double x) {
  return x > 0 ? 1 : 0.01;
}
inline double ReLU(double x) {
  return x > 0 ? x : 0;
}
inline double der_ReLU(double x) {
  return x > 0;
}
double sigmoid(double x);
inline double der_sigmoid(double x) {
  double sigm = sigmoid(x);
  return sigm * (1 - sigm);
}
inline double linear(double x) {
  return x;
}
inline double der_linear(double) {
  return 1;
}
// La softmax è dicharata/definita all'interno della classe
// ***Fine varie funzioni di attivazione



class Node {
 public:
  // constructors
  Node(const std::vector<double>(ran_we), double ran_bias, bool use_bias = true) : bias(ran_bias), weights(ran_we), bias_enabled(use_bias) {}
  Node() = default;

  // Updaters of weights during training
  void change_weights(const std::vector<double>& grad);
  // Overrided
  void change_weights(std::vector<double>::const_iterator beg, const std::vector<double>::const_iterator end);

  // Operators
  double operator[](const int index) const { return weights[index]; }
  double operator=(double val) { return value = val; }
  operator double() const { return value; }

  // Getters
  double get_bias() const { return bias; }
  template<class T>
    double get_val(const std::vector<T>& input_nodes);  // Ricordarsi di passargli il vettore
                                                        // con i valori della funzione di
                                                        // attivazione e non dei neuroni
 private:
  // Class structure
  double value;
  double bias;
  std::vector<double> weights;
  bool bias_enabled = true;
};




class NeuralWeb {
 public:
  // Constructors
  NeuralWeb() = default;
  NeuralWeb(const std::vector<unsigned int>& len_vec, double act(double) = leaky_ReLU, double der_act(double) = der_leaky_ReLU);

  // Everything about training and asking the Web
  std::vector<double> train_output_layer(const std::vector<double> &input, const std::vector<double>& solution);
  // Overrided
  std::vector<double> train_output_layer(const std::vector<double> &input, size_t solution);
  std::vector<double> ask_output_layer(const std::vector<double> &input);
  size_t train(const std::vector<double> &input, const std::vector<double>& solution);
  // Overrided
  size_t train(const std::vector<double> &input, size_t solution);
  size_t ask(const std::vector<double> &input);
  std::vector<double> inverse_ask_sigm_sigm(const std::vector<double> &output);
  std::vector<double> improve_input(std::vector<double> &input, const std::vector<double> &solution, const double gradient_step, double &norm_grad);

  // Everything about saving and loading weights
  void load_web_ascii(std::istream &is);
  void load_web_bin(const std::string &file_name);
  void load_web(const std::string &file_name);
  void save_web_ascii(std::ostream &os) const;
  void save_web_bin(const std::string &file_name) const;
  void save_web(const std::string &file_name, bool go_ascii = true) const;

  // Printers
  void print_values(std::ostream &os = std::cout) const;

  // Setters
  void set_cost_function(const int a) { cost_function = a; }
  void set_last_layer_activation_function(double f(double), double der_f(double)) {
    if (f) {
      last_layer_act = f;
      last_layer_der_act = der_f;
      using_softmax = false;
    }
  }
  void set_batch (size_t b) { max_n_grads = b; }
  void set_tau (double t) { tau = t; }
  void set_grad_pass (double g_p) { grad_pass = g_p; }
  void set_bias(bool b) { bias_enabled = b; }
  void set_hidden_layer_activation_function(double f(double), double der_f(double)) {
    if (f) {
      hidden_layer_act = f;
      hidden_layer_der_act = der_f;
    }
  }
  void set_softmax(bool set) {
    using_softmax = set;
  }

  // Getters
  double grad_mod() const {
    return grad_module;
  }
  std::string layers_info() const;
  std::vector<unsigned int> layers_info_vec() const {
    return n_nodes;
  }

 private:
  // Class structure
  std::vector<std::vector<Node>> nodes;
  std::vector<unsigned int> n_nodes;

  // Parameters
  size_t max_n_grads = 1;  // Il numero di gradienti che deve collezionare prima di aggiornare pesi e bias. Anche chiamato batch
  double tau = 0;  // La cost function è C(w) + tau/2 * ||w||^2
  double grad_pass = 0.01;  // Il passo del gradiente
  double grad_module;
  bool bias_enabled = true;
  size_t n_grads = 0;  // Il numero di gradienti attualmente collezionati. Non deve essere cambiato, all'inizio DEVE ESSERE 0

  // All about asking and training
  // std::vector<std::vector<double>> gradients;
  std::vector<double> grad;
  void update_result(const std::vector<double>& input);
  void calculate_gradient(const std::vector<double>& input, const std::vector<double>& solution);
  void update_weights_bias();

  // All about functions used
  double softmax(double to_act);
  double der_softmax(double to_der) {
    double ret = softmax(to_der);
    return ret * (1 - ret);
  }
  bool using_softmax = false;
  void update_softmax_denominator();
  double softmax_denominator;
  double (*hidden_layer_act)(double);  // Funzione di attivazione negli hidden layers
  double (*hidden_layer_der_act)(double);  // Derivata funzione di attivazione
  double (*last_layer_act)(double) = sigmoid;
  double (*last_layer_der_act)(double) = der_sigmoid;
  int cost_function = 0;  // 0: Quadratic; 1: Cross-Entropy
  std::string hidden_layer_string() const;
  std::string last_layer_string() const;
  void str_to_hidden_layer(const std::string &s);
  void str_to_last_layer(const std::string &s);
};


#endif  // GUARD_NEURAL_WEB_H
