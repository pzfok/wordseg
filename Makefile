all:
	g++ -o seg test.cpp tree.cpp symbol.cpp seg.cpp hmm.cpp ../commom/func.cpp -D_FILE_OFFSET_BITS=64
	g++ -fPIC -o wordseg.so wordseg.cpp tree.cpp symbol.cpp seg.cpp hmm.cpp ../commom/func.cpp ../commom/func.h -shared -I /home/seanxywang/app/include/python2.7/ -I /home/seanxywang/app/lib/python2.7/config/ -L /home/seanxywang/app/lib/python2.7/config/
clean:
	$(RM) -rf seg wordseg.so
