#CC := g++
CC := clang++
# CFLAGS := -g -Wall -std=c++11 -DDEBUG
CFLAGS := -g -Wall -std=c++11

SRCDIR := src
BUILDDIR := build
BINDIR := bin

TARGET := $(BINDIR)/chrgfx
INC := -I inc

SRCEXT := cpp
SRC := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJ := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRC:.$(SRCEXT)=.o))

$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) -L/usr/lib/libpng.so -lpng $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC)  -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR)/* $(TARGET)

init:
	mkdir $(BUILDDIR)
	mkdir $(BINDIR)

