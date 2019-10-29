
CC = gcc
TARGET = main

run:
	$(CC) -o $(TARGET) $(TARGET).c context.c fb_objects.c image.c
	sudo ./$(TARGET)

check:
	$(CC) -o $(TARGET) $(TARGET).c context.c fb_objects.c image.c
	sudo valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(TARGET)
