PROGRAM := finaltest.js
TABLEDIR := bin/waketable
EXECUTABLE := false

OBJECTDIR := bin/wakeobj
SRCDIR := src/wake/test
SOURCEFILES := $(wildcard $(SRCDIR)/*.wk)
DEPFILES := ${SOURCEFILES:.wk=.d}
OBJECTFILES := $(subst $(SRCDIR),$(OBJECTDIR),${SOURCEFILES:.wk=.o})
TABLEFILES := $(subst $(SRCDIR),$(TABLEDIR),${SOURCEFILES:.wk=.table})
LIBRARYFILES := bin/wakeobj/std.o bin/wakeobj/UndefinedIndexException.o bin/wakeobj/System.o
WUNITLIBRARYROOT := ../wUnit/bin/wakeobj
WUNITTABLEROOT := ../wUnit/bin/waketable
TESTLIBRARYFILES := $(WUNITLIBRARYROOT)/TestResultReporter.o $(WUNITLIBRARYROOT)/Asserts.o
LIBRARYTABLES := $(WUNITTABLEROOT)/Asserts.table $(WUNITTABLEROOT)/TestResultReporter.table

bin/$(PROGRAM): $(OBJECTFILES) $(TABLEFILES) $(LIBRARYFILES) tests
ifeq ($(EXECUTABLE), true)
	bin/wake -l -d $(TABLEDIR) -o bin/$(PROGRAM) $(OBJECTFILES) $(LIBRARYFILES)
endif

.PHONY:
tests: bin/$(PROGRAM)-test
	node bin/$(PROGRAM)-test

bin/$(PROGRAM)-test: $(OBJECTFILES) $(TESTLIBRARYFILES) $(LIBRARYFILES) $(TABLEFILES) bin/waketable/System.table
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
