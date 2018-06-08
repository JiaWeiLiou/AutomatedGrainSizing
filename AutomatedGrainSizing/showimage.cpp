#include "showimage.h"

ShowImage::ShowImage(QWidget *parent)
	: QWidget(parent)
{
	initial();

	/* set widget*/
	setMouseTracking(true);								// tracking mouse location
	setAcceptDrops(true);								// set widget can be drop
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

	/* clear image4Points */
	image4Points.clear();
	image2Points.clear();
}

void ShowImage::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) {
		event->acceptProposedAction();
	}
}

void ShowImage::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty()) {
		return;
	}

	fileName = urls.first().toLocalFile();
	if (fileName.isEmpty()) {
		return;
	}

	img.load(fileName);		// load image
	initial();				// reset parameter
	update();
}

void ShowImage::resizeEvent(QResizeEvent *event)
{
	if (scale > minScale) {
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
	if (scale > minScale) {
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
	if (event->buttons() == Qt::LeftButton) {
		if (scale > minScale) {
			setCursor(Qt::ClosedHandCursor);	// set cursor to closed hand type
		} else {
			setCursor(Qt::ArrowCursor);	// set cursor to arrow type
		}
		pos1 = QPointF(event->pos());
		update();
		// set point
	} else if (event->buttons() == Qt::RightButton) {
		// set point's location can be mod
		modified = false;
		for (size_t i = 0; i < image4Points.size(); ++i) {
			QPointF pixelPos = image4Points[i] * scale + newDelta;
			QPointF mousePos = QPointF(event->pos());
			if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 50) {
				setCursor(Qt::CrossCursor);										// set cursor to cross type
				QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
				// limit the point in the image
				if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
					// record file.
					image4Points[i] = imagePos;
					if (image4Points.size() == 5 && i == 0) {
						image4Points[4] = imagePos;	// modify end point
					}
					outBorder = false;
					modified = true;
					update();
				} else {
					outBorder = true;
				}
				break;
			}
		}

		// limit the point number to image
		if (image4Points.size() < 4 && !modified) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			// limit the point in the image
			if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
				// record file.
				image4Points.push_back(imagePos);
				outBorder = false;
				update();
			} else {
				outBorder = true;
			}
		} else if (image2Points.size() < 2 && !modified) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			// limit the point in the image
			if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
				// record file.
				image2Points.push_back(imagePos);
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
	if (event->buttons() == Qt::LeftButton) {
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
	} else if (event->buttons() == Qt::RightButton) {
		// set point's location can be mod
		if (modified) {
			for (size_t i = 0; i < image4Points.size(); ++i) {
				QPointF pixelPos = image4Points[i] * scale + newDelta;
				QPointF mousePos = QPointF(event->pos());
				if (qSqrt(qPow(mousePos.x() - pixelPos.x(), 2) + qPow(mousePos.y() - pixelPos.y(), 2)) < 50) {
					setCursor(Qt::CrossCursor);	// set cursor to cross type
					QPointF imagePos = (mousePos - newDelta) / scale;	// change to image's pixel coordinate
					// limit the point in the image
					if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
						// record file.
						image4Points[i] = imagePos;
						if (image4Points.size() == 5 && i == 0) {
							image4Points[4] = imagePos;	// modify end point
						}
						outBorder = false;
						update();
					} else {
						outBorder = true;
					}
					break;
				}
			}
		}

		// limit the point number to image
		if (image4Points.size() <= 4 && !modified) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
			// limit the point in the image
			if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
				// record file.
				// mousePress points is out of border 
				if (outBorder) {
					image4Points.push_back(imagePos);
					outBorder = false;
					// mousePress points isn't out of border 
				} else {
					image4Points[image4Points.size() - 1] = imagePos;
				}
				update();
			}
		} else if (image2Points.size() <= 2 && !modified) {
			setCursor(Qt::CrossCursor);	// set cursor to cross type
			QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;	// change to image's pixel coordinate
																			// limit the point in the image
			if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
				// record file.
				// mousePress points is out of border 
				if (outBorder) {
					image2Points.push_back(imagePos);
					outBorder = false;
					// mousePress points isn't out of border 
				} else {
					image2Points[image2Points.size() - 1] = imagePos;
				}
				update();
			}
		}
	}
}

void ShowImage::mouseReleaseEvent(QMouseEvent *event)
{
	if (scale > minScale) {
		setCursor(Qt::OpenHandCursor);	// set cursor to open hand type
	} else {
		setCursor(Qt::ArrowCursor);	// set cursor to arrow type
	}
	
	if (event->button() == Qt::LeftButton) {

		oldDelta = newDelta;	// record the distance of drag image

	} else if (event->button() == Qt::RightButton) {
		// points has been mod and set to next with false
		modified == false;

		if (image4Points.size() == 4) {
			image4Points.push_back(image4Points[0]);	// put first point back to prevent press and move to add new point
		} else if (image2Points.size() == 2) {
			image2Points.push_back(image2Points[0]);	// put first point back to prevent press and move to add new point
		}

		size_t sum = image4Points.size() + image2Points.size();
		if (image4Points.size() == 5) {
			sum -= 1;
		}
		if (image2Points.size() == 3) {
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
		image4Points.clear();	// clear 4 points
		image2Points.clear();	// clear 2 points
		update();
	} else if (event->key() == Qt::Key_Backspace) {
		if (image2Points.size() > 0) {
			if (image2Points.size() == 3) {
				image2Points.pop_back();	// delete points
			}
			image2Points.pop_back();	// delete points
			update();
		} else if (image4Points.size() > 0) {
			if (image4Points.size() == 5) {
				image4Points.pop_back();	// delete points
			}
			image4Points.pop_back();	// delete points
			update();
		}
	}

	size_t sum = image4Points.size() + image2Points.size();
	if (image4Points.size() == 5) {
		sum -= 1;
	}
	if (image2Points.size() == 3) {
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

	QPainter painter(this);

	/* draw image */
	QRectF rect(newDelta.x() - 0.5 * scale, newDelta.y() - 0.5 * scale, imgW * scale, imgH * scale);	// draw range
	painter.drawImage(rect, img);	// draw image

	/* draw 4 line first */
	if (image4Points.size() > 1) {
		for (int i = 0; i < image4Points.size() - 1; ++i) {
			painter.setPen(QPen(Qt::green, 3));
			painter.drawLine(image4Points[i] * scale + newDelta, image4Points[i + 1] * scale + newDelta);
		}
		if (image4Points.size() == 4) {
			painter.drawLine(image4Points[0] * scale + newDelta, image4Points[3] * scale + newDelta);
		}
	}

	/* draw 4 points second */
	if (image4Points.size() > 0) {
		for (int i = 0; i < image4Points.size(); ++i) {
			painter.setPen(QPen(Qt::red, 5));
			painter.drawPoint(image4Points[i] * scale + newDelta);
		}
	}

	/* draw 2 line first */
	if (image2Points.size() > 1) {
		painter.setPen(QPen(Qt::blue, 3));
		painter.drawLine(image2Points[0] * scale + newDelta, image2Points[1] * scale + newDelta);
	}

	/* draw 2 points second */
	if (image2Points.size() > 0) {
		for (int i = 0; i < image2Points.size(); ++i) {
			painter.setPen(QPen(Qt::red, 5));
			painter.drawPoint(image2Points[i] * scale + newDelta);
		}
	}
}