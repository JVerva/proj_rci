CC = gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES = contact.o node_protocols.o main.o cmds.o utils.o
TARGET = cot

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~