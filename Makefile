
CC = gcc
TARGET = main

run:
	$(CC) -o $(TARGET) $(TARGET).c context.c
	sudo ./$(TARGET)

check:
	$(CC) -o $(TARGET) $(TARGET).c context.c
	sudo valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(TARGET)
