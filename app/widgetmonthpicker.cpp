#include "widgetmonthpicker.h"
#include "ui_widgetmonthpicker.h"

#include <QHash>
#include <QMessageBox>

/***********************************************************************************/
// http://doc.qt.io/qt-5/qdate.html#month
const QHash<int, QString> months {
	{1, "January"},
	{2, "February"},
	{3, "March"},
	{4, "April"},
	{5, "May"},
	{6, "June"},
	{7, "July"},
	{8, "August"},
	{9, "September"},
	{10, "October"},
	{11, "November"},
	{12, "December"}
};

/***********************************************************************************/
WidgetMonthPicker::WidgetMonthPicker(QWidget* parent) : QWidget{parent},
														m_ui{new Ui::WidgetMonthPicker} {
	m_ui->setupUi(this);
}

/***********************************************************************************/
WidgetMonthPicker::~WidgetMonthPicker() {
	delete m_ui;
}

/***********************************************************************************/
void WidgetMonthPicker::setStartEndDate(const QDate& start, const QDate& end) {
	m_startDate = start;
	m_endDate = end;

	populateMonth(start.year(), m_ui->comboBoxStartMonth);
	populateMonth(end.year(), m_ui->comboBoxEndMonth);

	// Populate years
	for (auto i = start.year(); i < end.year() + 1; ++i) {
		m_ui->comboBoxStartYear->addItem(QString::number(i));
		m_ui->comboBoxEndYear->addItem(QString::number(i));
	}
	// Set end year to be latest year.
	m_ui->comboBoxEndYear->setCurrentIndex(m_ui->comboBoxEndYear->count() - 1);

	m_showErrorDialog = true;
}

/***********************************************************************************/
void WidgetMonthPicker::on_comboBoxStartMonth_currentIndexChanged(int index) {
	validateMonthSelection();
}

/***********************************************************************************/
void WidgetMonthPicker::on_comboBoxEndMonth_currentIndexChanged(int index) {
	validateMonthSelection();
}

/***********************************************************************************/
void WidgetMonthPicker::on_comboBoxStartYear_currentIndexChanged(const QString& arg1) {
	populateMonth(arg1.toInt(), m_ui->comboBoxStartMonth);
}

/***********************************************************************************/
void WidgetMonthPicker::on_comboBoxEndYear_currentIndexChanged(const QString& arg1) {
	populateMonth(arg1.toInt(), m_ui->comboBoxEndMonth);
}

/***********************************************************************************/
void WidgetMonthPicker::populateMonth(const int year, QComboBox* comboBox) {
	comboBox->clear();

	if (year == m_startDate.year()) {
		for (auto i = m_startDate.month(); i < 13; ++i) {
			comboBox->addItem(months[i]);
		}
		return;
	}

	if (year == m_endDate.year()) {
		for (auto i = 1; i < m_endDate.month() + 1; ++i) {
			comboBox->addItem(months[i]);
		}
		// Set end month to be latest month.
		comboBox->setCurrentIndex(comboBox->count() - 1);
		return;
	}

	for (auto i = 1; i < 13; ++i) {
		comboBox->addItem(months[i]);
	}
}

/***********************************************************************************/
void WidgetMonthPicker::validateMonthSelection() {
	if (!m_showErrorDialog) {
		return;
	}
	if (m_ui->comboBoxStartYear->currentText() != m_ui->comboBoxEndYear->currentText()) {
		return;
	}

	// If the start month is after the end month
	if (m_ui->comboBoxStartMonth->currentIndex() > m_ui->comboBoxEndMonth->currentIndex()) {
		QMessageBox::critical(this,
							  tr("Error..."),
							  tr("Start date cannot be after end date!"));
	}
}
