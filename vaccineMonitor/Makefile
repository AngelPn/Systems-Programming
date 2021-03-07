#paths
SRC = src
MODULES = mylib
ODIR = build
CORE = $(SRC)/core

#compiler
CC = gcc

#executable's name
EXEC = vaccineMonitor

#compile options
CFLAGS = -Wall -g -I$(CORE)
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes

OBJS =  $(SRC)/main.o
OBJS += $(CORE)/citizenRecord.o $(CORE)/utils.o $(CORE)/virus.o $(CORE)/country.o
include $(MODULES)/LinkedList/make.inc
include $(MODULES)/SkipList/make.inc
include $(MODULES)/HashTable/make.inc
include $(MODULES)/BloomFilter/make.inc
include $(MODULES)/Date/make.inc

$(EXEC): clean $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) -lm
	mkdir -p $(ODIR)
	mv $(OBJS) $(ODIR)

# delete executable & object files
clean:
	rm -f $(EXEC)
	rm -rf $(ODIR)

# #paths
# IDIR = include
# SRC = src
# MDIR = mylib
# ODIR = build

# CORE = $(SRC)/core

# #compiler
# CC = gcc

# #compile options
# CFLAGS = -Wall -g -I$(IDIR) -I$(CORE)
# VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
# TARGETS = vaccineMonitor

# _DEPS = List.h HashTable.h SkipList.h BloomFilter.h citizenRecord.h utils.h date.h virus.h common_types.h
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# _OBJS = $(MDIR)/List.o $(MDIR)/HashTable.o $(MDIR)/SkipList.o $(MDIR)/BloomFilter.o $(SRC)/citizenRecord.o $(SRC)/main.o $(SRC)/utils.o $(SRC)/date.o $(SRC)/virus.o
# OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))


# $(ODIR)/%.o: %.c $(DEPS)
# 	@mkdir -p $(ODIR)/$(SRC)
# 	@mkdir -p $(ODIR)/$(MDIR)
# 	$(CC) -c -o $@ $< $(CFLAGS)

# all: $(TARGETS)

# vaccineMonitor: $(OBJS)
# 	$(CC) -o $@ $^ -lm

# clean: 
# 	rm -f $(ODIR)/$(SRC)/*.o $(ODIR)/$(MDIR)/*.o vaccineMonitor
# 	rmdir $(ODIR)/$(SRC) $(ODIR)/$(MDIR) $(ODIR)