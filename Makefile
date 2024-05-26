# Makefile
#
# Primary instructions on creating the target executable.
#
# The -print versions include all printed game details and decisions.
#   They are not included by default because it skews the total computation metrics.
#

.PHONY: default default-print clean release release-print

CC = gcc
CFLAGS = -Wall

SRCS = main.c queue.c list.c
OBJS = $(SRCS:.c=.o)

TARGET = fourknights


default:
	$(MAKE) clean
	$(MAKE) $(TARGET)

default-print:
	$(MAKE) clean
	$(MAKE) default-sub-print

default-sub-print: CFLAGS += -DFN_DEBUG=1
default-sub-print: $(TARGET)

release:
	$(MAKE) clean
	$(MAKE) release-sub

release-print:
	$(MAKE) clean
	$(MAKE) release-sub-print

release-sub: CFLAGS += -O3
release-sub: $(TARGET)

release-sub-print: CFLAGS += -O3 -DFN_DEBUG=1
release-sub-print: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:
	rm -f $(OBJS) $(TARGET)
