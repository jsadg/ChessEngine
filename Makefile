# Compiler and flags
CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -O2

# Source and object files
SRCS = main.c attacks.c search.c movegen.c board.c bitboard.c io.c magics.c
OBJDIR = build
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

# Output executable
TARGET = chess

# Default rule
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compile each .c file into build directory
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Run program
run: $(TARGET)
	./$(TARGET)
