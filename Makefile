CC = clang 
LDFLAGS = -lgpiod

all : main 
	./main

main : main.c 
	$(CC) $< -o $@ $(LDFLAGS)

clean :
	rm main 
