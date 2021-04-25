#paths
SRC = src
MODULES = mylib
ODIR = build
RDIR = $(SRC)/relations
TDIR = $(SRC)/tools
MNTR = monitor
MTDIR = $(MNTR)/tools

#compiler
CC = gcc

#executable's name
EXEC = travelMonitor
EXEC_MONITOR = processMonitor

#compile options
CFLAGS = -Wall -g -I$(MODULES)/LinkedList -I$(MODULES)/SkipList -I$(MODULES)/HashTable 
CFLAGS += -I$(MODULES)/BloomFilter -I$(MODULES)/Date -I$(TDIR) -I$(RDIR) -I$(MTDIR)

COMMON_OBJS = $(MODULES)/BloomFilter/BloomFilter.o $(MODULES)/Date/date.o $(MODULES)/LinkedList/List.o
COMMON_OBJS += $(MODULES)/HashTable/HashTable.o $(MODULES)/SkipList/SkipList.o

#src .o
OBJS = $(SRC)/main.o $(RDIR)/monitor.o $(TDIR)/utils.o $(TDIR)/ipc.o

#monitor .o
MNTR_OBJS = $(MNTR)/monitor_main.o $(MTDIR)/signal_functions.o

all: $(EXEC) $(EXEC_MONITOR)
	mkdir -p $(ODIR)
	mv -f $(COMMON_OBJS) $(ODIR)
	mv -f $(OBJS) $(ODIR)
	mv -f $(MNTR_OBJS) $(ODIR)

$(EXEC): clean $(OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(OBJS) -o $(EXEC) -lm

$(EXEC_MONITOR): clean $(MNTR_OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(MNTR_OBJS) -o $(EXEC_MONITOR) -lm

# delete executable & object files
clean:
	rm -f $(EXEC)
	rm -f $(EXEC_MONITOR)
	rm -rf $(ODIR)
	rm -rf temp