CC = gcc
CFLAGS = -Wall -Wextra -std=c11

OBJS = main.o hardware.o so.o programs.o utils.o mem_mngr.o

all: vm_so

vm_so: $(OBJS)
	$(CC) $(CFLAGS) -o vm_so $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) vm_so
