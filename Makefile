# Makefile for ud-0.5-utf8 

export CFLAGS
CC		:= gcc
WARN		:= -Wall -Wmissing-prototypes
CFLAGS		:= -O2 -fomit-frame-pointer $(machine)
LDFLAGS		:= -lcursesw
PROGR   	:= ud
BINDIR		:= /usr/local/bin
dictdir		:= /usr/local/share/dict
datadir		= dict
dictfiles 	:= enua.dict uacomp.dict    # add your own


all: ud.c
	$(CC) $(WARN) $(CFLAGS) $(LDFLAGS) ud.c -o $(PROGR)
			
install: all
	install $(PROGR) $(BINDIR)
	[ -d $(dictdir) ] || install -d $(dictdir)
	for file in $(dictfiles); do\
	    echo Installing $$file...;\
	    install -v -m 0644 $(datadir)/$$file $(dictdir) || break;\
	done
clean:
	rm ud


