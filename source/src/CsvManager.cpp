#include "CsvManager.hpp"

#include <fstream>
#include <stdio.h>

using namespace std;

const char* CsvManager::path = "csv/reports.csv";
const char CsvManager::delimiter = ';';

//restore csv file
void CsvManager::refresh() {
    remove(CsvManager::path);//Remove the report.csv if it already exists
    ofstream ofile;
    ofile.open(CsvManager::path);//open a new outputstream to csv/report.csv
    ofile << "time_stamp" << delimiter << "ground_steering_0" << delimiter << "ground_steering_1" << delimiter << endl;//add column titles
    ofile.close(); //close outputstream
}

void CsvManager::add(uint32_t ts, double gr0, double gr1) {
    ofstream csv;
    csv.open(CsvManager::path, fstream::app | fstream::out);
    
    // write ts, gr0 and gr1 separated with the standard delimiter to csv and end the line
    csv << ts << delimiter << gr0 << delimiter << gr1 << delimiter << endl;
    csv.close();
}