# Name of the project
PROJ_NAME=boiler_controller
 
# .c files
C_SOURCE=$(wildcard ./src/*.c)
 
# .h files
H_SOURCE=$(wildcard ./include/*.h)
 
# Object files
OBJ=$(subst .c,.o,$(subst source,objects,$(C_SOURCE)))
 
# Compiler and linker
CC=gcc
 
# Flags for compiler
CC_FLAGS=-c         \
		 -W         \
		 -Wall 		\

# additional Libraries
LDFLAGS=-pthread
 
# Command used at clean target
RM = rm -rf
 
#
# Compilation and linking
#
all: objFolder $(PROJ_NAME)
 
$(PROJ_NAME): $(OBJ)
	@ echo 'Building binary using GCC linker: $@'
	$(CC) $^ -o $@ $(LDFLAGS)
	@ echo 'Finished building binary: $@'
	@ echo ' '
 
./objects/%.o: ./src/%.c ./include/%.h
	@ echo 'Building target using GCC compiler: $<'
	$(CC) $< $(CC_FLAGS) -o $@
	@ echo ' '
 
./objects/main.o: ./src/main.c $(H_SOURCE)
	@ echo 'Building target using GCC compiler: $<'
	$(CC) $< $(CC_FLAGS) -o $@
	@ echo ' '
 
objFolder:
	@ mkdir -p objects
 
clean:
	@ $(RM) ./objects/*.o $(PROJ_NAME) *~
	@ rmdir objects
 
.PHONY: all clean