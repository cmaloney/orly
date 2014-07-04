.NOTPARALLEL:

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
PACKAGE_DIR=$(PREFIX)/packages
DATA_DIR=$(PREFIX)/data
RELEASE_OUT=../out/release/
#Apps get installed on 'make install'
ORLY_APPS=orly/orlyc orly/server/orlyi orly/spa/spa orly/client/orly_client orly/indy/disk/util/orly_dm orly/core_import
ORLY_DATASET_GEN=beer complete_graph game_of_thrones money_laundering belgian_beer friends_of_friends matrix shakespeare social_graph twitter twitter_ego
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
	install -d $(BINDIR) $(PACKAGE_DIR) $(DATA_DIR)
	install -m755 -t $(BINDIR) $(addprefix $(RELEASE_OUT), $(ORLY_APPS))
	install -m755 -t $(BINDIR) $(addprefix $(RELEASE_OUT), $(addprefix orly/data/,$(ORLY_DATASET_GEN)))
	#Mark the root of the package repository
	touch $(PACKAGE_DIR)/__orly__
	install -m644 -t $(PACKAGE_DIR) $(wildcard orly/data/*.orly)
