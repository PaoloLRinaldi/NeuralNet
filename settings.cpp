// Copyright
#include "./settings.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <fstream>

using std::string;
using std::vector;
using std::pair;
using std::getline;
using std::istringstream;
using std::endl;
using std::cin;
using std::cout;
using std::find_if;
using std::to_string;

template <typename T>
string setting_elem(const pair<bool, T> &elem, bool force_print = false);
template <>
string setting_elem<bool>(const pair<bool, bool> &elem, bool force_print);
template <>
string setting_elem<int>(const pair<bool, int> &elem, bool force_print);
template <>
string setting_elem<vector<unsigned int>>(const pair<bool, vector<unsigned int>> &elem, bool force_print);
template <>
string setting_elem<string>(const pair<bool, string> &elem, bool) { return elem.second; }


template <typename T>
string setting_elem(const pair<bool, T> &elem, bool force_print) {
  string ret;
  if (elem.first || force_print)
    ret = to_string(elem.second);
  return ret;
}
template <>
string setting_elem<int>(const pair<bool, int> &elem, bool force_print) {
  string ret;
  if (elem.first || force_print)
    ret = to_string(elem.second);
  return ret;
}
template <>
string setting_elem<bool>(const pair<bool, bool> &elem, bool force_print) {
  string ret;
  if (elem.first || force_print) {
    // Ho fatto che yes è 0, e no è 1
    if (elem.second)
      ret = to_string(0);
    else
      ret = to_string(1);
  }
  return ret;
}

template <>
string setting_elem<vector<unsigned int>>(const pair<bool, vector<unsigned int>> &elem, bool force_print) {
  string ret;
  if (elem.first || force_print) {
    for (auto a : elem.second)
      ret += to_string(a) + "_";
  }
  return ret;
}


template <typename T>
pair <bool, T> next_setting(std::istream &is) {
  string line;
  string::iterator it;
  auto is_not_space = [] (char a) { return !isspace(a); };
  do {
    if (!getline(is, line))
      throw std::domain_error("Settings file in wrong format!");
    it = find_if(line.begin(), line.end(), is_not_space);
  } while (it == line.end() || *it == '#');
  it = std::find(it, line.end(), '=');
  if (it == line.end())
    throw std::domain_error("Settings file in wrong format!");
  ++it;
  it = find_if(it, line.end(), is_not_space);
  if (it == line.end())
    return {false, T()};

  istringstream sin(string(it, line.end()));
  T ret;
  if (!(sin >> ret))
    throw std::domain_error("Wrong format of input in settings!");
  char c = 0;
  while (sin.get(c) && c == ' ') {}
  if (!sin.eof())
    throw std::domain_error("Wrong format of input in settings, "
                            "too many inputs!");
  return {true, ret};
}

pair <bool, bool> next_setting_y_n(std::istream &is, const string &err_msg, bool force_ask = true) {
  auto a = next_setting<int>(is);
  if (force_ask && (!a.first ||
                    (a.second != 0 && a.second != 1)))
    throw std::domain_error(err_msg);
  // Metto il "not" perché ho sempre fatto
  // 0 = yes, 1 = no
  return {force_ask, !static_cast<bool>(a.second)};
}


void Settings::instructions(std::ostream &os) {
  os << "This programs trains a neural web on" << endl
     << "a set of input provided by the user." << endl
     << "To use this program you need two files:" << endl
     << "1: A settings file" << endl
     << "2: A file with all the inputs" << endl
     << "If you want, it will be automatically" << endl
     << "generated a settings file in which you" << endl
     << "only have to insert the values you want." << endl
     << "The second (and eventually third) file(s)" << endl
     << "must be (a) binary file(s) with the following" << endl
     << "format (same as the MNIST format):" << endl
     << "- The first two bytes must be 0" << endl
     << "- The third byte will identify the" << endl
     << "  type of the single numbers used" << endl
     << "  in the file:" << endl
     << "  - 0x08 -> unsigned char (1 byte)" << endl
     << "  - 0x09 -> char (1 byte)" << endl
     << "  - 0x0B -> short (2 bytes)" << endl
     << "  - 0x0C -> int (4 bytes)" << endl
     << "  - 0x0D -> float (4 bytes)" << endl
     << "  - 0x0E -> double (8 bytes)" << endl
     << "- The fourth byte must tell the number" << endl
     << "  of the dimentions of the inputs:" << endl
     << "  - 1 if a single input is composed" << endl
     << "    by a single number" << endl
     << "  - 2 if a single input is a vector" << endl
     << "  - 3 if a single input is a matrix" << endl
     << "  - ..." << endl
     << "The next numbers stored must be 4" << endl
     << "bytes integers that tell how long" << endl
     << "are the input in the relative dimention:" << endl
     << "- The first will tell how many inputs are stored" << endl
     << "- The second will tell the lenght of the vectors" << endl
     << "  or the numbers of rows in the matrices etc." << endl
     << "- The third will tell the numbers of columns" << endl
     << "  in the matrices" << endl
     << "- ..." << endl
     << "The following bytes are the input. Data must" << endl
     << "be stored like in a C array, i.e. the index" << endl
     << "in the last dimention changes the fastest." << endl
     << "If you provide only one file it must have" << endl
     << "both inputs and solutions in it. The solution" << endl
     << "must be the last number in the input, so in this" << endl
     << "case the best things to do is to structure inputs" << endl
     << "as vectors, and not matices, because otherwise" << endl
     << "there would't by simmetry along all the dimentions." << endl
     << "If you provide two files, the first file's format" << endl
     << "can be anything you want, the solutions file, on" << endl
     << "the other hand, must be composed only by single," << endl
     << "integer numbers." << endl << endl;
}

std::ostream &Settings::settings_file_content(std::ostream &os) {
  return os << "# SETTINGS INDEX" << endl
            << "# PART 1 --------- Set up of the Neural Web" << endl
            << "  # PART 1.1 ----- General web structure" << endl
            << "    # PART 1.1.1 - Load a preexisting Neural Web" << endl
            << "    # PART 1.1.2 - Structure of a new Neural Web" << endl
            << "  # PART 1.2 ----- Parameters of the Neural Web" << endl
            << "# PART 2 --------- Inputs/Solutions file(s) infos" << endl
            << "# PART 3 --------- Training/Testing settings" << endl
            << "  # PART 3.1 ----- Testing session" << endl
            << "  # PART 3.2 ----- Training and test sets" << endl
            << "    # PART 3.2.1 - Load sets file" << endl
            << "    # PART 3.2.2 - Set up sets" << endl
            << "  # PART 3.3 ----- Set up training settings" << endl
            << "# PART 4 --------- Saving sets and web files" << endl
            << "# PART 5 --------- Update this settings file" << endl
            << endl
            << "# +++ BEGINNING OF PART 1 +++ Set up of the Neural Web" << endl
            << endl
            << "  # +++ BEGINNING OF PART 1.1 +++ General web structure" << endl
            << endl
            << "    # +++ BEGINNING OF PART 1.1.1 +++ Load a preexisting Neural Web" << endl
            << endl
            << "      # DON'T MODIFY THIS SECTION (1.1.1) IF YOU WANT" << endl
            << "      # TO CREATE A NEW NEURAL WEB" << endl
            << endl
            <<      "      Web file = " << setting_elem(web_file) << endl
            << endl
            << "    # +++ END OF PART 1.1.1 +++  Load a preexisting Neural Web" << endl
            << endl
            << "    # +++ BEGINNING OF PART 1.1.2 +++ Structure of a new Neural Web" << endl
            << endl
            << "      # IF YOU HAVE INSERTED THE \"Web file\"" << endl
            << "      # IN SECTION (1.1.1) THEN" << endl
            << "      # THIS SECTION (1.1.2) WILL BE IGNORED" << endl
            << endl
            << "      Web structure (input_layer_[hidden_layers_separated_by_underscore]_output_layer) = " << setting_elem(web_structure, true) << endl
            << "      Hidden layers activation function (sigmoid, ReLU, leaky_ReLU, linear) = " << setting_elem(hlact) << endl
            << "      Output layer activation function (sigmoid, ReLU, leaky_ReLU, linear) = " << setting_elem(olact) << endl
            << "      Cost function (0: Quadratic, 1: Cross-Entropy) = " << setting_elem(static_cast<pair<bool, int>>(cost_function)) << endl
            << endl
            <<  "    # +++ END OF PART 1.1.2 +++ Structure of a new Neural Web" << endl
            << endl
            << "  # +++ END OF PART 1.1 +++ General web structure" << endl
            << endl
            << "  # +++ BEGINNING OF PART 1.2 +++ Parameters of the Neural Web" << endl
            << endl
            << "    # IF YOU WANT TO MODIFY A NEURAL WEB YOU" << endl
            << "    # UPLOADED THEN MAKE SURE YOU LEAVE IN" << endl
            << "    # BLANK THOSE PARAMETERS YOU WANT TO" << endl
            << "    # BE UNCHANGED, OR AT LEAST INSERT" << endl
            << "    # THE SAME VALUES. THE VALUES ALREADY" << endl
            << "    # INSERTED ARE THE DEFAULT VALUES" << endl
            << endl
            << "    Batch lenght (how many inputs before modifying the web) = " << setting_elem(static_cast<pair<bool, int>>(batch)) << endl
            << "    Tau (keeps wieghts around 0 to stay away from 0 derivarive in sigmoid) = " << setting_elem(tau) << endl
            << "    Reducer of gradient step = " << setting_elem(gradient_step) << endl
            << "    Use bias (0: yes, 1: no) = " << setting_elem(bias, true) << endl
            << endl
            << "  # +++ END OF PART 1.2 +++ Parameters of the Neural Web" << endl
            << endl
            << "# +++ END OF PART 1 +++ Set up of the Neural Web" << endl
            << endl
            << "# +++ BEGINNING OF PART 2 +++ Inputs/Solutions file(s) infos" << endl
            << endl
            << "  Data in little endian (used by most intel processors) (0) or big endian (1) = " << setting_elem(is_little_endian, true) << endl
            << "  Data stored tadily (0: yes, 1: no) = " << setting_elem(data_stored_tadily, true) << endl
            << endl
            << "  # LEAVE THE MINIMUM VALUE AND THE" << endl
            << "  # MAXIMUM VALUE SET TO 0 (OR BLANK, WHERE" << endl
            << "  # IN THIS CASE, BLANK = 0) IF YOU DON'T" << endl
            << "  # WANT THE INPUT TO BE NORMALIZED." << endl
            << "  # NOTE THAT THE NEURAL WEB EXPECTS" << endl
            << "  # THE INPUT TO BE IN" << endl
            << "  # RANGE [0.01, 0.99]" << endl
            << "  Minimum value of input = " << setting_elem(min_norm) << endl
            << "  Maximum value of input = " << setting_elem(max_norm) << endl
            << endl
            << "# +++ END OF PART 2 +++ Inputs/Solutions file(s) infos" << endl
            << endl
            << "# +++ BEGINNING OF PART 3 +++ Training/Testing settings" << endl
            << endl
            << "  # +++ BEGINNING OF PART 3.1 +++ Testing session" << endl
            << endl
            << "    # BY SETTING THIS SETTING TO YES," << endl
            << "    # SECTIONS (3.2), (3.3), (4) AND" << endl
            << "    # (5) WILL BE IGNORED" << endl
            << "    Test session only (0: yes, 1: no) = " << setting_elem(test_only, true) << endl
            << endl
            << "    # IF YOU SET THIS SETTING TO 1 YOU NEED" << endl
            << "    # TO PROVIDE A \"Sets file\" AS WELL" << endl
            << "    # IN SECTION (3.2.1)" << endl
            << "    Test on the entire input collection (0) or only on the test part of the set (1) = " << setting_elem(test_entire, true) << endl
            << endl
            << "  # +++ END OF PART 3.1 +++ Testing session" << endl
            << endl
            << "  # +++ BEGINNING OF PART 3.2 +++ Training and test sets" << endl
            << endl
            << "    # +++ BEGINNING OF PART 3.2.1 +++ Load sets file" << endl
            << endl
            <<  "      # LEAVE BLANK THIS SECTION (3.2.1) IF" << endl
            << "      # YOU WANT TO SET UP NEW SETS" << endl
            << endl
            << "      Sets file = " << setting_elem(sets_file) << endl
            << endl
            << "    # +++ END OF PART 3.2.1 +++ Load sets file" << endl
            << endl
            << "    # +++ BEGINNING OF PART 3.2.2 +++ Set up sets" << endl
            << endl
            << "      # THIS SECTION (3.2.2) WILL BE IGNORED" << endl
            << "      # IF YOU PROVIDED THE \"Sets file\" IN" << endl
            << "      # SECTION (3.2.1) OR IF YOU WILL SET" << endl
            << "      # \"Test session only\" TO YES IN" << endl
            << "      # SESSION (3.1)" << endl
            << "      " << endl
            << "      Test set size (if data stored tadily, the output layer must be a divisor of both test size and total size) = " << setting_elem(static_cast<pair<bool, size_t>>(test_sz), true) << endl
            << endl
            << "    # +++ END OF PART 3.2.2 +++ Set up sets" << endl
            << endl
            << "  # +++ END OF PART 3.2 +++ Training and test sets" << endl
            << endl
            << "  # +++ BEGINNING OF PART 3.3 +++ Set up training settings" << endl
            << endl
            << "    # THIS SECTION (3.3) WILL BE CONSIDERED ONLY IF" << endl
            << "    # \"Test session only\" IN SECTION (3.1)" << endl
            << "    # IS SET TO NO" << endl
            << endl
            << "    # THE SETTINGS IN THIS SECTION (3.3)" << endl
            << "    # ARE NOT MUTUALLY EXCLUSIVE. THE" << endl
            << "    # TRAINING WILL GO ON UNTIL" << endl
            << "    # ONE OF THESE SETTINGS IS VERIFIED" << endl
            << endl
            << "    Train until it is very likely it is overfitted (0: yes, 1: no) = " << setting_elem(train_until_overfitted, true) << endl
            << "    Max training cycles = " << setting_elem(static_cast<pair<bool, size_t>>(training_cycles)) << endl
            << endl
            << "    # THIS SETTING WILL BE CONSIDERATED ONLY IF" << endl
            << "    # A SETS FILE HAS BEEN LOADED AND IT HAS" << endl
            << "    # THE FOLLOWING FORMAT:" << endl
            << "    # perc_<integer_part>_<decimals>_layersinfo_<layers_separated_by_underscore>_trainsz_<train size>_testsz_<test size>_traininground_<training round>_sets.txt" << endl
            << "    # THE WEB FILE BE BETTER BE OF THE SAME" << endl
            << "    # FORMAT AND HAVE THE SAME NUMBERS THOUGH" << endl
            << "    Train until it gets a better percentage (0: yes, 1: no) = " << setting_elem(train_until_better, true) << endl
            << endl
            << "    Train until it gets a better percentage than (you can leave blank) = " << setting_elem(train_until_better_than) << endl
            << endl
            << "  # +++ END OF PART 3.3 +++ Set up training settings" << endl
            << endl
            << "# +++ END OF PART 3 +++ Training/Testing settings" << endl
            << endl
            << "# +++ BEGINNING OF PART 4 +++ Saving sets and web files" << endl
            << endl
            << "  # THIS SETTING WILL BE CONSIDERED ONLY IF" << endl
            << "  # NO PREVIOUS PERCENTAGE HAS BEEN FOUND OR IF" << endl
            << "  # IT HAS BEEN FOUND AND A BETTER PERCENTAGE" << endl
            << "  # HAS BEEN ACHIEVED" << endl
            << "  Save file when training finishes (0: yes, 1: no) = " << setting_elem(save_when_finishes) << endl
            << endl
            << "  # THIS SETTING WILL BE CONSIDERATED ONLY IF" << endl
            << "  # BOTH THE SETS AND WEB FILES PROVIDED" << endl
            << "  # HAVE THE FORMAT DESCRIBED IN SECTION (3.2)" << endl
            << "  Overwrite sets and weights file if percentage is better (0: yes, 1: no) = " << setting_elem(overwrite_if_better) << endl
            << endl
            << "# +++ END OF PART 4 +++ Saving sets and web files" << endl
            << endl
            << "# +++ BEGINNING OF PART 5 +++ Update this settings file" << endl
            << endl
            << "  # THIS SECTION (5) WILL BE CONSIDERATED" << endl
            << "  # ONLY IF AT THE END OF THE TRAINING" << endl
            << "  # NEW WEB AND SETS FILES HAVE BEEN" << endl
            << "  # SAVED (OR OVERWEITTEN THE PREVIOUS)" << endl
            << endl
            << "  Update this settings file to be used on updated web and sets file (0: yes, 1: no) = " << setting_elem(update_this) << endl
            << endl
            << "# +++ END OF PART 5 +++ Update this settings file" << endl;
}


// DEVO AGGIUSTARE QUESTA FUNZIONE PERCHE' SE VOGLIO IL TEST
// ONLY ALLORA NON MI FREGA NIENTE DEI TEST O DI SALVARE
// FILE O DI AGGIORNARE IL SETUP
void Settings::read_settings_file(const string &setts_file) {
  std::ifstream fin(setts_file);
  if (!fin.is_open())
    throw std::domain_error("Can't open settings file!");

  web_file = next_setting<string>(fin);
  auto web_str = next_setting<string>(fin);
  if (web_file.first) {
    web_structure = {false, vector<unsigned int>()};
  } else {
    if (!web_str.first) {
      throw std::domain_error("You didn't insert a \"Web file\",\n"
                              "you must provide a web structure!");
    } else {
      // Tolgo i '_' "tutti a destra"
      string::iterator end = web_str.second.end();
      while (*--end == '_') {}
      ++end;
      std::replace(web_str.second.begin(), end, '_', ' ');
      web_str.second = string(web_str.second.begin(), end);

      // Se trovo un carattere diverso da spazio e cifre mi arrabbio
      if (find_if(web_str.second.begin(),
                  web_str.second.end(),
                  [] (char c) { return !isspace(c) &&
                                       !isdigit(c); }) !=
          web_str.second.end())
        throw std::domain_error("A strange way to define a structure!");
      istringstream sweb_str(web_str.second);
      std::copy(std::istream_iterator<unsigned int>(sweb_str),
                std::istream_iterator<unsigned int>(),
                back_inserter(web_structure.second));
      if (web_structure.second.size() == 1)
        throw std::domain_error("Only one layer? Mh...");
      if (std::find(web_structure.second.begin(),
                    web_structure.second.end(),
                    0) !=
          web_structure.second.end())
        throw std::domain_error("That's nice, a layer with "
                                "0 neurons... like you!");
      web_structure.first = true;
    }
  }
  hlact = next_setting<string>(fin);
  olact = next_setting<string>(fin);
  cost_function = next_setting<int>(fin);
  batch = next_setting<int>(fin);
  tau = next_setting<double>(fin);
  gradient_step = next_setting<double>(fin);
  bias = next_setting_y_n(fin,
                          "Set bias or not?",
                          !web_file.first);

  is_little_endian = next_setting_y_n(fin,
                                          "Little endian or big endian?");
  data_stored_tadily = next_setting_y_n(fin,
                                            "How is data stored? Tadily?");
  // Lo so che la value initialization del double
  // è 0, ma voglio metterep per esteso 0 nel
  // caso in cui non venga messo il valore
  min_norm = next_setting<double>(fin);
  if (!min_norm.first) min_norm.second = 0;
  max_norm = next_setting<double>(fin);
  if (!max_norm.first) max_norm.second = 0;

  test_only = next_setting_y_n(fin,
                                   "Test only or train too?");
  test_entire = next_setting_y_n(fin,
                                 "Test on entire or on everything?",
                                 test_only.second);
  sets_file = next_setting<string>(fin);
  if (test_entire.first && !test_entire.second && !sets_file.first)
    throw std::domain_error("You didn't provide a \"Sets file\""
                            "but you want to test only on the "
                            "test set. Decide!");
  test_sz = next_setting<size_t>(fin);
  if (!test_only.second && !sets_file.first) {
    if (!test_sz.first)
      throw std::domain_error("You did't provide a \"Sets file\","
                              "insert at least the test size!");
  } else {
    // Credo che questa riga sia un po' inutile,
    // ma effettivamente giusta
    test_sz.first = false;
  }
  train_until_overfitted = next_setting_y_n(fin,
                                            "Train until it is overfitted?",
                                            !test_only.second);
  training_cycles = next_setting<size_t>(fin);
  auto sets_info = get_file_name_infos(sets_file.second);
  train_until_better = next_setting_y_n(fin,
                                        "Train until it gets better?",
                                        sets_info.is_good && sets_info.is_set && sets_info.perc >= 0 && !test_only.second);
  train_until_better_than = next_setting<double>(fin);
  save_when_finishes = next_setting_y_n(fin,
                                        "Save file when finisfes?",
                                        !test_only.second);
  auto web_info = get_file_name_infos(web_file.second);
  overwrite_if_better = next_setting_y_n(fin,
                                         "Overwrite if it gets better?",
                                         !test_only.second &&
                                         ((web_info.is_good && web_info.is_set && web_info.perc >= 0) ||
                                         (sets_info.is_good && sets_info.is_set && sets_info.perc >= 0)));
  update_this = next_setting_y_n(fin,
                                 "Update settings file?",
                                 !test_only.second);
  if (update_this.first &&
      update_this.second)
    overwrite_if_better = {true, true};
}


FileNameInfo get_file_name_infos(const string &file_name) {
  FileNameInfo ret;
  string to_compare;
  auto it = file_name.cbegin();

  // Definisco un paio di funzioni che mi sono comode solo qui
  // La prima
  auto get_integer = [&] (unsigned int &n) -> bool {
    auto beg = it;
    while (it != file_name.cend() && std::isdigit(*it)) ++it;
    if (it == file_name.cend() || *it != '_') return false;
    n = std::stoi(string(beg, it));
    ++it;
    return true;
  };
  // La seconda
  auto get_string = [&] (const string &s) -> bool {
    to_compare = s;
    if (static_cast<unsigned int>(std::distance(it, file_name.cend())) <
        to_compare.size()) return false;
    if (!equal(to_compare.cbegin(), to_compare.cend(), it))
      return false;
    it = it + to_compare.size();
    return true;
  };



  // Starting raeding from here
  // Reading percentage string
  if (!get_string("perc_")) {
    ret.is_good = false;
    return ret;
  }

  // Reading percentage value
  auto beg_n = it;
  if (get_string("NULL_")) {
    ret.perc = -1;
  } else {
    // Ora ci sono due numeri interi separati da '_'
    // Primo
    unsigned int useless;
    if (!get_integer(useless)) {
      ret.is_good = false;
      return ret;
    }
    // Secondo
    if (!get_integer(useless)) {
      ret.is_good = false;
      return ret;
    }
    --it;  // Perché se no mi trovo su quello dopo '_' invece
    // di quello su '_'
    string s_to_double = string(beg_n, it);
    std::replace(s_to_double.begin(), s_to_double.end(), '_', '.');
    ret.perc = std::stod(s_to_double);
    ++it;
  }


  // Ora tocca a layers info
  if (!get_string("layersinfo_")) {
    ret.is_good = false;
    return ret;
  }

  // Ora tocca a i layers separati da '_'
  while (it != file_name.cend() && *it != 't') {
    unsigned int layer;
    if (!get_integer(layer)) {
      ret.is_good = false;
      return ret;
    }
    ret.layersinfo.push_back(layer);
  }

  if (ret.layersinfo.size() <= 1)
    throw std::domain_error("Reading a file where the "
                            "neural web has 1 layer!?!");

  // Ora tocca a trainsz
  if (!get_string("trainsz_")) {
    ret.is_good = false;
    return ret;
  }

  // Ora proprio il numero intero train size
  if (!get_integer(ret.train_sz)) {
    ret.is_good = false;
    return ret;
  }

  // Ora tocca a testsz
  if (!get_string("testsz_")) {
    ret.is_good = false;
    return ret;
  }

  // Ora proprio il numero intero test size
  if (!get_integer(ret.test_sz)) {
    ret.is_good = false;
    return ret;
  }

  // Ora tocca a training round
  if (!get_string("traininground_")) {
    ret.is_good = false;
    return ret;
  }

  // Ora proprio il numero intero training round
  if (!get_integer(ret.traininground)) {
    ret.is_good = false;
    return ret;
  }

  // E' un file sets?
  auto beg_save_type = it;
  if (get_string("sets.txt")) {
    if (it == file_name.cend()) {
      ret.is_set = true;
      ret.is_web = false;
      ret.is_good = true;
      return ret;
    } else {
      ret.is_good = false;
      return ret;
    }
  }

  // No? Allora sarà un file web?
  it = beg_save_type;
  if (get_string("web.txt")) {
    if (it == file_name.cend()) {
      ret.is_set = true;
      ret.is_web = false;
      ret.is_good = true;
      return ret;
    } else {
      ret.is_good = false;
      return ret;
    }
  }

  // No? Non so cosa sia fijo mio
  ret.is_good = false;
  return ret;
}

