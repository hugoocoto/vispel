CC = cc -fsanitize=address,null -std=gnu99
INC = -I.
LIB = $(wildcard src/*.h src/core/*.h) src/stb_ds.h
SRC = $(wildcard src/*.c src/core/*.c)
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
OBJ_DIR = ./objs
BUILD_DIR = ./build
OUT = $(BUILD_DIR)/vspli

test: $(OUT)
	./$(OUT) ./examples/test.vspl

$(OUT): $(LIB) $(OBJ) $(OBJ_DIR) $(BUILD_DIR) wc.md
	$(CC) $(OBJ) $(INC) -o $(OUT)

wc.md: $(SRC) $(LIB)
	cloc src --by-file --not-match-f='stb_ds\.h' --hide-rate --md > wc.md

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR)/%.o: %.c $(LIB)
	mkdir -p $(dir $@) && $(CC) -c $< $(INC) -o $@

clean:
	rm -rf $(OBJ_DIR)

install: $(OUT)
	mv $(OUT) ~/.local/bin/$(OUT)
	chmod +x ~/.local/bin/$(OUT)


src/stb_ds.h:
	wget https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_ds.h -P./src
