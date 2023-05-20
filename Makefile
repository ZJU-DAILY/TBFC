MSG = update

clean:
	rm -fr build && rm -fr bin

debug-build: clean
	mkdir -p bin && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j

build: clean
	mkdir -p bin && mkdir build && cd build && cmake .. && make -j

git-push:
	git add . && git commit -m "$(MSG)" && git push