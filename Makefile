all: rgb-xyv rgb-hsv rgb-hsv rgb-gradient rgb-xyv-gradient rgb-xyv-bilinear

rgb-xyv : rgb-xyv.o libimage.o
	gcc -o  rgb-xyv rgb-xyv.o libimage.o -lm
rgb-xyv.o : rgb-xyv.c libimage.h
	gcc -Wall -c rgb-xyv.c 

rgb-hsv : rgb-hsv.o libimage.o
	gcc -o rgb-hsv rgb-hsv.o libimage.o 
rgb-hsv.o : rgb-hsv.c libimage.h
	gcc -Wall -c rgb-hsv.c

rgb-gradient : rgb-gradient.o libimage.o
	gcc -o rgb-gradient rgb-gradient.o libimage.o
rgb-gradient.o : rgb-gradient.c libimage.h
	gcc -Wall -c rgb-gradient.c

rgb-xyv-gradient : rgb-xyv-gradient.o libimage.o
	gcc -o rgb-xyv-gradient rgb-xyv-gradient.o libimage.o -lm
rgb-xyv-gradient.o : rgb-xyv-gradient.c libimage.h
	gcc -Wall -c rgb-xyv-gradient.c


rgb-xyv-bilinear : rgb-xyv-bilinear.o libimage.o
	gcc -o rgb-xyv-bilinear rgb-xyv-bilinear.o libimage.o -lm
rgb-xyv-bilinear.o : rgb-xyv-bilinear.c libimage.h
	gcc -Wall -c rgb-xyv-bilinear.c
	
		
libimage.o : libimage.c libimage.h
	gcc -Wall -c libimage.c            

clean:
	rm -f *.o core
