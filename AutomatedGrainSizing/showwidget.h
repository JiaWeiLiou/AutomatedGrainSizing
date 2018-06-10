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

	ShowImage		*imageWidget;
	QCheckBox		*warpCheckBox;
	QLabel			*widthLabel;
	QLineEdit		*widthLineEdit;
	QLabel			*wUintLabel;
	QLabel			*heightLabel;
	QLineEdit		*heightLineEdit;
	QLabel			*hUintLabel;
	QPushButton		*startPushButton;
	QPointF realSize;
	int mumax;
	vector<float> ellipseM;
	vector<float> ellipseL;

signals:
	void emitRealSize(QPointF);

private slots:
	void setWidgetEnable();
	void setLineEditEnable(int checkBoxState);
};