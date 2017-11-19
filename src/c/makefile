
# ---------------------------------------------------------------------------- #
CC	= cc
INCLUDE	= $HOME/include
LIB	= $HOME/lib
BIN	= $HOME/bin
LINK	= -lm -lc
FLAG 	=  -O -s
# ---------------------------------------------------------------------------- #

.c.o:
	$(CC) $(FLAG) -I$(INCLUDE) -c $<

# ---------------------------------------------------------------------------- #

all:	fserv fcli

FSERV	=	ftrsd.o ftrfsnd.o ftrtcpfsnd.o ftrlsdir.o ftrconf.o ftrserv.o ftrlog.o ftrconn.o  ftrcomm.o fileproc.o ftrcfg.o
fserv:	$(FSERV)
		$(CC) $(FLAG) $(FSERV) $(LINK) -o fserv

FCLI	=	ftrclient.o ftrfsnd.o ftrtcpfsnd.o ftrconf.o ftrlog.o ftrconn.o ftrcomm.o ftrcfg.o
fcli:		$(FCLI)
		$(CC) $(FLAG) $(FCLI) $(LINK) -o fcli
		cp ./fcli $(PRJDIR)/bin


clean:
	rm -f *.o
	rm -f fcli
	rm -f fserv

