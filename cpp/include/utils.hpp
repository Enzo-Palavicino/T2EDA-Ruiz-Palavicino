#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "poscode.hpp"

#include <cstddef>
#include <string>
#include <vector>

class IndexLinkedList {
  public:
    struct Node {
      std::size_t index{0};
      Node *next{nullptr};
    };

    IndexLinkedList();
    void pushBack(Node *node);
    Node *head() const;
    void clear();

  private:
    Node *first;
    Node *last;
};

void quick_sort(Poscode *A, std::size_t n);
void merge_sort(Poscode *A, std::size_t n);
void radix_sort(Poscode *A, std::size_t n);

Poscode *readCodes(const std::string &strfile, std::size_t n);
void deleteCodes(Poscode *codes);

double computeAverage(const std::vector<double> &samples);
double computeStdDeviation(const std::vector<double> &samples, double mean);

#endif

