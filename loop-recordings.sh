#!/bin/bash

END=5

for i in $(seq 1 $END); 
    do ./run.sh ${i} || true;
    # python3 analyser/src/mergeCSV.py csv/report${i}.csv unzipped/csv/report${i}.csv;
    # cat mergedReports.csv;
    # python3 analyser/src/analyser.py < mergedReports.csv;
    # mv performance.png "performancePlot${NR}.png";
    # rm mergedReports.csv
done