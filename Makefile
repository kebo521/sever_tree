CFLAGS  := $(INCLUDE_DIR)
CFLAGS  += -g 
CFLAGS  += -Wall 

#SRC := *.c
#SRC := gfs.c gfs_BR.c gfs_spiflash.c gfs_test.c gfs_util.c gTreeFS_API.c
//SRC := ExpresBasics.c ExpresFunction.c ExpresLogic.c tree.c TagBuff.c des.c sm4.c base64.c sdebug.c main.c
SRC := ./exp/*.c
SRC += ./gTreeFs/*.c
SRC += ./wallet/*.c

INC =-I.
INC +=-I./exp
INC +=-I./gTreeFs
INC +=-I./wallet

LIB =-lm -lpthread

all:
	$(CC) $(INC) -g -Wall $(SRC) $(LIB) -o run/gtree
clean:
	rm -f bin/*
	