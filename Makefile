.NOTPARALLEL:

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
RELEASE_OUT=../out/release
#Apps get installed on 'make install'
ORLY_APPS=orly/orlyc orly/server/orlyi orly/spa/spa orly/client/orly_client orly/indy/disk/util/orly_dm
#Utils are simply things we like making sure still build
ORLY_UTIL=starsha/starsha orly/core_import
ORLY_DATA_IMPORTERS=$(addprefix orly/data/,beer complete_graph game_of_thrones money_laundering belgian_beer 				   \
		friends_of_friends matrix shakespeare)

ORLY_MAIN_TARGETS=$(ORLY_APPS) $(ORLY_UTIL) $(ORLY_DATA_IMPORTERS)
STARSHA=starsha $(STARSHA_FLAGS)

.PHONY: apps release test test_build test_lang clean install

apps: tools/starsha
	$(STARSHA) $(ORLY_MAIN_TARGETS)

release: tools/starsha
	$(STARSHA) --config=release $(ORLY_MAIN_TARGETS)

tools/starsha:
	./bootstrap.sh

test: apps
	$(STARSHA) --all="*.test" --test

test_build: apps
	$(STARSHA) --all="*.test"

test_lang: apps
	cd orly/lang_tests; ./run_tests.py

clean:
	rm -f ../.starsha/.notes
	rm -rf ../out/
	rm -f tools/starsha

install: release
	cd $(RELEASE_OUT); install -m755 -t $(BINDIR) $(ORLY_APPS)