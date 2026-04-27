#include "../include/Output.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>


Output::Output(double similarity) : similarity_(similarity){
    if(similarity_ < 0.0 || similarity_ > 1.0){
        similarity_ = 0.80;
    }
}

void Output::Print_Stdout(std::vector<Scaner::FileInfo> &files){
    Print(files, std::cout);
}

void Output::Print_File(std::vector<Scaner::FileInfo> &files, std::filesystem::path &o_path) {
    if (o_path.has_parent_path()) {
        std::filesystem::create_directories(o_path.parent_path());
    }

    std::ofstream out(o_path);

    if (!out) {
        throw std::runtime_error("Can't open output file: " + o_path.string());
    }

    Print(files, out);
}

void Output::Print(std::vector<Scaner::FileInfo> &files, std::ostream &out){
    std::size_t empty_cnt = 0;
    std::size_t duplicate_cnt = 0;
    std::size_t similar_cnt = 0;

    for (auto &file: files){
        if(file.is_empty)
            empty_cnt++;
        if(!file.duplicates.empty())
            duplicate_cnt++;
        if(!file.similar.empty())
            similar_cnt++;
    }

    if(files.empty()){
        out << "Файлы не найдены\n";
        return;
    }

    out << "              ОТЧЁТ             \n";
    out << "========================================\n";
    out << "Всего файлов:          " << files.size() << '\n';
    out << "Пустых файлов:         " << empty_cnt << '\n';
    out << "Файлов с дубликатами:  " << duplicate_cnt << '\n';
    out << "Файлов с похожими:     " << similar_cnt << '\n';
    out << "========================================\n\n";

    for(std::size_t i = 0; i < files.size(); ++i){
        Print_File_Info(files[i], i + 1, out);
    }

}

std::string Output::Size_Mb(std::size_t bytes){
    const char *name[] = {"B", "KB", "MB", "GB", "TB"};

    double value = double(bytes);
    std::size_t name_index = 0;

    while(value >= 1024.0 && name_index + 1 < 5){
        value /= 1024.0;
        ++name_index;
    }
    if (name_index == 0) {
        return std::to_string(bytes) + " " + name[name_index];
    }


    value = std::round(value * 1000.0) / 1000.0;

    std::string number = std::to_string(value);

    while (!number.empty() && number.back() == '0') {
        number.pop_back();
    }

    if (!number.empty() && number.back() == '.') {
        number.pop_back();
    }

    return number + " " + name[name_index];
}


void Output::Print_Path_List(std::vector<std::string> &path, std::ostream &out){
    if(path.empty()){
        out << " нет\n";
        return;
    }
    for(auto &pat : path){
        out << " -" << pat << '\n';
    }
}

void Output::Print_File_Info(Scaner::FileInfo &file, std::size_t index, std::ostream &out){

     out << "----------------------------------------\n";
  out << "Файл №" << index << '\n';
  out << "Имя:      " << file.head << '\n';
  out << "Путь:     " << file.path << '\n';
  out << "Размер:   " << Size_Mb(file.size) << '\n';
  out << "Пустой:   " << (file.is_empty ? "да" : "нет") << '\n';

  out << "Дубликаты:\n";

  Print_Path_List(file.duplicates, out);
  int percent = (int)(similarity_ * 100);
  out << "Похожие > " << percent << "%:\n";
  Print_Path_List(file.similar, out);

  out << '\n';
}
