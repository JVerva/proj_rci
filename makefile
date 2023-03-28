CC = gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES = names.o routing.o contact.o utils.o node_protocols.o cmds.o main.o
TARGET = cot

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~