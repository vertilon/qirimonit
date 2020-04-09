build:
	mkdir build
	cc qirimonit.c -o build/qirimonit

.PHONY: clean
clean:
	rm -rf build
