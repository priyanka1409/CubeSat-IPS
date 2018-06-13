// Opencv.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include <algorithm> 
#include <vector>
#include <ctime>
#include <string.h>
//#include "opencv2\opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/flann.hpp"
//#include "opencv2\objdetect.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "cpl_string.h"
#include "gdal.h"
#include "gdalwarper.h"
#include "ogr_spatialref.h"
#include "vrtdataset.h"
#include "gdal_vrt.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;


const char* result_window = "Result window";

void readme();
void testing(const GDALWarpOptions *options);
void testDupli(char **list);
void testStrd(const char *str);
void crop2(const char *inputPath, const char *cropPath, double *topLeftX, double *topLeftY, double *bottomRightX, double *bottomRightY);
void crop(const char *inputPath, const char *cropPath, double *topLeftX, double *topLeftY, double width, double height);
void coordinates(const char *inputPath, const char *cropPath, double *topLeftX, double *topLeftY, double *bottomRightX, double *bottomRightY);
void warpTry(const char *inputPath, const char *outputPath, double *topLeftX, double *topLeftY, double *bottomRightX, double *bottomRightY);
void printCoords(const char *inputPath, double *topLeftX, double *topLeftY, double *bottomRightX, double *bottomRightY);

/** @function main */
int main(int argc, char** argv)
{
	std::clock_t start;
	start = std::clock();
	//initModule_nonfree();
	//"C:\\Users\\Priyanka\\Desktop\\imageresource\\AOI2.tif" "C:\\Users\\Priyanka\\Desktop\\imageresource\\Level2_GeoTiff.tif"

	GDALDataset  *poDataset;
	GDALAllRegister();

	if (argc != 3)
	{
		readme(); return -1;
	}

	Mat img_object = imread(argv[1], cv::IMREAD_GRAYSCALE);
	Mat img_scene = imread(argv[2], cv::IMREAD_GRAYSCALE);


	if (!img_object.data || !img_scene.data)
	{
		std::cout << " --(!) Error reading images " << std::endl; return -2;
	}

	//*******//
	namedWindow(result_window, WINDOW_NORMAL);

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 0;
	//FeatureDetector * detector = new SIFT();
	Ptr<SURF> detector = SURF::create(minHessian);

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

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance <= 3 * min_dist)
			//{ if(matches[i].distance < (max_dist/1.6))
		{
			good_matches.push_back(matches[i]);
		}
	}

	printf("-- good matches : %d \n", good_matches.size());

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
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4); //green
	line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 200), 4); //blue-turquoise
	line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(255, 0, 0), 4); //red
	line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(255, 0, 255), 4); //pink

																																			   //-- Show detected matches
																																			   //if (!img_matches.data) {
																																			   //imshow("Good Matches & Object detection", img_matches);
																																			   //}
																																			   //else std::cout << "helloooooooo" << std::endl;
																																			   //*****//
	imshow(result_window, img_matches);
	//double width = std::max(cv::norm(scene_corners[0] - scene_corners[3]), cv::norm(scene_corners[1] - scene_corners[2]));
	//double height = std::max(cv::norm(scene_corners[0] - scene_corners[1]), cv::norm(scene_corners[3] - scene_corners[2]));
	const char* cropPath = "C:\\Users\\Priyanka\\Desktop\\imageresource\\final.tif";
	////printf("%f, %f, %f, %f\n", std::min(scene_corners[0].x, std::min(scene_corners[3].x, std::min(scene_corners[1].x, scene_corners[2].x))), std::min(scene_corners[0].y, std::min(scene_corners[3].y, std::min(scene_corners[1].y, scene_corners[2].y))),
	////std::max(scene_corners[0].x, std::max(scene_corners[3].x, std::max(scene_corners[1].x, scene_corners[2].x))), std::max(scene_corners[0].y, std::max(scene_corners[3].y, std::max(scene_corners[1].y, scene_corners[2].y))));
	printf("%f, %f, %f, %f\n", scene_corners[0].x, scene_corners[0].y, scene_corners[2].x, scene_corners[2].y);
	//crop(argv[2], cropPath, (double)scene_corners[3].x, (double)scene_corners[3].y, width, height);
	double xMin = std::min(scene_corners[0].x, std::min(scene_corners[3].x, std::min(scene_corners[1].x, scene_corners[2].x)));
	double yMin = std::min(scene_corners[0].y, std::min(scene_corners[3].y, std::min(scene_corners[1].y, scene_corners[2].y)));
	double xMax = std::max(scene_corners[0].x, std::max(scene_corners[3].x, std::max(scene_corners[1].x, scene_corners[2].x)));
	double yMax = std::max(scene_corners[0].y, std::max(scene_corners[3].y, std::max(scene_corners[1].y, scene_corners[2].y)));

	//*****//crop2(argv[2], cropPath, &xMin, &yMin, &xMax, &yMax);

	//coordinates(argv[2], cropPath, &xMin, &yMin, &xMax, &yMax);
	//crop(argv[2], cropPath, &xMin, &yMin, (xMax-xMin), (yMax-yMin));
	//warpTry(argv[2], argv[1], &xMin, &yMin, &xMax, &yMax);
	double ulx = scene_corners[0].x;
	double uly = scene_corners[0].y;
	double lrx = scene_corners[2].x;
	double lry = scene_corners[2].y;


	printCoords(argv[2], &ulx, &uly, &lrx, &lry);

	std::clock_t stop;
	stop = clock();
	std::cout << "time: " << (stop - start) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
	waitKey(0);
	return 0;
}

/** @function readme */
void readme()
{
	std::cout << " Usage: ./SURF_descriptor <img1> <img2>" << std::endl;
}

void crop2(const char *inputPath, const char *cropPath, double *topLeftX, double *topLeftY, double *bottomRightX, double *bottomRightY) {
	double nPixels = *bottomRightX - *topLeftX;
	double nLines = *bottomRightY - *topLeftY;
	VRTDataset *vroutput = (VRTDataset*)VRTCreate(nPixels, nLines);
	GDALDataset *input = (GDALDataset *)GDALOpen(inputPath, GA_ReadOnly);

	// Copy dataset info
	const char* pszProjection = input->GetProjectionRef();
	if (pszProjection != NULL && strlen(pszProjection) > 0)
		vroutput->SetProjection(pszProjection);

	double adfGeoTransform[6];
	if (input->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		// Adapt the geotransform matrix to the subarea
		adfGeoTransform[0] += *topLeftX * adfGeoTransform[1]
			+ *topLeftY * adfGeoTransform[2];
		adfGeoTransform[3] += *topLeftX * adfGeoTransform[4]
			+ *topLeftY * adfGeoTransform[5];

		vroutput->SetGeoTransform(adfGeoTransform);
	}

	vroutput->SetMetadata(input->GetMetadata());
	float *pafScanline;
	pafScanline = (float *)CPLMalloc(sizeof(float)*nPixels*nLines);
	GDALDataType eBandType;

	for (int i = 0; i < input->GetRasterCount(); ++i)
	{
		GDALRasterBand* poSrcBand = input->GetRasterBand(i + 1);
		eBandType = poSrcBand->GetRasterDataType();

		poSrcBand->RasterIO(GF_Read, (int)*topLeftX, (int)*topLeftY, (int)nPixels, (int)nLines, pafScanline, (int)nPixels, (int)nLines, eBandType, 0, 0, NULL);
		vroutput->AddBand(eBandType, NULL);
		VRTSourcedRasterBand* poVRTBand = (VRTSourcedRasterBand*)vroutput->GetRasterBand(i + 1);
		/*poVRTBand->AddSimpleSource(poSrcBand,
		*topLeftX, *topLeftX,
		nPixels, nLines,
		0, 0, nPixels, nLines);
		poVRTBand->CopyCommonInfoFrom(poSrcBand);*/
		//poVRTBand->IRasterIO(GF_Write, 0, 0, (int)nPixels, (int)nLines,
		//pafScanline, (int)nPixels, (int)nLines, eBandType, 0, 0, NULL);
		poVRTBand->SetMetadata(poSrcBand->GetMetadata());


	}
	GDALDriver *pDriver = (GDALDriver *)GDALGetDriverByName("GTiff");
	GDALDataset *output;
	output = pDriver->CreateCopy(cropPath, vroutput, FALSE, NULL, NULL, NULL);
	GDALRasterBand* poDstBand = output->GetRasterBand(1);
	poDstBand->RasterIO(GF_Write, 0, 0, (int)nPixels, (int)nLines,
		pafScanline, (int)nPixels, (int)nLines, eBandType, 0, 0, NULL);
	GDALClose((GDALDatasetH)output);

}

void warpTry(const char *inputPath, const char *outputPath, double *topLeftX, double *topLeftY, double *bottomRightX, double *bottomRightY) {
	double nPixels = *bottomRightX - *topLeftX;
	double nLines = *bottomRightY - *topLeftY;
	VRTDataset *vroutput = (VRTDataset*)VRTCreate(nPixels, nLines);
	GDALDataset *input = (GDALDataset *)GDALOpen(inputPath, GA_ReadOnly);
	VRTDataset *vroutput2 = (VRTDataset*)GDALOpen(inputPath, GA_Update);

	// Copy dataset info
	const char* pszProjection = input->GetProjectionRef();
	if (pszProjection != NULL && strlen(pszProjection) > 0)
		vroutput->SetProjection(pszProjection);

	double adfGeoTransform[6];
	if (input->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		// Adapt the geotransform matrix to the subarea
		adfGeoTransform[0] += *topLeftX * adfGeoTransform[1]
			+ *topLeftY * adfGeoTransform[2];
		adfGeoTransform[3] += *topLeftX * adfGeoTransform[4]
			+ *topLeftY * adfGeoTransform[5];

		vroutput->SetGeoTransform(adfGeoTransform);
	}

	vroutput->SetMetadata(input->GetMetadata());
	float *pafScanline;
	pafScanline = (float *)CPLMalloc(sizeof(float)*nPixels*nLines);
	GDALDataType eBandType;

	for (int i = 0; i < input->GetRasterCount(); ++i)
	{
		GDALRasterBand* poSrcBand = input->GetRasterBand(i + 1);
		eBandType = poSrcBand->GetRasterDataType();

		poSrcBand->RasterIO(GF_Read, (int)*topLeftX, (int)*topLeftY, (int)nPixels, (int)nLines, pafScanline, (int)nPixels, (int)nLines, eBandType, 0, 0, NULL);
		vroutput->AddBand(eBandType, NULL);
		VRTSourcedRasterBand* poVRTBand = (VRTSourcedRasterBand*)vroutput->GetRasterBand(i + 1);
		/*poVRTBand->AddSimpleSource(poSrcBand,
		*topLeftX, *topLeftX,
		nPixels, nLines,
		0, 0, nPixels, nLines);
		poVRTBand->CopyCommonInfoFrom(poSrcBand);*/
		//poVRTBand->IRasterIO(GF_Write, 0, 0, (int)nPixels, (int)nLines,
		//pafScanline, (int)nPixels, (int)nLines, eBandType, 0, 0, NULL);
		poVRTBand->SetMetadata(poSrcBand->GetMetadata());


	}
	GDALDriver *pDriver = (GDALDriver *)GDALGetDriverByName("GTiff");
	GDALDataset *output1, *output;
	output = pDriver->CreateCopy("temp.tif", vroutput2, FALSE, NULL, NULL, NULL); //APPEND_SUBDATASET=YES
																				  //output = pDriver->CreateCopy("temp.tif", vroutput, FALSE, { "APPEND_SUBDATASET" }, NULL, NULL);

	GDALRasterBand* poDstBand = output->GetRasterBand(1);
	poDstBand->RasterIO(GF_Write, 0, 0, (int)nPixels, (int)nLines,
		pafScanline, (int)nPixels, (int)nLines, eBandType, 0, 0, NULL);
	GDALClose((GDALDatasetH)output);

	GDALClose((GDALDatasetH)input);
	GDALClose((GDALDatasetH)output);

}

void coordinates(const char *inputPath, const char *cropPath, double *topLeftX, double *topLeftY, double *bottomRightX, double *bottomRightY) {
	GDALDataset *input, *output;
	GDALDriver *hDriver;
	input = (GDALDataset *)GDALOpen(inputPath, GA_ReadOnly);
	hDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	OGRSpatialReference oSRS;
	const char *pszSrcWKT;
	char *pszDstWKT = NULL;
	pszSrcWKT = GDALGetProjectionRef(input);

	oSRS.SetUTM(23, FALSE);
	oSRS.SetWellKnownGeogCS("WGS84");
	oSRS.exportToWkt(&pszDstWKT);
	double GT[6];
	double nPixels = *bottomRightX - *topLeftX;
	double nLines = *bottomRightY - *topLeftY;

	GDALCreateAndReprojectImage(input, pszSrcWKT, cropPath, pszDstWKT, hDriver, NULL, GRA_NearestNeighbour, 0.0, 0.0,
		NULL, NULL, NULL);
	output = (GDALDataset *)GDALOpen(cropPath, GA_Update);

	// Setup warp options.
	GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();
	psWarpOptions->hSrcDS = input;
	psWarpOptions->hDstDS = output;
	psWarpOptions->nBandCount = 1;
	psWarpOptions->panSrcBands =
		(int *)CPLMalloc(sizeof(int) * psWarpOptions->nBandCount);
	psWarpOptions->panSrcBands[0] = 1;
	psWarpOptions->panDstBands =
		(int *)CPLMalloc(sizeof(int) * psWarpOptions->nBandCount);
	psWarpOptions->panDstBands[0] = 1;
	psWarpOptions->pfnProgress = GDALTermProgress;
	// Establish reprojection transformer.
	psWarpOptions->pTransformerArg =
		GDALCreateGenImgProjTransformer(input,
			GDALGetProjectionRef(input),
			output,
			GDALGetProjectionRef(output),
			FALSE, 0.0, 1);
	psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
	psWarpOptions->eResampleAlg = GRA_NearestNeighbour;
	psWarpOptions->eWorkingDataType = GDT_UInt16;
	//psWarpOptions->papszWarpOptions = nullptr;
	// Initialize and execute the warp operation.
	GDALWarpOperation oOperation;
	oOperation.Initialize(psWarpOptions);
	oOperation.ChunkAndWarpImage((int)*topLeftX, (int)*topLeftY, (int)nPixels, (int)nLines);
	GDALClose((GDALDatasetH)output);

	//gdalwarp -te


	/*
	if (input->GetGeoTransform(GT) == CE_None) {
	double topLeftXGeo = GT[0] + *topLeftX*GT[1] + *topLeftY*GT[2];
	double topLeftYGeo = GT[3] + *topLeftX*GT[4] + *topLeftY*GT[5];
	double bottomRightXGeo = GT[0] + *bottomRightX*GT[1] + *bottomRightY*GT[2];
	double bottomRightYGeo = GT[3] + *bottomRightX*GT[4] + *bottomRightY*GT[5];
	printf("%f, %f, %f, %f \n", topLeftXGeo, topLeftYGeo, bottomRightXGeo, bottomRightYGeo);

	output = hDriver->Create(cropPath, nPixels, nLines, 1, GDT_UInt16, NULL);

	///void *hTransformArg;
	///hTransformArg =
	///GDALCreateGenImgProjTransformer(input, pszSrcWKT, NULL, pszDstWKT, FALSE, 0, 1);
	output->SetProjection(pszDstWKT);
	output->SetGeoTransform(GT);

	//GDALWarpOperation *poOperation;
	//poOperation = new GDALWarpOperation;
	//const char *cutline = "CUTLINE = ";

	std::string cutlin = "CUTLINE = " + std::to_string(topLeftXGeo) + ", " + std::to_string(bottomRightYGeo)
	+ ", " + std::to_string(bottomRightXGeo) + ", " + std::to_string(topLeftYGeo) + "";
	char *cutline2 = new char;
	cutline2 = &cutlin[0u];

	GDALWarpOptions *reprojectOptions = new GDALWarpOptions;
	char** saveme = { &cutline2 };
	reprojectOptions->papszWarpOptions = new char *[1];
	*reprojectOptions->papszWarpOptions = cutline2;
	reprojectOptions->papszWarpOptions[1] = NULL;
	//testing(reprojectOptions);
	//printf("%d", strlen(*test));

	//GDALReprojectImage(input, pszSrcWKT, output, pszDstWKT, GRA_NearestNeighbour, 0.0, 0.0, NULL, NULL, NULL);

	//GDALClose((GDALDatasetH)input);
	//delete[] reprojectOptions->papszWarpOptions;
	//GDT_UInt16
	}
	*/

}
void testStrd(const char *str) {
	//printf("%d", strlen(str));
}

void testDupli(char **list) {
	char **p = list;
	int nItems = 0;

	while (*p != NULL)
	{
		++nItems;
		printf(*p);
		++p;
	}

	printf("Items: %d \n", nItems);
	testStrd(*p);
}

void testing(const GDALWarpOptions *options) {

	testDupli(options->papszWarpOptions);
}

void crop(const char *inputPath, const char *cropPath, double *topLeftX, double *topLeftY,
	double width, double height)
{
	GDALDriverH hDriver;
	GDALDataType eDT;
	GDALDatasetH hDstDS;
	GDALDatasetH hSrcDS;
	// Open the source file.
	hSrcDS = GDALOpen(inputPath, GA_ReadOnly);
	CPLAssert(hSrcDS != NULL);
	// Create output with same datatype as first input band.
	eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS, 1));
	// Get output driver (GeoTIFF format)
	hDriver = GDALGetDriverByName("GTiff");
	CPLAssert(hDriver != NULL);
	// Get Source coordinate system.
	const char *pszSrcWKT;
	char *pszDstWKT = NULL;
	pszSrcWKT = GDALGetProjectionRef(hSrcDS);
	CPLAssert(pszSrcWKT != NULL && strlen(pszSrcWKT) > 0);
	// Setup output coordinate system that is UTM 23S WGS84.
	OGRSpatialReference oSRS;
	oSRS.SetUTM(23, FALSE);
	oSRS.SetWellKnownGeogCS("WGS84");
	oSRS.exportToWkt(&pszDstWKT);
	// Create a transformer that maps from source pixel/line coordinates
	// to destination georeferenced coordinates (not destination
	// pixel line).  We do that by omitting the destination dataset
	// handle (setting it to NULL).
	void *hTransformArg;
	hTransformArg =
		GDALCreateGenImgProjTransformer(hSrcDS, pszSrcWKT, NULL, pszDstWKT,
			TRUE, 0, 0);
	CPLAssert(hTransformArg != NULL);
	// Get approximate output georeferenced bounds and resolution for file.
	double adfDstGeoTransform[6];
	int nPixels = 0, nLines = 0;
	CPLErr eErr;
	eErr = GDALSuggestedWarpOutput(hSrcDS,
		GDALGenImgProjTransform, hTransformArg,
		adfDstGeoTransform, &nPixels, &nLines);
	CPLAssert(eErr == CE_None);
	GDALDestroyGenImgProjTransformer(hTransformArg);
	// Create the output file.
	hDstDS = GDALCreate(hDriver, cropPath, nPixels, nLines,
		GDALGetRasterCount(hSrcDS), eDT, NULL);
	CPLAssert(hDstDS != NULL);
	// Write out the projection definition.
	GDALSetProjection(hDstDS, pszDstWKT);
	GDALSetGeoTransform(hDstDS, adfDstGeoTransform);
	GDALClose((GDALDatasetH)hDstDS);
}

void printCoords(const char *inputPath, double *topLeftX, double *topLeftY, double *bottomRightX, double *bottomRightY) {

	GDALDataset *input, *output;
	GDALDriver *hDriver;
	input = (GDALDataset *)GDALOpen(inputPath, GA_ReadOnly);
	double GT[6];
	if (input->GetGeoTransform(GT) == CE_None) {
		double topLeftXGeo = GT[0] + *topLeftX*GT[1] + *topLeftY*GT[2];
		double topLeftYGeo = GT[3] + *topLeftX*GT[4] + *topLeftY*GT[5];
		double bottomRightXGeo = GT[0] + *bottomRightX*GT[1] + *bottomRightY*GT[2];
		double bottomRightYGeo = GT[3] + *bottomRightX*GT[4] + *bottomRightY*GT[5];
		printf("%f, %f, %f, %f \n", topLeftXGeo, topLeftYGeo, bottomRightXGeo, bottomRightYGeo);
	}
}



