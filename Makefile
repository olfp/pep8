#
# pep8 top level Makefile
#

DIRS := src ttysrv doc # scope is java, don't include
CLEANDIRS=$(addsuffix .clean,$(DIRS))

.PHONY: all $(DIRS) $(CLEANDIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) --directory=$@

clean: $(CLEANDIRS)

$(CLEANDIRS):
	$(MAKE) -C $(basename $@) clean 

# eof

