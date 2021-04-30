#
# pep8 top level Makefile
#

# export GPIO = No

DIRS := src ttysrv doc # scope is java, don't include
CLEANDIRS=$(addsuffix .clean,$(DIRS))

.PHONY: all $(DIRS) $(CLEANDIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) --directory=$@

clean: $(CLEANDIRS)

$(CLEANDIRS):
	$(MAKE) -C $(basename $@) clean 

install:
	$(MAKE) --directory=src install
	$(MAKE) --directory=ttysrv install

# eof

