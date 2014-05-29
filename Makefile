#Copyright 2014-5 sxniu

CXX = g++
LIBS = -ljpeg -lopencv_core -lopencv_highgui -lopencv_imgproc
INCS = -I.
LIBDIRS = /usr/local/lib

imageprocessing : \
	src/main.cpp \
	src/RWImage.cpp \
	src/ImageData.cpp \
	src/utils.cpp \
	src/knockout_matting.cpp \
	src/region_filling_by_edge_tracing.cpp \
        src/imageprocess.cpp \
        src/replace.cpp \
        src/scale.cpp \
        src/inpainting.cpp \
        src/Sketch.cpp \
        include/imageprocess.h \
        include/inpainting.h \
        include/replace.h \
        include/scale.h \
        include/Sketch.h \
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
