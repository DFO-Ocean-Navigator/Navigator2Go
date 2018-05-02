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
	void on_actionAbout_Qt_triggered();

	void on_actionOpen_triggered();

private:
	Ui::MainWindow* m_ui;
};

#endif // MAINWINDOW_H
