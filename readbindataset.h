#ifndef GUARD_READBINDATASET_H  //READ_BIN_DATASET?
#define GUARD_READBINDATASET_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <fstream>
#include <string>

class ReadBinDataset {
public:
  //Constructors
  // Il copy constructor è deleted di default perché
  // uno dei data members è un ifstream!
  explicit ReadBinDataset(const std::string& filename, double mx = 0, double mn = 0, bool little_end = true);

  //Getters
  virtual std::pair<std::vector<double>, unsigned int> operator[] (size_t index) const;
  virtual size_t size() const { return dimentions[0]; }
  std::vector<double> get_hole_input(size_t index) const;
  std::vector<double> get_hole_input_norm(size_t index) const;
  std::vector<unsigned int> get_dimentions() const { return dimentions; }
  virtual ~ReadBinDataset() = default;

private:
  unsigned char n_dimentions;  //Numero di dimensioni dei dati salvati sul file. Se è 1 i dati sono singoli numeri, se è 2 sono vettori, se è 3 sono matrici e così via
  std::vector<unsigned int> dimentions;  //Il numero di elementi in ogni dimensione. dimentions[0] sarà il numero di input
  mutable std::ifstream fis;  //Input stream file
  unsigned char data_type;  //Byte che identifica il tipo degli input salvati (int, double, ecc.)
  unsigned int data_size;  //La lunghezza dei dati salvati; strettamente legato a data_type, se quest'ultimo identifica in allora data_size = 4 e così via.
  unsigned int single_input_size;  //La dimensione in byte di un singolo input
  unsigned int begin_data_pos;  //La posizione in byte del primissimo input
  bool is_little_endian = true;  //Dice se lavorare in little endian o big endian
  int min_norm;  // Per normalizzare l'input
  int max_norm;  // Per normalizzare l'input

  std::istream& read_magic_number();  //Legge e interpreta i primi byte del file

  // Useful functions
  double switch_datatype() const;  //A seconda di data_type, restituisce il valore letto nel file nel punto in cui si trova fis e lo casta a double
  std::istream& jump_to(unsigned int point) const {  //Porta fis a leggere in una posizione precisa
    if (point >= dimentions.front())
      throw std::range_error("Input out of range!");
    return fis.seekg(begin_data_pos + point * single_input_size);
  }

  // Lettura dei byte del file
  template <typename T> T get_binary_integer() const {
    char buf[sizeof(T)];
    fis.read(buf, sizeof(T));
    if (!is_little_endian)
      std::reverse(&buf[0], &buf[sizeof(T)]);
    T *d = reinterpret_cast<T*>(buf);
    return *d;
  }
  template <typename T> T get_binary_float() const {
    char buf[sizeof(T)];
    fis.read(buf, sizeof(T));
    //Ho fatto una funzione diversa per i float perché credo che la lettura non cambi da little a big endian
    T *d = reinterpret_cast<T*>(buf);
    return *d;
  }
};

inline double normalize(double a, double min, double max) {
  return static_cast<double>(a - min) * 0.98 / max + 0.01;
}


#endif  //GUARD_READBINDATASET_H
