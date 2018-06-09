#pragma once

#include <QtWidgets/QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QIcon>
#include <QCursor>
#include <QImage>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPointF>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <QStyleOption>
#include <QStyle>
#include <QTransform>
#include <cmath>
#include <opencv2/opencv.hpp>

#define imgW (showImage.size().width())	// image width
#define imgH (showImage.size().height())	// image height
#define winW (width())				// window width
#define winH (height())				// window height

class ShowImage: public QWidget
{
	Q_OBJECT

public:
	ShowImage(QWidget *parent = Q_NULLPTR);
	QVector<QPointF> showImage4Points;				// show 4 image points' pixel
	QVector<QPointF> showImage2Points;				// ahow 2 image points' pixel
	QVector<QPointF> rawImage4Points;				// record 4  raw image points' pixel
	QVector<QPointF> rawImage2Points;				// record 2  raw image points' pixel
	QVector<QPointF> warpImage4Points;				// record 4 warp image points' pixel
	QVector<QPointF> warpImage2Points;				// record 2 warp image points' pixel
	bool loading = false;							// loading file
	QImage showImage;								// image to show
	QImage rawImage;								// store raw image
	QImage warpImage;								// store transform image
	void initial();									// initial and rest widget

protected:
	void resizeEvent(QResizeEvent *event);		// window resize
	void wheelEvent(QWheelEvent *event);		// wheel zoom in and out
	void mousePressEvent(QMouseEvent *event);	// mouse press		(overload from QWidget)
	void mouseMoveEvent(QMouseEvent *event);	// mouse move		(overload from QWidget)
	void mouseReleaseEvent(QMouseEvent *event);	// mouse release	(overload from QWidget)
	void keyPressEvent(QKeyEvent *event);		// keyboard press	(overload from QWidget)
	void paintEvent(QPaintEvent *event);		// drawing the result

signals:
	void pointsChange(size_t);					// points change

private slots:
	void getRealSize(QPointF);	// get lineEdit size
	void getState(int);			// get checkbox state

private:
	float maxScale = 0.0f;	// maximum scale
	float minScale = 0.0f;	// minimun scale
	float scale = 0.0f;		// scale to draw
	QPointF newDelta;		// new displacement
	QPointF oldDelta;		// old displacement
	QPointF pos1;			// mouse press position 1
	QPointF pos2;			// mouse press position 2
	bool outBorder;			// record point is out of border or not
	bool modified;			// record point is modified or not
	char modifyState;		// record modify state of points
	QPointF realSize;		// record lineEdit size
	int state = 0;			// record checkBox state
	void perspectiveTransform();			// do perspective projection transform
	QImage ShowImage::Mat2QImage(const cv::Mat& mat);	// Mat to QImage
	cv::Mat ShowImage::QImage2Mat(QImage image);		// QImage to Mat
};
