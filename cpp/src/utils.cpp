#include "utils.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

inline bool lessThan(const Poscode &lhs, const Poscode &rhs) {
  return lhs.getData() < rhs.getData();
}

inline bool lessEqual(const Poscode &lhs, const Poscode &rhs) {
  return lhs.getData() <= rhs.getData();
}

class CharMapper {
  public:
    CharMapper() {
      mapping.fill(0);
      for (std::size_t i = 0; i < 10; ++i) {
        mapping[static_cast<unsigned char>('0' + i)] = i;
      }
      for (std::size_t i = 0; i < 26; ++i) {
        mapping[static_cast<unsigned char>('A' + i)] = 10 + i;
        mapping[static_cast<unsigned char>('a' + i)] = 10 + i;
      }
    }

    std::size_t bucketIndex(char value) const {
      return mapping[static_cast<unsigned char>(value)];
    }

    std::size_t bucketCount() const { return 36; }

  private:
    std::array<std::size_t, 256> mapping{};
};

void countingSortByPosition(const Poscode *input, Poscode *output, std::size_t n,
                            std::size_t position, const CharMapper &mapper) {
  if (n == 0) {
    return;
  }

  std::vector<IndexLinkedList> buckets(mapper.bucketCount());
  std::vector<IndexLinkedList::Node> nodes(n);

  for (std::size_t i = 0; i < n; ++i) {
    char value = input[i].getValue(position);
    std::size_t bucket = mapper.bucketIndex(value);

    nodes[i].index = i;
    nodes[i].next = nullptr;
    buckets[bucket].pushBack(&nodes[i]);
  }

  std::size_t outIndex = 0;
  for (std::size_t bucket = 0; bucket < buckets.size(); ++bucket) {
    for (auto node = buckets[bucket].head(); node != nullptr; node = node->next) {
      output[outIndex++] = input[node->index];
    }
    buckets[bucket].clear();
  }
}

std::size_t medianOfThree(Poscode *A, std::size_t low, std::size_t high) {
  std::size_t mid = low + (high - low) / 2;

  if (lessThan(A[mid], A[low])) {
    std::swap(A[mid], A[low]);
  }
  if (lessThan(A[high], A[low])) {
    std::swap(A[high], A[low]);
  }
  if (lessThan(A[high], A[mid])) {
    std::swap(A[high], A[mid]);
  }

  return mid;
}

std::size_t partition(Poscode *A, std::size_t low, std::size_t high) {
  std::size_t pivotIndex = medianOfThree(A, low, high);
  std::swap(A[pivotIndex], A[high]);
  const Poscode &pivot = A[high];

  std::size_t i = low;
  for (std::size_t j = low; j < high; ++j) {
    if (lessThan(A[j], pivot)) {
      std::swap(A[i], A[j]);
      ++i;
    }
  }
  std::swap(A[i], A[high]);
  return i;
}

void quickSortRecursive(Poscode *A, std::size_t low, std::size_t high) {
  while (low < high) {
    std::size_t pivotIndex = partition(A, low, high);

    if (pivotIndex > 0 && (pivotIndex - low) < (high - pivotIndex)) {
      if (pivotIndex > 0) {
        quickSortRecursive(A, low, pivotIndex - 1);
      }
      low = pivotIndex + 1;
    } else {
      quickSortRecursive(A, pivotIndex + 1, high);
      if (pivotIndex == 0) {
        break;
      }
      high = pivotIndex - 1;
    }
  }
}

void mergeSections(Poscode *A, Poscode *buffer, std::size_t left, std::size_t mid,
                   std::size_t right) {
  std::size_t i = left;
  std::size_t j = mid + 1;
  std::size_t k = left;

  while (i <= mid && j <= right) {
    if (lessEqual(A[i], A[j])) {
      buffer[k++] = A[i++];
    } else {
      buffer[k++] = A[j++];
    }
  }

  while (i <= mid) {
    buffer[k++] = A[i++];
  }

  while (j <= right) {
    buffer[k++] = A[j++];
  }

  for (std::size_t idx = left; idx <= right; ++idx) {
    A[idx] = buffer[idx];
  }
}

void mergeSortRecursive(Poscode *A, Poscode *buffer, std::size_t left,
                        std::size_t right) {
  if (left >= right) {
    return;
  }

  std::size_t mid = left + (right - left) / 2;
  mergeSortRecursive(A, buffer, left, mid);
  mergeSortRecursive(A, buffer, mid + 1, right);
  mergeSections(A, buffer, left, mid, right);
}

} // namespace

IndexLinkedList::IndexLinkedList() : first(nullptr), last(nullptr) {}

void IndexLinkedList::pushBack(Node *node) {
  if (node == nullptr) {
    return;
  }
  if (first == nullptr) {
    first = node;
    last = node;
    return;
  }
  last->next = node;
  last = node;
}

IndexLinkedList::Node *IndexLinkedList::head() const { return first; }

void IndexLinkedList::clear() {
  first = nullptr;
  last = nullptr;
}

void quick_sort(Poscode *A, std::size_t n) {
  if (A == nullptr || n < 2) {
    return;
  }
  quickSortRecursive(A, 0, n - 1);
}

void merge_sort(Poscode *A, std::size_t n) {
  if (A == nullptr || n < 2) {
    return;
  }
  std::vector<Poscode> buffer(n);
  mergeSortRecursive(A, buffer.data(), 0, n - 1);
}

void radix_sort(Poscode *A, std::size_t n) {
  if (A == nullptr || n < 2) {
    return;
  }

  constexpr std::size_t CODE_LENGTH = 6;
  CharMapper mapper;
  std::vector<Poscode> buffer(n);

  Poscode *source = A;
  Poscode *destination = buffer.data();

  for (std::size_t pos = CODE_LENGTH; pos > 0; --pos) {
    countingSortByPosition(source, destination, n, pos - 1, mapper);
    std::swap(source, destination);
  }

  if (source != A) {
    for (std::size_t i = 0; i < n; ++i) {
      A[i] = source[i];
    }
  }
}

Poscode *readCodes(const std::string &strfile, std::size_t n) {
  Poscode *codes = new Poscode[n];

  std::ifstream inputFile(strfile);
  if (!inputFile.is_open()) {
    std::cerr << "Error: no se pudo abrir el archivo " << strfile << std::endl;
    delete[] codes;
    return nullptr;
  }

  std::string line;
  for (std::size_t i = 0; i < n; ++i) {
    if (!std::getline(inputFile, line)) {
      std::cerr << "Error: el archivo contiene menos líneas de las esperadas." << std::endl;
      delete[] codes;
      return nullptr;
    }
    codes[i] = Poscode(line);
  }

  return codes;
}

void deleteCodes(Poscode *codes) {
  delete[] codes;
}

double computeAverage(const std::vector<double> &samples) {
  if (samples.empty()) {
    return 0.0;
  }

  double sum = 0.0;
  for (double value : samples) {
    sum += value;
  }
  return sum / static_cast<double>(samples.size());
}

double computeStdDeviation(const std::vector<double> &samples, double mean) {
  std::size_t n = samples.size();
  if (n < 2) {
    return 0.0;
  }

  double accum = 0.0;
  for (double value : samples) {
    double diff = value - mean;
    accum += diff * diff;
  }
  double variance = accum / static_cast<double>(n - 1);
  return std::sqrt(variance);
}
