#ifndef CSVMANAGER_H
#define CSVMANAGER_H

#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace std;

class CsvManager {
    private:
        static const char* path;
        static const char delimiter;
    public:
        static void refresh();
        static void add(uint32_t ts, double gr0, double gr1);
};

#endif
