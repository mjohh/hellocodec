#.c<->.o
#.SUFFIXS:.c .o

CUR_DIR=$(shell pwd)
SRC_DIR=$(CUR_DIR)/src
INCLUDES_DIR=$(CUR_DIR)/include
CFLAGS=-I$(INCLUDES_DIR) -Wall -g -std=c99
VPATH=$(SRC_DIR)
OBJ_DIR=$(CUR_DIR)/obj

CC=gcc
SRCS=$(wildcard $(SRC_DIR)/*.c)
	
_OBJS=$(SRCS:.c=.o)
OBJS=$(subst $(SRC_DIR),$(OBJ_DIR),$(_OBJS))

EXEC=test

start: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS)
	@echo 'Done!'


$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c
	$(CC) -o $@ -c $(CFLAGS) $<

clean:
	rm -f $(OBJS)
