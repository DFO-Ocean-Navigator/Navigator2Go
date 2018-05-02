#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class MainWindow;
}

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

private:
	Ui::MainWindow* m_ui;

	QString m_configFileName;
};

#endif // MAINWINDOW_H
