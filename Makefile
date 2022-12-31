trickle: $(wildcard res/**/*)
	g++ **/*.cpp *.cpp -I . -lcrypto -o trickle

clean:
	rm trickle