
CC = gcc
TARGET = main

run:
	$(CC) -o $(TARGET) $(TARGET).c src/context.c src/fb_objects.c src/sprite.c src/game.c src/inputs.c -lm

check:
	$(CC) -o $(TARGET) $(TARGET).c src/context.c src/fb_objects.c src/sprite.c src/game.c src/inputs.c -lm
	sudo valgrind --leak-check=full --track-origins=yes --log-file="valgrind.log" ./$(TARGET)

clean:
	rm -f valgrind.log
	rm -f $(TARGET)
