// Copyright
#include <vector>
#include <stdexcept>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <random>
#include <fstream>
#include "./neural_web.h"
#include "./readwritebin.h"


using std::istringstream;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::default_random_engine;
using std::normal_distribution;

template<class T>
double Node::get_val(const std::vector<T>& input_values) {
  if (input_values.size() != weights.size())
    throw std::invalid_argument("Number of nodes doesn't "
                                "correspond to the number "
                                "of weights for this node");
  value = bias_enabled ? bias : 0;
  for (unsigned int i = 0; i != weights.size(); ++i) {
    value += input_values[i] * weights[i];
  }
  return value;
}

void Node::change_weights(
    std::vector<double>::const_iterator beg,
    const std::vector<double>::const_iterator end) {
  vector<double>::iterator we_it = weights.begin();
  while (beg != end - 1)
    *we_it++ -= *beg++;
  bias -= *beg;
}

void Node::change_weights(const std::vector<double>& grad) {
  for (unsigned int i = 0; i != weights.size(); ++i) {
    // assumo di non aver preso il negativo del gradiente
    weights[i] -= grad[i];
  }
  bias -= grad.back();
}

/*******************************************
 *               NEURALWEB                 *
 *******************************************/

NeuralWeb::NeuralWeb(const std::vector<unsigned int>& len_vec,
                     double act(double),
                     double der_act(double)) :
    n_nodes(len_vec),
    hidden_layer_act(act),
    hidden_layer_der_act(der_act) {
  default_random_engine e(std::time(0));
  size_t n_variables = 0;
  // Metto il "<" perché len_vec può essere vuoto
  for (unsigned int i = 1; i < n_nodes.size(); ++i) {
    vector<Node> new_nodes;
    normal_distribution<> n(0.0, 1.0 / std::sqrt(n_nodes[i]));
    for (unsigned int k = 0; k != n_nodes[i]; ++k) {
      vector<double> ran_weights;
      for (unsigned int j = 0; j != n_nodes[i - 1]; ++j) {
        ran_weights.push_back(n(e));
      }
      // Per ora assegno al bias un valore tra -1 e 1
      Node init_node(ran_weights,
                     (static_cast<double>(std::rand()) / RAND_MAX) * 2 - 1,
                     bias_enabled);
      new_nodes.push_back(init_node);
    }
    nodes.push_back(new_nodes);

    n_variables += (n_nodes[i - 1] + 1) * n_nodes[i];
  }

  grad.resize(n_variables, 0);
}


vector<double> NeuralWeb::ask_output_layer(const std::vector<double>& input) {
  if (input.size() != n_nodes[0]) {
    throw std::invalid_argument("Number of nodes doesn't correspond "
                                "to the nomber of weights for this node");
  }

  update_result(input);

  // in train non va fatto, perché già c'è in calculate_gradient
  update_softmax_denominator();

  return vector<double>(nodes.back().begin(), nodes.back().end());
}

vector<double> NeuralWeb::train_output_layer(const std::vector<double>& input,
                                          const std::vector<double>& solution) {
  if (input.size() != n_nodes[0] || solution.size() != n_nodes.back())
    throw std::invalid_argument("Number of nodes doesn't correspond "
                                "to the number of weights for this node");

  update_result(input);
  calculate_gradient(input, solution);
  return vector<double>(nodes.back().begin(), nodes.back().end());
}

// Overrided
vector<double> NeuralWeb::train_output_layer(const std::vector<double>& input,
                                             size_t solution) {
  if (solution >= n_nodes.back())
    throw std::invalid_argument("Solution over range");
  vector<double> vec_sol(n_nodes.back(), 0.01);
  vec_sol[solution] = 0.99;
  return train_output_layer(input, vec_sol);
}

size_t NeuralWeb::ask(const std::vector<double>& input) {
  vector<double> output_layer = ask_output_layer(input);
  // Returno l'indice dell'elemento più grande;
  // max_element individua l'iteratore, distance la distanza tra iteratori
  return std::distance(output_layer.begin(),
                       std::max_element(output_layer.begin(),
                                        output_layer.end()));
}

size_t NeuralWeb::train(const std::vector<double>& input,
                        const std::vector<double>& solution) {
  vector<double> output_layer = train_output_layer(input, solution);
  return std::distance(output_layer.begin(),
                       std::max_element(output_layer.begin(),
                                        output_layer.end()));
}

// Overrided
size_t NeuralWeb::train(const std::vector<double>& input, size_t solution) {
  vector<double> output_layer = train_output_layer(input, solution);
  return std::distance(output_layer.begin(),
                       std::max_element(output_layer.begin(),
                                        output_layer.end()));
}

void NeuralWeb::update_result(const std::vector<double>& input) {
  for (unsigned int j = 0; j != nodes[0].size(); ++j)
    // Non credo vada applicata la funzione di attivazione anche all'input
    nodes[0][j].get_val(input);


  for (unsigned int i = 1; i != nodes.size(); ++i) {
    vector<double> activated_nodes(nodes[i - 1].size());
    std::transform(nodes[i - 1].begin(),
                   nodes[i - 1].end(),
                   activated_nodes.begin(),
                   hidden_layer_act);
    for (unsigned int j = 0; j != nodes[i].size(); ++j) {
      nodes[i][j].get_val(activated_nodes);
    }
  }
}

// Questa funzione va bene solo se si è selto sigmoide come funzione
// di attivazione per gli hidden e la retta per quello finale
vector<double> NeuralWeb::inverse_ask_sigm_sigm(const vector<double> &output) {
  // assegno all'output layer l'output desiderato
  for (unsigned int i = 0; i != n_nodes.back(); ++i)
    nodes.back()[i] = std::log(output[i] / (1 - output[i]));

  // MI ASPETTO CHE L'INPUT SIA GIA' NORMALIZZATO
  // quando k == 1 giù ci sta il return
  for (int k = nodes.size(); ; --k) {
    vector<double> vec(n_nodes[k - 1]);


    // Mi ricavo il layer di nodi subito prima (riga x colonna)
    for (unsigned int i =  0; i != n_nodes[k - 1]; ++i) {
      for (unsigned int j = 0; j != n_nodes[k]; ++j)
        vec[i] += nodes[k - 1][j][i] * nodes[k - 1][j];
    }


    // Ora normalizzo tra 0.01 e 0.99
    double min = vec[0];
    for (unsigned int i = 1; i != n_nodes[k - 1]; ++i)
      if (vec[i] < min) min = vec[i];

    for (unsigned int i = 0; i != n_nodes[k - 1]; ++i)
      vec[i] -= min;

    double max = min;
    for (unsigned int i = 0; i != n_nodes[k - 1]; ++i)
      if (vec[i] > max) max = vec[i];

    for (unsigned int i = 0; i != n_nodes[k - 1]; ++i)
      vec[i] = vec[i] * 0.98 / max + 0.01;

    if (k == 1) return vec;

    for (unsigned int i = 0; i != n_nodes[k - 1]; ++i)
      // l'inversa della funzione di attivazione
      nodes[k - 2][i] = std::log(vec[i] / (1 - vec[i]));
  }
}

void NeuralWeb::str_to_hidden_layer(const std::string &s) {
  if (s == "sigmoid") {
    hidden_layer_act = sigmoid;
    hidden_layer_der_act = der_sigmoid;
  } else if (s == "ReLU") {
    hidden_layer_act = ReLU;
    hidden_layer_der_act = der_ReLU;
  } else if (s == "leaky_ReLU") {
    hidden_layer_act = leaky_ReLU;
    hidden_layer_der_act = der_leaky_ReLU;
  } else if (s == "linear") {
    hidden_layer_act = linear;
    hidden_layer_der_act = der_linear;
  } else if (s == "NULL") {
    hidden_layer_act = nullptr;
    hidden_layer_der_act = nullptr;
  } else if (s == "User_defined_function") {
      throw std::domain_error("User defined activation function!");
  } else {
      throw std::domain_error("Can't detect activation function!");
  }
}

void NeuralWeb::str_to_last_layer(const std::string &s) {
  if (s == "softmax") {
    using_softmax = true;
  } else if (s == "sigmoid") {
    last_layer_act = sigmoid;
    last_layer_der_act = der_sigmoid;
  } else if (s == "ReLU") {
    last_layer_act = ReLU;
    last_layer_der_act = der_ReLU;
  } else if (s == "leaky_ReLU") {
    last_layer_act = leaky_ReLU;
    last_layer_der_act = der_leaky_ReLU;
  } else if (s == "linear") {
    last_layer_act = linear;
    last_layer_der_act = der_linear;
  }  else if (s == "NULL") {
    last_layer_act = nullptr;
    last_layer_der_act = nullptr;
  } else if (s == "User_defined_function") {
      throw std::domain_error("User defined activation function!");
  }  else {
      throw std::domain_error("Can't detect activation function!");
  }
}

void NeuralWeb::load_web_ascii(std::istream& is) {
  std::string s;
  getline(is, s);
  if (!is || s.empty()) throw std::domain_error("Unable to read file!");
  n_nodes.clear();
  nodes.clear();
  // Ignoro la struttura

  // Parametri
  is >> max_n_grads;
  is >> tau;
  is >> grad_pass;
  is >> bias_enabled;
  is >> n_grads;

  if (!is) throw std::domain_error("Something went wrong while "
                                    "loading parameters");

  is.get();  // Per prendere l'"a capo"
  // Hidden layer act func
  getline(is, s);
  str_to_hidden_layer(s);

  // Output layer act func
  getline(is, s);
  str_to_last_layer(s);

  // Cost function
  is >> cost_function;

  is.get();  // Per prendere l'"a capo"

  // Takes the first line of input
  while (getline(is, s) && !s.empty()) {
    vector<Node> layer;
    // Starting from that first line just taken
    // and proceeding with the next ones
    do {
      istringstream sis(s);
      double bias;
      sis >> bias;
      vector<double> weights;
      double d;
      // Preso tutti i pesi che partono da un neurone
      while (sis >> d) weights.push_back(d);
      // ovvero voglio che lo faccia una sola volta
      if (layer.empty()) n_nodes.push_back(weights.size());
      layer.emplace_back(weights, bias);
      // Until he finds a blank line
    } while (getline(is, s) && !s.empty());
    nodes.emplace_back(std::move(layer));
  }
  std::reverse(n_nodes.begin(), n_nodes.end());
  // aggiungo la lunghezza del layer di neuroni di output
  n_nodes.push_back(nodes.front().size());
  // Li ho printati al contrario, che ci devo fa...?
  std::reverse(nodes.begin(), nodes.end());

  n_grads = 0;
  // gradients.clear();
  grad.clear();
  size_t n_variables = 0;
  for (size_t i = 0; i != n_nodes.size() - 1; ++i)
    n_variables += (n_nodes[i] + 1) * n_nodes[i + 1];
  grad.resize(n_variables, 0);
}

void NeuralWeb::load_web_bin(const std::string &file_name) {
  n_nodes.clear();
  nodes.clear();

  // Primi 2 bytes
  Bin f(file_name);
  for (int i = 0; i != 4; ++i) {
    if (f.get_value<unsigned char>())
      throw std::domain_error("First four bytes are not null!");
  }

  // Struttura
  unsigned int linfo_sz = f.get_value<unsigned int>();
  n_nodes = f.get_values<unsigned int>(linfo_sz);

  // Parametri
  max_n_grads = f.get_value<unsigned int>();
  tau = f.get_value<double>();
  grad_pass = f.get_value<double>();
  bias_enabled = f.get_value<unsigned char>();
  n_grads = f.get_value<unsigned int>();

  // Funzioni di attivazione
  unsigned char hlen = f.get_value<unsigned char>();
  str_to_hidden_layer(f.get_string(hlen));
  unsigned char llen = f.get_value<unsigned char>();
  str_to_last_layer(f.get_string(llen));

  // Cost function
  cost_function = f.get_value<unsigned char>();

  // Pesi e bias
  for (unsigned int i = n_nodes.size() - 1; i != 0; --i) {  // Voglio che si fermi a 1
    vector<Node> layer;
    for (unsigned int j = 0; j != n_nodes[i]; ++j) {
      double bias = f.get_value<double>();
      layer.emplace_back(f.get_values<double>(n_nodes[i - 1]), bias);
    }
    nodes.emplace_back(std::move(layer));
  }

  // Sono stati printati al contrario
  std::reverse(nodes.begin(), nodes.end());

  // Preparazione del gradiente
  n_grads = 0;
  grad.clear();
  size_t n_variables = 0;
  for (size_t i = 0; i != n_nodes.size() - 1; ++i)
    n_variables += (n_nodes[i] + 1) * n_nodes[i + 1];
  grad.resize(n_variables, 0);
}

void NeuralWeb::load_web(const std::string& file_name) {
  Bin tmp(file_name);
  bool go_ascii = tmp.get_value<unsigned char>() ||
                  tmp.get_value<unsigned char>() ||
                  tmp.get_value<unsigned char>() ||
                  tmp.get_value<unsigned char>();
  tmp.close();

  if (go_ascii) {
    std::ifstream fin(file_name);
    load_web_ascii(fin);
    return;
  }
  load_web_bin(file_name);
}

std::string NeuralWeb::hidden_layer_string() const {
  if (hidden_layer_act == sigmoid)
    return "sigmoid";
  else if (hidden_layer_act == ReLU)
    return "ReLU";
  else if (hidden_layer_act == leaky_ReLU)
    return "leaky_ReLU";
  else if (hidden_layer_act == linear)
    return "linear";
  else if (hidden_layer_act == nullptr)
    return "NULL";
  else
    return "User_defined_function";
}

std::string NeuralWeb::last_layer_string() const {
  if (using_softmax)
    return "softmax";
  if (last_layer_act == sigmoid)
    return "sigmoid";
  else if (last_layer_act == ReLU)
    return "ReLU";
  else if (last_layer_act == leaky_ReLU)
    return "leaky_ReLU";
  else if (last_layer_act == linear)
    return "linear";
  else if (last_layer_act == nullptr)
    return "NULL";
  else
    return "User_defined_function";
}

void NeuralWeb::save_web_ascii(std::ostream& os) const {
  // Prima salvo la struttura (che verrà ignorata),
  // poi i parametri,
  // poi i pesi

  // Struttura
  os << layers_info() << endl;
  // Parametri
  os << max_n_grads << endl;
  os << tau << endl;
  os << grad_pass << endl;
  os << bias_enabled << endl;
  os << n_grads << endl;

  // Funzione di attivazione hidden layers
  os << hidden_layer_string() << endl;

  // Funzione attivazione output layer
  os << last_layer_string() << endl;

  // Cost function (0: Quadratic, 1: Cross-Entropy)
  os << cost_function << endl;

  for (auto it = nodes.crbegin(); it != nodes.crend(); ++it) {
    for (unsigned int i = 0; i != it->size(); ++i) {
      os << (*it)[i].get_bias();
      // ovvero j è l'indice che scorre sulla riga di neuroni subito prima
      // di quella che si sta guardando e serve, appunto, per dire quel
      // peso a quale neurone della fila prima è collegato (il -2 è perché
      // -1 per il classico motivo che si va oltre, l'altro -1 per dire di
      // guardare la riga prima)
      for (unsigned int j = 0;
           j != n_nodes[n_nodes.size() - (it - nodes.rbegin()) - 2];
           ++j) {
         os << " " << (*it)[i][j];
      }
      os << endl;
    }
    os << endl;
  }
  os << endl << endl;
}

void NeuralWeb::save_web_bin(const std::string &file_name) const {
  Bin f(file_name);
  f.write_many<unsigned char>({0, 0, 0, 0});
  vector<unsigned int> linfo(layers_info_vec());
  f.write<unsigned int>(linfo.size());
  f.write_many<unsigned int>(linfo.begin(), linfo.end());  // sono unsigned int
  f.write<unsigned int>(max_n_grads);
  f.write<double>(tau);
  f.write<double>(grad_pass);
  f.write<unsigned char>(bias_enabled ? 1 : 0);
  f.write<unsigned int>(n_grads);

  // Funzione di attivazione hidden layers
  std::string hlay = hidden_layer_string();
  f.write<unsigned char>(hlay.size());
  f.write_string(hlay);

  // Funzione attivazione output layer
  std::string llay = last_layer_string();
  f.write<unsigned char>(llay.size());
  f.write_string(llay);

  f.write<unsigned char>(cost_function);

  for (auto it = nodes.crbegin(); it != nodes.crend(); ++it) {
    for (unsigned int i = 0; i != it->size(); ++i) {
      f.write<double>((*it)[i].get_bias());
      for (unsigned int j = 0;
           j != n_nodes[n_nodes.size() - (it - nodes.rbegin()) - 2];
           ++j) {
        f.write<double>((*it)[i][j]);
      }
    }
  }
}

void NeuralWeb::save_web(const std::string& file_name, bool go_ascii) const {
  if (go_ascii) {
    std::ofstream fout(file_name);
    save_web_ascii(fout);
    return;
  }
  save_web_bin(file_name);
}

// Prima si calcola il denominatore, POI si
// può applicare la softmax ai vari nodi
void NeuralWeb::update_softmax_denominator() {
  softmax_denominator = 0;
  for (const auto& a : nodes.back())
    softmax_denominator += std::exp(a);
}


double NeuralWeb::softmax(double to_act) {
  return std::exp(to_act) / softmax_denominator;
}


double sigmoid(double x) {
  return 1 / (1 + std::exp(-x));
}

// input non è const di proposito, lo modifico
std::vector<double> NeuralWeb::improve_input(vector<double> &input,
                                             const vector<double>& solution,
                                             const double gradient_step,
                                             double &norm_grad) {
  auto ret = ask_output_layer(input);
  vector<double> dCdl(n_nodes[nodes.size()]);

  // CONSIDERO IL CASO IN CUI NON SI USI LA SOFTMAX E
  // SI USI LA FUNZIONE QUADRATICA STANDARD

  for (unsigned int i = 0; i != dCdl.size(); ++i)
    // ogni neurone non è attivato di default
    dCdl[i] = last_layer_act(nodes.back()[i]) - solution[i];

  for (int i = nodes.size() - 1; i !=  -1; --i) {
    vector<double> new_dCdl(n_nodes[i]);
    for (unsigned k = 0; k != n_nodes[i]; ++k) {
      for (unsigned j = 0; j != n_nodes[i + 1]; ++j) {
        bool is_last_layer = i == static_cast<int>(nodes.size() - 1);
        // Preso il neurone non attivato l(i)(k), ho
        // dC/dact(l(i)(k)) = sum_{j} dC/dact(l(i+1)(j)) *
        //                    dact(l(i+1)(j))/dl(i+1)(j) *
        //                    dl(i+1)(j)/dact(l(i)(k)).
        // L'ultimo sarà uguale al peso
        new_dCdl[k] += dCdl[j] *
            (is_last_layer ?
             last_layer_der_act :
             hidden_layer_der_act)(nodes[i][j]) * nodes[i][j][k];
      }
    }
    dCdl = std::move(new_dCdl);
  }

  for (unsigned int i = 0; i != input.size(); ++i)
    input[i] -= dCdl[i] * gradient_step;

  norm_grad = 0.0;
  for (auto a : dCdl)
    norm_grad += a * a;

  norm_grad = std::sqrt(norm_grad);

  return ret;
}

void NeuralWeb::calculate_gradient(const vector<double>& input,
                                   const vector<double>& solution) {
  // vector<double> grad;
  double scale = grad_pass / max_n_grads;
  // Il prodotto della derivata parziale della Cost
  // rispetto al valore della activation
  // function per la derivata dell'activation
  // function (nel punto z, che è la somma
  // pesata più bias)
  vector<double> dCda_dadz;
  if (using_softmax) update_softmax_denominator();

  for (size_t i = 0; i != n_nodes.back(); ++i) {
    switch (cost_function) {
    case 0:
      if (!using_softmax)
        // in realtà il 2 non svolge nessun ruolo utile
	dCda_dadz.push_back((/*2 * */(last_layer_act(nodes.back()[i]) - solution[i])) * last_layer_der_act(nodes.back()[i]));
      else
	dCda_dadz.push_back((/*2 * */(softmax(nodes.back()[i]) - solution[i])) * der_softmax(nodes.back()[i]));
      break;
    case 1:
      if (!using_softmax)
        // Fammi capire: per passare dalla cost function
        // quadratica standard, alla cross-entropy basta
        // semplicemente sostituire la riga subito sopra
        // con questa? Mah.
        // In realtà last_layer_act(nodes.back()[i])
        // andrebbe modificato per la somma dei neuroni
        // attivati del last layer (o output layer). Nel
        // caso si usi la softmax fa 1.
        dCda_dadz.push_back(last_layer_act(nodes.back()[i]) - solution[i]);
      else
        dCda_dadz.push_back(softmax(nodes.back()[i]) - solution[i]);
      break;
    }
  }

  // Il cuore della funzione: la backpropagation
  // degli hidden layers (tranne l'ultimo)
  vector<double>::iterator it = grad.begin();

  // Non voglio che arrivi a 0, per cui l'ultimo layer sarà nodes[1]
  for (unsigned int i = nodes.size() - 1; i != 0; --i) {
    // FASE 1/2: uso il dCda_dadz creati precedentemente (nell'iterazione
    // prima o all'inizio della funzione)
    for (size_t j = 0; j != nodes[i].size(); ++j) {
      for (size_t k = 0; k != nodes[i - 1].size(); ++k) {
        // è giusto nodes[i][j][k]? Comunque quel + tau è per quella
        // storia di allontanarsi dai bordi della sigmoide per non
        // rendere troppo lento l'apprendimento
        *it++ += (dCda_dadz[j] * hidden_layer_act(nodes[i - 1][k]) +
                  (i != nodes.size() - 1 ? tau * nodes[i][j][k] : 0)) * scale;
      }
      *it++ += dCda_dadz[j] * scale;  // bias
    }

    // FASE 2/2: creo new_dCda_dadz per la prossima
    // iterzione o per l'ultimo layer nodes[0]
    vector<double> new_dCda_dadz;
    for (size_t j = 0; j != nodes[i - 1].size(); ++j) {
      double sum = 0;
      for (size_t k = 0; k != nodes[i].size(); ++k) {
        // dC/da(n) * da(n)/dz * dz/da(n-1), dove dz/da(n-1) è il peso
        sum += dCda_dadz[k] * nodes[i][k][j];
      }
      new_dCda_dadz.push_back(sum * hidden_layer_der_act(nodes[i - 1][j]));
    }

    dCda_dadz = std::move(new_dCda_dadz);
  }  // fine for sulle i

  // nodes[0] lo faccio a parte
  for (size_t i = 0; i != nodes[0].size(); ++i) {
    for (size_t j = 0; j != input.size(); ++j) {
      *it++ += dCda_dadz[i] * input[j] * scale;
    }
    *it ++= dCda_dadz[i] * scale;
  }
  if (it != grad.end())
    throw std::domain_error("Something went wrong during training!");
  // gradients.emplace_back(std::move(grad));

  if (++n_grads == max_n_grads) {
    update_weights_bias();
    n_grads = 0;
  }
}


void NeuralWeb::update_weights_bias() {
  /*
  auto grad_length = gradients[0].size();
  auto grad_num = gradients.size();
  // dai vari gradienti ricavo il gradiente medio
  vector<double> avarage_grad;
  avarage_grad.reserve(grad_length);
  for (size_t i = 0; i != grad_length; ++i) {
    double sum = 0;
    for (size_t j = 0; j != grad_num; ++j)
      sum += gradients[j][i];
    // faccio la media e moltiplico l'intero vettore
    // (ovvero ogni singola componente) per un fattore
    // che mi limita il singolo passo del gradiente (grad_pass)
    avarage_grad.push_back(grad_pass * sum / max_n_grads);
  }
  */

  // Prima c'era sempre avarage_grad al posto di grad
  // da qui in poi
  // Qui aggiorno tutti i pesi e i bias
  vector<double>::size_type read_grad = 0;
  for (int i = nodes.size() - 1; i != -1; --i) {
    for (size_t j = 0; j != nodes[i].size(); ++j) {
      nodes[i][j].change_weights(grad.begin() + read_grad,
                                 grad.begin() + read_grad + n_nodes[i] + 1);
      read_grad += n_nodes[i] + 1;  // il "+ 1" è il bias
    }
  }

  // Aggiorno grad_module, ovvero il modulo del gradiente
  double sum = 0;
  for (const auto a : grad)
    sum += a * a;
  grad_module = std::sqrt(sum);

  // gradients.clear();
  grad = std::move(vector<double>(grad.size(), 0));
}



void NeuralWeb::print_values(std::ostream& os) const {
  vector<Node>::size_type max_len = n_nodes[0];

  for (const auto& a : nodes)
    max_len = max_len > a.size() ? max_len : a.size();

  os << std::string(max_len - n_nodes[0], ' ');
  os << "?";
  for (size_t i = 1; i != n_nodes[0]; ++i)
    os << " ?";
  os << endl;
  for (const auto &a : nodes) {
    os << std::string(max_len - a.size(), ' ');
    os << a[0];
    for (size_t i = 1; i != a.size(); ++i)
      os << " " << a[i];
    os << endl;
  }

  os << endl << endl << endl;

  os << std::string(max_len - n_nodes[0], ' ');
  os << "?";
  for (size_t i = 1; i != n_nodes[0]; ++i)
    os << " ?";
  os << endl;
  for (size_t i = 0; i != nodes.size() - 1; ++i) {
    os << std::string(max_len - nodes[i].size(), ' ');
    os << hidden_layer_act(nodes[i][0]);
    for (size_t j = 1; j != nodes[i].size(); ++j)
      os << " " << hidden_layer_act(nodes[i][j]);
    os << endl;
  }
  os << std::string(max_len - nodes.back().size(), ' ');
  os << last_layer_act(nodes.back()[0]);
  for (size_t j = 1; j != nodes.back().size(); ++j)
    os << " " << last_layer_act(nodes.back()[j]);
  os << endl;
}

std::string NeuralWeb::layers_info() const {
  std::string layers_string;
  for (const auto a : n_nodes) layers_string += std::to_string(a) + "_";
  return layers_string;
}
