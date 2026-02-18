CC ?= cc
CFLAGS ?= -std=c11 -O2 -g -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Isrc
LDFLAGS ?=

SRC := $(shell find src -name "*.c")
OBJ := $(SRC:.c=.o)
BIN := hosp_mvp

.PHONY: build run clean

build: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

run: build
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
