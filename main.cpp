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
    int numCarros = 0;
    cap >> frame;
    Mat meanBG = Mat::zeros(frame.size(), CV_32FC3);

    float th = 0.3;
    char str[50] = "Numero de carros: 0";
    while(cap.isOpened()){

        Mat frame,diff, frameDraw;
        
        cap >> frame;

        frameDraw = frame.clone();

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


        Rect r=Rect(0,frame.rows - 200,frame.cols, 14);
        rectangle(frame,r,Scalar(255,0,0),-1,8,0);

        cvtColor(diff, diff, COLOR_BGR2GRAY);


        for (int r = 1; r < 3; r++)
        {
            Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(4*r + 1, 4*r +1));
            morphologyEx(diff, diff, MORPH_CLOSE, kernel);
            morphologyEx(diff, diff, MORPH_OPEN, kernel);
        }
        
        threshold(diff, diff, 38, 255, cv::THRESH_BINARY ); // 32

       // dilate(diff, diff, 0, Point(-1, -1), 8, 1, 1);
        vector<vector< Point > >conts;
        vector<vector< Point > >contsHull;
        vector<vector< Point > >contsHull2;
        vector<Vec4i> hierarchy;


        findContours(diff, conts, hierarchy, RETR_TREE  , CHAIN_APPROX_NONE);
        

        vector<vector<Point> >hull( conts.size() );
        Scalar color = Scalar( 0 ,191,255);

        
//faz o hull dos contornos iniciais
        for(int i=0; i<conts.size(); i++){

            if(conts[i].size() > 150 && hierarchy[i][3] == -1){   // se for maior e nao tiver pai (coitado)


                convexHull( Mat(conts[i]), hull[i], false );

                drawContours( diff , hull, i, Scalar(255,255,255), -1, 8, vector<Vec4i>(), 0, Point() );
                
            }

        }
// calcula momentos de cada contorno dps do hull se < 100     
        
        int numMom = 0;
        findContours(diff, contsHull, RETR_LIST  , CHAIN_APPROX_NONE);
        vector<Moments> mom(contsHull.size());
        for(int i=0; i<contsHull.size(); i++){
            if(contsHull[i].size() > 1){

                mom[numMom] = moments(contsHull[i],true);
                numMom++;

            }
        }
// pra cada momento faz o centro e calcula a distancia pros outros e desenha se perto
        for(int i =0 ; i < numMom;i++){
            Point p(mom[i].m10/mom[i].m00, mom[i].m01/mom[i].m00);
            for(int j = i; j < numMom; j++){
                Point p2(mom[j].m10/mom[j].m00, mom[j].m01/mom[j].m00);
                double dist  = norm(p-p2);
                if( dist < 30){
                    line(diff, p, p2, cv::Scalar(255,255,255), 4);
                }
            }
        }

// //acha o novo contorno e faz hull dnv
//         findContours(diff, contsHull, RETR_LIST  , CHAIN_APPROX_NONE);
//         for(int i=0; i<contsHull.size(); i++){

//             if(contsHull[i].size() > 150){   // se for maior e nao tiver pai (coitado)


//                 convexHull( Mat(conts[i]), hull[i], false );

//                 drawContours( diff , hull, i, Scalar(255,255,255), -1, 8, vector<Vec4i>(), 0, Point() );
                
//             }

//         }
        findContours(diff, contsHull, RETR_LIST  , CHAIN_APPROX_NONE);
        for(int i=0; i<contsHull.size(); i++){
            if(contsHull[i].size() > 100){


                
                Mat M;

                Rect boundRect = boundingRect(Mat(contsHull[i]));
                rectangle( frame, boundRect.tl(), boundRect.br(), Scalar( 0 ,191,255), 2, 8, 0 );
                Point center_of_rect = (boundRect.br() + boundRect.tl())*0.5;
                
                
                if (r.contains(center_of_rect)){
                    numCarros++;
                    //cout << numCarros << "\n";
                    rectangle(frame,r,Scalar(30,190,10),-1,8,0);
                    
                    sprintf(str,"Numero de carros: %d", numCarros );
                }
            }
            putText(frame, str, Point(100,100), FONT_HERSHEY_PLAIN,4, Scalar(0,0,255),3);
            cout << "Num carros: " << numCarros << "\n";
        }

        
        bitwise_not(diff,diff);                 // inverte a mascara diff para fazer subtracao
        cvtColor(diff,diff,COLOR_GRAY2BGR);

        subtract(frameDraw,diff,frameDraw);         // subtrair a mascara do frame e guarda em frameDraw
        
        
    // Display the resulting frame
        imshow( "FrameOriginal", frame );
        imshow("Recorte", frameDraw);
        imshow("Diff", diff);
        imshow("BG",meanBG);
        

    // Press  ESC on keyboard to exit
        waitKey(0);
        //char c=(char)waitKey(20);
        //if(c==20)
         //   break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}