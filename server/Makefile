TARGET = main
INC=../protocol
INC2=../public
INC3=../list
INC4=../file
INC5=../client

CC = gcc

OBJS= smain.o  server.o $(INC)/fileProtocol.o $(INC)/chatProtocol.o $(INC2)/public.o $(INC3)/usrlist.o $(INC3)/fileList.o $(INC4)/file.o

INCLUDES=-I$(INC) -I$(INC2) -I$(INC3) -I$(INC4) -I$(INC5)
CFLAGS= -pthread

all:$(TARGET)

%.o:%.c
#	$(CC) -c $(INCLUDES) $< -o $@
	$(CC) -c $(INCLUDES) $(CFLAGS) $< -o $@

$(TARGET):$(OBJS)
#	$(CC) -o $@ $^ $(LDFLAGS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

clean:
	rm *.o $(TARGET)  $(INC)/fileProtocol.o 
	       $(INC)/chatProtocol.o $(INC2)/public.o 
		   $(INC3)/usrlist.o $(INC3)/fileList.o $(INC4)/file.o 
