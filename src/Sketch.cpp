// Copyright 2014-4 xtu
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <string>
#include "include/Sketch.h"
#include "include/RWImage.h"
#include "include/ImageData.h"
#define RED 0x00ff0000
#define GREEN 0x0000ff00
#define BLUE 0x000000ff
#define BLACK 0x00000000
#define NO_ALPHA 0x00ffffff
#define SCALING 0.618  //  缩放倍数为0.618倍
#define scaling_save_path "opencvsave.bmp"
#define AMPLIFYFILENAME "amplify.bmp"
#define CANNY "canny.bmp"
sketch::sketch() {
}
std::string sketch::Edge(const char* filename) {
    IplImage * pImage = NULL;
    IplImage * pImage8uGray = NULL;
    IplImage * pImage8uSmooth = NULL;
    IplImage * pImage16uGraySobel = NULL;
    IplImage * pImage8uGraySobelShow = NULL;
    //  --------------
    IplImage * pImagePlanes[3] = {NULL, NULL, NULL};
    IplImage * pImage16uColorSobel = NULL;
    IplImage * pImage8uColorSobelShow = NULL;
    //  --------------
    IplImage * pImage16uGrayLaplace = NULL;
    IplImage * pImage8uGrayLaplaceShow = NULL;
    //  --------------
    IplImage * pImage8uGrayCanny = NULL;
    //  ==================灰度图像Sobel变换=====================
    pImage = cvLoadImage(filename, -1);
    pImage8uGray = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);
    pImage8uSmooth = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);
    pImage8uGraySobelShow = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);
    //  转灰度
    cvCvtColor(pImage, pImage8uGray, CV_BGR2GRAY);
    //  高斯滤波
    cvSmooth(pImage8uGray, pImage8uSmooth, CV_GAUSSIAN, 3, 0, 0);
    //  cvSobel要求目标图像必须是IPL_DEPTH_16S
    pImage16uGraySobel = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_16S, 1);
    //  计算一阶x方向的差分,也可以计算一阶y方向
    cvSobel(pImage8uSmooth, pImage16uGraySobel, 0, 1, 3);
    //  再把格式转回来,用于显示
    cvConvertScaleAbs(pImage16uGraySobel, pImage8uGraySobelShow, 1, 0);

    //  ==================彩色图像Sobel变换=====================
    int i;
    for (i = 0; i < 3; i++) {
    pImagePlanes[i] = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);
    }
    pImage16uColorSobel = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_16S, 1);
    pImage8uColorSobelShow = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 3);
    //  分成3个单通道
    cvCvtPixToPlane(pImage, pImagePlanes[0], pImagePlanes[1], pImagePlanes[2], NULL);
    for (i = 0; i < 3; i++) {
    cvSobel(pImagePlanes[i], pImage16uColorSobel, 0, 1, 3);
    cvConvertScaleAbs(pImage16uColorSobel, pImagePlanes[i], 1, 0);
    }
    cvCvtPlaneToPix(pImagePlanes[0], pImagePlanes[1], pImagePlanes[2], NULL, pImage8uColorSobelShow);

    //  ==================灰度图像Laplace变换=====================
    pImage16uGrayLaplace = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_16S, 1);
    pImage8uGrayLaplaceShow = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);
    cvLaplace(pImage8uSmooth, pImage16uGrayLaplace, 3);
    cvConvertScaleAbs(pImage16uGrayLaplace, pImage8uGrayLaplaceShow, 1, 0);

    //  ==================灰度图像Canny变换=====================
    pImage8uGrayCanny = cvCreateImage(cvGetSize(pImage), IPL_DEPTH_8U, 1);
    cvCanny(pImage8uSmooth, pImage8uGrayCanny, 100, 200, 3);
    cvSaveImage(CANNY, pImage8uGrayCanny);
    cvWaitKey(0);
    cvReleaseImage(&pImage);
    cvReleaseImage(&pImage8uGray);
    cvReleaseImage(&pImage8uSmooth);
    cvReleaseImage(&pImage16uGraySobel);
    cvReleaseImage(&pImage8uGraySobelShow);
    cvReleaseImage(&pImage16uColorSobel);
    cvReleaseImage(&pImage8uColorSobelShow);
    cvReleaseImage(&pImagePlanes[0]);
    cvReleaseImage(&pImagePlanes[1]);
    cvReleaseImage(&pImagePlanes[2]);
    cvReleaseImage(&pImage16uGrayLaplace);
    cvReleaseImage(&pImage8uGrayLaplaceShow);
    return "test.bmp";
}
sketch::~sketch() {
}
