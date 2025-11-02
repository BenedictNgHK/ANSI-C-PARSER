SRC:=$(wildcard *.cpp)
# These files are for tesing or generating code, should not be compiled
NONEXEC:= test.cpp table.cpp 
EXEC:=$(filter-out $(NONEXEC), $(SRC))
TARGET:=AST
OBJ:=$(subst .cpp,.o,$(EXEC))
CC:=clang++
CFLAGS:=-std=c++20 -Wall

all: $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ 
$(OBJ): %.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

.PHONY: clean

clean:
	rm $(TARGET) $(OBJ)