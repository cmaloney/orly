PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
RELEASE_OUT=../out/release
#Apps get installed on 'make install'
STIG_APPS=orly/orlyc orly/server/orlyi orly/spa/spa orly/client/orly_client orly/indy/disk/util/orly_dm
#Utils are simply things we like making sure still build
STIG_UTIL=starsha/starsha starsha/dummy orly/core_import

.PHONY: apps release test test_build test_lang clean install

apps: tools/starsha
	starsha $(STIG_APPS) $(STIG_UTIL)

release: tools/starsha
	starsha $(STIG_APPS) $(STIG_UTIL) --config=release

tools/starsha:
	./bootstrap.sh

test: apps
	starsha --all="*.test" --test

test_build: apps
	starsha --all="*.test"

test_lang: apps
	cd orly/lang_tests; ./run_tests.py

clean:
	rm -f ../.starsha/.notes
	rm -rf ../out/
	rm -f tools/starsha

install: release
	cd $(RELEASE_OUT); install -m755 -t $(BINDIR) $(STIG_APPS)