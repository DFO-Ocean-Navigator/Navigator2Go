#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class MainWindow;
}
class QListWidgetItem;

/***********************************************************************************/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:

	// Menu callbacks
	void on_actionAbout_Qt_triggered();
	void on_actionOpen_triggered();
	void on_actionClose_triggered();
	void on_actionSave_triggered();

	// Add Dataset butto callback
	void on_buttonAddDataset_clicked();

	// List item is double clicked
	void on_listWidget_itemDoubleClicked(QListWidgetItem* item);

	void on_pushButtonDeleteDataset_clicked();

private:
	Ui::MainWindow* m_ui;

	QString m_configFileName;
	QJsonObject m_documentRootObject;

	bool m_isUnsavedData = false;
};

#endif // MAINWINDOW_H
