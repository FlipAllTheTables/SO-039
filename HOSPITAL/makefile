# Grupo 039
# Rafael Ribeiro	- 58193
# Pedro Duque		- 52753
# Francisco Santos	- 59871

OBJECTS = configuration.o doctor.o main.o memory.o patient.o process.o receptionist.o synchronization.o

BIN_DIR = bin
INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src

configuration.o = configuration.h main.h
doctor.o = doctor.h main.h memory.h
main.o = main.h memory.h process.h
memory.o = memory.h
patient.o = main.h memory.h patient.h
process.o = process.h memory.h main.h patient.h receptionist.h doctor.h
receptionist.o = main.h memory.h receptionist.h
synchronization.o = synchronization.h

CFLAGS = -Wall -I $(INC_DIR) -g
CC = gcc

vpath %.o $(OBJ_DIR)

hOSpital: $(OBJECTS)
	$(CC) $(addprefix $(OBJ_DIR)/,$(OBJECTS)) -o $(BIN_DIR)/hOSpital

%.o: src/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $<

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/hOSpital