#include "ImgMatcher.h"
#include "GDALImpl.h"
#include <stdio.h>
#include <iostream>
#include <algorithm> 
#include <vector>
#include <ctime>
#include <string.h>
#include <cstring>
#include "opencv2\opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/flann.hpp"
#include "opencv2\objdetect.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "cpl_conv.h"
#include "cpl_string.h"


using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

void ImgMatcher::setObjMat(std::string sourceImg) {
	img_object = imread(sourceImg, cv::IMREAD_GRAYSCALE);
	ogFilePath = sourceImg;
}

void ImgMatcher::setScMat(std::string geoImg) {
	img_scene = imread(geoImg, cv::IMREAD_GRAYSCALE);
	inputPath = geoImg;
}

void ImgMatcher::setOutPath(std::string outPath) {
	outputPath = outPath;
}

void ImgMatcher::setFileName(std::string fName) {
	fileName = fName;
}

std::string ImgMatcher::getInPath() {
	return inputPath;
}

std::string ImgMatcher::getOutPath() {
	return outputPath;
}

std::string ImgMatcher::getFileName() {
	return fileName;
}

std::string ImgMatcher::getOgFilePath() {
	return ogFilePath;
}

void ImgMatcher::matchImgs() {

	//to track the time taken from matching to creating output file
	clock_t start;
	//timer begins
	start = clock();
	
	if (!img_object.data || !img_scene.data)
	{
		cout << " --(!) Error reading images " << endl; return;
	}

	//*******/
	const char* match_window = "Match Result window";
	namedWindow(match_window, WINDOW_NORMAL);

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 20;
	//FeatureDetector * detector = new SIFT(); 
	Ptr<SURF> detector = SURF::create(minHessian);
	//Ptr<ORB> detector = ORB::create();
	//detector->setThreshold(3e-4);

	std::vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;

	//-- Step 2: Calculate descriptors (feature vectors)
	detector->detectAndCompute(img_object, Mat(), keypoints_object, descriptors_object);
	detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);

	if (descriptors_object.type() != CV_32F) {
		descriptors_object.convertTo(descriptors_object, CV_32F);
	}

	if (descriptors_scene.type() != CV_32F) {
		descriptors_scene.convertTo(descriptors_scene, CV_32F);
	}

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	//BFMatcher matcher(NORM_L1);
	//BFMatcher matcher(NORM_HAMMING);
	std::vector< DMatch > matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("\n-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance <= 3 * min_dist)
		//if(matches[i].distance < (max_dist/1.6))
		{
			good_matches.push_back(matches[i]);
		}
	}


		printf("-- good matches : %d \n\n", good_matches.size());

		Mat img_matches;
		drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		//-- Localize the object
		std::vector<Point2f> obj;
		std::vector<Point2f> scene;

		for (int i = 0; i < good_matches.size(); i++)
		{
			//-- Get the keypoints from the good matches
			obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
			scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
		}

		Mat H = findHomography(obj, scene, RANSAC);

		//-- Get the corners from the image_1 ( the object to be "detected" )
		std::vector<Point2f> obj_corners(5);
		obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
		obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
		obj_corners[4] = cvPoint(img_object.cols / 2, img_object.rows / 2);
		//obj_corners[4] = cvPoint(img_object.cols / 2, 0); obj_corners[5] = cvPoint(0, img_object.rows / 2);
		//obj_corners[6] = cvPoint(img_object.cols, img_object.rows / 2); obj_corners[7] = cvPoint(img_object.cols / 2, img_object.rows);
		std::vector<Point2f> scene_corners(5);

		perspectiveTransform(obj_corners, scene_corners, H);

		//-- Draw lines between the corners (the mapped object in the scene - image_2 )
		line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(255, 255, 255), 4); //green
		line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(255, 255, 255), 4); //blue-turquoise
		line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(255, 255, 255), 4); //red
		line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(255, 255, 255), 4); //pink		

		imshow(match_window, img_matches);


		std::string temp = getInPath();
		const char * cstr = new char[temp.length() + 1];
		cstr = temp.c_str();
		//passCoordinates(getOutPath(), cstr, getFileName(), scene_corners);
		GDALImpl gdal;
		gdal.transformCoords2(getOutPath(), cstr, getOgFilePath(), getFileName(), img_object.cols, img_object.rows, scene_corners[0], scene_corners[1], scene_corners[3], scene_corners[2], scene_corners[4], start);

		if (waitKey() == 27) destroyAllWindows();
}


void ImgMatcher::passCoordinates(std::string outputPath, const char* inputPath, std::string fileName, std::vector<cv::Point2f> scene_corners) {
	double xMin = std::min(scene_corners[0].x, std::min(scene_corners[3].x, std::min(scene_corners[1].x, scene_corners[2].x)));
	double yMin = std::min(scene_corners[0].y, std::min(scene_corners[3].y, std::min(scene_corners[1].y, scene_corners[2].y)));
	double xMax = std::max(scene_corners[0].x, std::max(scene_corners[3].x, std::max(scene_corners[1].x, scene_corners[2].x)));
	double yMax = std::max(scene_corners[0].y, std::max(scene_corners[3].y, std::max(scene_corners[1].y, scene_corners[2].y)));

	double ulx = scene_corners[0].x;
	double uly = scene_corners[0].y;
	double lrx = scene_corners[2].x;
	double lry = scene_corners[2].y;

	GDALImpl gdal;
	gdal.transformCoords(outputPath, inputPath, fileName, &xMin, &yMin, &xMax, &yMax);
	//gdal.transformCoords(outputPath, inputPath, fileName, &ulx, &uly, &lrx, &lry);
}
