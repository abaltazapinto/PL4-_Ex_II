CC = gcc
CFLAGS = -O2 -Wall
TARGET = pl4_ex2_pwm_user

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

run:
	sudo ./$(TARGET)

run-arg:
	sudo ./$(TARGET) 128

clean:
	rm -f $(TARGET)