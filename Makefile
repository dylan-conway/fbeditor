
CC = gcc
TARGET = main

run:
	$(CC) -o $(TARGET) $(TARGET).c src/context.c src/fb_objects.c src/image.c
	sudo ./$(TARGET)

check:
	$(CC) -o $(TARGET) $(TARGET).c src/context.c src/fb_objects.c src/image.c
	sudo valgrind --leak-check=full --track-origins=yes --log-file="valgrind.log" ./$(TARGET)

clean:
	rm -f valgrind.log
	rm -f $(TARGET)
