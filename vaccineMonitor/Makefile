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
CFLAGS = -Wall -g -I$(RDIR) -I$(TDIR)
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes

OBJS =  $(SRC)/main.o
OBJS += $(RDIR)/citizenRecord.o $(RDIR)/virus.o $(RDIR)/country.o
OBJS += $(TDIR)/dataStore.o $(TDIR)/utils.o
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