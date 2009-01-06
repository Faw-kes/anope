include ./Makefile.inc

MAKEARGS = 'CFLAGS=${CFLAGS}' 'CC=${CC}' 'ANOPELIBS=${ANOPELIBS}' \
	   'LDFLAGS=${LDFLAGS}' 'INSTDIR=${INSTDIR}' 'INSTALL=${INSTALL}' \
           'INCLUDEDIR=${INCLUDEDIR}' 'RM=${RM}' 'CP=${CP}' \
      	   'TOUCH=${TOUCH}' 'SHELL=${SHELL}' \
   	   'RUNGROUP=${RUNGROUP}' \
	   'PROFILE=${PROFILE}' 'SHARED=${SHARED}' \
	   'MODULEFLAGS=${MODULEFLAGS}' 'MAKEBIN=${MAKEBIN}'

OBJECTS= $(SRCS:.c=.o)
SO_FILES=$(OBJECTS:.o=.s)
CDEFS= -rdynamic -Wall

all: modules subs

modules: $(OBJECTS) $(SO_FILES)

install:
	$(CP) ./*.so $(INSTDIR)/data/modules

distclean: spotless

.c.o:
	@$(MAKEBIN) $(CC) ${CFLAGS} ${CDEFS} ${MODULEFLAGS} -I../${INCLUDEDIR} -c $<

.o.s:
	@$(MAKEBIN) $(CC) ${SHARED} ../mod_version.o $< -o $*.so ${PROFILE}
	@$(TOUCH) $*.s

subs:
	@for i in $(SUBS); do \
        echo "make all in $$i..."; \
        (cd $$i; $(MAKE) $(MAKEARGS) all); done

subs_clean:
	@for i in $(SUBS); do \
	echo "cleaning in $$i..."; \
	(cd $$i; $(MAKE) clean); done

clean: subs_clean
	rm -f *.o *.s *.so *.c~ core

spotless: subs_clean
	rm -f *.o *.s *.so *.c~ core Makefile.inc
