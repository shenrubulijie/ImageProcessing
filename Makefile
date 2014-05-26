#Copyright 2014-5 sxniu

CXX = g++
LIBS = -ljpeg -lopencv_core -lopencv_highgui
INCS = -I.

imageprocessing : \
	src/main.cpp \
	src/RWImage.cpp \
	src/ImageData.cpp \
	src/utils.cpp \
	src/knockout_matting.cpp \
	src/region_filling_by_edge_tracing.cpp \
	include/RWImage.h \
	include/ImageData.h \
	include/utils.h \
	include/knockout_matting.h \
	include/region_filling_by_edge_tracing.h
	$(CXX) $^ $(LIBS) $(INCS) -o $@

clean :
	rm -f imageprocessing
	rm -f *.jpg
	rm -f *.bmp
