CFLAGS = -Wall

xfse: xfse.c xfse.h
	$(CC) $(CFLAGS) xfse.c -o xfse

run: xfse
	./xfse	