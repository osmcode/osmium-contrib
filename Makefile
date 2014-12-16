
PROJECTS := `find . -mindepth 2 -name Makefile | xargs dirname | cut -c3-`

all:
	for i in $(PROJECTS); do \
	    $(MAKE) -C $$i; \
	done

clean:
	for i in $(PROJECTS); do \
	    $(MAKE) -C $$i clean; \
	done

distclean:
	for i in $(PROJECTS); do \
	    rm -fr $$i/build; \
	done

