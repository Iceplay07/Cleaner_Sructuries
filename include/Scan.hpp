#include <filesystem>
#include <string>
#include <vector>

#ifndef SCAN_H_H
#define SCAN_H_H

class Scaner {
public:
  struct FileInfo {
    std::string head;
    std::string path;
    std::size_t size = 0;
    bool is_empty = false;
    std::vector<std::string> duplicates;
    std::vector<std::string> similar;
  };

  explicit Scaner(double similarity = 0.80);

  std::vector<FileInfo> Scan(std::filesystem::path &root);

private:
  struct PreparedFile {
    FileInfo info;
    std::string content;
    std::vector<std::string> mean;
  };

  double similarity_;

  bool Is_Code_File(std::filesystem::path &path);
  bool Can_Compare_Similar(std::filesystem::path &a, std::filesystem::path &b);

  std::string Read_Full_File(std::filesystem::path &path);
  std::vector<std::string> Read_Mean_Lines(std::filesystem::path &path);
  std::string Norm_Line(std::string &line);

  bool Is_Empty(std::string &cont);
  std::size_t Count_Common_Lines(std::vector<std::string> &smal,
                                 std::vector<std::string> &big);
                                 
  void Mark_Duplicates(std::vector<PreparedFile> &files);
  void Mark_Similar(std::vector<PreparedFile> &files);
};

#endif // !SCAN_H_H
