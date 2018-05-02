#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>

/***********************************************************************************/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow) {
	m_ui->setupUi(this);

	// Set dark stylesheet
	QFile f(":qdarkstyle/style.qss");
	if (!f.exists()) {
		qDebug() << "Unable to set stylesheet, file not found.";
	}
	else {
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}
}

/***********************************************************************************/
MainWindow::~MainWindow() {
	delete m_ui;
}

/***********************************************************************************/
void MainWindow::on_actionAbout_Qt_triggered() {
	QMessageBox::aboutQt(this);
}

/***********************************************************************************/
void MainWindow::on_actionOpen_triggered() {

}
