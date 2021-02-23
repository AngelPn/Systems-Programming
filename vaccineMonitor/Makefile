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
TARGETS = main

_DEPS = ADTList.h citizenRecord.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJS = $(MDIR)/ADTList.o $(SRC)/citizenRecord.o $(SRC)/main.o 
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))


$(ODIR)/%.o: %.c $(DEPS)
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(TARGETS)

main: $(OBJS)
	$(CC) -o $@ $^ -no-pie $(LIBS)

clean: 
	rm -f $(ODIR)/*.o $(BDIR)/*
	rmdir $(ODIR) $(BDIR)