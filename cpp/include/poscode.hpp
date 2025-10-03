#ifndef __POSCODE_HPP__
#define __POSCODE_HPP__

#include <cstddef>
#include <string>

class Poscode {
  private:
    std::string data;

  public:
    Poscode();
    explicit Poscode(const std::string &value);

    char getValue(std::size_t i) const;
    const std::string &getData() const;
};

#endif
