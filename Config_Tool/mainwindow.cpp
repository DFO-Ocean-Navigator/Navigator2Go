#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_datasetview.h"

#include "dialogdatasetview.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

/***********************************************************************************/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow) {
	m_ui->setupUi(this);

	setWindowTitle(tr("Ocean Navigator Dataset Config Editor"));

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

	statusBar()->showMessage(tr("Open a config file from File -> Open, or use CTRL + O."));

	m_ui->labelList->setText(tr("Double-click on a dataset to edit it's properties, or click on the Add Dataset button."));
	m_ui->labelList->setVisible(false);
	m_ui->buttonAddDataset->setText(tr("Add Dataset"));
	m_ui->buttonAddDataset->setEnabled(false);
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
	QFileDialog dialog(this);
	dialog.setWindowTitle(tr("Open Dataset Config File"));
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Config Files (*.json)"));
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory("/home/nabil/");

	// Open file dialog
	if (dialog.exec()) {
		m_configFileName = dialog.selectedFiles().at(0); // Get selected file.
	}

	// If a file was actually selected (ignore cancel or close)
	if (!m_configFileName.isEmpty()) {

		// Open file
		QFile f(m_configFileName);
		f.open(QFile::ReadOnly | QFile::Text);
		const QString contents = f.readAll();
		f.close();

		// Parse json
		QJsonParseError error;
		const auto jsonDocument = QJsonDocument::fromJson(contents.toUtf8(), &error);
		// Check for errors
		if (jsonDocument.isNull()) {
			QMessageBox msgBox(this);
			msgBox.setText(tr("Error parsing JSON file."));
			msgBox.setInformativeText(error.errorString());
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();

			return;
		}
		m_documentRootObject = jsonDocument.object(); // Get copy of root object

		for (const auto& datasetName : m_documentRootObject.keys()) {
			m_ui->listWidget->addItem(datasetName);
		}

		m_ui->labelList->setVisible(true);
		m_ui->buttonAddDataset->setEnabled(true);
		statusBar()->showMessage("");
	}
}

/***********************************************************************************/
void MainWindow::on_actionClose_triggered() {

	if (m_isUnsavedData) {
		// show confirm quit dialog
	}

	close();
}


/***********************************************************************************/
void MainWindow::on_actionSave_triggered() {

	if (!m_configFileName.isEmpty()) {
		const QJsonDocument doc(m_documentRootObject);

		QSaveFile f("/home/nabil/test.txt");
		f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate); // Overwrite original file.
		f.write(doc.toJson());
		f.commit();

		statusBar()->showMessage(tr("Config file saved!"));

		m_isUnsavedData = false;
	}
}

/***********************************************************************************/
void MainWindow::on_buttonAddDataset_clicked() {
	m_isUnsavedData = true;

	m_ui->listWidget->addItem("new_dataset_" + QString::number(qrand()));
}

/***********************************************************************************/
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {

	DialogDatasetView dialog(this);
	const auto datasetKey = item->text();
	dialog.SetData(datasetKey, m_documentRootObject[datasetKey].toObject());

	if (dialog.exec()) {
		const auto data = dialog.GetData();

		m_documentRootObject[datasetKey] = data.second;
	}
}
