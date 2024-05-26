#ifndef ARR_SCALE_H
#define ARR_SCALE_H

#include <algorithm>
#include <iterator>

struct Coords {
  int xmin;
  int ymin;
  int length;
};

template <class T>
Coords centered_mat_coords(const T& mat, int l) {
  Coords ret;

  //ora cerco di centrare l'immagine in un quadrato più piccolo
  ret.xmin = l;
  ret.ymin = l;
  int xmax = 0, ymax = 0;
  for (int i = 0; i != l; ++i) {
    for (int j = 0; j != l; ++j) {
      if (mat[i][j] != 0) {
	ret.ymin = i;
	break;
      }
    }
    if (ret.ymin != l) break;
  }
  for (int i = 0; i != l; ++i) {
    for (int j = ret.ymin; j != l; ++j) {
      if (mat[j][i] != 0) {
	ret.xmin = i;
	break;
      }
    }
    if (ret.xmin != l) break;
  }
  for (int i = l - 1; i != -1; --i) {
    for (int j = l - 1; j != ret.xmin - 1; --j) {
      if (mat[i][j] != 0) {
	ymax = i;
	break;
      }
    }
    if (ymax != 0) break;
  }
  for (int i = l - 1; i != -1; --i) {
    for (int j = ymax; j != ret.ymin - 1; --j) {
      if (mat[j][i] != 0) {
	xmax = i;
	break;
      }
    }
    if (xmax != 0) break;
  }

  //streamsize sz = cin.precision();
  //cout.precision(3);
    
  ret.length = std::max(xmax - ret.xmin, ymax - ret.ymin);
  ret.xmin -= (ret.length - (xmax - ret.xmin)) / 2;
  ret.ymin -= (ret.length - (ymax - ret.ymin)) / 2;

  return ret;
}



template <class T, class K>
void arr_shrink1(T beg1, T end1, int l, K beg2, int L, double h) {
  int rest = l;  //il resto della frazione precedente: in (0, l]
  while (beg1 != end1) {
    *beg1 += h * rest * *beg2++ / L;
    int k = L - rest;
    while (k >= l) {
      *beg1 += h * l * *beg2++ / L;
      k -= l;
    }
    if (k) *beg1 += h * k * *beg2 / L; //Non ho incrementato apposta, questo elemento dovrà contribuire pure al prossimo elemento di beg1
    beg1++;
    rest = l - k;
  }
}


template <class T, class K>
void arr_shrink2(T& arr1, int l, K& arr2, int xbeg, int ybeg, int L) {
  int rest = l;
  int y1 = 0;
  while (y1 != l) {
    arr_shrink1(std::begin(arr1[y1]), std::begin(arr1[y1]) + l, l, std::begin(arr2[ybeg++]) + xbeg, L, double(rest) / L);
    int k = L - rest;
    while (k >= l) {
      arr_shrink1(
std::begin(arr1[y1]), std::begin(arr1[y1]) + l, l, std::begin(arr2[ybeg++]) + xbeg, L, double(l) / L);
      k -= l;
    }
    if (k) arr_shrink1(std::begin(arr1[y1]), std::begin(arr1[y1]) + l, l, std::begin(arr2[ybeg]) + xbeg, L, double(k) / L);  //Non ho incrementato ybeg di proposito
    //for (int x = 0; x != l; ++x) arr1[y1][x] /= L;  //Fare questa operazione a questo punto non va bene in più dimensioni! Va diviso per L ogni valore che si aggiunge a questo elemento dell'array, non l'elemento tutto insieme alla fine
    rest = l - k;
    ++y1;
  }
}


template <class T, class K>
void arr_shrink2(T& arr1, int l, K& arr2, const Coords& c) {
  return arr_shrink2(arr1, l, arr2, c.xmin, c.ymin, c.length);
}
  
template <class T, class K>
void arr_stretch1(T beg1, T end1, int L, K beg2, int l, double h) {
  int rest = L;
  while (beg1 != end1) {
    if (rest >= l) {
      *beg1++ += *beg2 * h;
      rest -= l;
    } else {
      *beg1 += *beg2++ * rest * h / l;
      *beg1 += *beg2 * (l - rest) * h / l;
      ++beg1;
      rest = L + rest - l;
    }
  }
}

template <class T, class K>
void arr_stretch2(T& arr1, int L, K& arr2, int xbeg, int ybeg, int l) {
  int rest = L;
  int y1 = 0;
  while (y1 != L) {
    if (rest >= l) {
      arr_stretch1(std::begin(arr1[y1]), std::begin(arr1[y1]) + L, L, std::begin(arr2[ybeg]) + xbeg, l, 1);
      ++y1;
      rest -= l;
    } else {
      arr_stretch1(std::begin(arr1[y1]), std::begin(arr1[y1]) + L, L, std::begin(arr2[ybeg]) + xbeg, l, double(rest) / l);
      ++ybeg;
      arr_stretch1(std::begin(arr1[y1]), std::begin(arr1[y1]) + L, L, std::begin(arr2[ybeg]) + xbeg, l, double(l - rest) / l);
      //for (int x = 0; x != L; ++x) arr1[y1][x] /= l;  //In più dimensioni questa riga non va bene; prima si dividono per l i valori, poi si mettono sulla riga, se no ei rischia di dividere per l valori vecchi già divisi
      ++y1;
      rest = L + rest - l;
    }
  }
}

template <class T, class K>
void arr_stretch2(T& arr1, int L, K& arr2, const Coords& c) {
  return arr_stretch2(arr1, L, arr2, c.xmin, c.ymin, c.length);
}


#endif  //ARR_SCALE_H
