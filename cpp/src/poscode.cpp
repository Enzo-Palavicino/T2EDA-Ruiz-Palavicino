#include "poscode.hpp"

#include <stdexcept>

Poscode::Poscode() : data("") {}

Poscode::Poscode(const std::string &value) : data(value) {}

char Poscode::getValue(std::size_t i) const {
  if (i >= data.size()) {
    throw std::out_of_range("Índice fuera de rango en Poscode::getValue");
  }
  return data[i];
}

const std::string &Poscode::getData() const { return data; }

