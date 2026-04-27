#include "../include/Scan.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <unordered_map>
#include <unordered_set>
#include <iterator>
#include <thread>
#include <mutex>

bool Is_Space(const std::string &s){
  for (char c : s){
    if(!std::isspace(c)){
      return false;
    }
  }
  return true;
}

bool Scaner::Is_Empty(std::string &cont){
  return Is_Space(cont);
}


bool Scaner::Is_Code_File(std::filesystem::path &path){
  std::string ep = path.extension().string();
  static const std::vector<std::string> ban = {
        // картинки
        ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".webp", ".tiff", ".ico", ".svg",

        // аудио
        ".mp3", ".wav", ".flac", ".aac", ".ogg", ".m4a",

        // видео
        ".mp4", ".avi", ".mkv", ".mov", ".wmv", ".webm",

        // архивы
        ".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz",

        // документы/бинарные
        //".pdf", ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx",

        // исполняемые и бинарные
        ".exe", ".dll", ".so", ".bin", ".obj", ".o", ".class", ".jar"
    };

    for (const auto& banned : ban) {
        if (ep == banned) {
            return false;
        }
    }

    return true;
}


bool Scaner::Can_Compare_Similar(std::filesystem::path &a, std::filesystem::path &b) {
  std::string ea = a.extension().string();
  std::string eb = b.extension().string();
  if((ea == ".c" || ea == ".cpp") && (eb == ".hpp" || eb == ".h")){
    return false;
  }
  if((eb == ".c" || eb == ".cpp") && (ea == ".hpp" || ea == ".h")){
    return false;
  }
  return true;
}





Scaner::Scaner(double similarity) : similarity_(similarity) {
  if (similarity_ < 0.0 || similarity_ > 1.0) {
    similarity_ = 0.8;
  }
}

std::vector<Scaner::FileInfo> Scaner::Scan(std::filesystem::path &root) {
  if (!std::filesystem::exists(root)) {
    throw std::runtime_error("Directory does not exist: " + root.string());
  }

  if (!std::filesystem::is_directory(root)) {
    throw std::runtime_error("Path is not a directory: " + root.string());
  }

  std::vector<PreparedFile> files;

  std::error_code ec;
  std::filesystem::recursive_directory_iterator it(
      root, std::filesystem::directory_options::skip_permission_denied, ec);
  std::filesystem::recursive_directory_iterator end;

  for (; it != end; it.increment(ec)) {
    if (ec) {
      ec.clear();
      continue;
    }

    auto &entry = *it;

    if (!entry.is_regular_file(ec)) {
      ec.clear();
      continue;
    }

    auto path = entry.path();

    if(!Is_Code_File(path)){
      continue;
    }

    PreparedFile pf;
    pf.info.head = path.filename().string();
    pf.info.path = path.string();

    pf.info.size = entry.file_size(ec);
    if (ec) {
      pf.info.size = 0;
      ec.clear();
    }

    pf.content = Read_Full_File(path);
    pf.mean = Read_Mean_Lines(path);
    pf.info.is_empty = Is_Empty(pf.content);

    files.push_back(std::move(pf));
  }

  Mark_Duplicates(files);
  Mark_Similar(files);

  std::vector<FileInfo> result;
  result.reserve(files.size());

  for (auto &file : files) {
    result.push_back(std::move(file.info));
  }

  return result;
}




std::string Scaner::Read_Full_File(std::filesystem::path &path){
  std::ifstream in (path, std::ios::binary);
  if (!in){
    return "";
  }
  return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}


std::vector<std::string> Scaner::Read_Mean_Lines(std::filesystem::path &path){
  std::vector<std::string> res;
  std::ifstream in(path);
  if (!in){
    return res;
  }
  std::string line;
  while(std::getline(in, line)){
    std::string norm = Norm_Line(line);
    if(!norm.empty() && norm[0] != '#'){
      res.push_back(norm);
    }
  }
  return res;

}

std::string Scaner::Norm_Line(std::string &line){
  std::string res;
  for(char c : line){
    if(!std::isspace(c)){
      res.push_back(tolower(c));
    }
  }
  return res;
}



std::string Norm_Line_(std::string &line){
  std::string res;
  for(char c : line){
    if(!std::isspace(c)){
      res.push_back(tolower(c));
    }
  }
  return res;
}


bool Check_Line(std::string &line){
  std::string res = Norm_Line_(line);
  if(res == "{}" || res == "()" || res == "[]" || res == "{" || res == "}" || res == "(" || res == ")" || res == "[" || res == "]"){
    return false;
  }
  return true;
}


std::size_t Scaner::Count_Common_Lines(std::vector<std::string> &smal, std::vector<std::string> &big){
  std::unordered_map<std::string, std::size_t> cnt;

  std::unordered_set<std::string> small_set;
  std::unordered_set<std::string> big_set;

  for(auto& line: big){
    if(line.size() >= 4 && Check_Line(line)){
      big_set.insert(line);
    }
  }
  for(auto& line: smal){
    if(line.size() >= 4 && Check_Line(line)){
      small_set.insert(line);
    }
  }
  std::size_t res = 0;
  
  for(auto &line : small_set){
    if(big_set.find(line) != big_set.end()){
      res++;
    }
  }

  return res;
}



void Scaner::Mark_Duplicates(std::vector<Scaner::PreparedFile>& files){
  for(std::size_t i = 0; i < files.size(); ++i){
    for (std::size_t j = i + 1; j < files.size(); ++j){
      if(files[i].info.is_empty || files[j].info.is_empty){
        continue;
      }
      if(files[i].content == files[j].content){
        files[i].info.duplicates.push_back(files[j].info.path);
        files[j].info.duplicates.push_back(files[i].info.path);
      }
    }
  }
}

void Scaner::Mark_Similar(std::vector<Scaner::PreparedFile>& files) {
  if (files.size() < 2) {
    return;
  }

  std::mutex mtx;

  unsigned int thread_count = std::thread::hardware_concurrency();
  if (thread_count == 0) {
    thread_count = 4;
  }

  if (thread_count > files.size()) {
    thread_count = files.size();
  }

  std::vector<std::thread> th;

  auto work = [&](std::size_t left, std::size_t right) {
    for (std::size_t i = left; i < right; ++i) {
      for (std::size_t j = i + 1; j < files.size(); ++j) {
        if (files[i].info.is_empty || files[j].info.is_empty) {
          continue;
        }

        if (files[i].content == files[j].content) {
          continue;
        }

        std::filesystem::path path_i(files[i].info.path);
        std::filesystem::path path_j(files[j].info.path);

        if (!Can_Compare_Similar(path_i, path_j)) {
          continue;
        }

        std::size_t big_index = i;
        std::size_t small_index = j;

        if (files[j].mean.size() > files[i].mean.size()) {
          big_index = j;
          small_index = i;
        } else if (files[j].mean.size() == files[i].mean.size()) {
          if (files[j].info.size > files[i].info.size) {
            big_index = j;
            small_index = i;
          }
        }

        std::unordered_set<std::string> small_set;

        for (auto& line : files[small_index].mean) {
          if (line.size() >= 4 && Check_Line(line)) {
            small_set.insert(line);
          }
        }

        if (small_set.empty()) {
          continue;
        }

        std::size_t cnt =
            Count_Common_Lines(files[small_index].mean, files[big_index].mean);

        double ratio = (double)(cnt) / small_set.size();

        if (ratio >= similarity_) {
          std::lock_guard<std::mutex> lock(mtx);
          files[big_index].info.similar.push_back(files[small_index].info.path);
        }
      }
    }
  };

  std::size_t cnt_th = files.size() / thread_count;
  std::size_t start = 0;

  for (unsigned int i = 0; i < thread_count; ++i) {
    std::size_t end = (i == thread_count - 1) ? files.size() : start + cnt_th;
    th.push_back(std::thread(work, start, end));
    start = end;
  }

  for (auto& t : th) {
    t.join();
  }
}



