.NOTPARALLEL:

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
RELEASE_OUT=../out/release
#Apps get installed on 'make install'
ORLY_APPS=orly/orlyc orly/server/orlyi orly/spa/spa orly/client/orly_client orly/indy/disk/util/orly_dm
#Utils are simply things we like making sure still build
ORLY_UTIL=starsha/starsha orly/core_import

.PHONY: apps release test test_build test_lang clean install

apps: tools/starsha
	starsha $(STARSHA_FLAGS) $(ORLY_APPS) $(ORLY_UTIL)

release: tools/starsha
	starsha $(STARSHA_FLAGS) $(ORLY_APPS) $(ORLY_UTIL) --config=release

tools/starsha:
	./bootstrap.sh

test: apps
	starsha $(STARSHA_FLAGS) --all="*.test" --test

test_build: apps
	starsha $(STARSHA_FLAGS) --all="*.test"

test_lang: apps
	cd orly/lang_tests; ./run_tests.py

clean:
	rm -f ../.starsha/.notes
	rm -rf ../out/
	rm -f tools/starsha

install: release
	cd $(RELEASE_OUT); install -m755 -t $(BINDIR) $(ORLY_APPS)