#include "showmainwindow.h"

ShowMainWindow::ShowMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	QTime t;
	t.start();
	while (t.elapsed()<1000);
	setWindowTitle("Automated Grain Sizing");
	showWidget = new ShowWidget(this);
	setCentralWidget(showWidget);
	setAcceptDrops(true);								// set main window can be drop
	createActions();
	createMenus();
}

void ShowMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) {
		event->acceptProposedAction();
	}
}

void ShowMainWindow::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty()) {
		return;
	}

	filePathName = urls.first().toLocalFile();
	if (filePathName.isEmpty()) {
		return;
	}

	int pos1 = filePathName.lastIndexOf('/');
	QString fileName = filePathName.right(filePathName.size() - pos1 - 1);		//file name

	setWindowTitle(fileName + QString(" - Automated Grain Sizing"));		// set windows title

	showWidget->warpCheckBox->setCheckState(Qt::Unchecked);					// set unclick

	bool success = showWidget->imageWidget->loadImage(filePathName);		// store image
	if (!success) {
		setWindowTitle(QString("Automated Grain Sizing"));					// set windows title
	} else {
		loadParameterAction->setEnabled(true);
		closeImageAction->setEnabled(true);
	}
}

void ShowMainWindow::createActions()
{
	// Open Image
	openImageAction = new QAction("&Open Image(O)...", this);
	openImageAction->setShortcut(tr("Ctrl+O"));
	connect(openImageAction, SIGNAL(triggered()), this, SLOT(showOpenImage()));

	// Load Parameter
	loadParameterAction = new QAction("&Load Parameter(L)...", this);
	loadParameterAction->setEnabled(false);
	loadParameterAction->setShortcut(tr("Ctrl+L"));
	connect(loadParameterAction, SIGNAL(triggered()), this, SLOT(showLoadParameter()));

	// Close Image
	closeImageAction = new QAction("&Close Image(C)", this);
	closeImageAction->setEnabled(false);
	closeImageAction->setShortcut(tr("Ctrl+C"));
	connect(closeImageAction, SIGNAL(triggered()), this, SLOT(showCloseImage()));

	// Quit
	quitAction = new QAction("&Quit(Q)", this);
	quitAction->setShortcut(tr("Ctrl+Q"));
	connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

	// Information
	information = new QAction("Information", this);
	//connect(information, SIGNAL(triggered()), this, SLOT(close()));

	// Information
	about = new QAction("About", this);
	//connect(about, SIGNAL(triggered()), this, SLOT(QApplication::aboutQt()));
}

void ShowMainWindow::createMenus()
{
	// File Menu
	fileMenu = menuBar()->addMenu("&File(F)");
	fileMenu->addAction(openImageAction);
	fileMenu->addAction(loadParameterAction);
	fileMenu->addSeparator();
	fileMenu->addAction(closeImageAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	// Options Menu
	optionsMenu = menuBar()->addMenu("&Options(O)");

	// Help Menu
	helpMenu = menuBar()->addMenu("&Help(H)");
	helpMenu->addAction(information);
	helpMenu->addAction(about);
}

void ShowMainWindow::showOpenImage()
{
	QString _path = QProcessEnvironment::systemEnvironment().value("USERPROFILE") + "\\Desktop";
	filePathName = QFileDialog::getOpenFileName(this, "Open Image", _path, "JPEG (*.JPG;*.JPEG;*.JPE);;PNG (*.PNG;*.PNS);;BMP (*.BMP;*.RLE;*.DIB);;TIFF (*.TIF;*.TIFF);;All Files (*.*)");
	if (filePathName.isEmpty()) {
		return;
	}

	int pos1 = filePathName.lastIndexOf('/');
	QString fileName = filePathName.right(filePathName.size() - pos1 - 1);		//file name

	setWindowTitle(fileName + QString(" - Automated Grain Sizing"));		// set windows title

	showWidget->warpCheckBox->setCheckState(Qt::Unchecked);					// set unclick

	bool success = showWidget->imageWidget->loadImage(filePathName);		// store image
	if (!success) {
		setWindowTitle(QString("Automated Grain Sizing"));					// set windows title
	} else {
		loadParameterAction->setEnabled(true);
		closeImageAction->setEnabled(true);
	}
}

void ShowMainWindow::showLoadParameter()
{
	QString _path = QProcessEnvironment::systemEnvironment().value("USERPROFILE") + "\\Desktop";
	QString paramFilePathName = QFileDialog::getOpenFileName(this, "Load Parameter", _path, "PARAM (*.PARAM);;All Files (*.*)");
	showWidget->imageWidget->loadParameter(paramFilePathName);
}

void ShowMainWindow::showCloseImage()
{
	loadParameterAction->setEnabled(false);
	closeImageAction->setEnabled(false);
	showWidget->warpCheckBox->setCheckState(Qt::Unchecked);		// set unclick
	showWidget->imageWidget->clearPoints();
	repaint();						// repaint image widget
	showWidget->imageWidget->showImage = QImage();
	showWidget->imageWidget->loading = false;					// show loading
	showWidget->imageWidget->initial();							// initial image widget
}