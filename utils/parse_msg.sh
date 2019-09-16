#!/bin/bash

files=`ls perf-data/msg_*.csv`
outfile=perf-data/msg.csv
rm -f ${outfile}

echo "config,prio,run,pipes,stages,repetition,burst_size,pdu_size,time" > ${outfile}

for f in ${files}
do
	cat $f >> ${outfile}
done
