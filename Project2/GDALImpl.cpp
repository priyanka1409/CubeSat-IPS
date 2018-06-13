#include "GDALImpl.h"
#include "CmdImpl.h"
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <algorithm> 
#include <vector>
#include <ctime>
#include <string.h>
#include "stdlib.h"
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
#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "osr_cs_wkt.h"

using namespace std;

void GDALImpl::transformCoords(std::string outputPath, const char* inputPath, std::string fileName, double * topLeftX, double * topLeftY, double * bottomRightX, double * bottomRightY) {
	GDALDataset *input;
	GDALAllRegister();
	GDALDriver *hDriver;
	hDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	//inputPath = "C:\\Users\\user\\Desktop\\TestCases\\7731\\T012017070314332300A_L2.tif";
	input = (GDALDataset *)GDALOpen(inputPath, GA_ReadOnly);
	double GT[6];
	OGRSpatialReference oSRS;
	const char* pszWkt = input->GetProjectionRef();
	char *pszWkt_tmp = (char *)pszWkt;
	oSRS.importFromWkt(&pszWkt_tmp);
	//printf(input->GetProjectionRef());
	int EPSG = atoi(oSRS.GetAttrValue("AUTHORITY", 1));
	printf("EPSG: %d ", EPSG);
	if (input != NULL) {
		if (input->GetGeoTransform(GT) == CE_None) {

			double topLeftXGeo = GT[0] + *topLeftX*GT[1] + *topLeftY*GT[2];
			double topLeftYGeo = GT[3] + *topLeftX*GT[4] + *topLeftY*GT[5];
			double bottomRightXGeo = GT[0] + *bottomRightX*GT[1] + *bottomRightY*GT[2];
			double bottomRightYGeo = GT[3] + *bottomRightX*GT[4] + *bottomRightY*GT[5];
			CmdImpl c;
			c.createCommands(outputPath, inputPath, fileName, topLeftXGeo, topLeftYGeo, bottomRightXGeo, bottomRightYGeo, EPSG);
		}
	}
	else printf("OOPSIE");
}

void GDALImpl::transformCoords2(std::string outputPath, const char* inputPath, std::string ogFilePath, std::string fileName, int width, int height, cv::Point2f topLeft, cv::Point2f topRight, cv::Point2f bottomLeft, cv::Point2f bottomRight, cv::Point2f center, std::clock_t start) {
	GDALDataset *input;
	GDALAllRegister();
	GDALDriver *hDriver;
	hDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	//inputPath = "C:\\Users\\user\\Desktop\\TestCases\\7731\\T012017070314332300A_L2.tif";
	input = (GDALDataset *)GDALOpen(inputPath, GA_ReadOnly);
	double GT[6];
	OGRSpatialReference oSRS;
	const char* pszWkt = input->GetProjectionRef();
	char *pszWkt_tmp = (char *)pszWkt;
	oSRS.importFromWkt(&pszWkt_tmp);
	//printf(pszWkt);
	int EPSG = atoi(oSRS.GetAttrValue("AUTHORITY", 1));
	if (input != NULL) {
		if (input->GetGeoTransform(GT) == CE_None) {

			double topLeftXGeo = GT[0] + topLeft.x*GT[1] + topLeft.y*GT[2];
			double topLeftYGeo = GT[3] + topLeft.x*GT[4] + topLeft.y*GT[5];

			double topRightXGeo = GT[0] + topRight.x*GT[1] + topRight.y*GT[2];
			double topRightYGeo = GT[3] + topRight.x*GT[4] + topRight.y*GT[5];

			double bottomLeftXGeo = GT[0] + bottomLeft.x*GT[1] + bottomLeft.y*GT[2];
			double bottomLeftYGeo = GT[3] + bottomLeft.x*GT[4] + bottomLeft.y*GT[5];

			double bottomRightXGeo = GT[0] + bottomRight.x*GT[1] + bottomRight.y*GT[2];
			double bottomRightYGeo = GT[3] + bottomRight.x*GT[4] + bottomRight.y*GT[5];

			double centerXGeo = GT[0] + center.x*GT[1] + center.y*GT[2];
			double centerYGeo = GT[3] + center.x*GT[4] + center.y*GT[5];
			
			CmdImpl c;
			
			c.createCommands2(outputPath, ogFilePath, fileName, width, height, topLeftXGeo, topLeftYGeo, 
				topRightXGeo, topRightYGeo, bottomLeftXGeo, bottomLeftYGeo, bottomRightXGeo, 
				bottomRightYGeo, centerXGeo, centerYGeo, EPSG, start);

			std::string temp = outputPath + "\\" + fileName;
			const char * outFilePath = new char[temp.length() + 1];
			outFilePath = temp.c_str();

			assignProj(outFilePath, pszWkt);
		}
	}
}

void GDALImpl::assignProj(const char* outFilePath, const char* projection) {
	GDALDataset *output;
	output = (GDALDataset *)GDALOpen(outFilePath, GA_Update);

	output->SetProjection(projection);

	GDALClose((GDALDatasetH)output);

}

std::string GDALImpl::changeSlashes(std::string text) {
	for (int i = 0; i < text.size(); i++)
	{
		if (text[i] == '\\')
		{
			text.insert(i, "\\");
			i++;
		}
	}
	//printf(text.c_str());
	return text;
}

