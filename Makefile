
PROJECTS := `find . -mindepth 2 -name Makefile | xargs dirname | cut -c3-`

all:
	for i in $(PROJECTS); do \
	    make -C $$i; \
	done

clean:
	for i in $(PROJECTS); do \
	    make -C $$i clean; \
	done

