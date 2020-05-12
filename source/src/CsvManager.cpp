#include "CsvManager.hpp"

using namespace std;

const char* CsvManager::path = "/csv/reports.csv";
const char CsvManager::delimiter = ';';

void CsvManager::refresh(){
    remove(CsvManager::path);
    ofstream ofile;
    ofile.open(CsvManager::path);
    ofile << "time_stamp" << delimiter << "ground_steering_0" << delimiter << "ground_steering_1" << delimiter << endl;
    ofile.close();
}

void CsvManager::add(uint32_t ts, double gr0, double gr1){
    ofstream csv;
    csv.open(CsvManager::path, fstream::app | fstream::out);
    csv << ts << delimiter << gr0 << delimiter << gr1 << delimiter << endl;
    csv.close();
}