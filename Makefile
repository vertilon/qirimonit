build: clean
	mkdir build
	cc qirimonit.c helper.c outputs.c metrics.c -o build/qirimonit

.PHONY: clean
clean:
	rm -rf build
