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

#define imgW (imgS.size().width())	// image width
#define imgH (imgS.size().height())	// image height
#define winW (width())				// window width
#define winH (height())				// window height

class ShowImage: public QWidget
{
	Q_OBJECT

public:
	ShowImage(QWidget *parent = Q_NULLPTR);
	QVector<QPointF> image4Points;				// record 4 image points' pixel
	QVector<QPointF> image2Points;				// record 2 image points' pixel
	bool loading = false;						// loading file
	QImage imgS;								// image to show
	QImage imgB;								// store raw image
	QImage imgA;								// store transform image
	void initial();								// initial and rest widget

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
	int state;				// record checkBox state
	void doPPT();			// do perspective projection transform
	QImage ShowImage::Mat2QImage(const cv::Mat& mat);	// Mat to QImage
	cv::Mat ShowImage::QImage2Mat(QImage image);		// QImage to Mat
};
