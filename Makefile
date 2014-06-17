.NOTPARALLEL:

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
RELEASE_OUT=../out/release
#Apps get installed on 'make install'
ORLY_APPS=orly/orlyc orly/server/orlyi orly/spa/spa orly/client/orly_client orly/indy/disk/util/orly_dm

.PHONY: apps debug release bootstrap nycr test test_lang clean install

debug: bootstrap
	jhm

release: bootstrap
	jhm -c release

bootstrap: tools/jhm nycr

tools/jhm:
	./bootstrap.sh

nycr: tools/jhm
	jhm -c bootstrap

test: bootstrap
	jhm --test

test_lang: debug
	lang_test.py -d orly/data orly/lang_tests

clean:
	rm -rf ../out/
	rm -rf ../.jhm
	rm -f tools/jhm
	rm -f tools/make_dep_file

install: release
	cd $(RELEASE_OUT); install -m755 -t $(BINDIR) $(ORLY_APPS)
