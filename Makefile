CPPFLAGS = -std=c++20 -g -O0 -Wall -Wextra -Wpedantic -Isrc/
LDFLAGS = -lSDL2 -lSDL2_image

BUILD_DIR = build

TARGET = $(BUILD_DIR)/minesweeper

SOURCE_FILES = $(shell find src/ -name '*.cpp')
OBJECT_FILES = $(SOURCE_FILES:src/%.cpp=$(BUILD_DIR)/%.o)
DEPEND_FILES = $(OBJECT_FILES:.o=.d)

.PHONY: clean run

$(TARGET): $(OBJECT_FILES)
	@mkdir -p $(@D)
	g++ $(OBJECT_FILES) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	g++ $(CPPFLAGS) -MMD -MP $< -c -o $@

clean:
	rm -r $(BUILD_DIR)

run: $(TARGET)
	$(TARGET)

-include $(DEPEND_FILES)
