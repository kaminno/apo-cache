CFLAGS+= -mssse3 -g -O1 -Wall -Werror -std=c99 -lm -lrt
CC=gcc
FILE=du_cache
ZIP=zip

all: $(FILE)

$(FILE): main.c
	$(CC) $(CFLAGS) main.c -o $(FILE)

dozip:
	$(ZIP) $(FILE)_ernee main.c
