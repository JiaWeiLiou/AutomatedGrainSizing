#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>

using namespace std;
using namespace cv;

// Minimum watershed segment label
#define LABEL_MINIMUM 0
// Label for the watersheds pixels (local minima)
#define LABEL_RIDGE -1
// Label for pixels that are not local minima
#define LABEL_NOLOCALMINIMUM -2
// Label for pixels that are or have been in the OrderedQueques
#define LABEL_PROCESSING -3
// Label for unprocessed pixels
#define LABEL_UNPROCESSED -4

//Find Root
//labeltable : labels table
//label : label value for look up
//return : the root of the label
int findroot(int labeltable[], int label);

//Find Connect Object 
//binary : binary image(8UC1)
//labelImg : labels image(32SC1)
//nears : nears type(4/6/8)
//return : labels total number without 0(background)
int bwlabel(InputArray _binary, OutputArray _labelImg, int nears);

//Remove Ambient Light for Area
//image : raw image (8UC3)
//gray : gray image (8UC1)
void RGBToGray(InputArray _image, OutputArray _gray);

//Box Blur
//gray : gray image (8UC1)
//blur : blur image (8UC1)
//r : radius of box
void boxBlurM(InputArray _gray, OutputArray _blur, size_t r);

//Fast Gaussian Blur
//gray : gray image (8UC1)
//blur : blur image (8UC1)
//sigma : sigma of gaussian kernal
//n : repeat number
void GaussianBlurF(InputArray _gray, OutputArray _blur, const double sigma, size_t n);

//Remove Ambient Light for Area
//gray : gray image (8UC1)
//blur : blur image (8UC1)
//divide : remove ambient light image (8UC1)
void DivideArea(InputArray _gray, InputArray _blur, OutputArray _divide);

//Kittler Threshold for Area
//gray : gray image (8UC1)
//binary : binary image (8UC1(BW))
void KittlerThresholdArea(InputArray _gray, OutputArray _binary);

//Calculate Image Gradient
//gray : gray image(8UC1)
//gradient : gradient image(8UC1)
void Gradient(InputArray _gray, OutputArray _gradient);

//Remove Ambient Light And Binary for Image Gradient
//gradient : gradient image(8UC1)
//blur : blur gradient image (8UC1)
//divide : remove ambient light gradient image (8UC1(BW))
void DivideLineBinary(InputArray _gradient, InputArray _blur, OutputArray _divide);

//Hysteresis Cut Binary Image to Line by Area
//binary : binary image (8UC1(BW))
//area : area binary image (8UC1(BW))
//line : line binary image (8UC1(BW))
void HysteresisCut(InputArray _binary, InputArray _area, OutputArray _line);

//Combine Area and Line Extraction
//area : area binary image (8UC1(BW))
//line : line binary image (8UC1(BW))
//binary : combine binary image (8UC1(BW))
void Combine(InputArray _area, InputArray _line, OutputArray _binary);

//Clear Black Noise
//binary : binary image (8UC1(BW))
//clear : clear noise image (8UC1(BW))
//mumax : max grain size of image
void ClearNoise(InputArray _binary, OutputArray _clear, float mumax);

//Image Reconstruct
//marker : marker image (32FC1)
//mask : mask image (32FC1)
void Reconstruct(InputArray _marker, InputArray _mask, OutputArray _hdistance);

//H Minima Transform
//distance : distance image (32FC1)
//hdistance : distance image with h minima transform (32FC1)
//h : height
void HMinimaTransform(InputArray _distance, OutputArray _hdistance, float h);

//Detect Regional Minima
//distance : distance image (32FC1)
//seed : seed image (8UC1(BW))
void DetectRegionalMinima(InputArray _distance, OutputArray _seed);

//Extend Regional Minima
//distance : distance image (32FC1)
//seed : seed image (8UC1(BW))
//h : height
void ExtendRegionalMinima(InputArray _distance, OutputArray _seed, float h);

//Distance Cut
//distance : distance image (32FC1)
//seed : seed image (8UC1(BW))
void DistanceCut(InputArray _distance, OutputArray _seed);

//Add Unseed Labels
//binary : binary image (8UC1(BW))
//seed : seed image (8UC1(BW))
//fseed : finial Seed
void AddSeed(InputArray _binary, InputArray _seed, OutputArray _fseed);

class PixelElement
{
public:
	PixelElement(float k, int j, int i) {
		value = k;
		x = j;
		y = i;
	}
	float value;  ///< pixel intensity
	int x;      ///< x coordinate of pixel
	int y;      ///< y coordinate of pixel
};

class mycomparison
{
	bool reverse;
public:
	/** \brief constructor
	* \param revparam true if we want to sort from large to small*/
	mycomparison(const bool& revparam = true)
	{
		reverse = revparam;
	}

	/** \brief comparison operator: compares the values of two pixel elements
	* \param lhs first pixel element
	* \param rhs second pixel element*/
	bool operator() (PixelElement& lhs, PixelElement& rhs) const
	{
		if (reverse) return (lhs.value > rhs.value);
		else return (lhs.value < rhs.value);
	}
};

//Detect Minima
//distance : distance image (32FC1)
//labelImg : labels image(8UC3)
//sortedQueue : sorted queue
void DetectMinima(InputArray _distance, InputArray _seed, OutputArray _labelImg, priority_queue<PixelElement, vector<PixelElement>, mycomparison> &sortedQueue);

bool CheckForAlreadyLabeledNeighbours(int x, int y, Mat &label, Point2i &outLabeledNeighbour, int &outLabel);

bool CheckIfPixelIsWatershed(int x, int y, Mat &label, Point2i &inLabeledNeighbour);

// Watershed Transform
//binary : binary image (8UC1(BW))
//seed : seed image (8UC1(BW))
//distance : distance image (32FC1)
//object : object image (8UC1(BW))
void WatershedTransform(InputArray _binary, InputArray _seed, InputArray _distance, OutputArray _object);

//Delete Edge object
//binary : binary image (8UC1(BW))
//object : object image (8UC1(BW))
void DeleteEdge(InputArray _binary, OutputArray _object);

//Fitting Ellipse
//object : object image(8UC1(BW))
//ellipse_M : short axis of ellipse
//ellipse_L : long axis of ellipse
void FitEllipse(InputArray _object, vector<float> &ellipse_M, vector<float> &ellipse_L);

void AutomatedGrainSizing(Mat image, Point2i realSize, int mumax, vector<float> &ellipse_M, vector<float> &ellipse_L);