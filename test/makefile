TARGET = image.out

CC = gcc

CFLAGS = -ansi -pedantic -Wall -Werror -g3 -std=c99
	

INC_DIR = ../inc
SRC_DIR = ../src

CFLAGS += -I$(INC_DIR)

SOURCES = image_processing.c image.c


OBJECTS = $(SOURCES:.c=.o)


$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -lm -o $(TARGET)

image.o: image.c $(INC_DIR)/image_processing.h
	$(CC) $(CFLAGS) -c image.c

image_processing.o: $(SRC_DIR)/image_processing.c $(INC_DIR)/image_processing.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/image_processing.c


clean:
	-rm $(TARGET) *.o

cleangrind:
	-rm *.log

run:  $(TARGET)
	 ./$(TARGET)

check: clean run

grind: valgrind helgrind
valgrind:  $(TARGET)
	 valgrind --log-file=valgrind.log --leak-check=full --track-origins=yes ./$(TARGET)
helgrind:  $(TARGET)
	 valgrind --tool=helgrind --log-file=helgrind.log ./$(TARGET)

gdb:  $(TARGET)
	 gdb -q ./$(TARGET)
