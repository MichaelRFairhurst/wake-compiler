PROGRAM := finaltest.js
TABLEDIR := bin/waketable
OBJECTDIR := bin/wakeobj
SRCDIR := src/wake/test
SOURCEFILES := $(wildcard $(SRCDIR)/*.wk)
DEPFILES := ${SOURCEFILES:.wk=.d}
OBJECTFILES := $(subst $(SRCDIR),$(OBJECTDIR),${SOURCEFILES:.wk=.o})
TABLEFILES := $(subst $(SRCDIR),$(TABLEDIR),${SOURCEFILES:.wk=.table})
LIBRARYFILES := bin/wakeobj/std.o
TESTLIBRARYFILES := ../wUnit/bin/wakeobj/TestResultReporter.o ../wUnit/bin/wakeobj/Asserts.o
LIBRARYTABLES := ../wUnit/bin/waketable/Asserts.table ../wUnit/bin/waketable/TestResultReporter.table

bin/$(PROGRAM): $(OBJECTFILES) $(TABLEFILES) $(LIBRARYFILES) tests
	bin/wake -l -d $(TABLEDIR) -o bin/$(PROGRAM) $(OBJECTFILES) $(LIBRARYFILES)

.PHONY:
tests: bin/$(PROGRAM)-test
	node bin/$(PROGRAM)-test

bin/$(PROGRAM)-test: $(OBJECTFILES) $(TESTLIBRARYFILES) $(LIBRARYFILES) $(TABLEFILES)
	wunit-compiler
	bin/wake bin/TestSuite.wk -d $(TABLEDIR) -o bin/TestSuite.o
	bin/wake -l -d $(TABLEDIR) $(OBJECTFILES) $(TESTLIBRARYFILES) $(LIBRARYFILES) bin/TestSuite.o -o bin/$(PROGRAM)-test -c TestSuite -m 'tests()'

to-md5 = $1 $(addsuffix .md5,$1)

%.md5: % FORCE
	@$(if $(filter-out $(shell cat $@ 2>/dev/null),$(shell md5sum $*)),md5sum $* > $@)

FORCE:

$(addprefix $(TABLEDIR)/,$(notdir $(LIBRARYTABLES))): $(LIBRARYTABLES)
	cp $(LIBRARYTABLES) $(TABLEDIR)

$(SRCDIR)/%.d: $(SRCDIR)/%.wk
	@./generate-makefile.sh $< $(TABLEDIR) > $@

$(TABLEDIR)/%.table: $(SRCDIR)/%.wk
	bin/wake $< -d $(TABLEDIR) -t

$(OBJECTDIR)/%.o: $(SRCDIR)/%.wk
	bin/wake $< -d $(TABLEDIR) -o $@

ifneq "$(MAKECMDGOALS)" "clean"
-include ${SOURCEFILES:.wk=.d}
endif

clean:
	rm $(TABLEFILES) || :
	rm $(DEPFILES) || :
	rm $(OBJECTFILES) || :
	rm bin/TestSuite.wk || :
	rm bin/TestSuite.o || :
	rm bin/$(PROGRAM) || :
	rm bin/$(PROGRAM)-test || :
	find . -name '*.md5' -delete
