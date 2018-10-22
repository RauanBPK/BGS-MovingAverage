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
using namespace std;
using namespace cv;

int isPixelClose(Vec3b a, Vec3b b, int tol){
	if(abs(a[0]-b[0]) < tol
	&& abs(a[1]-b[1]) < tol
	&& abs(a[2]-b[2]) < tol){
		return 1;
	}else{
		return 0;
	}
}


// int meanMat(Mat a, float aW, Mat b, float bW, Mat mean){
// 	if(a.size() != b.size())
// 		return -1;
// 	for(int x = 0 ; x < a.cols; x++)
// 		for(int y=0; y < a.rows; y++){

// 			mean.at<Vec3b>(y,x)[0] = (aW*a.at<Vec3b>(y,x)[0] + bW* b.at<Vec3b>(y,x)[0]);
// 			mean.at<Vec3b>(y,x)[1] = (aW*a.at<Vec3b>(y,x)[1] + bW* b.at<Vec3b>(y,x)[1]);
// 			mean.at<Vec3b>(y,x)[2] = (aW*a.at<Vec3b>(y,x)[2] + bW* b.at<Vec3b>(y,x)[2]);

// 		}
// 	return 0;
// }



int meanMat(Mat a, Mat b, Mat mean){
	if(a.size() != b.size())
		return -1;
	float aW,bW;
	for(int x = 0 ; x < a.cols; x++)
		for(int y=0; y < a.rows; y++){
			if(isPixelClose(a.at<Vec3b>(y,x), b.at<Vec3b>(y,x),10)){
				aW = 0.9;
				bW = 0.1;
			}else{
				aW = 0.3;
				bW = 0.7;
			}

			mean.at<Vec3b>(y,x)[0] = (aW*a.at<Vec3b>(y,x)[0] + bW* b.at<Vec3b>(y,x)[0]);
			mean.at<Vec3b>(y,x)[1] = (aW*a.at<Vec3b>(y,x)[1] + bW* b.at<Vec3b>(y,x)[1]);
			mean.at<Vec3b>(y,x)[2] = (aW*a.at<Vec3b>(y,x)[2] + bW* b.at<Vec3b>(y,x)[2]);

		}
	return 0;
}

void calcBG(vector<Mat> frames, int rows, int cols, Mat mean){
	for(int x = 0; x < cols; x++)
		for(int y = 0 ; y < rows; y++){
			vector<int> R,G,B;
			for(int i = 0; i < frames.size(); i++){
				R.push_back(frames[i].at<Vec3b>(y,x)[0]);
				G.push_back(frames[i].at<Vec3b>(y,x)[1]);
				B.push_back(frames[i].at<Vec3b>(y,x)[2]);
			}
			
			// vector<int> histR(256,0);
			// vector<int> histG(256,0);
			// vector<int> histB(256,0);
			// for(int a = 0; a < frames.size(); a++){

			// 	histR[R[a]]++;
			// 	histG[G[a]]++;
			// 	histB[B[a]]++;
			// }

			
			mean.at<Vec3b>(y,x)[0] = accumulate(R.begin(), R.end(),	0)/R.size();
			mean.at<Vec3b>(y,x)[1] = accumulate(G.begin(), G.end(), 0)/G.size();
			mean.at<Vec3b>(y,x)[2] = accumulate(B.begin(), B.end(),0)/B.size();

			// mean.at<Vec3b>(y,x)[0] = distance(histR.begin(), max_element(histR.begin(),histR.end()));
			// mean.at<Vec3b>(y,x)[1] = distance(histG.begin(), max_element(histG.begin(), histG.end()));
			// mean.at<Vec3b>(y,x)[2] = distance(histB.begin(), max_element(histB.begin(), histB.end()));

		}
}


int main(int argc, char *argv[]){
	if(argc < 2){
		cout << "Wrong number of arguments.\n";
		return -1;
	}
	namedWindow("Frame", WINDOW_KEEPRATIO);
	namedWindow("Diff", WINDOW_KEEPRATIO);
	namedWindow("BG", WINDOW_KEEPRATIO);
	//VideoCapture cap("passarela.mp4");
	//VideoCapture cap("highway.mp4"); 

 VideoCapture cap(argv[1]); 

	if(!cap.isOpened()){
		cout << "Error opening video stream or file" << endl;
		return -1;
	}




	
	vector<Mat> frameHist;
	int maxHist = 20;

	Mat firstFrame;
	cap >> firstFrame;	
	Mat meanBG = Mat::zeros(firstFrame.size(), CV_32FC3);
	// cap >> meanBG;
	Mat frame;

	

	int count;
	float th = 0.3;
	while(cap.isOpened()){
		
		Mat frame,diff,diff2, frameDraw;
		cap >> frame;

		if (frame.empty())
			break;
		
        Mat floatimg;
        meanBG.convertTo(meanBG, CV_32FC3);
        frame.convertTo(floatimg, CV_32FC3);
		
		accumulateWeighted(floatimg, meanBG, th);
		//accumulateWeighted(floatimg, meanBG, 0.01);
		//th -= 0.0025;
		th -= 0.005;

		if(th < 0.02){
			th = 0.02;
		}
		meanBG.convertTo(meanBG, CV_8UC3);
		absdiff(meanBG,frame,diff);

		absdiff(frame,firstFrame,diff2);
		count++;
		if(count >=0 ){
			count = 0;
			firstFrame = frame;
		}

		cvtColor(diff, diff, COLOR_BGR2GRAY);
		cvtColor(diff2, diff2, COLOR_BGR2GRAY);
    	

    	for (int r = 1; r < 2; r++)
    	{
    		Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(4*r + 1, 4*r +1));
    		morphologyEx(diff, diff, MORPH_CLOSE, kernel);
    		morphologyEx(diff, diff, MORPH_OPEN, kernel);


    	}
    	threshold(diff, diff, 32, 255, cv::THRESH_BINARY ); //| THRESH_OTSU

    	Mat im_floodfill = diff.clone();
    	floodFill(im_floodfill, cv::Point(0,0), Scalar(255));
    	Mat im_floodfill_inv;
   		
   		bitwise_not(im_floodfill, im_floodfill_inv);
     
    	// Combine the two images to get the foreground.
   		Mat im_out = (diff | im_floodfill_inv);

		dilate(im_out,im_out,Mat(),Point(-1,-1), 3, 1 ,1 );
    	for (int r = 1; r < 3; r++)
    	{
    		Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3*r + 1, 3*r +1));
    		morphologyEx(im_out, im_out, MORPH_CLOSE, kernel);
    		morphologyEx(im_out, im_out, MORPH_OPEN, kernel);

    		
    	}

    	vector<vector< Point > >conts;
    	vector<Vec4i> hierarchy;
    	findContours(im_out, conts, hierarchy, RETR_TREE  , CHAIN_APPROX_NONE);
    	frameDraw = frame.clone();
    	for(int i=0; i<conts.size(); i++)
    	{	
			drawContours(frameDraw, conts, i, Scalar(0,0,255), 1, LINE_AA); //desenha contorno
		}
		
		
    // Display the resulting frame
		imshow( "Frame", frameDraw );
		imshow("Diff", im_out);
		imshow("diff2",diff);
		imshow("BG",meanBG);
		

    // Press  ESC on keyboard to exit
		char c=(char)waitKey(50);
		if(c==50)
			break;
	}

	cap.release();
	destroyAllWindows();

	return 0;
}