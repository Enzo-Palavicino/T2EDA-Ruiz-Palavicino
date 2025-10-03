#include "poscode.hpp"
#include "utils.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct DatasetConfig {
  std::string name;
  std::string filename;
  std::size_t count;
};

struct AlgorithmConfig {
  std::string name;
  void (*function)(Poscode *, std::size_t);
};

struct AlgorithmResult {
  std::string dataset;
  std::string algorithm;
  std::vector<double> samples;
  double average{0.0};
  double stddev{0.0};
};

std::string joinPath(const std::string &dir, const std::string &filename) {
  if (dir.empty()) {
    return filename;
  }

  char last = dir.back();
  if (last == '/' || last == '\\') {
    return dir + filename;
  }

  return dir + "/" + filename;
}

bool isSorted(const std::vector<Poscode> &data) {
  for (std::size_t i = 1; i < data.size(); ++i) {
    if (data[i - 1].getData() > data[i].getData()) {
      return false;
    }
  }
  return true;
}

void printUsage(const std::string &programName) {
  std::cout << "Uso: " << programName
            << " [--data-dir RUTA] [--runs N] [--verify]" << std::endl;
  std::cout << "  --data-dir RUTA  Directorio que contiene los archivos codes_*.txt" << std::endl;
  std::cout << "  --runs N        Número de ejecuciones por algoritmo (por defecto 5)" << std::endl;
  std::cout << "  --verify        Verifica que el resultado de cada ordenamiento esté ordenado" << std::endl;
}

int main(int argc, char **argv) {
  std::string dataDir = "build";
  std::size_t runs = 5;
  bool verify = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--data-dir" && i + 1 < argc) {
      dataDir = argv[++i];
    } else if (arg == "--runs" && i + 1 < argc) {
      std::istringstream ss(argv[++i]);
      std::size_t value = 0;
      ss >> value;
      if (!ss || value == 0) {
        std::cerr << "Error: el valor de --runs debe ser un entero positivo." << std::endl;
        return 1;
      }
      runs = value;
    } else if (arg == "--verify") {
      verify = true;
    } else if (arg == "--help") {
      printUsage(argv[0]);
      return 0;
    } else {
      std::cerr << "Argumento no reconocido: " << arg << std::endl;
      printUsage(argv[0]);
      return 1;
    }
  }

  const std::vector<DatasetConfig> datasets = {
      {"500K", "codes_500K.txt", 500000},
      {"1M", "codes_1M.txt", 1000000},
      {"10M", "codes_10M.txt", 10000000},
  };

  const std::vector<AlgorithmConfig> algorithms = {
      {"RadixSort", radix_sort},
      {"QuickSort", quick_sort},
      {"MergeSort", merge_sort},
  };

  std::vector<AlgorithmResult> results;
  results.reserve(datasets.size() * algorithms.size());

  for (const auto &dataset : datasets) {
    std::string path = joinPath(dataDir, dataset.filename);
    std::cout << "Cargando " << path << " (" << dataset.count << " códigos)" << std::endl;

    Poscode *rawData = readCodes(path, dataset.count);
    if (rawData == nullptr) {
      std::cerr << "No fue posible cargar el dataset requerido." << std::endl;
      return 1;
    }

    std::vector<Poscode> base(rawData, rawData + dataset.count);
    deleteCodes(rawData);

    for (const auto &algorithm : algorithms) {
      AlgorithmResult result;
      result.dataset = dataset.name;
      result.algorithm = algorithm.name;
      result.samples.reserve(runs);

      for (std::size_t run = 0; run < runs; ++run) {
        std::vector<Poscode> work = base;
        auto start = std::chrono::steady_clock::now();
        algorithm.function(work.data(), work.size());
        auto end = std::chrono::steady_clock::now();

        double seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
        result.samples.push_back(seconds);

        if (verify && !isSorted(work)) {
          std::cerr << "Error: el algoritmo " << algorithm.name
                    << " no generó un arreglo ordenado para el dataset " << dataset.name << std::endl;
          return 1;
        }
      }

      result.average = computeAverage(result.samples);
      result.stddev = computeStdDeviation(result.samples, result.average);
      results.push_back(result);

      std::cout << "  " << algorithm.name << " completado." << std::endl;
    }
  }

  std::cout << std::endl;
  std::cout << std::left << std::setw(10) << "Dataset" << std::setw(12) << "Algoritmo"
            << std::setw(16) << "Promedio (s)" << std::setw(18) << "Desviación (s)" << std::endl;
  std::cout << std::string(56, '-') << std::endl;

  std::cout << std::fixed << std::setprecision(6);
  for (const auto &row : results) {
    std::cout << std::left << std::setw(10) << row.dataset << std::setw(12) << row.algorithm
              << std::setw(16) << row.average << std::setw(18) << row.stddev << std::endl;
  }

  std::cout.unsetf(std::ios::floatfield);
  std::cout << std::endl;

  for (const auto &row : results) {
    std::cout << row.dataset << " - " << row.algorithm << " tiempos (s): ";
    for (std::size_t i = 0; i < row.samples.size(); ++i) {
      if (i > 0) {
        std::cout << ", ";
      }
      std::cout << std::setprecision(6) << std::fixed << row.samples[i];
    }
    std::cout.unsetf(std::ios::floatfield);
    std::cout << std::endl;
  }

  return 0;
}

