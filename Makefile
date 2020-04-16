# INSTALL supported only on freebsd

build: clean
	mkdir build
	cc qirimonit.c helper.c outputs.c metrics.c -o build/qirimonit

.PHONY: clean
clean:
	rm -rf build

DATA_PATH ?= /usr/local/share/qirimonit/data/
install: build
	mkdir -p $(DATA_PATH)
	cp configs/rc.d_freebsd /etc/rc.d/qirimonit
	sed -i -e 's,_qirimonit_data_file_,$(DATA_PATH)/load_avg.csv,g' /etc/rc.d/qirimonit
	cp build/qirimonit /usr/local/bin/qirimonit
	if [ -z $(grep "qirimonit_enabled=\"YES\"" "/etc/rc.conf") ]; then echo "qirimonit_enabled=\"YES\"" >> /etc/rc.conf; fi
	service qirimonit start

uninstall:
	service qirimonit stop
	sed -i -e '/qirimonit_enabled="YES"/d' /etc/rc.conf
	rm -rf $(DATA_PATH)
	rm -rf /etc/rc.d/qirimonit
	rm -rf /usr/local/bin/qirimonit
