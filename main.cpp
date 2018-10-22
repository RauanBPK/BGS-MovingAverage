#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <numeric>

using namespace cv;
using namespace std;



int main(int argc, char *argv[]){
    if(argc < 2){
        cout << "Wrong number of arguments.\n";
        cout << "./<bin> + \"video_input.mp4\"\n";
        return -1;
    }
    namedWindow("FrameOriginal", WINDOW_KEEPRATIO);
    namedWindow("Diff", WINDOW_KEEPRATIO);
    namedWindow("Recorte", WINDOW_KEEPRATIO);
    namedWindow("BG", WINDOW_KEEPRATIO);

    VideoCapture cap(argv[1]); 

    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    Mat frame;
    cap >> frame;
    Mat meanBG = Mat::zeros(frame.size(), CV_32FC3);

    float th = 0.3;
    while(cap.isOpened()){
        
        Mat frame,diff, frameDraw;
        cap >> frame;
        //medianBlur(diff, diff, 5);

        if (frame.empty())
            break;
        
        Mat floatimg;
        meanBG.convertTo(meanBG, CV_32FC3);
        frame.convertTo(floatimg, CV_32FC3);
        
        accumulateWeighted(floatimg, meanBG, th);

        th -= 0.005;

        if(th < 0.02){
            th = 0.02;
        }
        meanBG.convertTo(meanBG, CV_8UC3);
        absdiff(meanBG,frame,diff);

        cvtColor(diff, diff, COLOR_BGR2GRAY);

        for (int r = 1; r < 3; r++)
        {
            Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(4*r + 1, 4*r +1));
            morphologyEx(diff, diff, MORPH_CLOSE, kernel);
            morphologyEx(diff, diff, MORPH_OPEN, kernel);
        }
        
        threshold(diff, diff, 38, 255, cv::THRESH_BINARY ); // 32

        vector<vector< Point > >conts;
        vector<Vec4i> hierarchy;
        findContours(diff, conts, hierarchy, RETR_TREE  , CHAIN_APPROX_NONE);
        

        vector<vector<Point> >hull( conts.size() );
        Scalar color = Scalar( 255,255,255);
        for(int i=0; i<conts.size(); i++)
        {   
            //drawContours(frameDraw, conts, i, Scalar(0,0,255), 1, LINE_AA); //desenha contorno
            convexHull( Mat(conts[i]), hull[i], false );
            drawContours( diff , hull, i, color, -1, 8, vector<Vec4i>(), 0, Point() );

        }

        frameDraw = frame.clone();
        bitwise_not(diff,diff);                 // inverte a mascara diff para fazer subtracao
        cvtColor(diff,diff,COLOR_GRAY2BGR);

        subtract(frame,diff,frameDraw);         // subtrair a mascara do frame e guarda em frameDraw
        
        
    // Display the resulting frame
        imshow( "FrameOriginal", frame );
        imshow("Recorte", frameDraw);
        imshow("Diff", diff);
        imshow("BG",meanBG);
        

    // Press  ESC on keyboard to exit
        char c=(char)waitKey(20);
        if(c==20)
            break;
     }

     cap.release();
     destroyAllWindows();

     return 0;
 }