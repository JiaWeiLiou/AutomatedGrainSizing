#include "showimage.h"

ShowImage::ShowImage(QWidget *parent)
	: QWidget(parent)
{
	initial();
	clearPoints();
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

	repaint();
}

void ShowImage::clearPoints()
{
	/* clear points */
	if (checkBoxState == Qt::Unchecked) {
		rawImage4Points.clear();
		rawImage2Points.clear();
		image4PointsFull = 0;
		image2PointsFull = 0;
		image4PointModified = 1;
		emit pointsNumberChanged();
	} else {
		warpImage2Points.clear();
		image2PointsFull = 0;
		emit pointsNumberChanged();
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
		modifiedPointState = 0;

		if (checkBoxState == Qt::Unchecked) {
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
						image4PointModified = 1;
						outBorder = false;
						modifiedPointState = i + 1;
						update();
					} else {
						outBorder = true;
					}
					break;
				}
			}

			// mod 2 points
			if (!modifiedPointState) {
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
							outBorder = false;
							modifiedPointState = i + 5;
							update();
						} else {
							outBorder = true;
						}
						break;
					}
				}
			}

			// limit the point number to image
			if (!modifiedPointState && !image4PointsFull) {
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					rawImage4Points.push_back(imagePos);
					image4PointModified = 1;
					emit pointsNumberChanged();
					outBorder = false;
					update();
				} else {
					outBorder = true;
				}
			} else if (!modifiedPointState && !image2PointsFull) {
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					rawImage2Points.push_back(imagePos);
					emit pointsNumberChanged();
					outBorder = false;
					update();
				} else {
					outBorder = true;
				}
			}
		} else {
			// mod 2 points
			for (size_t i = 0; i < warpImage2Points.size(); ++i) {
				QPointF pixelPos = warpImage2Points[i] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				// distance lower than 5
				if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 5) {
					setCursor(Qt::CrossCursor);										// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
					// limit the point in the image
					if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
						// record file.
						warpImage2Points[i] = imagePos;
						outBorder = false;
						modifiedPointState = i + 5;
						update();
					} else {
						outBorder = true;
					}
					break;
				}
			}
			if (!modifiedPointState && !image2PointsFull) {
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					warpImage2Points.push_back(imagePos);
					emit pointsNumberChanged();
					outBorder = false;
					update();
				} else {
					outBorder = true;
				}
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

		if (checkBoxState == Qt::Unchecked) {
			// set point's location can be mod
			// mod 4 points
			if (modifiedPointState > 0 && modifiedPointState <= 4) {
				QPointF pixelPos = rawImage4Points[modifiedPointState - 1] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				// move distance lower than 50
				if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 50) {
					setCursor(Qt::CrossCursor);	// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
					// limit the point in the image
					if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
						// record file.
						rawImage4Points[modifiedPointState - 1] = imagePos;
						image4PointModified = 1;
						outBorder = false;
						update();
					} else {
						outBorder = true;
					}
				}
			// mod 2 points
			} else if (modifiedPointState > 4) {
				QPointF pixelPos = rawImage2Points[modifiedPointState - 5] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				// move distance lower than 50
				if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 50) {
					setCursor(Qt::CrossCursor);	// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
					// limit the point in the image
					if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
						// record file.
						rawImage2Points[modifiedPointState - 5] = imagePos;
						outBorder = false;
						update();
					} else {
						outBorder = true;
					}
				}
			}

			// limit the point number to image
			if (!modifiedPointState && !image4PointsFull) {
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					// mousePress points is out of border 
					if (outBorder) {
						rawImage4Points.push_back(imagePos);
						image4PointModified = 1;
						outBorder = false;
						// mousePress points isn't out of border 
					} else {
						rawImage4Points[rawImage4Points.size() - 1] = imagePos;
					}
					update();
				}
			} else if (!modifiedPointState && !image2PointsFull) {
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
		} else {
			// mod 2 points
			if (modifiedPointState > 4) {
				QPointF pixelPos = warpImage2Points[modifiedPointState - 5] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				// move distance lower than 50
				if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 50) {
					setCursor(Qt::CrossCursor);	// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
					// limit the point in the image
					if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
						// record file.
						warpImage2Points[modifiedPointState - 5] = imagePos;
						outBorder = false;
						update();
					} else {
						outBorder = true;
					}
				}
			}
			if (!modifiedPointState && !image2PointsFull) {
				setCursor(Qt::CrossCursor);	// set cursor to cross type
				QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					// mousePress points is out of border 
					if (outBorder) {
						warpImage2Points.push_back(imagePos);
						outBorder = false;
						// mousePress points isn't out of border 
					} else {
						warpImage2Points[warpImage2Points.size() - 1] = imagePos;
					}
					update();
				}
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
		modifiedPointState = 0;
		if (checkBoxState == Qt::Unchecked) {
			image4PointsFull = rawImage4Points.size() == 4 ? 1 : 0;
			image2PointsFull = rawImage2Points.size() == 2 ? 1 : 0;
		} else {
			image2PointsFull = warpImage2Points.size() == 2 ? 1 : 0;
		}
	}
	update();
}

void ShowImage::keyPressEvent(QKeyEvent *event)
{
	// press keyboard Esc to give up setting point
	if (event->key() == Qt::Key_Escape) {
		clearPoints();
		update();
	} else if (event->key() == Qt::Key_Backspace) {
		if (checkBoxState == Qt::Unchecked) {
			if (rawImage2Points.size()) {
				rawImage2Points.pop_back();	// delete points
				image2PointsFull = 0;
				emit pointsNumberChanged();
				update();
			} else if (rawImage4Points.size()) {
				rawImage4Points.pop_back();	// delete points
				image4PointsFull = 0;
				emit pointsNumberChanged();
				update();
			}
		} else {
			if (warpImage2Points.size()) {
				warpImage2Points.pop_back();	// delete points
				image2PointsFull = 0;
				emit pointsNumberChanged();
				update();
			}
		}
	}
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

	if (checkBoxState == Qt::Unchecked) {
	/* draw 4 line first */
		if (rawImage4Points.size() > 1) {
			for (int i = 0; i < rawImage4Points.size() - 1; ++i) {
				painter.setPen(QPen(Qt::green, 3));
				painter.drawLine(rawImage4Points[i] * scale + newDelta, rawImage4Points[(i + 1)] * scale + newDelta);
				if (rawImage4Points.size() == 4) {
					painter.drawLine(rawImage4Points[0] * scale + newDelta, rawImage4Points[3] * scale + newDelta);
				}
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
	} else {
		/* draw 2 line first */
		if (warpImage2Points.size() > 1) {
			painter.setPen(QPen(Qt::blue, 3));
			painter.drawLine(warpImage2Points[0] * scale + newDelta, warpImage2Points[1] * scale + newDelta);
		}
		/* draw 2 points second */
		if (warpImage2Points.size() > 0) {
			for (int i = 0; i < warpImage2Points.size(); ++i) {
				painter.setPen(QPen(Qt::red, 5));
				painter.drawPoint(warpImage2Points[i] * scale + newDelta);
			}
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
	if (image4PointModified) {
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

		warpImage4Points << QPointF(0, 0) << QPointF(W, 0) << QPointF(W, H) << QPointF(0, H) << QPointF(0, 0);
		cv::Point2f aPt[4] = { cv::Point2f(0, 0), cv::Point2f(W, 0), cv::Point2f(W, H), cv::Point2f(0, H) };
		cv::Mat rawImg = ShowImage::QImage2Mat(rawImage);
		perspectiveMatrix = cv::getPerspectiveTransform(bPt, aPt);

		cv::Mat warpImg;
		cv::warpPerspective(rawImg, warpImg, perspectiveMatrix, cv::Size(W, H), cv::INTER_CUBIC);
		warpImage = ShowImage::Mat2QImage(warpImg);
	}
}

void ShowImage::getRealSize(QPointF size)
{
	realSize = size;
}

void ShowImage::getCheckBoxState(int checkBoxState)
{
	if (checkBoxState == Qt::Unchecked) {
		ShowImage::checkBoxState = Qt::Unchecked;

		showImage = rawImage;
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

		initial();
	} else {
		ShowImage::checkBoxState = Qt::Checked;
		perspectiveTransform();
		image4PointModified = 0;
		showImage = warpImage;
		if (warpImage2Points.size() > rawImage2Points.size()) {
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

		initial();
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