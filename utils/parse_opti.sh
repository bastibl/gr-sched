#!/bin/bash

files=`ls perf-data/opti_*.csv`
outfile=perf-data/opti.csv
rm -f ${outfile}

echo "config,run,pipes,stages,samples,rt,work_avg,work_var,input_avg,input_var,output_avg,output_var,produced_avg,produced_var,throughput_avg,time" > ${outfile}

for f in ${files}
do
	cat $f >> ${outfile}
done
