#pragma once

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QToolTip>
#include <QString>
#include <QPointF>
#include "showimage.h"

class ShowWidget : public QWidget
{
	Q_OBJECT
public:
	ShowWidget(QWidget *parent = Q_NULLPTR);
	ShowImage *imageWidget;

private:
	QCheckBox *warpCheckBox;
	QLabel *widthLabel;
	QLineEdit *widthLineEdit;
	QLabel *wuintLabel;
	QLabel *heightLabel;
	QLineEdit *heightLineEdit;
	QLabel *huintLabel;
	QPushButton *startPushButton;

private slots:
	void setWidgetEnable(size_t);
	void setWidgetEnable();

signals:
	void emitRealSize(QPointF);
};