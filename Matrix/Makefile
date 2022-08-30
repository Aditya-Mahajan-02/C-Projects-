EXEC = tsp
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wextra
LDFLAGS = -lm


all: $(EXEC)

$(EXEC) : $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(FLAGS) -c $<

tidy:
	rm -rf $(OBJECTS)

clean: tidy
	rm -rf $(EXEC)

format:
	clang-format -i -style=file *.[ch]
