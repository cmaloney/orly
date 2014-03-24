PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
RELEASE_OUT=../out/release
#Apps get installed on 'make install'
STIG_APPS=stig/stigc stig/server/stigi stig/spa/spa stig/client/stig_client stig/indy/disk/util/stig_dm
#Utils are simply things we like making sure still build
STIG_UTIL=starsha/starsha starsha/dummy stig/core_import

.PHONY: test apps clean

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
	cd stig/lang_tests; ./run_tests.py

clean:
	rm -f ../.starsha/.notes
	rm -rf ../out/
	rm -f tools/starsha

install: release
	cd $(RELEASE_OUT); install -m755 -t $(BINDIR) $(STIG_APPS)