#include "automatedgrainsizing.h"

AutomatedGrainSizing::AutomatedGrainSizing(QWidget *parent)
	: QDialog(parent)
{
	progressDialog = new QProgressDialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
	progressDialog->setWindowModality(Qt::ApplicationModal);
	progressDialog->setMinimumDuration(0);
	progressDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	progressDialog->setWindowTitle("Please Wait");
	progressDialog->setLabelText("Running...");
	progressDialog->setCancelButtonText("Cancel");
}

int AutomatedGrainSizing::findroot(int labeltable[], int label)
{
	int x = label;
	while (x != labeltable[x]) {
		x = labeltable[x];
	}
	return x;
}

int AutomatedGrainSizing::bwlabel(InputArray _binary, OutputArray _labelImg, int nears)
{
	Mat binary = _binary.getMat();
	CV_Assert(binary.type() == CV_8UC1);

	_labelImg.create(binary.size(), CV_32SC1);
	Mat labelImg = _labelImg.getMat();
	labelImg = Scalar(0);

	if (nears != 4 && nears != 6 && nears != 8) { nears = 8; }

	int nobj = 0;    // number of objects found in image  
	int *labeltable = new int[binary.rows*binary.cols]();		// initialize label table with zero  
	int ntable = 0;

	//	labeling scheme
	//	+ - + - + - +
	//	| D | C | E |
	//	+ - + - + - +
	//	| B | A |   |
	//	+ - + - + - +
	//	A is the center pixel of a neighborhood.In the 3 versions of connectedness :
	//	4 : A connects to B and C
	//	6 : A connects to B, C, and D
	//	8 : A connects to B, C, D, and E

	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			if (binary.at<uchar>(i, j)) {   // if A is an object  
											// get the neighboring labels B, C, D, and E
				int B = (!j) ? 0 : findroot(labeltable, labelImg.at<int>(i, j - 1));
				int C = (!i) ? 0 : findroot(labeltable, labelImg.at<int>(i - 1, j));
				int D = (!i || !j) ? 0 : findroot(labeltable, labelImg.at<int>(i - 1, j - 1));
				int E = (!i || j == binary.cols - 1) ? 0 : findroot(labeltable, labelImg.at<int>(i - 1, j + 1));

				// apply 4 connectedness  
				if (nears == 4) {
					if (B && C) {	// B and C are labeled  
						labelImg.at<int>(i, j) = B;
						if (B != C) { labeltable[C] = B; }
					} else if (B) { // B is object but C is not  
						labelImg.at<int>(i, j) = B;
					} else if (C) {	// C is object but B is not  
						labelImg.at<int>(i, j) = C;
					} else {	// B, C, D not object - new object label and put into table  
						labelImg.at<int>(i, j) = labeltable[ntable] = ++ntable;
					}
					// apply 6 connected ness  
				} else if (nears == 6) {
					if (D) {	// D object, copy label and move on  
						labelImg.at<int>(i, j) = D;
					} else if (B && C) {	// B and C are labeled  
						if (B == C) {
							labelImg.at<int>(i, j) = B;
						} else {
							int tlabel = B < C ? B : C;
							labeltable[B] = tlabel;
							labeltable[C] = tlabel;
							labelImg.at<int>(i, j) = tlabel;
						}
					} else if (B) {	// B is object but C is not  
						labelImg.at<int>(i, j) = B;
					} else if (C) {	// C is object but B is not   
						labelImg.at<int>(i, j) = C;
					} else { // B, C, D not object - new object label and put into table
						labelImg.at<int>(i, j) = labeltable[ntable] = ++ntable;
					}
					// apply 8 connectedness  
				} else if (nears == 8) {
					if (B || C || D || E) {
						int tlabel;

						if (B) {
							tlabel = B;
						} else if (C) {
							tlabel = C;
						} else if (D) {
							tlabel = D;
						} else if (E) {
							tlabel = E;
						}

						labelImg.at<int>(i, j) = tlabel;

						if (B && B != tlabel) { labeltable[B] = tlabel; }
						if (C && C != tlabel) { labeltable[C] = tlabel; }
						if (D && D != tlabel) { labeltable[D] = tlabel; }
						if (E && E != tlabel) { labeltable[E] = tlabel; }
					} else { // label and put into table
						labelImg.at<int>(i, j) = labeltable[ntable] = ++ntable;
					}
				}
			} else { // A is not an object so leave it
				labelImg.at<int>(i, j) = 0;
			}
		}
	}

	// consolidate component table  
	for (size_t i = 0; i <= ntable; ++i) {
		labeltable[i] = findroot(labeltable, i);
	}

	// run image through the look-up table  
	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			labelImg.at<int>(i, j) = labeltable[labelImg.at<int>(i, j)];
		}
	}

	// count up the objects in the image 
	//clear all table label
	for (size_t i = 0; i <= ntable; ++i) {
		labeltable[i] = 0;
	}
	//calculate all label numbers
	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			++labeltable[labelImg.at<int>(i, j)];
		}
	}

	labeltable[0] = 0;		//clear 0 label
							// number the objects from 1 through n objects  and reset label table
	for (size_t i = 1; i <= ntable; ++i) {
		if (labeltable[i] > 0) {
			labeltable[i] = ++nobj;
		}
	}

	// run through the look-up table again  
	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			labelImg.at<int>(i, j) = labeltable[labelImg.at<int>(i, j)];
		}
	}

	delete[] labeltable;

	return nobj;
}

void AutomatedGrainSizing::RGBToGray(InputArray _image, OutputArray _gray)
{
	Mat image = _image.getMat();
	_gray.create(image.size(), CV_8UC1);
	Mat gray = _gray.getMat();

	if (image.channels() == 3) {
		for (size_t i = 0; i < gray.rows; ++i) {
			for (size_t j = 0; j < gray.cols; ++j) {
				gray.at<uchar>(i, j) = ((double)image.at<Vec3b>(i, j)[0] + (double)image.at<Vec3b>(i, j)[1] + (double)image.at<Vec3b>(i, j)[2]) / 3;
			}
		}
	} else if (image.channels() == 4) {
		for (size_t i = 0; i < gray.rows; ++i) {
			for (size_t j = 0; j < gray.cols; ++j) {
				gray.at<uchar>(i, j) = ((double)image.at<Vec4b>(i, j)[0] + (double)image.at<Vec4b>(i, j)[1] + (double)image.at<Vec4b>(i, j)[2]) / 3;
			}
		}
	} else {
		image.copyTo(gray);
	}
}

void AutomatedGrainSizing::boxBlurM(InputArray _gray, OutputArray _blur, size_t r)
{
	Mat gray = _gray.getMat();
	_blur.create(gray.size(), CV_8UC1);
	Mat blur = _blur.getMat();

	double iarr = 1.0f / (r + r + 1);
	Mat blurH(gray.size(), CV_8UC1, Scalar(0));

	//Horizontal
	for (size_t i = 0; i < gray.rows; ++i) {
		double val = 0;
		// first value
		for (size_t j = 1; j < r + 1; ++j) {
			val += gray.at<uchar>(i, j);
		}
		for (size_t j = 0; j < r - 1; ++j) {
			val += gray.at<uchar>(i, j);
		}

		// from 0 to cols
		for (size_t j = 0; j < gray.cols; ++j) {
			int j1 = gray.cols - 1 - abs((int)(j - gray.cols + 1 + r));
			int j2 = abs((int)(r - j + 1));
			val += gray.at<uchar>(i, j1) - gray.at<uchar>(i, j2);
			blurH.at<uchar>(i, j) = round(val * iarr);
		}
	}

	//Vertical
	for (size_t j = 0; j < gray.cols; ++j) {
		double val = 0;
		// first value
		for (size_t i = 1; i < r + 1; ++i) {
			val += blurH.at<uchar>(i, j);
		}
		for (size_t i = 0; i < r - 1; ++i) {
			val += blurH.at<uchar>(i, j);
		}

		// from 0 to rows
		for (size_t i = 0; i < gray.rows; ++i) {
			int i1 = gray.rows - 1 - abs((int)(i - gray.rows + 1 + r));
			int i2 = abs((int)(r - i + 1));
			val += blurH.at<uchar>(i1, j) - blurH.at<uchar>(i2, j);
			blur.at<uchar>(i, j) = round(val * iarr);
		}
	}
}

bool AutomatedGrainSizing::GaussianBlurF(InputArray _gray, OutputArray _blur, const double sigma, int n)
{
	Mat gray = _gray.getMat();
	_blur.create(gray.size(), CV_8UC1);
	Mat blur = _blur.getMat();

	float wIdeal = sqrt((12 * sigma*sigma / n) + 1);  // Ideal averaging filter width 
	int wl = floor(wIdeal);
	if (wl % 2 == 0) --wl;
	int wu = wl + 2;

	float mIdeal = (12 * sigma*sigma - n*wl*wl - 4 * n*wl - 3 * n) / (-4 * wl - 4);
	int m = round(mIdeal);
	// var sigmaActual = Math.sqrt( (m*wl*wl + (n-m)*wu*wu - n)/12 );

	Mat grayblur(gray.size(), CV_8UC1);
	gray.copyTo(grayblur);

	for (size_t i = 0; i < m + n; ++i) {
		size_t size = i < m ? wl : wu;
		boxBlurM(grayblur, blur, (size - 1) / 2);
		blur.copyTo(grayblur);

		if (progressDialog->wasCanceled()) return true;
		progressDialog->setValue(++num);
	}

	return false;
}

void AutomatedGrainSizing::DivideArea(InputArray _gray, InputArray _blur, OutputArray _divide)
{
	Mat gray = _gray.getMat();

	Mat blur = _blur.getMat();

	_divide.create(gray.size(), CV_8UC1);
	Mat divide = _divide.getMat();

	Scalar blurmean = cv::mean(blur);

	double mean = blurmean[0];

	for (size_t i = 0; i < gray.rows; ++i) {
		for (size_t j = 0; j < gray.cols; ++j) {
			double div = (double)gray.at<uchar>(i, j) - (double)blur.at<uchar>(i, j) + mean;
			div = div < 0 ? 0 : div;
			div = div > 255 ? 255 : div;
			divide.at<uchar>(i, j) = div;
		}
	}
}

void AutomatedGrainSizing::KittlerThresholdArea(InputArray _gray, OutputArray _binary)
{
	Mat gray = _gray.getMat();

	_binary.create(gray.size(), CV_8UC1);
	Mat binary = _binary.getMat();

	double *hist = new double[256]();
	for (size_t i = 0; i < gray.rows; ++i) {
		for (size_t j = 0; j < gray.cols; ++j) {
			++hist[gray.at<uchar>(i, j)];
		}
	}

	hist[0] = hist[1];
	hist[255] = hist[254];

	double sum = 0;
	for (size_t i = 0; i < 256; ++i) {
		sum += hist[i];
	}

	for (size_t i = 0; i < 256; ++i) {
		hist[i] = hist[i] / sum;
	}

	double *J = new double[256]();
	for (size_t i = 0; i < 256; ++i) {
		J[i] = DBL_MAX;
	}

	for (size_t T = 0; T < 256; ++T) {

		/* P1. P2 */

		double P1 = 0;
		for (size_t i = 0; i < T; ++i) {
			P1 += hist[i];
		}

		double P2 = 0;
		for (size_t i = T; i < 256; ++i) {
			P2 += hist[i];
		}

		if (P1 > 0 & P2 > 0) {

			/* Mu1. Sigma1 */

			double sumMu1 = 0;
			for (size_t i = 0; i < T; ++i) {
				sumMu1 += hist[i] * i;
			}
			double mu1 = sumMu1 / P1;

			double sumSigma1 = 0;
			for (size_t i = 0; i < T; ++i) {
				sumSigma1 += hist[i] * pow(i - mu1, 2);
			}
			double sigma1 = sqrt(sumSigma1 / P1);

			/* Mu2. Sigma2 */

			double sumMu2 = 0;
			for (size_t i = T; i < 256; ++i) {
				sumMu2 += hist[i] * i;
			}
			double mu2 = sumMu2 / P2;

			double sumSigma2 = 0;
			for (size_t i = T; i < 256; ++i) {
				sumSigma2 += hist[i] * pow(i - mu2, 2);
			}
			double sigma2 = sqrt(sumSigma2 / P2);

			/* J */

			if ((sigma1 > 0) & (sigma2 > 0)) {
				J[T] = 1 + 2 * (P1 * log(sigma1) + P2 * log(sigma2)) - 2 * (P1 * log(P1) + P2 * log(P2));
			}
		}
	}

	char th = 0;
	for (size_t i = 50; i < 255; ++i) {
		if (J[i] <= J[i + 1] && J[i] <= J[i - 1] && J[i + 1] != DBL_MAX && J[i - 1] != DBL_MAX) {
			th = i;
			break;
		}
	}

	delete[] hist;
	delete[] J;

	cv::threshold(gray, binary, th, 255, THRESH_BINARY);
}

void AutomatedGrainSizing::Gradient(InputArray _gray, OutputArray _gradient)
{
	Mat gray = _gray.getMat();

	_gradient.create(gray.size(), CV_8UC1);
	Mat gradient = _gradient.getMat();

	Mat grayRef;
	cv::copyMakeBorder(gray, grayRef, 1, 1, 1, 1, BORDER_REPLICATE);

	float gradx = 0.0f, grady = 0.0f;
	for (size_t i = 0; i < gradient.rows; ++i) {
		for (size_t j = 0; j < gradient.cols; ++j) {
			gradx = (grayRef.at<uchar>(i + 1, j + 2) - grayRef.at<uchar>(i + 1, j)) * 0.5f;
			grady = (grayRef.at<uchar>(i + 2, j + 1) - grayRef.at<uchar>(i, j + 1)) * 0.5f;
			gradient.at<uchar>(i, j) = sqrt(pow(gradx, 2) + pow(grady, 2));
		}
	}
}

void AutomatedGrainSizing::DivideLineBinary(InputArray _gradient, InputArray _blur, OutputArray _divide)
{
	Mat gradient = _gradient.getMat();

	Mat blur = _blur.getMat();

	_divide.create(gradient.size(), CV_8UC1);
	Mat divide = _divide.getMat();

	for (size_t i = 0; i < gradient.rows; ++i) {
		for (size_t j = 0; j < gradient.cols; ++j) {
			if (blur.at<uchar>(i, j) >= gradient.at<uchar>(i, j)) {
				divide.at<uchar>(i, j) = 255;
			} else {
				divide.at<uchar>(i, j) = 0;
			}
		}
	}
}

void AutomatedGrainSizing::HysteresisCut(InputArray _binary, InputArray _area, OutputArray _line)
{
	Mat binary = _binary.getMat();

	Mat area = _area.getMat();

	_line.create(binary.size(), CV_8UC1);
	Mat line = _line.getMat();

	Mat UT(binary.size(), CV_8UC1, Scalar(0));
	Mat MT(binary.size(), CV_8UC1, Scalar(0));

	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			if (!binary.at<uchar>(i, j)) {
				if (area.at<uchar>(i, j)) {
					MT.at<uchar>(i, j) = 255;
				} else {
					UT.at<uchar>(i, j) = 255;
				}
			}
		}
	}

	Mat labelImg;
	int labelNum = bwlabel(MT, labelImg, 4) + 1;	// include label 0
	int* labeltable = new int[labelNum]();		// initialize label table with zero  

	int B, C, D, E, F, G, H, I;
	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			//+ - + - + - +
			//| B | C | D |
			//+ - + - + - +
			//| E | A | F |
			//+ - + - + - +
			//| G | H | I |
			//+ - + - + - +

			B = (i == 0 || j == 0) ? 0 : UT.at<uchar>(i - 1, j - 1);
			C = (i == 0) ? 0 : UT.at<uchar>(i - 1, j);
			D = (i == 0 || j == binary.cols - 1) ? 0 : UT.at<uchar>(i - 1, j + 1);
			E = (j == 0) ? 0 : UT.at<uchar>(i, j - 1);
			F = (j == binary.cols - 1) ? 0 : UT.at<uchar>(i, j + 1);
			G = (i == binary.rows - 1 || j == 0) ? 0 : UT.at<uchar>(i + 1, j - 1);
			H = (i == binary.rows - 1) ? 0 : UT.at<uchar>(i + 1, j);
			I = (i == binary.rows - 1 || j == binary.cols - 1) ? 0 : UT.at<uchar>(i + 1, j + 1);

			// apply 8 connectedness  
			if (B || C || D || E || F || G || H || I) {
				++labeltable[labelImg.at<int>(i, j)];
			}
		}
	}

	labeltable[0] = 0;		//clear 0 label

	for (size_t i = 0; i < line.rows; ++i) {
		for (size_t j = 0; j < line.cols; ++j) {
			if (labeltable[labelImg.at<int>(i, j)] || UT.at<uchar>(i, j)) {
				line.at<uchar>(i, j) = 0;
			} else {
				line.at<uchar>(i, j) = 255;
			}
		}
	}

	delete[] labeltable;
}

void AutomatedGrainSizing::Combine(InputArray _area, InputArray _line, OutputArray _binary)
{
	Mat area = _area.getMat();

	Mat line = _line.getMat();

	_binary.create(area.size(), CV_8UC1);
	Mat binary = _binary.getMat();

	for (size_t i = 0; i < area.rows; ++i) {
		for (size_t j = 0; j < area.cols; ++j) {
			if (!area.at<uchar>(i, j) || !line.at<uchar>(i, j)) {
				binary.at<uchar>(i, j) = 0;
			} else {
				binary.at<uchar>(i, j) = 255;
			}
		}
	}
}

void AutomatedGrainSizing::ClearNoise(InputArray _binary, OutputArray _clear, float mumax)
{
	Mat binary = _binary.getMat();

	_clear.create(binary.size(), CV_8UC1);
	Mat clear = _clear.getMat();

	Mat rbinary(binary.rows + 2, binary.cols + 2, CV_8UC1, 255);
	for (size_t i = 1; i < rbinary.rows - 1; ++i) {
		for (size_t j = 1; j < rbinary.cols - 1; ++j) {
			rbinary.at<uchar>(i, j) = binary.at<uchar>(i - 1, j - 1) ? 0 : 255;
		}
	}

	Mat labelImg;
	int num = bwlabel(rbinary, labelImg, 8);

	int *labeltable = new int[num + 1]();

	// calculate obj size
	for (size_t i = 0; i < labelImg.rows; ++i) {
		for (size_t j = 0; j < labelImg.cols; ++j) {
			++labeltable[labelImg.at<int>(i, j)];
		}
	}

	// find max size
	int maxSize = 0;
	for (size_t i = 1; i < num + 1; ++i) {
		maxSize = labeltable[i] > maxSize ? labeltable[i] : maxSize;
	}

	int tolSize = round(mumax * mumax / 100);

	for (size_t i = 0; i < labelImg.rows; ++i) {
		for (size_t j = 0; j < labelImg.cols; ++j) {
			if (labeltable[labelImg.at<int>(i, j)] < tolSize) {
				rbinary.at<uchar>(i, j) = 0;
			}
		}
	}

	for (size_t i = 0; i < clear.rows; ++i) {
		for (size_t j = 0; j < clear.cols; ++j) {
			clear.at<uchar>(i, j) = rbinary.at<uchar>(i + 1, j + 1) ? 0 : 255;
		}
	}

	delete[] labeltable;
}

void AutomatedGrainSizing::Reconstruct(InputArray _marker, InputArray _mask, OutputArray _hdistance)
{
	Mat marker = _marker.getMat();

	Mat mask = _mask.getMat();

	_hdistance.create(mask.size(), CV_32FC1);
	Mat hdistance = _hdistance.getMat();

	// neighbourhood group forward scan
	Point2i  m[4] = { Point2i(-1, -1), Point2i(-1, 0), Point2i(-1, 1), Point2i(0, -1) };
	// neighbourhood group backward scan
	Point2i  n[4] = { Point2i(1, 1), Point2i(1, 0), Point2i(1, -1), Point2i(0, 1) };
	// complete neighbourhood 
	Point2i  nc[8] = { Point2i(-1, -1), Point2i(-1, 0), Point2i(-1, 1), Point2i(0, -1),
		Point2i(0, 1), Point2i(1, -1), Point2i(1, 0), Point2i(1, 1) };


	// forward reconstruction
	for (size_t i = 0; i < marker.rows; ++i) {
		for (size_t j = 0; j < marker.cols; ++j) {
			// find regional maximum of neighbourhood 
			float d = marker.at<float>(i, j);
			for (size_t k = 0; k < 4; ++k) {
				if (i + m[k].x >= 0 && j + m[k].y >= 0 && i + m[k].x < marker.rows && j + m[k].y < marker.cols) {
					d = std::max(marker.at<float>(i + m[k].x, j + m[k].y), d);
				}
			}

			// compare mask and maximum value of neighbourhood
			// write this value on position i,j
			d = std::min(mask.at<float>(i, j), d);
			hdistance.at<float>(i, j) = d;
		}
	}

	std::queue<Point2i> fifo;

	// backward reconstruction
	for (int i = marker.rows - 1; i >= 0; --i) {
		for (int j = marker.cols - 1; j >= 0; --j) {
			// find regional maximum of neighbourhood 
			float d = hdistance.at<float>(i, j);
			for (size_t k = 0; k < 4; ++k) {
				if (i + n[k].x >= 0 && j + n[k].y >= 0 && i + n[k].x < marker.rows && j + n[k].y < marker.cols) {
					d = std::max(hdistance.at<float>(i + n[k].x, j + n[k].y), d);
				}
			}

			// compare mask and maximum value of neighbourhood
			// write this value on position i,j
			d = std::min(mask.at<float>(i, j), d);
			hdistance.at<float>(i, j) = d;

			/* check if the current position might be changed in a forward scan */
			for (size_t k = 0; k < 4; ++k) {
				if (i + m[k].x >= 0 && j + m[k].y >= 0 && i + m[k].x < marker.rows && j + m[k].y < marker.cols) {
					float q = hdistance.at<float>(i + m[k].x, j + m[k].y);
					if ((q < d) && (q < mask.at<float>(i + m[k].x, j + m[k].y))) {
						fifo.push(Point2i(i, j));
						break;
					}
				}
			}
		}
	}

	while (!fifo.empty()) {
		Point2i base = fifo.front();
		fifo.pop();

		// complete neighbourhood
		for (size_t k = 0; k < 8; ++k) {
			Point2i base_new = base + nc[k];
			if (base_new.x >= 0 && base_new.y >= 0 && base_new.x < marker.rows && base_new.y < marker.cols) {
				if ((hdistance.at<float>(base_new.x, base_new.y) < hdistance.at<float>(base.x, base.y)) && (hdistance.at<float>(base_new.x, base_new.y) < mask.at<float>(base_new.x, base_new.y))) {
					hdistance.at<float>(base_new.x, base_new.y) = std::min(hdistance.at<float>(base.x, base.y), mask.at<float>(base_new.x, base_new.y));
					fifo.push(base_new);
				}
			}
		}
	}
}

void AutomatedGrainSizing::HMinimaTransform(InputArray _distance, OutputArray _hdistance, float h)
{
	Mat distance = _distance.getMat();

	_hdistance.create(distance.size(), CV_32FC1);
	Mat hdistance = _hdistance.getMat();

	Mat marker(distance.size(), CV_32FC1);
	for (size_t i = 0; i < distance.rows; ++i) {
		for (size_t j = 0; j < distance.cols; ++j) {
			marker.at<float>(i, j) = distance.at<float>(i, j) - h;
		}
	}

	Reconstruct(marker, distance, hdistance);
}

void AutomatedGrainSizing::DetectRegionalMinima(InputArray _distance, OutputArray _seed)
{
	Mat distance = _distance.getMat();

	_seed.create(distance.size(), CV_8UC1);
	Mat seed = _seed.getMat();
	seed = Scalar(255);

	queue<Point2i> *mpFifo = new queue<Point2i>();

	for (size_t y = 0; y < seed.rows; ++y) {
		for (size_t x = 0; x < seed.cols; ++x) {
			if (seed.at<uchar>(y, x)) {
				for (int dy = -1; dy <= 1; ++dy) {
					for (int dx = -1; dx <= 1; ++dx) {
						if ((x + dx >= 0) && (x + dx < distance.cols) && (y + dy >= 0) && (y + dy < distance.rows)) {
							// If pe2.value < pe1.value, pe1 is not a local minimum
							if (floor(distance.at<float>(y, x)) > floor(distance.at<float>(y + dy, x + dx))) {
								seed.at<uchar>(y, x) = 0;
								mpFifo->push(Point2i(x, y));

								while (!mpFifo->empty()) {
									Point2i pe3 = mpFifo->front();
									mpFifo->pop();

									int xh = pe3.x;
									int yh = pe3.y;

									for (int dyh = -1; dyh <= 1; ++dyh) {
										for (int dxh = -1; dxh <= 1; ++dxh) {
											if ((xh + dxh >= 0) && (xh + dxh < distance.cols) && (yh + dyh >= 0) && (yh + dyh < distance.rows)) {
												if (seed.at<uchar>(yh + dyh, xh + dxh)) {
													if (floor(distance.at<float>(yh + dyh, xh + dxh)) == floor(distance.at<float>(y, x))) {
														seed.at<uchar>(yh + dyh, xh + dxh) = 0;
														mpFifo->push(Point2i(xh + dxh, yh + dyh));
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void AutomatedGrainSizing::ExtendRegionalMinima(InputArray _distance, OutputArray _seed, float h)
{
	Mat distance = _distance.getMat();

	_seed.create(distance.size(), CV_8UC1);
	Mat seed = _seed.getMat();

	//float imageMinLength = distance.rows < distance.cols ? distance.rows : distance.cols;

	//for (size_t i = 0; i < distance.rows; ++i) {
	//	for (size_t j = 0; j < distance.cols; ++j) {
	//		distance.at<float>(i, j) = distance.at<float>(i, j) > imageMinLength * 0.025 ? imageMinLength * 0.025 : distance.at<float>(i, j);
	//	}
	//}

	Mat objectHMT;
	HMinimaTransform(distance, objectHMT, h);

	for (size_t i = 0; i < objectHMT.rows; ++i) {
		for (size_t j = 0; j < objectHMT.cols; ++j) {
			objectHMT.at<float>(i, j) = -objectHMT.at<float>(i, j);
		}
	}

	DetectRegionalMinima(objectHMT, seed);
}

void AutomatedGrainSizing::DistanceCut(InputArray _distance, OutputArray _seed)
{
	Mat distance = _distance.getMat();

	_seed.create(distance.size(), CV_8UC1);
	Mat seed = _seed.getMat();

	float imageMinLength = distance.rows < distance.cols ? distance.rows : distance.cols;

	for (size_t i = 0; i < distance.rows; ++i) {
		for (size_t j = 0; j < distance.cols; ++j) {
			seed.at<uchar>(i, j) = distance.at<float>(i, j) > imageMinLength * 0.025 ? 255 : 0;
		}
	}
}

void AutomatedGrainSizing::AddSeed(InputArray _binary, InputArray _seed, OutputArray _fseed)
{
	Mat binary = _binary.getMat();

	Mat seed = _seed.getMat();

	_fseed.create(binary.size(), CV_8UC1);
	Mat fseed = _fseed.getMat();
	seed.copyTo(fseed);

	Mat labels;
	int num = bwlabel(binary, labels, 4) + 1;	//include label 0

	int *labeltable = new int[num]();

	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			if (fseed.at<uchar>(i, j)) {
				++labeltable[labels.at<int>(i, j)];
			}
		}
	}

	labeltable[0] = 1;	// add background

	for (size_t i = 0; i < binary.rows; ++i) {
		for (size_t j = 0; j < binary.cols; ++j) {
			if (!labeltable[labels.at<int>(i, j)]) {
				fseed.at<uchar>(i, j) = 255;
			}
		}
	}

	delete[] labeltable;
	labeltable = nullptr;
}

void AutomatedGrainSizing::DetectMinima(InputArray _distance, InputArray _seed, OutputArray _labelImg, priority_queue<PixelElement, vector<PixelElement>, mycomparison> &sortedQueue)
{
	Mat distance = _distance.getMat();

	Mat seed = _seed.getMat();

	_labelImg.create(distance.size(), CV_32SC1);
	Mat labelImg = _labelImg.getMat();

	int num = bwlabel(seed, labelImg, 8);

	Mat element = (Mat_<uchar>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
	Mat seedDilate;
	morphologyEx(seed, seedDilate, MORPH_DILATE, element);

	for (size_t i = 0; i < seed.rows; ++i) {
		for (size_t j = 0; j < seed.cols; ++j) {
			if (seedDilate.at<uchar>(i, j) && !seed.at<uchar>(i, j)) {
				labelImg.at<int>(i, j) = LABEL_PROCESSING;
				sortedQueue.push(PixelElement(distance.at<float>(i, j), j, i));
			} else {
				labelImg.at<int>(i, j) = seed.at<uchar>(i, j) ? labelImg.at<int>(i, j) : LABEL_NOLOCALMINIMUM;
			}
		}
	}
}

bool AutomatedGrainSizing::CheckForAlreadyLabeledNeighbours(int x, int y, Mat &label, Point2i &outLabeledNeighbour, int &outLabel)
{
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			if ((x + dx >= 0) && (x + dx < label.cols) && (y + dy >= 0) && (y + dy < label.rows)) {
				if (label.at<int>(y + dy, x + dx) > LABEL_RIDGE) {
					outLabeledNeighbour = Point2i(x + dx, y + dy);
					outLabel = label.at<int>(y + dy, x + dx);
					return true;
				}
			}
		}
	}
	return false;
}

bool AutomatedGrainSizing::CheckIfPixelIsWatershed(int x, int y, Mat &label, Point2i &inLabeledNeighbour)
{
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			if ((x + dx >= 0) && (x + dx < label.cols) && (y + dy >= 0) && (y + dy < label.rows)) {
				if ((label.at<int>(y + dy, x + dx) >= LABEL_MINIMUM) && (label.at<int>(y + dy, x + dx) != label.at<int>(inLabeledNeighbour.y, inLabeledNeighbour.x)) && ((inLabeledNeighbour.x != x + dx) || (inLabeledNeighbour.y != y + dy))) {
					label.at<int>(y, x) = LABEL_RIDGE;
					return true;
				}
			}
		}
	}
	return false;
}

void AutomatedGrainSizing::WatershedTransform(InputArray _binary, InputArray _seed, InputArray _distance, OutputArray _object)
{
	Mat binary = _binary.getMat();

	Mat seed = _seed.getMat();

	Mat distance = _distance.getMat();

	_object.create(distance.size(), CV_8UC1);
	Mat object = _object.getMat();

	for (size_t i = 0; i < distance.rows; ++i) {
		for (size_t j = 0; j < distance.cols; ++j) {
			distance.at<float>(i, j) = -distance.at<float>(i, j);
		}
	}

	Mat label;
	priority_queue<PixelElement, vector<PixelElement>, mycomparison> mvSortedQueue;
	DetectMinima(distance, seed, label, mvSortedQueue);

	while (!mvSortedQueue.empty()) {
		PixelElement lItemA = mvSortedQueue.top();
		mvSortedQueue.pop();
		int labelOfNeighbour = 0;

		// (a) Pop element and find positive labeled neighbour
		Point2i alreadyLabeledNeighbour;
		int x = lItemA.x;
		int y = lItemA.y;

		// (b) Check if current pixel is watershed pixel by checking if there are different finally labeled neighbours
		if (CheckForAlreadyLabeledNeighbours(x, y, label, alreadyLabeledNeighbour, labelOfNeighbour)) {
			if (!(CheckIfPixelIsWatershed(x, y, label, alreadyLabeledNeighbour))) {
				// c) if not watershed pixel, assign label of neighbour and add the LABEL_NOLOCALMINIMUM neighbours to priority_queue for processing
				/*UpdateLabel(x, y, distance, label, labelOfNeighbour, mvSortedQueue);*/

				label.at<int>(y, x) = labelOfNeighbour;

				for (int dx = -1; dx <= 1; ++dx) {
					for (int dy = -1; dy <= 1; ++dy) {
						if ((x + dx >= 0) && (x + dx < label.cols) && (y + dy >= 0) && (y + dy < label.rows)) {
							if (label.at<int>(y + dy, x + dx) == LABEL_NOLOCALMINIMUM) {
								label.at<int>(y + dy, x + dx) = LABEL_PROCESSING;
								mvSortedQueue.push(PixelElement(distance.at<float>(y + dy, x + dx), x + dx, y + dy));
							}
						}
					}
				}
			}
		}
	}

	binary.copyTo(object);

	// d) finalize the labelImage
	for (size_t i = 0; i < object.rows; ++i) {
		for (size_t j = 0; j < object.cols; ++j) {
			if (label.at<int>(i, j) == LABEL_RIDGE) {
				object.at<uchar>(i, j) = 0;
			}
		}
	}
}

void AutomatedGrainSizing::DeleteEdge(InputArray _binary, OutputArray _object)
{
	Mat binary = _binary.getMat();

	_object.create(binary.size(), CV_8UC1);
	Mat object = _object.getMat();
	binary.copyTo(object);

	for (size_t i = 0; i < object.cols; ++i) {
		if (object.at<uchar>(0, i) == 255) {
			floodFill(object, Point(i, 0), 0);
		}
		if (object.at<uchar>(object.rows - 1, i) == 255) {
			floodFill(object, Point(i, object.rows - 1), 0);
		}
	}
	for (size_t i = 0; i < object.rows; ++i) {
		if (object.at<uchar>(i, 0) == 255) {
			floodFill(object, Point(0, i), 0);
		}
		if (object.at<uchar>(i, object.cols - 1) == 255) {
			floodFill(object, Point(object.cols - 1, i), 0);
		}
	}
}

void AutomatedGrainSizing::FitEllipse(InputArray _object, vector<float> &ellipse_M, vector<float> &ellipse_L)
{
	Mat object = _object.getMat();

	Mat labels;
	int objectNum = bwlabel(object, labels, 4);

	Mat elementO = (Mat_<uchar>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
	Mat objectErode;
	morphologyEx(object, objectErode, MORPH_ERODE, elementO);

	for (size_t i = 0; i < object.rows; ++i) {
		for (size_t j = 0; j < object.cols; ++j) {
			labels.at<int>(i, j) = objectErode.at<uchar>(i, j) ? 0 : labels.at<int>(i, j);
		}
	}

	vector<vector<Point2i>> pointset;
	for (size_t i = 0; i < objectNum; ++i) {
		pointset.push_back(vector<Point2i>());
	}

	for (size_t i = 0; i < labels.rows; ++i) {
		for (size_t j = 0; j < labels.cols; ++j) {
			if (labels.at<int>(i, j)) {
				pointset[labels.at<int>(i, j) - 1].push_back(Point2i(j, i));
			}
		}
	}

	for (size_t i = 0; i < objectNum; ++i) {
		if (pointset[i].size() > 5) {
			RotatedRect ellipse_obj = fitEllipse(pointset[i]);
			if (ellipse_obj.size.width < ellipse_obj.size.height) {
				ellipse_M.push_back(ellipse_obj.size.width);
				ellipse_L.push_back(ellipse_obj.size.height);
			} else {
				ellipse_M.push_back(ellipse_obj.size.height);
				ellipse_L.push_back(ellipse_obj.size.width);
			}
		}
	}

	std::sort(ellipse_M.begin(), ellipse_M.end());
	std::sort(ellipse_L.begin(), ellipse_L.end());
}

bool AutomatedGrainSizing::DoAutomatedGrainSizing(Mat image, Point2i realSize, int mumax, vector<float> &ellipse_M, vector<float> &ellipse_L)
{
	int n = 5;
	int ksize = round(mumax * 5);
	ksize = ksize % 2 ? ksize : ksize + 1;
	double sigma = ksize / 6.07;

	float wIdeal = sqrt((12 * sigma*sigma / n) + 1);  // Ideal averaging filter width 
	int wl = floor(wIdeal);
	if (wl % 2 == 0) --wl;
	float mIdeal = (12 * sigma*sigma - n*wl*wl - 4 * n*wl - 3 * n) / (-4 * wl - 4);
	int m = round(mIdeal);

	progressDialog->setRange(0, 18 + (n + m));

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat gray;
	RGBToGray(image, gray);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat grayBlur;
	bool cancel = GaussianBlurF(gray, grayBlur, sigma, n);

	if (progressDialog->wasCanceled() || cancel) return false;
	progressDialog->setValue(++num);

	Mat grayDIV;
	DivideArea(gray, grayBlur, grayDIV);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat grayTH;
	KittlerThresholdArea(grayDIV, grayTH);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat gradm;
	Gradient(gray, gradm);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat gradmBlur;
	blur(gradm, gradmBlur, Size(5, 5));

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat gradmDB;
	DivideLineBinary(gradm, gradmBlur, gradmDB);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat lineHC;
	HysteresisCut(gradmDB, grayTH, lineHC);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat objectCOM;
	Combine(grayTH, lineHC, objectCOM);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat objectOpen;
	Mat elementO = (Mat_<uchar>(5, 5) << 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0); // circle
	cv::morphologyEx(objectCOM, objectOpen, MORPH_OPEN, elementO);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat objectCN;
	ClearNoise(objectOpen, objectCN, mumax);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat objectDT;
	cv::distanceTransform(objectCN, objectDT, CV_DIST_L2, 3);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat objectEM;
	ExtendRegionalMinima(objectDT, objectEM, 5);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat objectAS;
	AddSeed(objectCN, objectEM, objectAS);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat objectWT;
	WatershedTransform(objectCN, objectAS, objectDT, objectWT);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	Mat objectDE;
	DeleteEdge(objectWT, objectDE);

	if (progressDialog->wasCanceled()) return false;
	progressDialog->setValue(++num);

	FitEllipse(objectDE, ellipse_M, ellipse_L);

	progressDialog->setValue(++num);
	return true;
}