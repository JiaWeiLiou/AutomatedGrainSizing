#pragma once

#include <QtWidgets/QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include "showwidget.h"

class AutomaticGrainSizing : public QMainWindow
{
	Q_OBJECT

public:
	AutomaticGrainSizing(QWidget *parent = Q_NULLPTR);
	void createActions();
	void createMenus();
private:
	ShowWidget *showWidget;

	// Menu Bar
	QMenu *fileMenu;
	QMenu *optionsMenu;
	QMenu *helpMenu;

	// File Menu Action
	QAction *openImageAction;
	QAction *loadParameterAction;
	QAction *closeImageAction;
	QAction *saveParameterAction;
	QAction *saveParameterAsAction;
	QAction *quitAction;

	// Options Menu Action
	QAction *eliminatingAmbientBrightness;
	QAction *imageThreshold;
	QAction *removeNoise;
	QAction *hmaximaTransform;

	// Help Menu Action
	QAction *information;
	QAction *about;
};
