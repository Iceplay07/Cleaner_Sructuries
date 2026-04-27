#include <iostream>
#include <string>
#include <vector>
#include "Scan.hpp"

#ifndef OUTPUT_H_H
#define OUTPUT_H_H

class Output {
public:

  explicit Output(double similarity = 0.80);

  void Print_Stdout(std::vector<Scaner::FileInfo> &files);
  void Print_File(std::vector<Scaner::FileInfo> &files, std::filesystem::path &o_path);

  void Print(std::vector<Scaner::FileInfo> &files, std::ostream &out);

private:
  std::string Size_Mb(std::size_t bytes);

  void Print_Path_List(std::vector<std::string> &path, std::ostream &out);
  void Print_File_Info(Scaner::FileInfo &file, std::size_t index, std::ostream &out);

  double similarity_;
};
#endif // !OUTPUT_H_H
