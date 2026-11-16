# Driblando Zagueiros - Makefile (w64devkit no Windows, ou Linux)
TARGET = DriblandoZagueiros
SRC    = main.c $(wildcard src/*.c)
CFLAGS = -std=c99 -Wall -Wextra -O2 -Iinclude

ifeq ($(OS),Windows_NT)
    RAYLIB_PATH ?= C:/raylib/raylib/src
    CFLAGS  += -I$(RAYLIB_PATH)
    LDFLAGS  = -L$(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -static
    TARGET  := $(TARGET).exe
else
    LDFLAGS  = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

$(TARGET): $(SRC) $(wildcard include/*.h)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

teste: CFLAGS += -DDZ_TESTE
teste: $(TARGET)

clean:
	rm -f $(TARGET)
