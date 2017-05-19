TARGET=a.out

CC=g++
OPT=
INC=
LIB=

-include makefile.opt

.SUFFIXES:.cc .c .o .h

SRC=$(shell ls *.cc)
HED=$(shell ls *.h)
OBJ=$(SRC:.cc=.o)

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(OPT) -o $(TARGET) $(OBJ) $(LIB)

.c.o:
	$(CC) $(OPT) -c $< $(INC)
.cc.o:
	$(CC) $(OPT) -c $< $(INC)

depend:
	g++ -MM -MG $(SRC) >makefile.depend

clean:
	rm -f $(TARGET) $(TARGET).exe
	rm -f *.o *.obj
	rm -f *~ *.~*

tar:
	tar cvzf $(TARGET).tar.gz $(SRC) $(HED) makefile

-include makefile.depend
