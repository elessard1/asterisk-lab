TARGET = lab.so
OBJECTS = lab.o
CC = gcc
CFLAGS = -Wall -Wextra -D'_GNU_SOURCE' -D'AST_MODULE="lab"' -g -fPIC
LDFLAGS = -Wall -shared

.PHONY: install clean

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

install: $(TARGET)
	cp $(TARGET) /usr/lib/asterisk/modules/

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
