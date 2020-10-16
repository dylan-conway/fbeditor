
make:
	gcc -o main main.c ctx.c input.c utils.c -lpthread

run:
	sudo ./main

check:
	sudo valgrind --leak-check=full --show-leak-kinds=all --tool=memcheck ./main

clean:
	rm -rf main

