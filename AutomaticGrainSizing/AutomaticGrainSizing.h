#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AutomaticGrainSizing.h"

class AutomaticGrainSizing : public QMainWindow
{
	Q_OBJECT

public:
	AutomaticGrainSizing(QWidget *parent = Q_NULLPTR);

private:
	Ui::AutomaticGrainSizingClass ui;
};
