cc = gcc
prom = paint.out
deps = board.h
obj = main.o board.o

$(prom): $(obj)
	$(cc) -o $(prom) $(obj)

%.o: %.c $(deps)
	$(cc) -c $< -o $@

clean:
	rm -rf *.o