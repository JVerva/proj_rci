CC = gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES =node_protocols.o names.o routing.o contact.o utils.o cmds.o main.o
TARGET = cot

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~