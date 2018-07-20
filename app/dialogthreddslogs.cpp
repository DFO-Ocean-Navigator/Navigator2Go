#include "dialogthreddslogs.h"
#include "ui_dialogthreddslogs.h"

DialogTHREDDSLogs::DialogTHREDDSLogs(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogTHREDDSLogs)
{
	ui->setupUi(this);
}

DialogTHREDDSLogs::~DialogTHREDDSLogs()
{
	delete ui;
}
