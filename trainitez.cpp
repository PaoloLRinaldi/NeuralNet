// Copyright
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <sstream>
#include <utility>
#include <iterator>

#include "./neural_web.h"
#include "./traintestsets.h"
#include "./readbindataset.h"
#include "./big_lib.h"
#include "./settings.h"
#include "./read_mnist.h"

using std::cin;
using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::sort;
using std::find_if;
using std::equal;
using std::to_string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::getline;
using std::find_if;
using std::isspace;
using std::istringstream;
using std::pair;
using std::back_inserter;
using std::isdigit;



void gen_settings_file();
bool y_n_request(const string &text, const string &dismiss_text);
NeuralWeb set_web_from_settings(const Settings &setts);
void do_test_only(NeuralWeb &web,
                  TrainTestSets<ReadBinDataset> &ttsets);
void do_train_test(NeuralWeb &web,
                   TrainTestSets<ReadBinDataset> &ttsets,
                   Settings &setup,
                   const string &settings_file_name);
void save_stuff(NeuralWeb &best_web,
                TrainTestSets<ReadBinDataset> &best_ttsets,
                Settings &setup,
                double best_perc,
                unsigned int best_training_counter,
                const string &settings_file_name);

int main(int argc, char *argv[]) {
  // check if the file name is given as argument
  if (argc == 1) {
    Settings::instructions();
    gen_settings_file();
    return 0;
  } else if (argc != 3 && argc != 4) {
    cerr << "SYNTAX ERROR:" << endl
         << "- No arguments                 => Instructions" << endl
         << "- 2/3 File names as arguments  => Program Execution" << endl;
    return 0;
  }

  // storing the arguments
  string settings_file_name = argv[1],
      data_file_name = argv[2],
      solutions_file_name;

  if (argc == 4) solutions_file_name = argv[3];

  Settings setup;
  setup.read_settings_file(settings_file_name);
  // set_web_from_settings già considera se l'utente
  // ha messo o meno "Web file"
  NeuralWeb web = set_web_from_settings(setup);

  // Setting ReadBinDataset to set TrainTestSets later
  ReadBinDataset *dataset;
  // Il puntatore sarà poi distrutto da TrainTestSets
  if (argc == 3) {
    dataset = new ReadBinDataset(data_file_name,
                                 setup.min_norm.second,
                                 setup.max_norm.second,
                                 setup.is_little_endian.second);
  } else {
    dataset = new Read_MNIST(data_file_name,
                             solutions_file_name,
                             setup.min_norm.second,
                             setup.max_norm.second,
                             setup.is_little_endian.second);
  }
  size_t test_size = setup.test_sz.second;
  if (setup.test_only.second && setup.test_entire.second)
    test_size = dataset->size();
  TrainTestSets<ReadBinDataset> ttsets(dataset,
                                       test_size,
                                       setup.data_stored_tadily.second,
                                       web.layers_info_vec().back());
  if (setup.sets_file.first &&
      (!setup.test_only.second ||
       (setup.test_entire.first && !setup.test_entire.second)))
    ttsets.load_sets(setup.sets_file.second);


  // ORA COMINCIA L'ALLENAMENTO
  if (setup.test_only.second) {
    do_test_only(web, ttsets);
    return 0;
  } else {
    do_train_test(web, ttsets, setup, settings_file_name);
    return 0;
  }

  return 0;
}

/*****************************************************************/

bool y_n_request(const string &text, const string &dismiss_text) {
  cout << text << " [y/n] : ";
  char answer = 0;
  cin >> answer;
  while (answer != 'y' && answer != 'n')
    cout << endl << "Invalid answer. Repeat: ";

  if (answer == 'n') {
    cout << dismiss_text << endl;
    return false;
  }
  return true;
}

void gen_settings_file() {
  if (!y_n_request("Do you want the settings file to\n"
                   "be automatically generated?",
                   "I only tried to help :("))
    return;

  auto file_list = getls_str("./");
  sort(file_list.begin(), file_list.end(),
       [] (const file_info &a, const file_info &b) {
         return string(a) < string(b);
       });
  string sett = "settings1.txt";
  string file_name = sett;
  auto is_sett = [&sett] (const file_info &a) {
    return sett.size() == a.name().size() && sett == a.name() && !a.is_dir();
  };
  auto it = find_if(file_list.begin(), file_list.end(), is_sett);
  if (it != file_list.end() &&
      !y_n_request("At least one settings file found.\n"
                   "Overwrite \"settings1.txt\" (y) or\n"
                   "keep everything and generate a\nnew one (n)?",
                   "Nothing will be overwritten :)")) {
    // Aggiustare questa parte: se dopo
    // settings1.txt ci sta settings1.txt~ lui pensa
    // che settings2.txt sia libero ma non è detto.
    // Si potrebbe controllare se invece dell'ultimo
    // numero, in settings compaia uno tra gli
    // ultimi 2 numeri. Vabbè vedere.
    // Risolto: in is_sett come confronto
    // mettevo solo l'"==", ci ho aggiunto il
    // confronto delle size()
    int sett_counter = 1;
    while (it != file_list.end() && is_sett(*it)) {
      ++it;
      sett = "settings" + to_string(++sett_counter) + ".txt";
    }
    file_name = sett;
  }

  ofstream fout(file_name);
  Settings().settings_file_content(fout);
}


NeuralWeb set_web_from_settings(const Settings &setts) {
  /*
  vector<unsigned int> layers;
  if (setts.web_file.first)
    layers = get_file_name_infos(setts.web_file.second).layersinfo;
  else
    layers = setts.web_structure.second;
  NeuralWeb web(layers);
  */
  NeuralWeb web(setts.web_structure.second);
  if (setts.web_file.first) {
    web.load_web(setts.web_file.second);
  } else {
    // Hidden layer
    if (setts.hlact.second == "sigmoid")
      web.set_hidden_layer_activation_function(sigmoid, der_sigmoid);
    else if (setts.hlact.second == "ReLU")
      web.set_hidden_layer_activation_function(ReLU, der_ReLU);
    else if (setts.hlact.second == "leaky_ReLU")
      web.set_hidden_layer_activation_function(leaky_ReLU, der_leaky_ReLU);
    else if (setts.hlact.second == "linear")
      web.set_hidden_layer_activation_function(linear, der_linear);
    else
      throw std::domain_error("Can't understand hidden layer activation "
                              "function!");

    // Output layer
    if (setts.hlact.second == "sigmoid")
      web.set_hidden_layer_activation_function(sigmoid, der_sigmoid);
    else if (setts.hlact.second == "softmax")
      web.set_softmax(true);
    else if (setts.hlact.second == "ReLU")
      web.set_hidden_layer_activation_function(ReLU, der_ReLU);
    else if (setts.hlact.second == "leaky_ReLU")
      web.set_hidden_layer_activation_function(leaky_ReLU, der_leaky_ReLU);
    else if (setts.hlact.second == "linear")
      web.set_hidden_layer_activation_function(linear, der_linear);
    else
      throw std::domain_error("Can't understand hidden layer activation "
                              "function!");

    // Cost function
    // Potrei scriverlo in forma compatta ma poiché
    // potrei cambiare il formato in futuro voglio
    // averlo scritto esteso
    if (setts.cost_function.second == 0)
      web.set_cost_function(0);
    else if (setts.cost_function.second == 1)
      web.set_cost_function(1);
    else
      throw std::domain_error("What's that cost function?");
  }

  if (setts.batch.first)
    web.set_batch(setts.batch.second);
  if (setts.tau.first)
    web.set_tau(setts.tau.second);
  if (setts.gradient_step.first)
    web.set_grad_pass(setts.gradient_step.second);
  if (setts.bias.first)
    web.set_bias(setts.bias.second);
  return web;
}



void do_test_only(NeuralWeb &web,
                  TrainTestSets<ReadBinDataset> &ttsets) {
  cout << test_result(web, ttsets, true, cout) << endl;
}


void do_train_test(NeuralWeb &web,
                   TrainTestSets<ReadBinDataset> &ttsets,
                   Settings &setup,
                   const string &settings_file_name) {
  // Backup elements
  NeuralWeb best_web = web;
  // Questo best_sets viene usato solo
  // per tenere traccia dell'ordine del
  // set, non può essere usata per altro
  // Non gli assegno nulla come oggetto da
  // gestire (nullptr)
  TrainTestSets<ReadBinDataset> best_ttsets(ttsets);

  // Percentage holders
  double old_perc = 0;
  double prev_perc = 0;
  double perc_to_beat = 0;
  double best_perc = -1;
  double prev_train_perc = -1;
  double train_perc = 0;
  auto info = get_file_name_infos(setup.sets_file.second);
  if (info.is_good &&
      info.is_set) {
    // Ho già fatto il check che andasse bene
    old_perc = info.perc;
  } else {
    old_perc = -1;
  }
  if (setup.train_until_better_than.first &&
      setup.train_until_better_than.second > 0)
    perc_to_beat = setup.train_until_better_than.second;
  else
    perc_to_beat = 0;

  // Definisco qualche flag per capire perché
  // il training si è fermato
  int finish_flags = 0;
  constexpr int train_cycles_finished = 1;
  constexpr int better_perc_old_file = 1 << 1;
  constexpr int better_perc_value = 1 << 2;
  constexpr int overfitted = 1 << 3;


  // Considering cases where the training
  // shouldn't even start
  if (setup.training_cycles.first &&
      setup.training_cycles.second == 0) {
    cout << "No training to be done, you wanted 0" << endl
         << "maximum training cycles." << endl;
    finish_flags |= train_cycles_finished;
  }
  if (setup.train_until_better_than.first &&
      old_perc >= perc_to_beat) {
    cout << "Loaded a set file where the percentage" << endl
         << "should be " << old_perc << " and you" << endl
         << "set the percentage goal to " << perc_to_beat << endl
         << "so there is no work to be done." << endl;
    finish_flags |= better_perc_value;
  }
  if (old_perc < 0 &&
      setup.web_file.first) {
    cout << "Percentage not found." << endl
         << "Calculating it:" << endl;
    best_perc = test_result(web, ttsets, true, cout);
    if (setup.train_until_better_than.first &&
        setup.train_until_better_than.second < best_perc) {
      cout << "The actual percentage is better than" << endl
           << "the goal percentage, no work to be done." << endl;
      if (setup.save_when_finishes.second) {
        cout << "Updating files..." << endl;
        save_web_sets(web, ttsets, 0, best_perc);
        cout << "Done." << endl;
        // Ricordardsi di implementare la parte
        // in cui aggiorno il setting file
        // mettendoci quelli nuovi
      }
      finish_flags |= better_perc_value;
    }
  }
  if (finish_flags) return;

  // training_counter è riferito a questa sessione,
  // quando e se verrà salvato il file verrà sommato
  // al counter della vecchia sessione
  unsigned int training_counter = 0;
  unsigned int best_training_counter = 0;
  unsigned int getting_worse_count = 0;

  // TRAINING
  while (!finish_flags) {
    cout << "NEW CYCLE" << endl;
    double perc;
    // Training and testing
    double grad_avg = train_from_set(web, ttsets, true, cout);
    perc = test_result(web, ttsets, true, cout);
    cout << "Percentage: " << perc << "%" << endl;
    cout << "Gradient average: " << grad_avg << endl;
    if (setup.train_until_overfitted.first &&
        setup.train_until_overfitted.second)
      train_perc = test_from_train_set(web, ttsets, true, cout);

    // Checking if it is in the way
    // of overfitting
    if (training_counter != 0 &&
        perc < prev_perc &&
        train_perc > prev_train_perc)
      ++getting_worse_count;
    else
      getting_worse_count = 0;
    prev_perc = perc;
    prev_train_perc = train_perc;

    // Incrementing training counter
    ++training_counter;

    // Update backup elements
    if (perc > best_perc) {
      // Questa operazione di copia è lentissima,
      // non so perché. Alla fine dece copiare
      // al massimo qualche milione di double,
      // però ci mette diversi secondi. E' già
      // lentissimo sotto i 30000
      best_web = web;
      // best_ttsets = TrainTestSets<ReadBinDataset>(ttsets, nullptr);
      best_ttsets = ttsets;
      best_perc = perc;
      best_training_counter = training_counter;

      // The part reguarding saving files
      if (setup.save_when_finishes.second && best_perc > old_perc) {
        save_stuff(best_web,
                   best_ttsets,
                   setup,
                   best_perc,
                   best_training_counter,
                   settings_file_name);
        cout << "Files updated succesfully!" << endl;
      }
    }

    // Setting all flags
    if (setup.training_cycles.first &&
        training_counter >= setup.training_cycles.second)
      finish_flags |= train_cycles_finished;
    if (setup.train_until_better.first &&
        setup.train_until_better.second &&
        perc > old_perc)
      finish_flags |= better_perc_old_file;
    if (setup.train_until_better_than.first &&
        perc > perc_to_beat)
      finish_flags |= better_perc_value;
    // Se per 3 volte il test sul training set migliora
    // e quello sul test set peggiora dì che è
    // overfittato
    if (setup.train_until_overfitted.second &&
        getting_worse_count >= 3)
      finish_flags |= overfitted;

    if (setup.train_until_overfitted.first &&
        setup.train_until_overfitted.second)
      cout << "Percentage on trainig set: " << train_perc << "%" << endl;
    cout << "Trainings cycles done: " << training_counter << endl;
    cout << endl;
  }

  // Printing the reason he stopped
  if (finish_flags & train_cycles_finished)
    cout << "Maximum training cycles reached." << endl;
  if (finish_flags & better_perc_old_file)
    cout << "Reached a better percentage than the old one." << endl;
  if (finish_flags & better_perc_value)
    cout << "Reached a better percentage than the one you inserted." << endl;
  if (finish_flags & overfitted)
    cout << "The Web was probably overfitted." << endl;
}

void save_stuff(NeuralWeb &best_web,
                TrainTestSets<ReadBinDataset> &best_ttsets,
                Settings &setup,
                double best_perc,
                unsigned int best_training_counter,
                const string &settings_file_name) {
  auto info = get_file_name_infos(setup.sets_file.second);
  if (setup.overwrite_if_better.first &&
      setup.overwrite_if_better.second) {
    exec("rm " + setup.web_file.second + " -f\n");
    exec("rm " + setup.sets_file.second + " -f\n");
  }
  unsigned int actual_tr_count = best_training_counter;
  if (info.is_good && info.is_set)
    actual_tr_count += info.traininground;
  save_web_sets(best_web,
                best_ttsets,
                actual_tr_count,
                best_perc);

  if (setup.update_this.second) {
    string prefix = gen_file_name(best_web,
                                  best_ttsets,
                                  actual_tr_count,
                                  best_perc);
    setup.web_file = {true, prefix + "_web.txt"};
    setup.sets_file = {true, prefix + "_sets.txt"};
    ofstream fout(settings_file_name);
    setup.settings_file_content(fout);
  }
}
