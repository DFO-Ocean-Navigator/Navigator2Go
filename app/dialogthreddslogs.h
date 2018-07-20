#ifndef DIALOGTHREDDSLOGS_H
#define DIALOGTHREDDSLOGS_H

#include <QDialog>

namespace Ui {
class DialogTHREDDSLogs;
}

class DialogTHREDDSLogs : public QDialog
{
	Q_OBJECT

public:
	explicit DialogTHREDDSLogs(QWidget *parent = 0);
	~DialogTHREDDSLogs();

private:
	Ui::DialogTHREDDSLogs *ui;
};

#endif // DIALOGTHREDDSLOGS_H
