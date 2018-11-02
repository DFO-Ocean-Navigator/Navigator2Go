#include "widgetmonthpicker.h"
#include "ui_widgetmonthpicker.h"

#include <QHash>
#include <QMessageBox>

/***********************************************************************************/
// http://doc.qt.io/qt-5/qdate.html#month
const QHash<int, QString> indexToMonth {
	{1, QStringLiteral("January")},
	{2, QStringLiteral("February")},
	{3, QStringLiteral("March")},
	{4, QStringLiteral("April")},
	{5, QStringLiteral("May")},
	{6, QStringLiteral("June")},
	{7, QStringLiteral("July")},
	{8, QStringLiteral("August")},
	{9, QStringLiteral("September")},
	{10, QStringLiteral("October")},
	{11, QStringLiteral("November")},
	{12, QStringLiteral("December")}
};

const QHash<QString, int> monthToIndex {
	{QStringLiteral("January"), 1},
	{QStringLiteral("February"), 2},
	{QStringLiteral("March"), 3},
	{QStringLiteral("April"), 4},
	{QStringLiteral("May"), 5},
	{QStringLiteral("June"), 6},
	{QStringLiteral("July"), 7},
	{QStringLiteral("August"), 8},
	{QStringLiteral("September"), 9},
	{QStringLiteral("October"), 10},
	{QStringLiteral("November"), 11},
	{QStringLiteral("December"), 12}
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
std::pair<QDate, QDate> WidgetMonthPicker::getStartEndDate() const {
	return {
		{
			m_ui->comboBoxStartYear->currentText().toInt(),
			monthToIndex[m_ui->comboBoxStartMonth->currentText()],
			1
		},
		{
			m_ui->comboBoxEndYear->currentText().toInt(),
			monthToIndex[m_ui->comboBoxEndMonth->currentText()],
			1
		}

	};
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
			comboBox->addItem(indexToMonth[i]);
		}
		return;
	}

	if (year == m_endDate.year()) {
		for (auto i = 1; i < m_endDate.month() + 1; ++i) {
			comboBox->addItem(indexToMonth[i]);
		}
		// Set end month to be latest month.
		comboBox->setCurrentIndex(comboBox->count() - 1);
		return;
	}

	for (auto i = 1; i < 13; ++i) {
		comboBox->addItem(indexToMonth[i]);
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
