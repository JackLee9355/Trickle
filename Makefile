trickle:
	g++ bencode/*.cpp torrent/*.cpp trickle.cpp -I . -lcrypto -o trickle

clean:
	rm trickle