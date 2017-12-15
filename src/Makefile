TARGET = miniportal
SRC = $(wildcard *.c *.cpp)
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC)))
CC = mipsel-openwrt-linux-uclibc-gcc
CPP = mipsel-openwrt-linux-uclibc-g++
INCLUDE = -I.
LINKPARAM = -lpthread -L.
CFLAGS = -std=gnu99

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.cpp
	$(CPP) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(LINKPARAM) $(OBJS)

clean:
	rm $(OBJS) $(TARGET)
