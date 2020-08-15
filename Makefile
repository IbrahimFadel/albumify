BIN=albumify
DEST_DIR=/usr/local/bin

CC=clang++

CPP_FLAGS=
LD_FLAGS=-lsfml-graphics -lsfml-window -lsfml-system -I/usr/include/python3.8 -L /usr/lib/x86_64-linux-gnu/ -lpython3.8 -lcurl

all:
		$(CC) $(CPP_FLAGS) $(LD_FLAGS) -o $(BIN) main.cpp

install:
	make
	install $(BIN) $(DEST_DIR)
	install albumify.py $(DEST_DIR)
