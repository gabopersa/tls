OBJECTS=main.o Implementation.o
tls: $(OBJECTS)
	gcc -pthread $(OBJECTS) -o $@ -g

Implementation.o: Implementation.c 
	gcc -pthread -c $< -o $@ -g

main.o: main.c Header.h
	gcc -pthread -c  $< -o $@ -g
