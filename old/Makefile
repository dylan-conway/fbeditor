
make:
	mkdir -p bin logs
	gcc src/main.c -o bin/main
	gcc src/input/main.c src/game/inputs.c src/game/context.c -o bin/input
	gcc src/game/main.c src/game/context.c src/game/fb_objects.c src/game/sprite.c src/game/game.c src/game/inputs.c -lm -o bin/game

run:
	sudo ./bin/main

check:
	sudo valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --trace-children=yes --log-file=logs/pid-%p.log ./bin/main

clean:
	rm -rf bin logs
