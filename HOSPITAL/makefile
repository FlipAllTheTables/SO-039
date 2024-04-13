OBJECTS = main.o memory.o patient.o process.o receptionist.o doctor.o

BIN_DIR = bin
INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src

main.o =  memory.h process.h main.h
memory.o = memory.h
process.o = process.h memory.h main.h patient.h receptionist.h doctor.h
patient.o = main.h memory.h patient.h
receptionist.o = main.h memory.h receptionist.h
doctor.o = doctor.h main.h memory.h

CFLAGS = -Wall -I $(INC_DIR) -g
CC = gcc

vpath %.o $(OBJ_DIR)

hOSpital: $(OBJECTS)
	$(CC) $(addprefix $(OBJ_DIR)/,$(OBJECTS)) -o $(BIN_DIR)/hOSpital

%.o: src/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $<

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/hOSpital