#include "showmainwindow.h"

ShowMainWindow::ShowMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
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

	if (!filePathName.isEmpty()) {
		int pos1 = filePathName.lastIndexOf('/');
		filePath = filePath.left(pos1 + 1);									//�ɮ׸��|
		fileName = filePathName.right(filePathName.size() - pos1 - 1);		//�ɮצW��
	}

	setWindowTitle(fileName + QString(" - Automated Grain Sizing"));

	showWidget->imageWidget->loading = true;			// show loading
	showWidget->imageWidget->repaint();					// force to reset window
	showWidget->imageWidget->img.load(filePathName);	// store image's
	showWidget->imageWidget->loading = false;			// show loading
	showWidget->imageWidget->initial();					// initial image widget
}

void ShowMainWindow::createActions()
{
	// Open Image
	openImageAction = new QAction("&Open Image(O)...", this);
	openImageAction->setShortcut(tr("Ctrl+O"));
	openImageAction->setToolTip("Open Image");

	// Load Parameter
	loadParameterAction = new QAction("&Load Parameter(L)...", this);
	loadParameterAction->setShortcut(tr("Ctrl+L"));
	loadParameterAction->setToolTip("Load Parameter");

	// Close Image
	closeImageAction = new QAction("&Close Image(C)", this);
	closeImageAction->setShortcut(tr("Ctrl+C"));
	closeImageAction->setToolTip("Close Image");

	// Save Parameter
	saveParameterAction = new QAction("&Save Parameter(S)", this);
	saveParameterAction->setShortcut(tr("Ctrl+S"));
	saveParameterAction->setToolTip("Save Parameter");

	// Save Parameter As
	saveParameterAsAction = new QAction("&Save Parameter As(A)...", this);
	saveParameterAsAction->setShortcut(tr("Ctrl+A"));
	saveParameterAsAction->setToolTip("Save Parameter As");

	// Quit
	quitAction = new QAction("&Quit(Q)", this);
	quitAction->setShortcut(tr("Ctrl+Q"));
	quitAction->setToolTip("Quit");
}

void ShowMainWindow::createMenus()
{
	// File Menu
	fileMenu = menuBar()->addMenu("&File(F)");
	fileMenu->addAction(openImageAction);
	fileMenu->addAction(loadParameterAction);
	fileMenu->addSeparator();
	fileMenu->addAction(closeImageAction);
	fileMenu->addAction(saveParameterAction);
	fileMenu->addAction(saveParameterAsAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	// Options Menu
	optionsMenu = menuBar()->addMenu("&Options(O)");

	// Help Menu
	helpMenu = menuBar()->addMenu("&Help(H)");
}