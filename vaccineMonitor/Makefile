#paths
IDIR = include
SRC = src
MDIR = modules
ODIR = bin

#compiler
CC = gcc

#compile options
CFLAGS = -Wall -g -I$(IDIR)
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
TARGETS = vaccineMonitor

_DEPS = List.h citizenRecord.h utils.h date.h HashTable.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJS = $(MDIR)/List.o $(MDIR)/HashTable.o $(SRC)/citizenRecord.o $(SRC)/main.o $(SRC)/utils.o $(SRC)/date.o 
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))


$(ODIR)/%.o: %.c $(DEPS)
	@mkdir -p $(ODIR)/$(SRC)
	@mkdir -p $(ODIR)/$(MDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(TARGETS)

vaccineMonitor: $(OBJS)
	$(CC) -o $@ $^

clean: 
	rm -f $(ODIR)/$(SRC)/*.o $(ODIR)/$(MDIR)/*.o vaccineMonitor
	rmdir $(ODIR)/$(SRC) $(ODIR)/$(MDIR) $(ODIR)