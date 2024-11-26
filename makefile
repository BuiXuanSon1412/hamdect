# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -g

# Linker flags
LDFLAGS = 

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Executable name
TARGET = $(BINDIR)/main

# Source files (located in /src)
SRCS = $(wildcard $(SRCDIR)/*.c)

# Object files (corresponding .o files stored in /obj)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Header files (located in /src)
HEADERS = $(wildcard $(SRCDIR)/*.h)

# Default target (all)
all: $(TARGET)

# Linking the executable, placing it in /bin
$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files, placing them in /obj
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

# Phony targets
.PHONY: clean all
