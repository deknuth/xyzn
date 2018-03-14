CC = gcc
XX = g++
CFLAGS = -Wall -O -I/usr/include  -L/usr/lib 
CTD = -I$(ORACLE_INCLUDES)  -L$(ORACLE_LIBS)
TARGET = auto
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 
%.o:%.cpp
	$(XX) $(CFLAGS) -c $< -o $@ 
SOURCES = $(wildcard *.c ./src/*.c)
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))
$(TARGET) : $(OBJS)
	$(CC) $(OBJS) $(CTD) -o $(TARGET) -lpthread  -lm -g -rdynamic -lcontrolcan
	chmod a+x $(TARGET)
clean:
	rm -rf $(wildcard auto *.log *.o ./src/*.o)
