# Main Makefile

# Default target to build everything
all: DC/bin/DataCreator DR/bin/DataReader DX/bin/DataCorruptor

# Targets to build each project
DC/bin/DataCreator:
	$(MAKE) -C DC

DR/bin/DataReader:
	$(MAKE) -C DR

DX/bin/DataCorruptor:
	$(MAKE) -C DX

# Clean up all projects
clean:
	$(MAKE) -C DC clean
	$(MAKE) -C DR clean
	$(MAKE) -C DX clean