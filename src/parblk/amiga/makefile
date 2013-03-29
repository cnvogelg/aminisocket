# makefile - master make file for aminisocket project

SUBDIRS=device test

all:
	@for subdir in $(SUBDIRS) ; do \
		$(MAKE) -C $$subdir all || exit 1 ;\
	done

install:
	@for subdir in $(SUBDIRS) ; do \
		$(MAKE) -C $$subdir install || exit 1 ;\
	done

clean:
	@for subdir in $(SUBDIRS) ; do \
		$(MAKE) -C $$subdir clean || exit 1 ;\
	done
