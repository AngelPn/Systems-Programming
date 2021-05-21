#paths
SRC = src
MODULES = mylib
ODIR = build
RDIR = $(SRC)/relations
TDIR = $(SRC)/tools
MNTR = monitor
MTDIR = $(MNTR)/tools
MRDIR = $(MNTR)/relations

#compiler
CC = gcc

#executable's name
EXEC = travelMonitorClient
EXEC_MONITOR = monitorServer

#compile options
CFLAGS = -Wall -g -I$(MODULES)/LinkedList -I$(MODULES)/SkipList -I$(MODULES)/HashTable -I$(MODULES)/IPC
CFLAGS += -I$(MODULES)/BloomFilter -I$(MODULES)/Date -I$(MODULES)/CyclicBuffer -I$(TDIR) -I$(RDIR) -I$(MTDIR) -I$(MRDIR)

COMMON_OBJS = $(MODULES)/BloomFilter/BloomFilter.o $(MODULES)/Date/date.o $(MODULES)/LinkedList/List.o
COMMON_OBJS += $(MODULES)/HashTable/HashTable.o $(MODULES)/SkipList/SkipList.o $(MODULES)/IPC/ipc.o

#src .o
OBJS = $(SRC)/main.o $(RDIR)/monitor.o $(RDIR)/virus_bloom.o $(RDIR)/country_requests.o
OBJS += $(TDIR)/utils.o $(TDIR)/queries.o

#monitor .o
MNTR_OBJS = $(MNTR)/monitor_main.o $(MTDIR)/dataStore.o $(MTDIR)/utils_queries.o
MNTR_OBJS += $(MRDIR)/country.o $(MRDIR)/virus.o $(MRDIR)/citizenRecord.o $(MODULES)/CyclicBuffer/CyclicBuffer.o

all: $(EXEC) $(EXEC_MONITOR)
	mkdir -p $(ODIR)
	mv -f $(COMMON_OBJS) $(ODIR)
	mv -f $(OBJS) $(ODIR)
	mv -f $(MNTR_OBJS) $(ODIR)

$(EXEC): clean $(OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(OBJS) -o $(EXEC) -lm -lpthread

$(EXEC_MONITOR): clean $(MNTR_OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(MNTR_OBJS) -o $(EXEC_MONITOR) -lm -lpthread

# delete executable & object files
clean:
	rm -f $(EXEC)
	rm -f $(EXEC_MONITOR)
	rm -rf $(ODIR)