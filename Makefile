# Main Makefile for Hoochamacallit System

# Directories
BIN_DIR = bin
DR_DIR = DR
DC_DIR = DC
DX_DIR = DX
INCLUDE_DIR = include

# Make sure the directories exist
$(shell mkdir -p $(BIN_DIR) $(DC_DIR)/$(BIN_DIR))

# Targets
all: dr dc dx

dr:
	$(MAKE) -C $(DR_DIR)
	cp $(DR_DIR)/$(BIN_DIR)/DataReader $(BIN_DIR)/

dc:
	$(MAKE) -C $(DC_DIR)

dx:
	$(MAKE) -C $(DX_DIR)
	cp $(DX_DIR)/$(BIN_DIR)/DX $(BIN_DIR)/

# Clean up
clean:
	$(MAKE) -C $(DR_DIR) clean
	$(MAKE) -C $(DC_DIR) clean
	$(MAKE) -C $(DX_DIR) clean
	rm -f $(BIN_DIR)/*

.PHONY: all dr dc dx clean