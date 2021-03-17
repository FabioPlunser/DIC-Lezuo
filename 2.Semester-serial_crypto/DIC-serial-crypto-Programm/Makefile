BOARD_VAR=native_posix_64


# Setup Code specific Paths
PRJ_DIR=code/
ABS_PRJ_DIR=$(abspath $(PRJ_DIR))
SOURCE_DIR=src/
# Create List of Files that impact the Executable
SOURCE_FILES=$(wildcard $(PRJ_DIR)/*.*) $(wildcard $(PRJ_DIR)/*/*.*)

# Setup Code specific Paths
PRJ_DIR=code/
ABS_PRJ_DIR=$(abspath $(PRJ_DIR))
SOURCE_DIR=src/
# Create List of Files that impact the Executable
SOURCE_FILES=$(wildcard $(PRJ_DIR)/*.*) $(wildcard $(PRJ_DIR)/*/*.*)

# Setup Zephyr specific Paths
ZEPHYR_HOME=~/zephyrproject/zephyr/
BUILD_DIR=$(ZEPHYR_HOME)build
EXEC_OUT=$(ZEPHYR_HOME)build/zephyr/zephyr.elf


# Setup Zephyr specific Paths
ZEPHYR_HOME=~/zephyrproject/zephyr/
BUILD_DIR=$(ZEPHYR_HOME)build
EXEC_OUT=$(ZEPHYR_HOME)build/zephyr/zephyr.elf

# Build Executable
all: $(EXEC_OUT)
	@$(MAKE) -s -C $(ZEPHYR_HOME) build

# Create Executable from Source-Files
$(EXEC_OUT): $(SOURCE_FILES)
	@echo "Building Project"
	@cd $(ZEPHYR_HOME) && west build -p auto -b $(BOARD_VAR) $(ABS_PRJ_DIR)
	@echo "west build -p auto -b $(BOARD_VAR) $(ABS_PRJ_DIR)"



# Run Executable
run: $(EXEC_OUT)
	@echo "Running Project"
	@$(EXEC_OUT)

	
# Clean generated Files
clean:
	@echo "Cleaning Build-Directory"
	@rm -rf $(BUILD_DIR)