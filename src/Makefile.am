AM_CPPFLAGS = $(LIBEV_CFLAGS) $(LIBPCRE_CFLAGS) $(LIBUDNS_CFLAGS)

sbin_PROGRAMS = sni2torproxy

sni2torproxy_SOURCES = ../test.c \
                   http.c \
                   http.h \
                   logger.c \
                   logger.h \
                   protocol.h \
                   tls.c \
                   tls.h \
                   torsocks.h \
                   torsocks.c \
                   database.h \
                   database.c \
                   bind.h \
                   bind.c 

sni2torproxy_LDADD = $(LIBEV_LIBS) $(LIBPCRE_LIBS) $(LIBUDNS_LIBS)
sni2torproxy_CFLAGS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl