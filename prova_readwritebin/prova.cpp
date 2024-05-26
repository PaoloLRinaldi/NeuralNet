// Copyright

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <cstdlib>
#include "../readwritebin.h"

using std::cout;
using std::endl;

class MyRand {
public:
  MyRand() {
    std::srand(unsigned(std::time(0)));
  }
  int operator()(int n) {
    if (n <= 0 || n > RAND_MAX)
      throw std::domain_error("Argument to nrand is out of range");
    const int bucket_size = RAND_MAX / n;
    int r;

    do {
      r = std::rand() / bucket_size;
    } while (r >= n);

    return r;
  }
  int operator()() { return this->operator()(100); }
  
  using result_type = int;
  static constexpr int max() { return RAND_MAX; }
  static constexpr int min() { return 0; }
};

// int MyRand::max = RAND_MAX;

int main() {
  MyRand myrand;

  WriteBin w("txt.txt", true);
  w.write(0);
  w.write(15);
  w.write(81.81);
  const std::string we("ciccio giuggio");
  w.write_string(we);
  w.write_string("ciccio giuggio");
  std::string eh = "chicco";
  w.write_string(eh);
  cout << "Size is " << w.size() << endl;
  w.close();

  cout << std::string(20, '*') << "Sezione 1" << endl;

  ReadBin r("txt.txt");
  cout << r.get_value<int>() << endl;
  cout << r.get_value<int>() << endl;
  cout << r.get_value<double>() << endl;
  cout << r.get_string(14) << endl;
  cout << r.get_string(std::string("ciccio giuggio").size()) << endl;
  cout << r.get_string(eh.size()) << endl;
  cout << "Size is " << r.size() << endl;
  r.close();

  cout << std::string(20, '*') << "Sezione 2" << endl;

  WriteBin w2("txt.txt");
  cout << "w2 size is " << w2.size() << endl;
  w2.jump_to(16);
  w2.write_string("fa");
  w2.close();

  cout << std::string(20, '*') << "Sezione 3" << endl;

  ReadBin r2("txt.txt");
  cout << r2.get_value<int>() << endl;
  cout << r2.get_value<int>() << endl;
  cout << r2.get_value<double>() << endl;
  cout << r2.get_string(std::string("ciccio giuggio").size()) << endl;
  cout << r2.get_string(std::string("ciccio giuggio").size()) << endl;
  cout << r2.get_string(eh.size()) << endl;
  cout << "Size is " << r2.size() << endl;
  r2.close();

  cout << std::string(20, '*') << "Sezione 4" << endl;

  /**************************************/

  std::vector<double> trash(20);
  for (int i = 0; i != 20; ++i) trash[i] = i * 0.5;

  WriteBin w3("txt2.txt", true);
  std::copy(trash.begin(), trash.end(), w3.begin<double>());
  // OVVIO CHE random_shuffle NON FUNZIONI, E' SOLO OUTPUT,
  // NON PUO' LEGGERE GLI ELEMENTI E STOREARLI DA QUALCHE
  // PARTE, SCEMO!
  // std::shuffle(w3.begin<double>(), w3.end<double>(), myrand);
  cout << "w3 size is " << w3.size() << endl;
  w3.close();

  cout << std::string(20, '*') << "Sezione 5" << endl;

  ReadBin r3("txt2.txt");
   std::copy(r3.begin<double>(), r3.end<double>(),
             std::ostream_iterator<double>(cout, " "));
  /*  Ovviamente non funziona
  for (double a : r3)
    cout << a << " ";
  */
  r3.close();
  cout << endl;
  cout << std::string(20, '*') << endl;
  cout << std::string(20, '*') << endl;
  cout << std::string(20, '*') << endl;
  cout << std::string(20, '*') << endl;
  cout << std::string(20, '*') << "Sezione 6" << endl;

  /****************************************/

  Bin wr("txt3.txt", true);
  wr.write(0);
  wr.write(15);
  wr.write(81.81);
  wr.write_string(we);
  wr.write_string("ciccio giuggio");
  wr.write_string(eh);
  wr.wjump_to(52);
  wr.write('y');
  cout << "Size is " << wr.size() << endl;

  wr.wjump_to(0);
  // wr.wjump_to(50);
  cout << wr.get_value<int>() << endl;
  cout << wr.get_value<int>() << endl;
  cout << wr.get_value<double>() << endl;
  cout << wr.get_string(14) << endl;
  cout << wr.get_string(std::string("ciccio giuggio").size()) << endl;
  cout << wr.get_string(eh.size()) << endl;
  // wr.rjump_to(52);
  cout << static_cast<int>(wr.get_value<char>()) << endl;
  cout << wr.get_value<char>() << endl;
  cout << wr.get_value<char>() << endl;
  cout << "Size is " << wr.size() << endl;
  wr.close();

  cout << std::string(20, '*') << "Sezione 7" << endl;

  Bin wr2("txt3.txt");
  wr2.rjump_to(52);
  char h = wr2;
  wr2.close();
  cout << h << endl;

  cout << std::string(20, '*') << "Sezione 8" << endl;

  Bin wr3("txt3.txt");
  auto iter3 = wr3.begin<int>();
  Bin wr4("txt4.txt", true);
  auto iter4 = wr4.begin<double>();
  *iter4 = *++iter3;
  wr3.close();
  wr4.rjump_to(0);
  cout << wr4.get_value<int>() << endl;
  wr4.close();

  cout << std::string(20, '*') << "Sezione 9" << endl;

  Bin wr5("txt2.txt", false);
  std::shuffle(wr5.begin<double>(), wr5.end<double>(), myrand);
  std::copy(wr5.begin<double>(), wr5.end<double>(),
            std::ostream_iterator<double>(cout, " "));
  cout << endl;
  wr5.close();

  cout << std::string(20, '*') << "Sezione 10" << endl;

  Bin wr6("txt4.txt", false);
  cout << "Size: " << wr6.size() << endl;
  auto iter6 = wr6.end<double>();
  cout << "Size again: " << wr6.size() << endl;
  ++iter6;
  cout << "Size again again: " << wr6.size() << endl;
  ++iter6;
  *iter6 = 17;
  cout << "Size again again again: " << wr6.size() << endl;
  wr6.close();

  cout << std::string(20, '*') << "Sezione 11" << endl;

  Bin wr7("txt5.txt", true);
  wr7.write_many({5, 8, 5, 1, 23}, 0);
  wr7.write_many(trash.begin(), trash.end());
  auto outs1 = wr7.get_values<int>(5, 0);
  auto outs2 = wr7.get_values<double>(20);
  for (auto a : outs1) cout << a << " ";
  cout << endl;
  for (auto a : outs2) cout << a << " ";
  cout << endl;
  wr7.wjump_to(wr7.size());
  auto char_vec = std::vector<char>(31, 'g');
  wr7.write_many(char_vec.begin(), char_vec.end());
  wr7.rmove_by<char>(-31);
  cout << wr7.get_string(31) << endl;
  cout << endl;
  wr7.close();

  cout << std::string(20, '*') << "Sezione 12" << endl;

  Bin wr8("txt6.txt", true);
  wr8.write_many(trash);
  wr8.wjump_to(0);
  auto outs3 = wr8.get_values<double>(20);
  wr8.write_many<int>(trash);
  wr8.rmove_by<int>(-20);
  auto outs4 = wr8.get_values<int>(20);
  for (auto a : outs3) cout << a << " ";
  cout << endl;
  for (auto a : outs4) cout << a << " ";
  cout << endl;
  wr8.write_many(trash, 0);
  wr8.write_many<int>(trash, wr8.wpos());
  wr8.write_many<double>(trash, 0);
  wr8.wjump_to(0);
  wr8.write_many(trash.begin(), trash.end());
  wr8.write_many<int>(trash.begin(), trash.end());
  outs3 = wr8.get_values<double>(20, 0);
  outs4 = wr8.get_values<int>(20);
  for (auto a : outs3) cout << a << " ";
  cout << endl;
  for (auto a : outs4) cout << a << " ";
  cout << endl;
  wr8.write_many(trash.begin(), trash.end(), 0);
  wr8.write_many<int>(trash.begin(), trash.end(), wr8.wpos());
  wr8.write_many<int>({0.5, 0.6, 1.6}, 0);
  outs4 = wr8.get_values<int>(3, 0);
  for (auto a : outs4) cout << a << " ";
  cout << endl;
  wr8.wjump_to(wr8.size());
  cout << "I'm in " << wr8.wpos() << " while size is " << wr8.size() << endl;
  wr8.rmove_by(1);
  cout << "I'm in " << wr8.rpos();
  cout << " while size is " << wr8.size() << endl;  // Voglio essere sicuro che lo faccia dopo
  wr8.write<int>(1.5, 0);
  cout << wr8.get_value<int>(0) << endl;

  wr8.close();

  cout << std::string(20, '*') << "Sezione 13" << endl;

  Bin wr9("txt6.txt", false);

  wr9.write<unsigned char>(228);
  wr9.rjump_to(0);
  cout << wr9.get_value() << endl;


  cout << std::string(20, '*') << "Sezione 14" << endl;


  return 0;
}
