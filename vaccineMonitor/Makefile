#paths
SRC = src
MODULES = mylib
ODIR = build
RDIR = $(SRC)/relations
TDIR = $(SRC)/tools

#compiler
CC = gcc

#executable's name
EXEC = vaccineMonitor

#compile options
CFLAGS = -Wall -g -I$(MODULES)/LinkedList -I$(MODULES)/SkipList -I$(MODULES)/HashTable 
CFLAGS += -I$(MODULES)/BloomFilter -I$(MODULES)/Date -I$(RDIR) -I$(TDIR)

OBJS =  $(SRC)/main.o
OBJS += $(RDIR)/citizenRecord.o $(RDIR)/virus.o $(RDIR)/country.o
OBJS += $(TDIR)/dataStore.o $(TDIR)/utils.o
OBJS += $(MODULES)/BloomFilter/BloomFilter.o
OBJS += $(MODULES)/Date/date.o
OBJS += $(MODULES)/HashTable/HashTable.o
OBJS += $(MODULES)/LinkedList/List.o
OBJS += $(MODULES)/SkipList/SkipList.o

$(EXEC): clean $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) -lm
	mkdir -p $(ODIR)
	mv $(OBJS) $(ODIR)

# delete executable & object files
clean:
	rm -f $(EXEC)
	rm -rf $(ODIR)