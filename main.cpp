#include "include/Scan.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

static void Print_List(const std::vector<std::string> &v,
                       const std::string &title) {
  std::cout << title << ":\n";
  if (v.empty()) {
    std::cout << "  нет\n";
    return;
  }

  for (const auto &x : v) {
    std::cout << "  - " << x << "\n";
  }
}

int main(int argc, char *argv[]) {
  try {
    std::filesystem::path root = ".";
    if (argc > 1) {
      root = argv[1];
    }

    Scaner sc(0.8);
    std::vector<Scaner::FileInfo> files = sc.Scan(root);

    std::sort(files.begin(), files.end(),
              [](const Scaner::FileInfo &a, const Scaner::FileInfo &b) {
                return a.path < b.path;
              });

    for (auto &file : files) {
      std::sort(file.duplicates.begin(), file.duplicates.end());
      std::sort(file.similar.begin(), file.similar.end());
    }

    std::cout << "Всего файлов: " << files.size() << "\n\n";

    for (const auto &file : files) {
      std::cout << "========================================\n";
      std::cout << "Имя: " << file.head << "\n";
      std::cout << "Путь: " << file.path << "\n";
      std::cout << "Размер: " << file.size << " байт\n";
      std::cout << "Пустой: " << (file.is_empty ? "да" : "нет") << "\n";

      Print_List(file.duplicates, "Дубликаты");
      Print_List(file.similar, "Похожие > 80%");
      std::cout << "\n";
    }
  } catch (const std::exception &e) {
    std::cerr << "Ошибка: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
