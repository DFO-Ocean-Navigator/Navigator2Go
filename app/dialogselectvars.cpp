#include "dialogselectvars.h"
#include "ui_dialogselectvars.h"

#include <netcdf4/ncVar.h>

/***********************************************************************************/
DialogSelectVars::DialogSelectVars(const netCDF::NcFile& ds, QWidget* parent) :	QDialog{parent},
																				m_ui(new Ui::DialogSelectVars) {
	m_ui->setupUi(this);

	m_ui->tableWidget->horizontalHeader()->setStretchLastSection(true); // Resize columns to widget width
	m_ui->tableWidget->setHorizontalHeaderLabels({	tr("Import?"),
													tr("Variable Key")
												 });

	const auto& vars{ ds.getVars() };
	for (const auto& var : vars) {
		m_ui->tableWidget->insertRow(m_ui->tableWidget->rowCount());
		const auto rowIdx{ m_ui->tableWidget->rowCount() - 1 };

		auto* const hidden{ new QTableWidgetItem() };
		hidden->setCheckState(Qt::Unchecked);
		m_ui->tableWidget->setItem(rowIdx, 0, hidden);


		auto* const item{ new QTableWidgetItem(var.first.c_str())};
		std::string long_name;
		var.second.getAtt("long_name").getValues(long_name);
		item->setToolTip(QString::fromStdString(long_name));

		m_ui->tableWidget->setItem(rowIdx, 1, item);
	}

}

/***********************************************************************************/
DialogSelectVars::~DialogSelectVars() {
	delete m_ui;
}

/***********************************************************************************/
QStringList DialogSelectVars::GetSelectedVars() const {
	QStringList selectedVars;

	for (auto row = 0; row < m_ui->tableWidget->rowCount(); ++row) {
		if (m_ui->tableWidget->item(row, 0)->checkState() == Qt::Checked) {
			selectedVars << m_ui->tableWidget->item(row, 1)->text();
		}
	}

	return selectedVars;
}

/***********************************************************************************/
void DialogSelectVars::on_tableWidget_cellClicked(int row, int column) {
	const auto oldCheckState{ m_ui->tableWidget->item(row, 0)->checkState() };

	if (oldCheckState == Qt::Checked) {
		m_ui->tableWidget->item(row, 0)->setCheckState(Qt::Unchecked);
	}
	else {
		m_ui->tableWidget->item(row, 0)->setCheckState(Qt::Checked);
	}
}
