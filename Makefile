bot: bot.c
	gcc bot.c -o bot -lini -ldyad -I. -I ini/include -I dyad/include -L. -L ini/lib -L dyad/lib -g
all: libini.a libdyad.a bot

libini.a: ini/src/ini.c ini/include/ini.h
	gcc -c ini/src/ini.c -o ini/lib/ini.o -g
	ar rcs ini/lib/libini.a ini/lib/ini.o

libdyad.a: dyad/src/dyad.c dyad/include/dyad.h
	gcc -c dyad/src/dyad.c -o dyad/lib/dyad.o -g
	ar rcs dyad/lib/libdyad.a dyad/lib/dyad.o

clean:
	rm bot
	rm dyad/lib/dyad.o
	rm dyad/lib/libdyad.a
	rm ini/lib/ini.o
	rm ini/lib/libini.a
