# Test program for MIDI scales 
# @Author: Jan-Willem Smaal 
# @Date: 3/9/2020 

CC=c++
CFLAGS=-Wall -Wc++11-extensions  
DEPS = midi-scales.h
OBJ = midi-scales-testprogram.o midi-scales.o 


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

midi-scales-testprogram: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~ a.out *~ 

# EOF 
