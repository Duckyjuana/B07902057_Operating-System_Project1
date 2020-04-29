main.o: main.c FIFO.o RR.o SJF.o pSJF_2.o func.o
	gcc -o main.c FIFO.o RR.o SJF.o pSJF_2.o func.o

main.o: main.c
	gcc -c main.c

FIFO.o: fifo.c
	gcc -g -c FIFO.c

RR.o: rr.c
	gcc -c RR.c

SJF.o: SJF.c
	gcc -c SJF.c

PSJF.o: PSJF.c 
	gcc -c PSJF.c

func.o: util.c
	gcc -c func.c

clean:
	rm *.o
