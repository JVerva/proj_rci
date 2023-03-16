CC = gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES = contact.o main.o
TARGET = cot

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~