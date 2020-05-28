#ifndef CSVMANAGER_H
#define CSVMANAGER_H

#include <iostream>

/**
 * Class for writing  
 * 
 */
class CsvManager {
    private:
        static const char* path;        // path for saving the csv file
        static const char delimiter;    // delimiter separating columns in csv
    public:
        static void refresh();
        
        /**
         * Add a set of values ot the csv file
         * 
         * @param ts timestamp to be added to the file
         * @param gr0 double to be added to the file (actual ground steering request in our application)
         * @param gr1 double to be added to the file (calculated ground steering request in our application)
         *  
         */
        static void add(int64_t ts, double gr0, double gr1);
};

#endif
