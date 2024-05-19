PLATFORM ?= PLATFORM_DESKTOP # one of [PLATFORM_DESKTOP]
BUILD_MODE ?= DEBUG # one of [DEBUG, RELEASE]
RAYLIB_INCLUDE_PATH ?= third_party/raylib-5.0_macos/include
RAYLIB_LIB_PATH ?= third_party/raylib-5.0_macos/lib/libraylib.a

SRC_DIR = src
BUILD_DIR = build
EXECUTABLE = $(BUILD_DIR)/main

CC = clang
CFLAGS = -c -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result
# BUILD_MODE is DEBUG
ifeq ($(BUILD_MODE), DEBUG)
	CFLAGS += -g -O0 -D_DEBUG
else
	CFLAGS += -O2
endif

INCLUDES = -Isrc/include -I$(RAYLIB_INCLUDE_PATH)

LINKER = clang
LFLAGS = -Wall
LIBS = $(RAYLIB_LIB_PATH) -framework OpenGL -framework OpenAL -framework IOKit -framework CoreVideo -framework Cocoa

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean

all: $(EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(DEPS) $(EXECUTABLE)

# Define the build rules
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(@D)
	$(LINKER) $(LFLAGS) $(OBJECTS) $(LIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ -D$(PLATFORM)

# Generate dependencies
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -MM -MT $(@:.d=.o) $< -MF $@ -D$(PLATFORM)

# Include the dependencies
-include $(DEPS)