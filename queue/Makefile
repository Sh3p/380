CFLAGS := -O0 -g -Wall -Werror

.PHONY: all handin test clean

all: qtest

handin: handin.tar

handin.tar: queue.c queue.h
	tar cf $@ $^

queue.c: queue.h harness.h

queue.o: queue.c

qtest: qtest.c report.c console.c harness.c queue.o

test: qtest
	@./driver.py -A

clean:
	-@rm -vf *.o *~ qtest
	-@rm -rvf *.dSYM
