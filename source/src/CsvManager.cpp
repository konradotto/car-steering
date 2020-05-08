#include "CsvManager.hpp"

using namespace std;

const char* CsvManager::path = "csv/reports.csv";

void CsvManager::refresh(){
    remove(CsvManager::path);
    ofstream ofile;
    ofile.open(CsvManager::path);
    ofile << "time_stamp;ground_steering_0;ground_steering_1;\n";
    ofile.close();
}

void CsvManager::add(uint32_t ts, double gr0, double gr1){
    ofstream csv;
    csv.open(CsvManager::path, fstream::app | fstream::out);
    csv << ts << ";" << gr0 << ";" << gr1 << ";\n";
    csv.close();
}