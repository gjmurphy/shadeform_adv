CC     = g++
CFLAGS = -g
TARGET = ExampleProject
OBJS   = main.o shdefrm_adv.o shdefrm_tools.o shdefrm_login.o shdefrm_save.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

shdefrm_adv.o: shdefrm_adv.cpp
	$(CC) $(CFLAGS) -c shdefrm_adv.cpp

shdefrm_tools.o: shdefrm_tools.cpp
	$(CC) $(CFLAGS) -c shdefrm_tools.cpp

shdefrm_login.o: shdefrm_login.cpp
	$(CC) $(CFLAGS) -c shdefrm_login.cpp

shdefrm_save.o: shdefrm_save.cpp
	$(CC) $(CFLAGS) -c shdefrm_save.cpp

clean:
	/bin/rm -f *.o $(TARGET)
