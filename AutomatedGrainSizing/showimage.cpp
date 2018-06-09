#include "showimage.h"

ShowImage::ShowImage(QWidget *parent)
	: QWidget(parent)
{
	initial();
	setMouseTracking(true);								// tracking mouse location
}

void ShowImage::initial()
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

	/* clear rawImage4Points */
	if (!state) {
		rawImage4Points.clear();
		rawImage2Points.clear();
		emit pointsChange(0);
	}

	repaint();
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
	repaint();
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

														// zoom in or ot
	if (event->angleDelta().y() > 0) {
		scale = scale * 1.2 > maxScale ? maxScale : scale * 1.2;
	} else {
		scale = scale / 1.2 < minScale ? minScale : scale / 1.2;
	}
	newDelta = pixelPos - scale * imagePos;
	oldDelta = newDelta;

	update();
}

void ShowImage::mousePressEvent(QMouseEvent *event)
{
	// drag image
	if (event->buttons() == Qt::LeftButton && !showImage.isNull()) {
		if (scale > minScale) {
			setCursor(Qt::ClosedHandCursor);	// set cursor to closed hand type
		} else {
			setCursor(Qt::ArrowCursor);	// set cursor to arrow type
		}
		pos1 = QPointF(event->pos());
		update();
		// set point
	} else if (event->buttons() == Qt::RightButton && !showImage.isNull()) {

		// set point's location can be mod
		modifyState = 0;
		modified = false;

		// mod 4 points
		for (size_t i = 0; i < rawImage4Points.size(); ++i) {
			QPointF pixelPos = rawImage4Points[i] * scale + newDelta;
			QPointF mousePos = QPointF(event->pos());
			// distance lower than 5
			if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 5) {
				setCursor(Qt::CrossCursor);										// set cursor to cross type
				QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					rawImage4Points[i] = imagePos;
					if (rawImage4Points.size() == 5 && i == 0) {
						rawImage4Points[4] = imagePos;	// modify end point
					}
					outBorder = false;
					modified = true;
					modifyState = i + 1;
					update();
				} else {
					outBorder = true;
				}
				break;
			}
		}

		// mod 2 points
		if (!modified) {
			for (size_t i = 0; i < rawImage2Points.size(); ++i) {
				QPointF pixelPos = rawImage2Points[i] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				// distance lower than 5
				if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 5) {
					setCursor(Qt::CrossCursor);										// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
																		// limit the point in the image
					if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
						// record file.
						rawImage2Points[i] = imagePos;
						if (rawImage2Points.size() == 3 && i == 0) {
							rawImage2Points[2] = imagePos;	// modify end point
						}
						outBorder = false;
						modified = true;
						modifyState = i + 5;
						update();
					} else {
						outBorder = true;
					}
					break;
				}
			}
		}

		// limit the point number to image
		if (rawImage4Points.size() < 4 && !modified) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			// limit the point in the image
			if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
				// record file.
				rawImage4Points.push_back(imagePos);
				outBorder = false;
				update();
			} else {
				outBorder = true;
			}
		} else if (rawImage2Points.size() < 2 && !modified) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			// limit the point in the image
			if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
				// record file.
				rawImage2Points.push_back(imagePos);
				outBorder = false;
				update();
			} else {
				outBorder = true;
			}
		}
	}
}

void ShowImage::mouseMoveEvent(QMouseEvent *event)
{
	// drag image
	if (event->buttons() == Qt::LeftButton && !showImage.isNull()) {
		if (scale > minScale) {
			setCursor(Qt::ClosedHandCursor);	// set cursor to closed hand type
		} else {
			setCursor(Qt::ArrowCursor);	// set cursor to arrow type
		}
		pos2 = QPointF(event->pos());
		/* new displacement add last displacement */
		newDelta = pos2 - pos1 + oldDelta;
		update();
		// set point
	} else if (event->buttons() == Qt::RightButton && !showImage.isNull()) {
		// set point's location can be mod
		// mod 4 points
		if (modified && modifyState <= 4) {
			QPointF pixelPos = rawImage4Points[modifyState - 1] * scale + newDelta;
			QPointF mousePos = QPointF(event->pos());
			// move distance lower than 50
			if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 50) {
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					rawImage4Points[modifyState - 1] = imagePos;
					if (rawImage4Points.size() == 5 && modifyState == 1) {
						rawImage4Points[4] = imagePos;	// modify end point
					}
					outBorder = false;
					update();
				} else {
					outBorder = true;
				}
			}
		// mod 2 points
		} else if (modified && modifyState > 4) {
			QPointF pixelPos = rawImage2Points[modifyState - 5] * scale + newDelta;
			QPointF mousePos = QPointF(event->pos());
			// move distance lower than 50
			if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 50) {
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					rawImage2Points[modifyState - 5] = imagePos;
					if (rawImage2Points.size() == 3 && modifyState - 5 == 0) {
						rawImage2Points[2] = imagePos;	// modify end point
					}
					outBorder = false;
					update();
				} else {
					outBorder = true;
				}
			}
		}

		// limit the point number to image
		if (rawImage4Points.size() <= 4 && !modified) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			// limit the point in the image
			if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
				// record file.
				// mousePress points is out of border 
				if (outBorder) {
					rawImage4Points.push_back(imagePos);
					outBorder = false;
					// mousePress points isn't out of border 
				} else {
					rawImage4Points[rawImage4Points.size() - 1] = imagePos;
				}
				update();
			}
		} else if (rawImage2Points.size() <= 2 && !modified) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
																			// limit the point in the image
			if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
				// record file.
				// mousePress points is out of border 
				if (outBorder) {
					rawImage2Points.push_back(imagePos);
					outBorder = false;
					// mousePress points isn't out of border 
				} else {
					rawImage2Points[rawImage2Points.size() - 1] = imagePos;
				}
				update();
			}
		}
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

		if (rawImage4Points.size() == 4) {
			rawImage4Points.push_back(rawImage4Points[0]);	// put first point back to prevent press and move to add new point
		} else if (rawImage2Points.size() == 2) {
			rawImage2Points.push_back(rawImage2Points[0]);	// put first point back to prevent press and move to add new point
		}

		size_t sum = rawImage4Points.size() + rawImage2Points.size();
		if (rawImage4Points.size() == 5) {
			sum -= 1;
		}
		if (rawImage2Points.size() == 3) {
			sum -= 1;
		}
		emit pointsChange(sum);
	}
	update();
}

void ShowImage::keyPressEvent(QKeyEvent *event)
{
	// press keyboard Esc to give up setting point
	if (event->key() == Qt::Key_Escape) {
		rawImage4Points.clear();	// clear 4 points
		rawImage2Points.clear();	// clear 2 points
		update();
	} else if (event->key() == Qt::Key_Backspace) {
		if (rawImage2Points.size() > 0) {
			if (rawImage2Points.size() == 3) {
				rawImage2Points.pop_back();	// delete points
			}
			rawImage2Points.pop_back();	// delete points
			update();
		} else if (rawImage4Points.size() > 0) {
			if (rawImage4Points.size() == 5) {
				rawImage4Points.pop_back();	// delete points
			}
			rawImage4Points.pop_back();	// delete points
			update();
		}
	}

	size_t sum = rawImage4Points.size() + rawImage2Points.size();
	if (rawImage4Points.size() == 5) {
		sum -= 1;
	}
	if (rawImage2Points.size() == 3) {
		sum -= 1;
	}
	emit pointsChange(sum);
}

void ShowImage::paintEvent(QPaintEvent *event)
{
	/* modified xDelta */
	// If image's horizontal size to show is longer than winW, set it to center.
	if ((imgW - 1) * scale < winW) {
		newDelta.rx() = winW / 2 - scale * (imgW - 1) / 2;
		// If image's horizontal delta is less than winW, set it to edge.
	} else if (newDelta.x() > 0) {
		newDelta.rx() = 0;
		// If image's horizontal delta is bigger than 0, set it to edge.
	} else if ((imgW - 1) * scale + newDelta.x() < winW) {
		newDelta.rx() = winW - (imgW - 1) * scale;
	}

	/* modified yDelta */
	// If image's vertical size to show is longer than winW, set it to center.
	if ((imgH - 1) * scale < winH) {
		newDelta.ry() = winH / 2 - scale * (imgH - 1) / 2;
		// If image's vertical delta is less than winW, set it to edge.
	} else if (newDelta.ry() > 0) {
		newDelta.ry() = 0;
		// If image's vertical delta is bigger than 0, set it to edge.
	} else if ((imgH - 1) * scale + newDelta.y() < winH) {
		newDelta.ry() = winH - (imgH - 1) * scale;
	}

	/* draw image */
	QPainter painter(this);
	if (loading) {
		painter.drawText(QRect(winW / 2 - 100 / 2, winH / 2 - 20 / 2, 100, 20), Qt::AlignCenter, "Loading Image...");	// draw loading text
	} else if (showImage.isNull()) {
		painter.drawText(QRect(winW / 2 - 50 / 2, winH / 2 - 20 / 2, 50, 20), Qt::AlignCenter, "No Image.");	// draw text
	} else {
		QRectF rect(newDelta.x() - 0.5 * scale, newDelta.y() - 0.5 * scale, imgW * scale, imgH * scale);	// draw range
		painter.drawImage(rect, showImage);	// draw image
	}
	
	/* draw 4 line first */
	if (rawImage4Points.size() > 1) {
		for (int i = 0; i < rawImage4Points.size() - 1; ++i) {
			painter.setPen(QPen(Qt::green, 3));
			painter.drawLine(rawImage4Points[i] * scale + newDelta, rawImage4Points[i + 1] * scale + newDelta);
		}
		if (rawImage4Points.size() == 4) {
			painter.drawLine(rawImage4Points[0] * scale + newDelta, rawImage4Points[3] * scale + newDelta);
		}
	}

	/* draw 4 points second */
	if (rawImage4Points.size() > 0) {
		for (int i = 0; i < rawImage4Points.size(); ++i) {
			painter.setPen(QPen(Qt::red, 5));
			painter.drawPoint(rawImage4Points[i] * scale + newDelta);
		}
	}

	/* draw 2 line first */
	if (rawImage2Points.size() > 1) {
		painter.setPen(QPen(Qt::blue, 3));
		painter.drawLine(rawImage2Points[0] * scale + newDelta, rawImage2Points[1] * scale + newDelta);
	}

	/* draw 2 points second */
	if (rawImage2Points.size() > 0) {
		for (int i = 0; i < rawImage2Points.size(); ++i) {
			painter.setPen(QPen(Qt::red, 5));
			painter.drawPoint(rawImage2Points[i] * scale + newDelta);
		}
	}

	/* set for setStyleSheet */
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ShowImage::perspectiveTransform()
{
	QVector<QPointF> L ;
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

	float A1 = std::sqrt(std::pow(bPt[0].x - bPt[1].x, 2) + std::pow(bPt[0].y - bPt[1].y, 2));
	float B1 = std::sqrt(std::pow(bPt[1].x - bPt[2].x, 2) + std::pow(bPt[1].y - bPt[2].y, 2));
	float A2 = std::sqrt(std::pow(bPt[2].x - bPt[3].x, 2) + std::pow(bPt[2].y - bPt[3].y, 2));
	float B2 = std::sqrt(std::pow(bPt[3].x - bPt[0].x, 2) + std::pow(bPt[3].y - bPt[0].y, 2));
	float A = A1 > A2 ? A1 : A2;
	float B = B1 > B2 ? B1 : B2;
	float Ar = B * float(realSize.x()) / float(realSize.y());
	float Br = A * float(realSize.y()) / float(realSize.x());
	
	float W, H;
	if (Br >= B) {
		W = A;
		H = Br;
	} else {
		W = Ar;
		H = B;
	}

	warpImage2Points << QPointF(0, 0) << QPointF(W, 0) << QPointF(W, H) << QPointF(0, H) << QPointF(0, 0);
	cv::Point2f aPt[4] = { cv::Point2f(0, 0), cv::Point2f(W, 0), cv::Point2f(W, H), cv::Point2f(0, H) };
	cv::Mat rawImg = ShowImage::QImage2Mat(rawImage);
	cv::Mat perspectiveMatrix = cv::getPerspectiveTransform(bPt, aPt);
	cv::Mat warpImg;
	cv::warpPerspective(rawImg, warpImg, perspectiveMatrix, cv::Size(W, H), cv::INTER_CUBIC);
	warpImage = ShowImage::Mat2QImage(warpImg);

	cv::Matx33f warpMatrix = perspectiveMatrix;
	for (size_t i = 0; i < rawImage2Points.size(); ++i) {
		cv::Point3f p(rawImage2Points[i].x(), rawImage2Points[i].y(), 1);
		p = warpMatrix * p;
		p = p * (1.0f / p.z);
		warpImage2Points << QPointF(p.x, p.y);
	}
}

void ShowImage::getState(int state)
{
	if (state == Qt::Unchecked) {
		showImage = rawImage;
		initial();
	} else {
		perspectiveTransform();
		showImage = warpImage;
		initial();
	}
}

void ShowImage::getRealSize(QPointF size)
{
	realSize = size;
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
		for (size_t row = 0; row < mat.rows; ++row)	{
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
	}
	return mat;
}