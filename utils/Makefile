SHELL=/bin/bash

MSGRESULTS=$(shell python3 -c 'import itertools; import numpy as np; print(" ".join(["perf-data/msg_{0:d}_{1:d}_{1:d}_{2:d}_{3:d}_{4:s}_{5:s}_.csv".format(*x) for x in itertools.product(range(10), np.arange(1,21), [1000], [500], ["true", "false"], ["fork"])]))')

BUFFERRESULTS=$(shell python -c 'import itertools; import numpy as np; print(" ".join(["perf-data/buf_{0}_{1}_{1}_{2}_{3}_{4}_.csv".format(*x) for x in itertools.product(range(10), np.arange(1,21), [100000000], ["true", "false"], ["fork"])]))')

OPTIRESULTS=$(shell python -c 'import itertools; import numpy as np; print(" ".join(["perf-data/opti_{0}_{1}_{2}_{3}_{4}_.csv".format(*x) for x in itertools.product(range(100), np.arange(1,21), [1000000000], ["true"], ["default", "opti", "perf"])]))')

REPETITIONS=10

.PHONY: setup all msg buf clean migrations resched opti

all: setup $(MSGRESULTS) $(BUFFERRESULTS)

msg: setup $(MSGRESULTS)

buf: setup $(BUFFERRESULTS)

opti: setup $(OPTIRESULTS)

setup:
	@echo "### SETTING UP"
	./create_cpuset.sh
	./cpugov.sh performance
	./irq_affinity.sh

perf-data/msg%.csv:
	mkdir -p perf-data
	@echo $@
	$(eval RUN=$(shell python -c "print \"$@\".split(\"_\")[1]"))
	$(eval PIPES=$(shell python -c "print \"$@\".split(\"_\")[2]"))
	$(eval STAGES=$(shell python -c "print \"$@\".split(\"_\")[3]"))
	$(eval BURST_SIZE=$(shell python -c "print \"$@\".split(\"_\")[4]"))
	$(eval PDU_SIZE=$(shell python -c "print \"$@\".split(\"_\")[5]"))
	$(eval RT=$(shell python -c "print \"-t\" if \"$@\".split(\"_\")[6] == \"true\" else \"\""))
	$(eval CONFIG=$(shell python -c "print \"$@\".split(\"_\")[7]"))
	@echo RUN=$(RUN)
	@echo PIPES=$(PIPES)
	@echo STAGES=$(STAGES)
	@echo BURST_SIZE=$(BURST_SIZE)
	@echo PDU_SIZE=$(PDU_SIZE)
	@echo CONFIG=$(CONFIG)
	@echo RT=$(RT)
	@echo REPETITIONS=$(REPETITIONS)

	cset shield --userset=sdr --exec -- ./build/run_msg_flowgraph -m --run=$(RUN) --pipes=$(PIPES) --stages=$(STAGES) --repetitions=$(REPETITIONS) --burst_size=$(BURST_SIZE) --pdu_size=$(PDU_SIZE) $(RT) --config=$(CONFIG) | grep -v cset > $@


perf-data/buf%.csv:
	mkdir -p perf-data
	@echo $@
	$(eval RUN=$(shell python -c "print \"$@\".split(\"_\")[1]"))
	$(eval PIPES=$(shell python -c "print \"$@\".split(\"_\")[2]"))
	$(eval STAGES=$(shell python -c "print \"$@\".split(\"_\")[3]"))
	$(eval SAMPLES=$(shell python -c "print \"$@\".split(\"_\")[4]"))
	$(eval RT=$(shell python -c "print \"-t\" if \"$@\".split(\"_\")[5] == \"true\" else \"\""))
	$(eval CONFIG=$(shell python -c "print \"$@\".split(\"_\")[6]"))
	@echo RUN=$(RUN)
	@echo PIPES=$(PIPES)
	@echo STAGES=$(STAGES)
	@echo SAMPLES=$(SAMPLES)
	@echo RT=$(RT)
	@echo CONFIG=$(CONFIG)

	cset shield --userset=sdr --exec -- ./build/run_buf_flowgraph -m --run $(RUN) --pipes=$(PIPES) --stages=$(STAGES) --samples=$(SAMPLES) $(RT) --config=$(CONFIG) | grep -v cset > $@


perf-data/opti%.csv:
	mkdir -p perf-data
	@echo $@
	$(eval RUN=$(shell python -c "print \"$@\".split(\"_\")[1]"))
	$(eval STAGES=$(shell python -c "print \"$@\".split(\"_\")[2]"))
	$(eval SAMPLES=$(shell python -c "print \"$@\".split(\"_\")[3]"))
	$(eval RT=$(shell python -c "print \"-t\" if \"$@\".split(\"_\")[4] == \"true\" else \"\""))
	$(eval CONFIG=$(shell python -c "print \"$@\".split(\"_\")[5]"))
	@echo RUN=$(RUN)
	@echo STAGES=$(STAGES)
	@echo SAMPLES=$(SAMPLES)
	@echo RT=$(RT)
	@echo CONFIG=$(CONFIG)

	cset shield --userset=sdr --exec -- ./build/run_opti_flowgraph -m --run $(RUN) --stages=$(STAGES) --samples=$(SAMPLES) $(RT) --config=$(CONFIG) | grep -v cset > $@


migrations: migrations_rt.csv migrations.csv 

migrations_rt.dat:
	sudo perf record -C 2,3,6,7 -o migrations_rt.dat -m 32768 -e sched:sched_migrate_task -- cset shield --userset=sdr --exec -- ./build/run_buf_flowgraph -m --run 0 --pipes=4 --stages=4 --samples=100000000 --config=fork -t
	sudo chmod a+rw migrations_rt.dat


migrations_rt.csv: migrations_rt.dat
	perf script -i migrations_rt.dat | eval/parse_perf_sched.py -w migrations > migrations_rt.csv


migrations.dat:
	sudo perf record -C 2,3,6,7 -o migrations.dat -m 65536 -e sched:sched_migrate_task -- cset shield --userset=sdr --exec -- ./build/run_buf_flowgraph -m --run 0 --pipes=4 --stages=4 --samples=100000000 --config=fork
	sudo chmod a+rw migrations.dat


migrations.csv: migrations.dat
	perf script -i migrations.dat | eval/parse_perf_sched.py -w migrations > migrations.csv


clean:
	rm -f migrations{,_rt}.{dat,dat.old,csv} perf.data{,.old} resched{,_rt}.{dat,dat.old,csv}


resched: resched_rt.csv resched.csv 

resched_rt.dat:
	sudo perf record -C 2,3,6,7 -o resched_rt.dat -m 65536 -e sched:sched_switch -- cset shield --userset=sdr --exec -- ./build/run_buf_flowgraph -m --run 0 --pipes=10 --stages=10 --samples=100000000 --config=fork -t
	sudo chmod a+rw resched_rt.dat


resched_rt.csv: resched_rt.dat
	perf script -i resched_rt.dat | eval/parse_perf_sched.py -w resched > resched_rt.csv


resched.dat:
	sudo perf record -C 2,3,6,7 -o resched.dat -m 65536 -e sched:sched_switch -- cset shield --userset=sdr --exec -- ./build/run_buf_flowgraph -m --run 0 --pipes=10 --stages=10 --samples=100000000 --config=fork
	sudo chmod a+rw resched.dat


resched.csv: resched.dat
	perf script -i resched.dat | eval/parse_perf_sched.py -w resched > resched.csv

