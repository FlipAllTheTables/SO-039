# Grupo 039
# Rafael Ribeiro	- 58193
# Pedro Duque		- 52753
# Francisco Santos	- 59871

OBJECTS = configuration.o doctor.o hosptime.o log.o main.o memory.o patient.o process.o receptionist.o stats.o synchronization.o

BIN_DIR = bin
INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src

configuration.o = configuration.h
doctor.o = doctor.h synchronization.h
hosptime.o = hosptime.h memory.h
log.o = hosptime.h log.h
main.o = configuration.h hosptime.h log.h main.h process.h stats.h
memory.o = memory.h
patient.o = hosptime.h patient.h
process.o = doctor.h patient.h process.h receptionist.h
receptionist.o = hosptime.h receptionist.h
stats.o = stats.h
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