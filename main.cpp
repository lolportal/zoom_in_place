#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath> // for abs() macro
using namespace std;

enum {
	ESC = 27,
	ENTER = 13,
	SPACE = 32,
};

const string captureWindow = "Capture";
const string zoomINwindow = "ZOOOOM in";
const string zoomOUTwindow = "Zoom out";
const string zoomINtrackbar = "IN";
const string zoomOUTtrackbar = "OUT";


int trackbar_pos;

cv::Point click_point;
const int max_zoom = 30;

bool resizeInPlaceOnCenter( cv::Mat inputArray, cv::Mat outputArray, double scale){
	if (inputArray.size().width != outputArray.size().width || inputArray.size().height != outputArray.size().height) return false;
	if (scale == 0) return false;
	cv::Size orig_size = inputArray.size();

	double ROI_scale = (scale >= 1.0 ? 1.0/scale : scale);

	cv::Size ROI_size(int(orig_size.width*ROI_scale), int(orig_size.height*ROI_scale));
	cv::Point ROI_origin((orig_size.width - ROI_size.width)/2, (orig_size.height - ROI_size.height)/2);
	cv::Rect result_ROI_rect(ROI_origin,ROI_size);

	cv::Mat result;

	if (scale >= 1.0){
		cv::Mat smallerROI(inputArray,result_ROI_rect);

		cv::resize(smallerROI,result,orig_size);
	}
	else {
		result = cv::Mat(orig_size,inputArray.type(),CV_RGB(0,0,0));
		cv::Mat smallerROI(result,result_ROI_rect);
		cv::resize(inputArray,smallerROI,ROI_size);
	}
	if (inputArray.size().width != result.size().width || inputArray.size().height != result.size().height) return false;
	result.copyTo(outputArray);
	return true;
}


bool resizeInPlace( cv::Mat inputArray, cv::Mat outputArray, cv::Point zoomCenter, double scale){
	if (inputArray.size().width != outputArray.size().width || inputArray.size().height != outputArray.size().height) return false;
	if (scale == 0) return false;
	cv::Size orig_size = inputArray.size();
	int x_delta = zoomCenter.x - orig_size.width/2;
	int y_delta = zoomCenter.y - orig_size.height/2;

	cv::Size center_ROI_size(orig_size.width - abs(x_delta), orig_size.height - abs(y_delta));

	int ROI_origin_x = (x_delta < 0) ? 0 : x_delta;
	int ROI_origin_y = (y_delta < 0) ? 0 : y_delta;

	cv::Point center_ROI_origin(ROI_origin_x,ROI_origin_y);
	cv::Rect ROI_rect(center_ROI_origin,center_ROI_size);
	cv::Mat zoom_center_ROI(inputArray,ROI_rect);

	cv::Mat temp(orig_size,inputArray.type(),CV_RGB(0,0,0));
	cv::Point temp_ROI_origin(abs(x_delta - ROI_origin_x), abs(y_delta - ROI_origin_y));
	cv::Rect temp_ROI_rect(temp_ROI_origin,center_ROI_size);
	cv::Mat temp_ROI(temp,temp_ROI_rect);
	zoom_center_ROI.copyTo(temp_ROI);

	double ROI_scale = (scale >= 1.0 ? 1.0/scale : scale);

	cv::Size ROI_size(int(orig_size.width*ROI_scale), int(orig_size.height*ROI_scale));
	cv::Point ROI_origin((orig_size.width - ROI_size.width)/2, (orig_size.height - ROI_size.height)/2);
	cv::Rect result_ROI_rect(ROI_origin,ROI_size);

	cv::Mat result;

	if (scale >= 1.0){
		cv::Mat smallerROI(temp,result_ROI_rect);

		cv::resize(smallerROI,result,orig_size);
	}
	else {
		result = cv::Mat(orig_size,inputArray.type(),CV_RGB(0,0,0));
		cv::Mat smallerROI(result,result_ROI_rect);
		cv::resize(temp,smallerROI,ROI_size);
	}
	if (inputArray.size().width != result.size().width || inputArray.size().height != result.size().height) return false;
	result.copyTo(outputArray);
	return true;
}

cv::Mat working_frame;
void onZoomInTrackbar(int pos, void* ){

	cv::Size orig_size = working_frame.size();
	double scale = 1.0/(pos+1);
	cv::Size ROI_size(int(orig_size.width*scale), int(orig_size.height*scale));
	
	cv::Point ROI_origin((orig_size.width - ROI_size.width)/2, (orig_size.height - ROI_size.height)/2);
	cv::Rect ROI_rect(ROI_origin,ROI_size);
	cv::Mat smallerROI(working_frame,ROI_rect);
	cv::Mat shown_result;
	cv::resize(smallerROI,shown_result,orig_size);
	cv::imshow(zoomINwindow,shown_result);
	cv::waitKey(1);
}

void onZoomOutTrackbar(int pos, void* ){

	cv::Size orig_size = working_frame.size();
	double scale = 1.0/(pos+1);
	cv::Size ROI_size(int(orig_size.width*scale), int(orig_size.height*scale));

	cv::Point ROI_origin((orig_size.width - ROI_size.width)/2, (orig_size.height - ROI_size.height)/2);
	cv::Rect ROI_rect(ROI_origin,ROI_size);
	cv::Mat shown_result(orig_size,working_frame.type(),CV_RGB(0,0,0));
	cv::Mat smallerROI(shown_result,ROI_rect);

	cv::resize(working_frame,smallerROI,ROI_size);
	cv::imshow(zoomOUTwindow,shown_result);
	cv::waitKey(1);
}


cv::Mat original_frame;

void onMouse(int event, int x, int y, int, void* userdata){
	if (event == CV_EVENT_LBUTTONDOWN){
//		cv::Mat img = *(cv::Mat*) userdata;
// 		cv::Size orig_size = img.size();
// 		int x_delta = x - orig_size.width/2;
// 		int y_delta = y - orig_size.height/2;
// 
// 		cv::Size ROI_size(orig_size.width - abs(x_delta), orig_size.height - abs(y_delta));
// 
// 		int ROI_origin_x = (x_delta < 0) ? 0 : x_delta;
// 		int ROI_origin_y = (y_delta < 0) ? 0 : y_delta;
// 
// 		cv::Point ROI_origin(ROI_origin_x,ROI_origin_y);
// 		cv::Rect ROI_rect(ROI_origin,ROI_size);
// 		cv::Mat zoom_center_ROI(img,ROI_rect);
// 		
// 		cv::Mat temp(orig_size,img.type(),CV_RGB(0,0,0));
// 		cv::Point temp_ROI_origin(abs(x_delta - ROI_origin_x), abs(y_delta - ROI_origin_y));
// 		cv::Rect temp_ROI_rect(temp_ROI_origin,ROI_size);
// 		cv::Mat temp_ROI(temp,temp_ROI_rect);
// 		zoom_center_ROI.copyTo(temp_ROI);
// 	
// 		cv::imshow(zoomINwindow,temp);
// 		cv::imshow(zoomOUTwindow,temp);
// 
// 		temp.copyTo(working_frame);
// 		click_point = cv::Point(x,y);
// 		cv::createTrackbar(zoomINtrackbar,zoomINwindow,&trackbar_pos,max_zoom,onZoomInTrackbar,&working_frame);
// 		cv::createTrackbar(zoomOUTtrackbar,zoomOUTwindow,&trackbar_pos,max_zoom,onZoomOutTrackbar,&working_frame);
// 		cv::waitKey(1);

		resizeInPlaceOnCenter(original_frame,original_frame, .5);
		imshow("desu",original_frame);
		cv::waitKey(1);
	}
}



int main()
try {
	cv::VideoCapture cap(CV_CAP_ANY);
	if (!cap.isOpened()) throw runtime_error("Can't open device...");
	
	cv::namedWindow(captureWindow);
	cv::setMouseCallback(captureWindow,onMouse);
	
	for (;;)
	{

		cap >> original_frame;

		cv::imshow(captureWindow,original_frame);
		int c = cv::waitKey(15);

		if (c == ESC) break;
	}
}

catch (exception& e){
	cerr << e.what() << endl;
}