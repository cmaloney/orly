.NOTPARALLEL:

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
RELEASE_OUT=../out/release
#Apps get installed on 'make install'
ORLY_APPS=orly/orlyc orly/server/orlyi orly/spa/spa orly/client/orly_client orly/indy/disk/util/orly_dm
JHM_CMD=jhm $(JHM_FLAGS)
.PHONY: apps debug release bootstrap nycr test test_lang clean install

debug: bootstrap
	$(JHM_CMD)

release: bootstrap
	$(JHM_CMD) -c release

bootstrap: tools/jhm nycr

tools/jhm:
	./bootstrap.sh

nycr: tools/jhm
	$(JHM_CMD) -c bootstrap

test: bootstrap
	$(JHM_CMD) --test

test_lang: debug
	lang_test.py -d orly/data orly/lang_tests

clean:
	rm -rf ../out/
	rm -rf ../.jhm
	rm -f tools/jhm
	rm -f tools/make_dep_file

install: release
	cd $(RELEASE_OUT); install -m755 -t $(BINDIR) $(ORLY_APPS)
