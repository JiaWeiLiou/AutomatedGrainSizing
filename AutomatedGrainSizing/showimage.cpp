#include "showimage.h"

ShowImage::ShowImage(QWidget *parent)
	: QWidget(parent)
{
	initial();
	setMouseTracking(true);				// tracking mouse location
}

void ShowImage::closeImage()
{
	showImage = QImage();
	rawImage = QImage();
	warpImage = QImage();
	initial();
}

bool ShowImage::loadImage(QString path)
{
	clearPoints();
	loading = true;								// show loading
	repaint();									// repaint image widget
	filePathName = path;
	bool success = rawImage.load(filePathName);	// store image's
	if (success) {
		loadParameter(filePathName);
	}
	showImage = rawImage;
	loading = false;							// show loading
	resetImage();									// initial image widget
	return success;
}

void ShowImage::loadParameter(QString filePathName)
{
	clearPoints();
	finished = 0;
	int pos1 = filePathName.lastIndexOf('.');
	QFile file(filePathName.left(pos1) + ".param");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream streamread(&file);
		size_t i = 0;
		while (!streamread.atEnd()) {
			QString line = streamread.readLine();
			int pos1 = line.indexOf('\t');
			if (i < 4 && checkBoxState == Qt::Unchecked) {
				float x = (line.left(pos1)).toFloat();
				float y = (line.right(line.size() - pos1 - 1)).toFloat();
				rawImage4Points << QPointF((line.left(pos1)).toFloat(), (line.right(line.size() - pos1 - 1)).toFloat());
			} else if (i < 6 && checkBoxState == Qt::Unchecked) {
				rawImage2Points << QPointF((line.left(pos1)).toFloat(), (line.right(line.size() - pos1 - 1)).toFloat());
			} else if (i == 6 && checkBoxState == Qt::Unchecked) {
				realSize = QPoint((line.left(pos1)).toInt(), (line.right(line.size() - pos1 - 1)).toInt());
				realSizeModified = 1;
				emit emitRealSize(realSize);
			}
			++i;
		}
		file.close();

		if (i < 6 || i > 9) {
			clearPoints();
		} else {
			image4PointsFulled = 1;							// image 4 points full
			image2PointsFulled = 1;							// image 2 points full
			image4PointModified = 1;						// image 4 points modified
			image2PointModified = 1;						// image 2 points modified
			emit image4PointsModified(4);
			emit image2PointsModified(2);
		}
	}
}

void ShowImage::saveFile(QString filePathName)
{
	int pos1 = filePathName.lastIndexOf('.');
	QFile Mfile(filePathName.left(pos1) + ".Mgsd");
	QFile Lfile(filePathName.left(pos1) + ".Lgsd");
	if (Mfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		QTextStream out(&Mfile);
		out << dec << fixed;
		for (int i = 0; i < ellipseM.size(); ++i) {
			out << ellipseM[i] << "\t";
		}
		out << endl;
		Mfile.close();
	}
	if (Lfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		QTextStream out(&Lfile);
		out << dec << fixed;
		for (int i = 0; i < ellipseL.size(); ++i) {
			out << ellipseL[i] << "\t";
		}
		out << endl;
		Lfile.close();
	}
}

void ShowImage::saveParameter(QString filePathName)
{
	int pos1 = filePathName.lastIndexOf('.');
	QFile file(filePathName.left(pos1) + ".param");
	if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		QTextStream out(&file);
		out << dec << fixed;
		for (int i = 0; i < rawImage4Points.size(); ++i) {
			out << QString::number(rawImage4Points[i].x()) << "\t" << QString::number(rawImage4Points[i].y()) << "\n";
		}
		for (int i = 0; i < rawImage2Points.size(); ++i) {
			out << QString::number(rawImage2Points[i].x()) << "\t" << QString::number(rawImage2Points[i].y()) << "\n";
		}
		out << QString::number(realSize.x()) << "\t" << QString::number(realSize.y()) << "\n";
		out << getMuMax() << "\n";
		out << endl;
		file.close();
	}
}

void ShowImage::resizeEvent(QResizeEvent *event)
{
	if (scale > minScale && !showImage.isNull()) {
		setCursor(Qt::OpenHandCursor);	// set cursor to open hand type
	} else {
		setCursor(Qt::ArrowCursor);	// set cursor to arrow type
	}

	/* calculate scale to show image*/
	float scaleW = (float)winW / (float)(imgW - 1);
	float scaleH = (float)winH / (float)(imgH - 1);
	// If the scale is to zoom in, keep scale to 1:1.
	if (scaleW >= 1 && scaleH >= 1) {
		scale = 1;
		minScale = 1;
		// If the scale is to zoom out and have been zoom in, adjudge below.
	} else {
		// If scale is lower than minimum scale, set scale = minScale.
		// else keep the scale.
		minScale = scaleW < scaleH ? scaleW : scaleH;
		scale = scale < minScale ? minScale : scale;
	}
	update();
}

void ShowImage::wheelEvent(QWheelEvent *event)
{
	if (scale > minScale && !showImage.isNull()) {
		setCursor(Qt::OpenHandCursor);	// set cursor to open hand type
	} else {
		setCursor(Qt::ArrowCursor);	// set cursor to arrow type
	}

	QPointF pixelPos = QPointF(event->posF());			// pixel coordinate
	QPointF imagePos = (pixelPos - newDelta) / scale;	// image coordinate

	if (event->angleDelta().y() > 0) {	// zoom in
		scale = scale * 1.2 > maxScale ? maxScale : scale * 1.2;
	} else {	// zoom out
		scale = scale / 1.2 < minScale ? minScale : scale / 1.2;
	}
	newDelta = pixelPos - scale * imagePos;
	oldDelta = newDelta;

	update();
}

void ShowImage::mousePressEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton && !showImage.isNull()) {	// drag image
		if (scale > minScale) {
			setCursor(Qt::ClosedHandCursor);	// set cursor to closed hand type
		} else {
			setCursor(Qt::ArrowCursor);	// set cursor to arrow type
		}
		pos1 = QPointF(event->pos());
		update();
	} else if (event->buttons() == Qt::RightButton && !showImage.isNull()) {	// set point

																				// set point's location can be mod
		modifiedPointState = 0;
		modifyPoints(event);
		addPoints(event);
	}
}

void ShowImage::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton && !showImage.isNull()) {	// drag image
		if (scale > minScale) {
			setCursor(Qt::ClosedHandCursor);	// set cursor to closed hand type
		} else {
			setCursor(Qt::ArrowCursor);	// set cursor to arrow type
		}
		pos2 = QPointF(event->pos());
		newDelta = pos2 - pos1 + oldDelta;	// new displacement add last displacement
		update();
	} else if (event->buttons() == Qt::RightButton && !showImage.isNull()) {	// set point

		modifyDragPoints(event);
		addDragPoints(event);
	}
}

void ShowImage::mouseReleaseEvent(QMouseEvent *event)
{
	if (scale > minScale && !showImage.isNull()) {
		setCursor(Qt::OpenHandCursor);	// set cursor to open hand type
	} else {
		setCursor(Qt::ArrowCursor);	// set cursor to arrow type
	}

	if (event->button() == Qt::LeftButton) {
		oldDelta = newDelta;	// record the distance of drag image
	} else if (event->button() == Qt::RightButton) {
		modifiedPointState = 0;
		if (checkBoxState == Qt::Unchecked) {
			image4PointsFulled = rawImage4Points.size() == 4 ? 1 : 0;
			image2PointsFulled = rawImage2Points.size() == 2 ? 1 : 0;
		} else {
			image2PointsFulled = warpImage2Points.size() == 2 ? 1 : 0;
		}
		if (image4PointsFulled) { emit image4PointsModified(4); }
		if (image2PointsFulled) { emit image2PointsModified(2); }
	}
	update();
}

void ShowImage::keyPressEvent(QKeyEvent *event)
{
	// press keyboard Esc to give up setting point
	if (event->key() == Qt::Key_Escape) {
		clearPoints();
	} else if (event->key() == Qt::Key_Backspace) {
		deletePoints();
	}
}

void ShowImage::paintEvent(QPaintEvent *event)
{
	/* modified xDelta */
	if ((imgW - 1) * scale < winW) {	// If image's horizontal size to show is longer than winW, set it to center.
		newDelta.rx() = winW / 2 - scale * (imgW - 1) / 2;
	} else if (newDelta.x() > 0) {	// If image's horizontal delta is less than winW, set it to edge.
		newDelta.rx() = 0;
	} else if ((imgW - 1) * scale + newDelta.x() < winW) {	// If image's horizontal delta is bigger than 0, set it to edge.
		newDelta.rx() = winW - (imgW - 1) * scale;
	}

	/* modified yDelta */
	if ((imgH - 1) * scale < winH) {	// If image's vertical size to show is longer than winW, set it to center.
		newDelta.ry() = winH / 2 - scale * (imgH - 1) / 2;
	} else if (newDelta.ry() > 0) {	// If image's vertical delta is less than winW, set it to edge.
		newDelta.ry() = 0;
	} else if ((imgH - 1) * scale + newDelta.y() < winH) {	// If image's vertical delta is bigger than 0, set it to edge.
		newDelta.ry() = winH - (imgH - 1) * scale;
	}

	/* show text or image */
	QPainter painter(this);
	if (loading) {	// show loading image
		painter.drawText(QRect(winW / 2 - 100 / 2, winH / 2 - 20 / 2, 100, 20), Qt::AlignCenter, "Loading Image...");
	} else if (showImage.isNull()) {	// show no image
		painter.drawText(QRect(winW / 2 - 50 / 2, winH / 2 - 20 / 2, 50, 20), Qt::AlignCenter, "No Image.");
	} else {	// show image
		QRectF rect(newDelta.x() - 0.5 * scale, newDelta.y() - 0.5 * scale, imgW * scale, imgH * scale);
		painter.drawImage(rect, showImage);	// draw image
	}

	/* show points and line */
	if (checkBoxState == Qt::Unchecked) {
		if (rawImage4Points.size() > 1) {	// draw 4 points' line
			for (int i = 0; i < rawImage4Points.size() - 1; ++i) {
				painter.setPen(QPen(Qt::green, 3));
				painter.drawLine(rawImage4Points[i] * scale + newDelta, rawImage4Points[(i + 1)] * scale + newDelta);
			}
			if (rawImage4Points.size() == 4) {
				painter.drawLine(rawImage4Points[0] * scale + newDelta, rawImage4Points[3] * scale + newDelta);
			}
		}
		if (rawImage4Points.size() > 0) {	// draw 4 ponts' points 
			for (int i = 0; i < rawImage4Points.size(); ++i) {
				painter.setPen(QPen(Qt::red, 5));
				painter.drawPoint(rawImage4Points[i] * scale + newDelta);
			}
		}
		if (rawImage2Points.size() > 1) {	// draw 2 points' line
			painter.setPen(QPen(Qt::blue, 3));
			painter.drawLine(rawImage2Points[0] * scale + newDelta, rawImage2Points[1] * scale + newDelta);
		}
		if (rawImage2Points.size() > 0) {	// draw 2 points' points
			for (int i = 0; i < rawImage2Points.size(); ++i) {
				painter.setPen(QPen(Qt::red, 5));
				painter.drawPoint(rawImage2Points[i] * scale + newDelta);
			}
		}
	} else {
		if (warpImage2Points.size() > 1) {	// draw 2 points' line
			painter.setPen(QPen(Qt::blue, 3));
			painter.drawLine(warpImage2Points[0] * scale + newDelta, warpImage2Points[1] * scale + newDelta);
		}
		if (warpImage2Points.size() > 0) {	// draw 2 points' points
			for (int i = 0; i < warpImage2Points.size(); ++i) {
				painter.setPen(QPen(Qt::red, 5));
				painter.drawPoint(warpImage2Points[i] * scale + newDelta);
			}
		}
	}

	/* set for setStyleSheet to show gray edge line */
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ShowImage::initial()
{
	clearPoints();
	finished = 0;
	realSizeModified = 1;
	checkBoxState = Qt::Unchecked;
	loading = false;

	/* set maximum scale (maximum scale is 1/5 screen width) */
	maxScale = QApplication::desktop()->screenGeometry().width() / 5;	// set maximum scale

	/* calculate scale to show image */
	float scaleW = (float)winW / (float)(imgW - 1);
	float scaleH = (float)winH / (float)(imgH - 1);
	// If the scale is to zoom in, keep scale to 1:1.
	if (scaleW >= 1 && scaleH >= 1) {
		scale = 1;
		minScale = 1;
		// If the scale is to zoom out and have been zoom in, adjudge below.
	} else {
		minScale = scaleW < scaleH ? scaleW : scaleH;
		scale = minScale;
	}

	update();
	QApplication::alert(this, 0);
}

void ShowImage::resetImage()
{
	/* set maximum scale (maximum scale is 1/5 screen width) */
	maxScale = QApplication::desktop()->screenGeometry().width() / 5;	// set maximum scale

																		/* calculate scale to show image */
	float scaleW = (float)winW / (float)(imgW - 1);
	float scaleH = (float)winH / (float)(imgH - 1);
	// If the scale is to zoom in, keep scale to 1:1.
	if (scaleW >= 1 && scaleH >= 1) {
		scale = 1;
		minScale = 1;
		// If the scale is to zoom out and have been zoom in, adjudge below.
	} else {
		minScale = scaleW < scaleH ? scaleW : scaleH;
		scale = minScale;
	}

	update();
}

void ShowImage::addPoints(QMouseEvent *event)
{
	if (checkBoxState == Qt::Unchecked) {
		if (!modifiedPointState && !image4PointsFulled) {	// add 4 ponts' location
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			if (checkPointLocation(imagePos)) {	// limit the point in the image
				rawImage4Points.push_back(imagePos);
				image4PointModified = 1;
				modifiedPointState = 0;
				outBorder = false;
				update();
				emit image4PointsModified(rawImage4Points.size());
			} else {
				outBorder = true;
			}
		} else if (!modifiedPointState && !image2PointsFulled) {	// add 2 ponts' location
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			if (checkPointLocation(imagePos)) {	// limit the point in the image
				rawImage2Points.push_back(imagePos);	// record point
				image2PointModified = 1;
				modifiedPointState = 0;
				outBorder = false;
				update();
				emit image2PointsModified(rawImage2Points.size());
			} else {
				outBorder = true;
			}
		}
	} else {
		if (!modifiedPointState && !image2PointsFulled) {	// add 2 ponts' location
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			if (checkPointLocation(imagePos)) {	// limit the point in the image
				warpImage2Points.push_back(imagePos);	// record point
				image2PointModified = 1;
				modifiedPointState = 0;
				outBorder = false;
				update();
				emit image2PointsModified(warpImage2Points.size());
			} else {
				outBorder = true;
			}
		}
	}
}

void ShowImage::addDragPoints(QMouseEvent *event)
{
	if (checkBoxState == Qt::Unchecked) {
		if (!modifiedPointState && !image4PointsFulled) {	// add 4 points' location
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			if (checkPointLocation(imagePos)) {	// limit the point in the image
				if (outBorder) {	// mousePress points is out of border 
					rawImage4Points.push_back(imagePos);	// record points
				} else {	// mousePress points isn't out of border 
					rawImage4Points[rawImage4Points.size() - 1] = imagePos;	// record points
				}
				image4PointModified = 1;
				modifiedPointState = 0;
				outBorder = false;
				update();
				emit image4PointsModified(rawImage4Points.size());
			}
		} else if (!modifiedPointState && !image2PointsFulled) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			if (checkPointLocation(imagePos)) {	// limit the point in the image
				if (outBorder) {	// mousePress points is out of border 
					rawImage2Points.push_back(imagePos);	// record points
				} else {	// mousePress points isn't out of border 
					rawImage2Points[rawImage2Points.size() - 1] = imagePos;	// record points
				}
				image2PointModified = 1;
				modifiedPointState = 0;
				outBorder = false;
				update();
				emit image2PointsModified(rawImage2Points.size());
			}
		}
	} else {
		if (!modifiedPointState && !image2PointsFulled) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			if (checkPointLocation(imagePos)) {	// limit the point in the image
				if (outBorder) {	// mousePress points is out of border 
					warpImage2Points.push_back(imagePos);	// record points
				} else {	// mousePress points isn't out of border 
					warpImage2Points[warpImage2Points.size() - 1] = imagePos;	// record points
				}
				image2PointModified = 1;
				modifiedPointState = 0;
				outBorder = false;
				update();
				emit image2PointsModified(warpImage2Points.size());
			}
		}
	}
}

void ShowImage::modifyPoints(QMouseEvent *event)
{
	if (checkBoxState == Qt::Unchecked) {
		if (!modifiedPointState) {	// modify 4 points' location
			for (size_t i = 0; i < rawImage4Points.size(); ++i) {
				QPointF pixelPos = rawImage4Points[i] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				if (qAbs(mousePos.x() - pixelPos.x()) + qAbs(mousePos.y() - pixelPos.y()) < 5) {	// distance lower than 5
					setCursor(Qt::CrossCursor);	// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
					if (checkPointLocation(imagePos)) {	// limit the point in the image
						rawImage4Points[i] = imagePos;	// record point
						image4PointModified = 1;
						modifiedPointState = i + 1;
						outBorder = false;
						update();
						emit image4PointsModified(rawImage4Points.size());
					} else {
						outBorder = true;
					}
					break;
				}
			}
		}
		if (!modifiedPointState) {	// modify 2 points' location
			for (size_t i = 0; i < rawImage2Points.size(); ++i) {
				QPointF pixelPos = rawImage2Points[i] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				if (qAbs(mousePos.x() - pixelPos.x()) + qAbs(mousePos.y() - pixelPos.y()) < 5) {	// distance lower than 5
					setCursor(Qt::CrossCursor);	// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
					if (checkPointLocation(imagePos)) {	// limit the point in the image
						rawImage2Points[i] = imagePos;	// record point
						image2PointModified = 1;
						modifiedPointState = i + 5;
						outBorder = false;
						update();
						emit image2PointsModified(rawImage2Points.size());
					} else {
						outBorder = true;
					}
					break;
				}
			}
		}
	} else {
		if (!modifiedPointState) {	// modify 2 points' location
			for (size_t i = 0; i < warpImage2Points.size(); ++i) {
				QPointF pixelPos = warpImage2Points[i] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 5) {	// distance lower than 5
					setCursor(Qt::CrossCursor);	// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
					if (checkPointLocation(imagePos)) {	// limit the point in the image
						warpImage2Points[i] = imagePos;	// record point
						image2PointModified = 1;
						modifiedPointState = i + 5;
						outBorder = false;
						update();
						emit image2PointsModified(warpImage2Points.size());
					} else {
						outBorder = true;
					}
					break;
				}
			}
		}
	}
}

void ShowImage::modifyDragPoints(QMouseEvent *event)
{
	if (checkBoxState == Qt::Unchecked) {
		if (modifiedPointState > 0 && modifiedPointState <= 4) {	// modify 4 points' location
			QPointF pixelPos = rawImage4Points[modifiedPointState - 1] * scale + newDelta;
			QPointF mousePos = QPointF(event->pos());
			if (qAbs(mousePos.x() - pixelPos.x()) + qAbs(mousePos.y() - pixelPos.y()) < 50) {	// move distance lower than 50
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
				if (checkPointLocation(imagePos)) {	// limit the point in the image
					rawImage4Points[modifiedPointState - 1] = imagePos; // record points
					image4PointModified = 1;
					outBorder = false;
					update();
					emit image4PointsModified(rawImage4Points.size());
				} else {
					outBorder = true;
				}
			}
		} else if (modifiedPointState > 4) {	// modify 2 points' location
			QPointF pixelPos = rawImage2Points[modifiedPointState - 5] * scale + newDelta;
			QPointF mousePos = QPointF(event->pos());
			if (qAbs(mousePos.x() - pixelPos.x()) + qAbs(mousePos.y() - pixelPos.y()) < 50) {	// move distance lower than 50
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
				if (checkPointLocation(imagePos)) {	// limit the point in the image
					rawImage2Points[modifiedPointState - 5] = imagePos; // record points
					image2PointModified = 1;
					outBorder = false;
					update();
					emit image2PointsModified(rawImage2Points.size());
				} else {
					outBorder = true;
				}
			}
		}
	} else {
		if (modifiedPointState > 4) {	// modify 2 points' location
			QPointF pixelPos = warpImage2Points[modifiedPointState - 5] * scale + newDelta;
			QPointF mousePos = QPointF(event->pos());
			if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 50) {	// move distance lower than 50
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate										
				if (checkPointLocation(imagePos)) {	// limit the point in the image
					warpImage2Points[modifiedPointState - 5] = imagePos; // record points
					image2PointModified = 1;
					outBorder = false;
					update();
					emit image2PointsModified(warpImage2Points.size());
				} else {
					outBorder = true;
				}
			}
		}
	}
}

void ShowImage::clearPoints()
{
	if (checkBoxState == Qt::Unchecked) {
		rawImage4Points.clear();
		rawImage2Points.clear();
		warpImage4Points.clear();
		warpImage2Points.clear();
		image4PointsFulled = 0;
		image2PointsFulled = 0;
		image4PointModified = 1;
		image2PointModified = 1;
		update();
		emit image4PointsModified(0);
		emit image2PointsModified(0);
	} else {
		rawImage2Points.clear();
		warpImage2Points.clear();
		image2PointsFulled = 0;
		image2PointModified = 1;
		update();
		emit image2PointsModified(0);
	}
}

void ShowImage::deletePoints()
{
	if (checkBoxState == Qt::Unchecked) {
		if (rawImage2Points.size()) {
			rawImage2Points.pop_back();
			image2PointsFulled = 0;
			image2PointModified = 1;
			update();
			emit image2PointsModified(rawImage2Points.size());
		} else if (rawImage4Points.size()) {
			rawImage4Points.pop_back();
			image4PointsFulled = 0;
			image4PointModified = 1;
			update();
			emit image4PointsModified(rawImage4Points.size());
		}
	} else {
		if (warpImage2Points.size()) {
			warpImage2Points.pop_back();
			image2PointsFulled = 0;
			image2PointModified = 1;
			update();
			emit image2PointsModified(warpImage2Points.size());
		}
	}
}

void ShowImage::raw2WarpImagePoints()
{
	if (rawImage2Points.size() < warpImage2Points.size()) {
		for (size_t i = warpImage2Points.size(); i > rawImage2Points.size(); --i) {
			warpImage2Points.pop_back();
		}
	} else {
		cv::Matx33f warpMatrix = perspectiveMatrix;
		for (size_t i = 0; i < rawImage2Points.size(); ++i) {
			cv::Point3f p(rawImage2Points[i].x(), rawImage2Points[i].y(), 1);
			p = warpMatrix * p;
			p = p * (1.0f / p.z);
			if (i < warpImage2Points.size()) {
				warpImage2Points[i] = QPointF(p.x, p.y);
			} else {
				warpImage2Points << QPointF(p.x, p.y);
			}
		}
	}
}

void ShowImage::warp2RawImagePoints()
{
	if (warpImage2Points.size() < rawImage2Points.size()) {
		for (size_t i = rawImage2Points.size(); i > warpImage2Points.size(); --i) {
			rawImage2Points.pop_back();
		}
	} else {
		cv::Matx33f warpMatrix = perspectiveMatrix;
		for (size_t i = 0; i < warpImage2Points.size(); ++i) {
			cv::Point3f p(warpImage2Points[i].x(), warpImage2Points[i].y(), 1);
			p = warpMatrix.inv() * p;
			p = p * (1.0f / p.z);
			if (i < rawImage2Points.size()) {
				rawImage2Points[i] = QPointF(p.x, p.y);
			} else {
				rawImage2Points << QPointF(p.x, p.y);
			}
		}
	}
}

QImage ShowImage::Mat2QImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1  
	if (mat.type() == CV_8UC1) {
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)  
		image.setColorCount(256);
		for (size_t i = 0; i < 256; ++i) {
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat  
		uchar *pSrc = mat.data;
		for (size_t row = 0; row < mat.rows; ++row) {
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
		// 8-bits unsigned, NO. OF CHANNELS = 3  
	} else if (mat.type() == CV_8UC3) {
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	} else if (mat.type() == CV_8UC4) {
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	} else {
		return QImage();
	}
}

cv::Mat ShowImage::QImage2Mat(QImage image)
{
	cv::Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_Grayscale8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

void ShowImage::perspectiveTransform()
{
	if (image4PointModified || realSizeModified) {

		image4PointModified = 0;
		realSizeModified = 0;

		/* sort points form left-top right-top right-bottom left-bottom*/
		QVector<QPointF> L;
		L << QPointF(0, 0) << QPointF(imgW, 0) << QPointF(imgW, imgH) << QPointF(0, imgH);

		cv::Point2f bPt[4];
		for (size_t i = 0; i < 4; ++i) {
			float min = qPow(imgW, 2) + qPow(imgH, 2);
			int minindex = 0;
			for (size_t j = 0; j < 4; ++j) {
				float imin = qPow(rawImage4Points[j].x() - L[i].x(), 2) + qPow(rawImage4Points[j].y() - L[i].y(), 2);
				if (imin < min) {
					min = imin;
					minindex = j;
				}
			}
			bPt[i] = cv::Point2f(rawImage4Points[minindex].x(), rawImage4Points[minindex].y());
		}

		/* set warp image size */
		float W1 = std::sqrt(std::pow(bPt[0].x - bPt[1].x, 2) + std::pow(bPt[0].y - bPt[1].y, 2));
		float H1 = std::sqrt(std::pow(bPt[1].x - bPt[2].x, 2) + std::pow(bPt[1].y - bPt[2].y, 2));
		float W2 = std::sqrt(std::pow(bPt[2].x - bPt[3].x, 2) + std::pow(bPt[2].y - bPt[3].y, 2));
		float H2 = std::sqrt(std::pow(bPt[3].x - bPt[0].x, 2) + std::pow(bPt[3].y - bPt[0].y, 2));
		float Wp = W1 > W2 ? W1 : W2;
		float Hp = H1 > H2 ? H1 : H2;
		float Wr = Hp * float(realSize.x()) / float(realSize.y());
		float Hr = Wp * float(realSize.y()) / float(realSize.x());

		float W, H;
		if (Hr >= Hp) {
			W = Wp;
			H = Hr;
		} else {
			W = Wr;
			H = Hp;
		}

		// set warpImage4Points
		warpImage4Points.clear();
		warpImage4Points << QPointF(0, 0) << QPointF(W, 0) << QPointF(W, H) << QPointF(0, H) << QPointF(0, 0);
		cv::Point2f aPt[4] = { cv::Point2f(0, 0), cv::Point2f(W, 0), cv::Point2f(W, H), cv::Point2f(0, H) };
		cv::Mat rawImg = ShowImage::QImage2Mat(rawImage);
		perspectiveMatrix = cv::getPerspectiveTransform(bPt, aPt);

		// set warpImage2Points
		raw2WarpImagePoints();

		// get warpImage
		cv::Mat warpImg;
		cv::warpPerspective(rawImg, warpImg, perspectiveMatrix, cv::Size(W, H), cv::INTER_CUBIC);
		warpImage = ShowImage::Mat2QImage(warpImg);

	} else if (image2PointModified) {
		
		image2PointModified = 0;

		// set warpImage2Points
		raw2WarpImagePoints();

	}
}

bool ShowImage::checkPointLocation(QPointF imagePos)
{
	if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
		return true;
	} else {
		return false;
	}
}

int ShowImage::getMuMax()
{
	if (warpImage2Points.size() != 2) {
		raw2WarpImagePoints();
	}
	mumax = qCeil(qSqrt(qPow(warpImage2Points[0].x() - warpImage2Points[1].x(), 2) + qPow(warpImage2Points[0].y() - warpImage2Points[1].y(), 2)));
	return mumax;
}

void ShowImage::getRealSize(QPoint size)
{
	realSizeModified = 1;
	realSize = size;
}

void ShowImage::getCheckBoxState(int state)
{
	if (state == Qt::Unchecked) {
		checkBoxState = Qt::Unchecked;
		warp2RawImagePoints();
		showImage = rawImage;
		resetImage();
	} else {
		checkBoxState = Qt::Checked;
		perspectiveTransform();
		showImage = warpImage;
		resetImage();
	}
}

void ShowImage::startProcessing()
{
	if (!finished || image4PointModified || image2PointModified) {
		progressBar = new AutomatedGrainSizing;
		if (checkBoxState != Qt::Unchecked) {
			warp2RawImagePoints();
		}
		perspectiveTransform();
		image2PointModified = 0;
		cv::Mat img = QImage2Mat(warpImage);
		finished = progressBar->DoAutomatedGrainSizing(img, cv::Point2i(realSize.x(), realSize.y()), getMuMax(), ellipseM, ellipseL);
		saveFile(filePathName);
		saveParameter(filePathName);
		emit processingFinish(finished);
		QApplication::alert(this, 0);
	}
}
