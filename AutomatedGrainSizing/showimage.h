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


#define imgW (img.size().width())	// image width
#define imgH (img.size().height())	// image height
#define winW (width())				// window width
#define winH (height())				// window height

class ShowImage: public QWidget
{
	Q_OBJECT

public:
	ShowImage(QWidget *parent = Q_NULLPTR);
	QVector<QPointF> image4Points;				// record 4 image points' pixel
	QVector<QPointF> image2Points;				// record 2 image points' pixel
	QImage img;									// store image
	void initial();								// initial and rest widget

protected:
	//void dragEnterEvent(QDragEnterEvent *event);// drag event
	//void dropEvent(QDropEvent *event);			// drop event
	void resizeEvent(QResizeEvent *event);		// window resize
	void wheelEvent(QWheelEvent *event);		// wheel zoom in and out
	void mousePressEvent(QMouseEvent *event);	// mouse press		(overload from QWidget)
	void mouseMoveEvent(QMouseEvent *event);	// mouse move		(overload from QWidget)
	void mouseReleaseEvent(QMouseEvent *event);	// mouse release	(overload from QWidget)
	void keyPressEvent(QKeyEvent *event);		// keyboard press	(overload from QWidget)
	void paintEvent(QPaintEvent *event);		// drawing the result

signals:
	void pointsChange(size_t);

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
};
