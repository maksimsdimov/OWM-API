CC = g++
CFLAGS = -std=c++14 -Wall -Werror -I/src/ -g 
DEPS = 
OBJ_DIR = obj
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))     
LIBS = -lpistache -pthread -lboost_system -lboost_iostreams

main: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o core *.core *.o temp.* *.db
