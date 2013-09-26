PARAMSETS=433 577 769 1153

# Directories.
TOPDIR = ..
BINDIR = $(TOPDIR)/bin
DATADIR = $(TOPDIR)/data
DIRS = $(BINDIR) $(DATADIR)

vpath %.dat $(DATADIR)

CFLAGS += -c
CFLAGS += -Wall
CFLAGS += -O3 -ffast-math -mtune=native -malign-double

LDFLAGS = -lfftw3 -lm

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=$(TOPDIR)/bin/bench


all: dirs wisdom data $(SOURCES) $(EXECUTABLE)

dirs : $(DIRS)
	mkdir -p $?

# Ensure all FFTW wisdom files exist
wisdom: $(addprefix $(TOPDIR)/data/, $(addsuffix _wisdom.dat, $(PARAMSETS)))
$(TOPDIR)/data/%_wisdom.dat :
	$(BINDIR)/wiseup.sh $*

data: $(addprefix $(TOPDIR)/data/, $(addsuffix _rader.dat, $(PARAMSETS)))
data: $(addprefix $(TOPDIR)/data/, $(addsuffix _perm.dat, $(PARAMSETS)))
data: $(addprefix $(TOPDIR)/data/, $(addsuffix _points.dat, $(PARAMSETS)))
%_rader.dat %_points.dat %_perm.dat:
	$(warning Runtime data for set $* not present ($@). See README.)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean
clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)



