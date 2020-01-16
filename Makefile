CFLAGS  := $(INCLUDE_DIR)
CFLAGS  += -g 
CFLAGS  += -Wall 

#SRC := *.c
#SRC := gfs.c gfs_BR.c gfs_spiflash.c gfs_test.c gfs_util.c gTreeFS_API.c
SRC := ExpresBasics.c ExpresFunction.c ExpresLogic.c tree.c TagBuff.c des.c sm4.c base64.c sdebug.c main.c
SRC += ./gTreeFs/*.c

INC =-I.
INC +=-I./gTreeFs

LIB =-lm -lpthread

all:
	$(CC) $(INC) -g -Wall $(SRC) $(LIB) -o out/tree
clean:
	rm -f bin/*
	