all: bouncer.cc
	g++ -o bouncer -I ../include/ -L../lib bouncer.cc `pkg-config --cflags --libs libavutil libavformat libavcodec`
clean:
	rm bouncer
	rm *.o
	rm *.mp4
	rm *.utah
movie:
	ffmpeg -f image2 -i frame%d.utah movie.mp4