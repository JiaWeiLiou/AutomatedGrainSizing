#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <QtWidgets/QDialog>
#include <QProgressDialog>

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

class AutomatedGrainSizing : public QDialog
{
	Q_OBJECT

public:
	AutomatedGrainSizing(QWidget *parent = Q_NULLPTR);
	QProgressDialog *progressDialog;
	int num = 0;

	//Find Root
	int findroot(int labeltable[], int label);
	//Find Connect Object 
	int bwlabel(InputArray _binary, OutputArray _labelImg, int nears);
	//Remove Ambient Light for Area
	void RGBToGray(InputArray _image, OutputArray _gray);
	//Box Blur
	void boxBlurM(InputArray _gray, OutputArray _blur, size_t r);
	//Fast Gaussian Blur
	void GaussianBlurF(InputArray _gray, OutputArray _blur, const double sigmam ,int n);
	//Remove Ambient Light for Area
	void DivideArea(InputArray _gray, InputArray _blur, OutputArray _divide);
	//Kittler Threshold for Area
	void KittlerThresholdArea(InputArray _gray, OutputArray _binary);
	//Calculate Image Gradient
	void Gradient(InputArray _gray, OutputArray _gradient);
	//Remove Ambient Light And Binary for Image Gradient
	void DivideLineBinary(InputArray _gradient, InputArray _blur, OutputArray _divide);
	//Hysteresis Cut Binary Image to Line by Area
	void HysteresisCut(InputArray _binary, InputArray _area, OutputArray _line);
	//Combine Area and Line Extraction
	void Combine(InputArray _area, InputArray _line, OutputArray _binary);
	//Clear Black Noise
	void ClearNoise(InputArray _binary, OutputArray _clear, float mumax);
	//Image Reconstruct
	void Reconstruct(InputArray _marker, InputArray _mask, OutputArray _hdistance);
	//H Minima Transform
	void HMinimaTransform(InputArray _distance, OutputArray _hdistance, float h);
	//Detect Regional Minima
	void DetectRegionalMinima(InputArray _distance, OutputArray _seed);
	//Extend Regional Minima
	void ExtendRegionalMinima(InputArray _distance, OutputArray _seed, float h);
	//Distance Cut
	void DistanceCut(InputArray _distance, OutputArray _seed);
	//Add Unseed Labels
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
	void DetectMinima(InputArray _distance, InputArray _seed, OutputArray _labelImg, priority_queue<PixelElement, vector<PixelElement>, mycomparison> &sortedQueue);
	bool CheckForAlreadyLabeledNeighbours(int x, int y, Mat &label, Point2i &outLabeledNeighbour, int &outLabel);
	bool CheckIfPixelIsWatershed(int x, int y, Mat &label, Point2i &inLabeledNeighbour);
	// Watershed Transform
	void WatershedTransform(InputArray _binary, InputArray _seed, InputArray _distance, OutputArray _object);
	//Delete Edge object
	void DeleteEdge(InputArray _binary, OutputArray _object);
	//Fitting Ellipse
	void FitEllipse(InputArray _object, vector<float> &ellipse_M, vector<float> &ellipse_L);

	void DoAutomatedGrainSizing(Mat image, Point2i realSize, int mumax, vector<float> &ellipse_M, vector<float> &ellipse_L);
};