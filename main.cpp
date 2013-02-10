/* 
 * File:   main.cpp
 * Author: alex
 *
 * Created on January 31, 2013, 5:10 AM
 */

#include <opencv2/imgproc/types_c.h>

#include "opencv/highgui.h"

void showImage();
void showVideo();
void jumpByVideo();
void onTrackbarSlide(int pos);
void imageTransormation();
IplImage* doPyrDown(IplImage* in, int filter);
IplImage* doCanny(IplImage* in, double lowThresh, double highThresh,
        double aperture);
void showSteamByCamera();

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    //showImage();
    //showVideo();
    //jumpByVideo();
    //imageTransormation();
    showSteamByCamera();

    return 0;
}

void showImage() {
    char filename[] = "/home/alex/Pictures/gui_comm_2.png";
    IplImage* img = cvLoadImage(filename);

    cvNamedWindow("SimpleImageViewer", CV_WINDOW_AUTOSIZE);

    // Первым параметром передается имя окна, 
    // в которое загрузить изображение 
    // (в данном случае то, которое было создано выше)
    cvShowImage("SimpleImageViewer", img);

    cvWaitKey(0);
    cvReleaseImage(&img);
    cvDestroyWindow("SimpleImageViewer");
}

void showVideo() {
    IplImage* frame = NULL;
    char filename[] = "/home/alex/Videos/[HDTV]Перевозчик 2.avi";
    char windowName[] = "SimpleVideoViewer";

    cvNamedWindow(windowName, CV_WINDOW_FREERATIO);
    CvCapture* capture = cvCreateFileCapture(filename);

    while (1) {
        char c;

        // Берем каждое следующее изображение видео
        // и кладем в структуру (IplImage*) capture
        // пока есть изображения (т.е. frame != NULL)
        frame = cvQueryFrame(capture);

        if (frame == NULL) break;
        cvShowImage(windowName, frame);

        // pause for input 33 ms between each frame
        // if not, then it will be set to –1
        // т.е. таким образом можно даже регулировать скорость видео
        // (т.е. скорость отображения кадров)
        c = cvWaitKey(33);

        // ESC = ASCII(27)
        if (c == 27) break;
    }

    cvReleaseCapture(&capture);
    cvDestroyWindow(windowName);
}



#include "opencv/cv.h"

int g_slider_position = 0;
CvCapture* g_capture = NULL;

void onTrackbarSlide(int pos) {
    cvSetCaptureProperty(
            g_capture,
            CV_CAP_PROP_POS_FRAMES, // установка позиции в единицах фрейма
            pos
            );
}

void jumpByVideo() {
    int frames = 0;
    char windowName[] = "SimpleVideoWithSlider";
    char filename[] = "/home/alex/Videos/[HDTV]Перевозчик 2.avi";

    cvNamedWindow(windowName, CV_WINDOW_FREERATIO);
    g_capture = cvCreateFileCapture(filename);
    frames = (int) cvGetCaptureProperty(
            g_capture,
            CV_CAP_PROP_FRAME_COUNT
            );

    if (frames != 0) {
        cvCreateTrackbar(
                "trackbar",
                windowName,
                &g_slider_position,
                frames, // MAX value for a slider
                onTrackbarSlide
                );
    }

    IplImage* frame = NULL;

    while (1) {
        char c = 0;

        frame = cvQueryFrame(g_capture);
        if (frame == NULL) break;
        cvShowImage(windowName, frame);
        c = cvWaitKey(33);
        if (c == 27) break;
    }


    cvReleaseCapture(&g_capture);
    cvDestroyWindow(windowName);
}



// Need cv.h

void imageTransormation() {
    IplImage* img = cvLoadImage("/home/alex/Pictures/smetanno-orehovyj-pirog-Mishel-1.jpg");
    IplImage* imgOut = NULL;
    char beforeWindowName[] = "BeforeTransformation";
    char afterWindowName[] = "AfterTransformation";

    cvNamedWindow(beforeWindowName, CV_WINDOW_FREERATIO);
    cvNamedWindow(afterWindowName, CV_WINDOW_FREERATIO);

    cvShowImage(beforeWindowName, img);

    imgOut = cvCreateImage(
            cvGetSize(img),
            IPL_DEPTH_8U,
            3
            );

    cvSmooth(img, imgOut, CV_GAUSSIAN, 135, 135);
    //imgOut = doPyrDown(img, IPL_GAUSSIAN_5x5);
    //imgOut = doCanny(img, 0.5, 0.5, 0.5);

    cvShowImage(afterWindowName, imgOut);

    cvWaitKey(0);
    cvReleaseImage(&imgOut);
    cvDestroyAllWindows();
}

/**
 * Создание изображения, наполовину меньшего длины и ширины исходного
 * @param in - исходное изображение
 * @param filter - фильтр
 * @return out
 */
IplImage* doPyrDown(IplImage* in, int filter = IPL_GAUSSIAN_5x5) {
    // Best to make sure input image is divisible by two
    assert(in->width % 2 == 0 && in->height % 2 == 0);

    IplImage* out = cvCreateImage(
            cvSize(in->width / 2, in->height / 2),
            in->depth,
            in->nChannels
            );

    cvPyrDown(in, out);
    return out;
}

/**
 * The Canny edge detector writes its output to a single channel (grayscale) image
 * @param in
 * @param lowThresh
 * @param highThresh
 * @param aperture
 * @return 
 */
IplImage* doCanny(IplImage* in, double lowThresh, double highThresh,
        double aperture) {
    if (in->nChannels != 1) {
        return NULL; // Canny only handles gray scale images
    }

    IplImage* out = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);
    cvCanny(in, out, lowThresh, highThresh, aperture);

    return out;
}

void showSteamByCamera() {
    CvCapture* capture = cvCreateCameraCapture(-1);
    IplImage* img = NULL;
    assert(capture != NULL);
    double fps = (double) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    
    
    cvNamedWindow("CaptureFromCamera", CV_WINDOW_FULLSCREEN);
    
    /* DON'T WORKING
    CvVideoWriter* videoWriter = cvCreateVideoWriter(
            "/home/alex/Videos/out.avi", 
            CV_FOURCC('T','H','E','O'), 
            fps, 
            cvSize(200, 200)
    ); */
    
//    assert(videoWriter != NULL);
    
    while (1) {
        img = cvQueryFrame(capture);
        
        // устроим небольшое real-time размытие, как с бадуна ))
        cvSmooth(img, img, CV_GAUSSIAN, 15, 15);
        
        if (img == NULL) break;
        cvShowImage("CaptureFromCamera", img);
//        cvWriteFrame(videoWriter, img);
        char c = cvWaitKey(4);
        if(c == 27) break;
    }
    
    cvReleaseImage(&img);
//    cvReleaseVideoWriter(&videoWriter);
    cvDestroyAllWindows();
}