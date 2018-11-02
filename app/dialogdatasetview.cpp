#include "ui_dialogdatasetview.h"
#include "dialogdatasetview.h"

#include "dialogselectvars.h"

#include "htmlhighlighter.h"
#include "network.h"

#include <QJsonValueRef>
#include <QJsonArray>
#include <QMessageBox>
#include <QThreadPool>

#include <netcdf4/ncDim.h>
#include <netcdf4/ncVar.h>

/***********************************************************************************/
DialogDatasetView::DialogDatasetView(QWidget* parent) :	QDialog(parent), m_ui(new Ui::DatasetView) {
	m_ui->setupUi(this);

	m_highlighter = new HTMLHighlighter(m_ui->plainTextEditHelp->document());

	m_ui->calendarWidgetStart->setVisible(false);
	m_ui->calendarWidgetEnd->setVisible(false);
	m_ui->labelStartDate->setVisible(false);
	m_ui->labelEndDate->setVisible(false);

	m_ui->widgetMonthPicker->setVisible(false);
}

/***********************************************************************************/
DialogDatasetView::~DialogDatasetView() {
	delete m_ui;
}

/***********************************************************************************/
void DialogDatasetView::SetData(const QString& datasetKey, const QJsonObject& object) {
	setWindowTitle(tr("Editing: ") + datasetKey);

	m_ui->lineEditKey->setText(datasetKey);
	m_ui->lineEditName->setText(object["name"].toString());
	m_ui->checkBoxDatasetEnabled->setCheckState( object["enabled"].toBool() ? Qt::Checked : Qt::Unchecked);
	m_ui->lineEditAttribution->setText(object["attribution"].toString());
	if (!object["cache"].isUndefined()) { // For forecasting
		m_ui->spinBoxCache->setEnabled(true);
		m_ui->spinBoxCache->setValue(object["cache"].toInt());
	}
	if (const auto& url{ object["url"].toString() }; !url.isEmpty()) {
		m_ui->lineEditURL->setText(object["url"].toString());
	}
	else {
		m_ui->lineEditURL->setText(QStringLiteral("http://localhost:8080/thredds/dodsC/%1/aggregated.ncml").arg(datasetKey));
	}
	m_ui->lineEditClima->setText(object["climatology"].toString());
	const auto idx{ m_ui->comboBoxQuantum->findText(object["quantum"].toString()) };
	m_ui->comboBoxQuantum->setCurrentIndex(idx);
	m_ui->plainTextEditHelp->document()->setPlainText(object["help"].toString());

	// Populate variables
	const auto& variables = object["variables"].toObject();
	for (const auto& key : variables.keys()) {
		m_ui->tableWidgetVariables->insertRow(m_ui->tableWidgetVariables->rowCount());

		const auto rowIdx = m_ui->tableWidgetVariables->rowCount() - 1;
		// Key
		m_ui->tableWidgetVariables->setItem(rowIdx, 0, new QTableWidgetItem(key));
		// Name
		m_ui->tableWidgetVariables->setItem(rowIdx, 1, new QTableWidgetItem(variables[key]["name"].toString()));
		// Units
		m_ui->tableWidgetVariables->setItem(rowIdx, 2, new QTableWidgetItem(variables[key]["unit"].toString()));
		// Scale
		const auto& scaleArray = variables[key]["scale"].toArray();
		m_ui->tableWidgetVariables->setItem(rowIdx, 3, new QTableWidgetItem(QString::number(scaleArray[0].toDouble())));
		m_ui->tableWidgetVariables->setItem(rowIdx, 4, new QTableWidgetItem(QString::number(scaleArray[1].toDouble())));

		// Scale factor
		const auto& factor = variables[key]["scale_factor"];
		m_ui->tableWidgetVariables->setItem(rowIdx, 5, new QTableWidgetItem(QString::number( !factor.isUndefined() ? factor.toDouble() : 1.0)));

		// Hidden
		// Gonna use a checkbox
		auto* const hidden = new QTableWidgetItem();
		hidden->setCheckState(variables[key]["hide"].toBool() ? Qt::Checked : Qt::Unchecked);
		m_ui->tableWidgetVariables->setItem(rowIdx, 6, hidden);
	}

	m_ui->labelDateRange->setVisible(false);
	m_ui->labelStartDate->setVisible(false);
	m_ui->labelEndDate->setVisible(false);
	m_ui->calendarWidgetStart->setVisible(false);
	m_ui->calendarWidgetEnd->setVisible(false);
	m_ui->labelVarSelection->setVisible(false);
	m_ui->listWidgetVariables->setVisible(false);
}

/***********************************************************************************/
void DialogDatasetView::SetData(const QJsonObject& datasetObj, QNetworkAccessManager& nam) {

	const auto& nameString = datasetObj["value"].toString();
	setWindowTitle(tr("Viewing ") + nameString);

	const auto& datasetIDString = datasetObj["id"].toString();

	m_ui->lineEditKey->setText(datasetIDString);
	m_ui->lineEditName->setText(nameString);
	m_ui->lineEditAttribution->setText(datasetObj["attribution"].toString());
	const auto& idx = m_ui->comboBoxQuantum->findText(datasetObj["quantum"].toString());
	m_ui->comboBoxQuantum->setCurrentIndex(idx);
	m_ui->plainTextEditHelp->document()->setHtml(datasetObj["help"].toString());

	// Get variables from api
	Network::MakeAPIRequest(nam, QStringLiteral("http://navigator.oceansdata.ca/api/variables/?dataset=") + datasetIDString,
							[&](const auto& doc) {
								const auto& root = doc.array();

								for (const auto& variable : root) {
									const auto& value = variable["value"].toString();

									m_ui->listWidgetVariables->addItem(value);
									m_variableMap.insert(value, variable["id"].toString());
								}
							}
	);

	// Figure out date range
	Network::MakeAPIRequest(nam, QStringLiteral("http://navigator.oceansdata.ca/api/timestamps/?dataset=") + datasetIDString,
							[&](const auto& doc) {

								const auto& root = doc.array();
								const auto& quantum{ datasetObj["quantum"].toString() };
								const auto& start{ root.first()["value"].toString() };
								const auto& end{ root.last()["value"].toString() };

								const auto& startDate{ QDate::fromString(start, Qt::DateFormat::ISODate) };
								const auto& endDate{ QDate::fromString(end, Qt::DateFormat::ISODate) };

								// Use correct time picking widget
								if (quantum == QStringLiteral("day") || quantum == QStringLiteral("hour")) {
									m_ui->calendarWidgetStart->setVisible(true);
									m_ui->calendarWidgetStart->setDateRange(startDate, endDate);
									m_ui->calendarWidgetStart->setSelectedDate(startDate);

									m_ui->calendarWidgetEnd->setVisible(true);
									m_ui->calendarWidgetEnd->setDateRange(startDate, endDate);
									m_ui->calendarWidgetEnd->setSelectedDate(endDate);

									m_ui->labelStartDate->setVisible(true);
									m_ui->labelEndDate->setVisible(true);
									return;
								}
								if (quantum == QStringLiteral("month")) {
									m_ui->widgetMonthPicker->setStartEndDate(startDate, endDate);
									m_ui->widgetMonthPicker->setVisible(true);
									return;
								}
							}
	);

	setReadOnlyUI();
}

/***********************************************************************************/
std::pair<QString, QJsonObject> DialogDatasetView::GetData() const {

	QJsonObject obj;
	obj.insert(QStringLiteral("name"), m_ui->lineEditName->text());
	obj.insert(QStringLiteral("enabled"), m_ui->checkBoxDatasetEnabled->isChecked());
	obj.insert(QStringLiteral("url"), m_ui->lineEditURL->text());
	if (m_ui->spinBoxCache->isEnabled()) {
		obj.insert(QStringLiteral("cache"), m_ui->spinBoxCache->value());
	}
	obj.insert(QStringLiteral("quantum"), m_ui->comboBoxQuantum->currentText());
	obj.insert(QStringLiteral("climatology"), m_ui->lineEditClima->text());
	obj.insert(QStringLiteral("attribution"), m_ui->lineEditAttribution->text());
	obj.insert(QStringLiteral("help"), m_ui->plainTextEditHelp->document()->toRawText());

	// Serialize variables
	QJsonObject variables;
	const auto rowCount{ m_ui->tableWidgetVariables->rowCount() };
	for (auto i = 0; i < rowCount; ++i) {
		QJsonObject var;

		const auto& nameText{ m_ui->tableWidgetVariables->item(i, 1)->text() };
		var.insert(QStringLiteral("name"), nameText);

		const auto& unitsText{ m_ui->tableWidgetVariables->item(i, 2)->text() };
		var.insert(QStringLiteral("unit"), unitsText);

		const auto scaleMin{ m_ui->tableWidgetVariables->item(i, 3)->text().toDouble() };
		const auto scaleMax{ m_ui->tableWidgetVariables->item(i, 4)->text().toDouble() };
		var.insert(QStringLiteral("scale"), QJsonArray({scaleMin, scaleMax}));

		const auto factor{ m_ui->tableWidgetVariables->item(i, 5)->text().toDouble() };
		var.insert(QStringLiteral("scale_factor"), factor);

		const auto isHidden{ m_ui->tableWidgetVariables->item(i, 6)->checkState() };
		var.insert(QStringLiteral("hide"), isHidden ? true : false);

		const auto& keyText{ m_ui->tableWidgetVariables->item(i, 0)->text() };
		variables.insert(keyText, var);
	}
	obj.insert(QStringLiteral("variables"), variables);

	return { m_ui->lineEditKey->text(), obj };
}

/***********************************************************************************/
DataDownloadDesc DialogDatasetView::GetDownloadData() const {

	QStringList vars;

	const auto& selectedItems{ m_ui->listWidgetVariables->selectedItems() };
	for (const auto& var : selectedItems) {
		vars << m_variableMap[var->text()];
	}

	QDate startDate, endDate;
	const auto& quantum{ m_ui->comboBoxQuantum->currentText() };
	if (quantum == QStringLiteral("day") || quantum == QStringLiteral("hour")) {
		startDate = m_ui->calendarWidgetStart->selectedDate();
		endDate = m_ui->calendarWidgetEnd->selectedDate();
	}
	else { // Month
		const auto& range{ m_ui->widgetMonthPicker->getStartEndDate() };
		startDate = range.first;
		endDate = range.second;
	}

	return {
		m_ui->lineEditKey->text(),
		m_ui->lineEditName->text(),
		m_ui->comboBoxQuantum->currentText(),
		startDate,
		endDate,
		vars
	};
}

/***********************************************************************************/
void DialogDatasetView::on_pushButtonAddVariable_clicked() {
	addEmptyVariable();
}

/***********************************************************************************/
int DialogDatasetView::addEmptyVariable() {
	m_ui->tableWidgetVariables->insertRow(m_ui->tableWidgetVariables->rowCount());

	const auto rowIdx{ m_ui->tableWidgetVariables->rowCount() - 1 };

	// Key
	m_ui->tableWidgetVariables->setItem(rowIdx, 0, new QTableWidgetItem());
	// Name
	m_ui->tableWidgetVariables->setItem(rowIdx, 1, new QTableWidgetItem());
	// Units
	m_ui->tableWidgetVariables->setItem(rowIdx, 2, new QTableWidgetItem());
	// Scale
	m_ui->tableWidgetVariables->setItem(rowIdx, 3, new QTableWidgetItem());
	m_ui->tableWidgetVariables->setItem(rowIdx, 4, new QTableWidgetItem());

	// Scale factor
	m_ui->tableWidgetVariables->setItem(rowIdx, 5, new QTableWidgetItem(QString::number(1)));

	// Hidden
	// Gonna use a checkbox
	auto* const hidden{ new QTableWidgetItem() };
	hidden->setCheckState(Qt::Unchecked);
	m_ui->tableWidgetVariables->setItem(rowIdx, 6, hidden);

	return rowIdx;
}

/***********************************************************************************/
void DialogDatasetView::checkInputEmpty(const QString& inputLabel, const QString& inputText) {
	if (inputText.isEmpty()) {
		QMessageBox::critical(this, tr("Alert"), inputLabel + tr(" cannot be empty!"), QMessageBox::Ok);
	}
}

/***********************************************************************************/
void DialogDatasetView::setReadOnlyUI() {
	m_ui->lineEditKey->setReadOnly(true);
	m_ui->lineEditName->setReadOnly(true);

	m_ui->labelEnabled->setVisible(false);
	m_ui->checkBoxDatasetEnabled->setEnabled(false);
	m_ui->checkBoxDatasetEnabled->setVisible(false);

	m_ui->lineEditAttribution->setReadOnly(true);

	m_ui->labelURL->setVisible(false);
	m_ui->lineEditURL->setVisible(false);
	m_ui->lineEditURL->setEnabled(false);

	m_ui->labelClimaURL->setVisible(false);
	m_ui->lineEditClima->setVisible(false);
	m_ui->lineEditClima->setEnabled(false);

	m_ui->pushButtonMagicScan->setVisible(false);
	m_ui->pushButtonMagicScan->setEnabled(false);

	m_ui->labelCache->setVisible(false);
	m_ui->spinBoxCache->setVisible(false);

	m_ui->comboBoxQuantum->setEnabled(false);

	m_ui->plainTextEditHelp->setReadOnly(true);

	m_ui->pushButtonAddVariable->setEnabled(false);
	m_ui->pushButtonAddVariable->setVisible(false);
	m_ui->pushButtonDeleteVariable->setEnabled(false);
	m_ui->pushButtonDeleteVariable->setVisible(false);

	m_ui->labelVariables->setVisible(false);
	m_ui->tableWidgetVariables->setVisible(false);
}

/***********************************************************************************/
void DialogDatasetView::on_pushButtonDeleteVariable_clicked() {
	const auto& reply{ QMessageBox::question(this, tr("Confirm Action"), tr("Delete selected variable?"),
									QMessageBox::Yes | QMessageBox::No)
					};

	if (reply == QMessageBox::Yes) {
		m_ui->tableWidgetVariables->removeRow(m_ui->tableWidgetVariables->currentRow());
	}
}

/***********************************************************************************/
void DialogDatasetView::on_lineEditKey_editingFinished() {

	const auto& text{ m_ui->lineEditKey->text() };
	checkInputEmpty(tr("Dataset Key"), text);

	setWindowTitle(tr("Editing: ") + text);
}

/***********************************************************************************/
void DialogDatasetView::on_lineEditName_editingFinished() {
	checkInputEmpty(tr("Dataset Name"), m_ui->lineEditName->text());
}

/***********************************************************************************/
void DialogDatasetView::on_lineEditURL_editingFinished() {
	checkInputEmpty(tr("Dataset URL"), m_ui->lineEditURL->text());
}

/***********************************************************************************/
void DialogDatasetView::keyPressEvent(QKeyEvent* e) {
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
		return;
	}
	QDialog::keyPressEvent(e);
}

/***********************************************************************************/
void DialogDatasetView::on_pushButtonMagicScan_clicked() {
	if (m_ui->lineEditURL->text().isEmpty()) {
		return;
	}

	m_ui->pushButtonMagicScan->setEnabled(false);

	auto* const task{ new Network::URLExistsRunnable{m_ui->lineEditURL->text(), 8080} };

	QObject::connect(task, &Network::URLExistsRunnable::urlResult, this, [&](const auto success) {
		if (success) {

			const netCDF::NcFile ds{m_ui->lineEditURL->text().toStdString(), netCDF::NcFile::read};
			if (ds.isNull()) {
				return;
			}

			if (DialogSelectVars dlg{ds, this}; dlg.exec()) {
				const auto& selectedVars{ dlg.GetSelectedVars() };
				const auto& dsVars{ ds.getVars() };

				for (const auto& var : selectedVars) {
					const auto rowIdx{ addEmptyVariable() };
					const auto& ncVar{ dsVars.find(var.toStdString())->second };

					// Key
					auto* const key{ new QTableWidgetItem() };
					key->setText(var);
					key->setFlags(key->flags() ^ Qt::ItemIsEditable); // Read-only
					m_ui->tableWidgetVariables->setItem(rowIdx, 0, key);

					// Name
					const auto& name_att{ ncVar.getAtt("long_name") };
					if (!name_att.isNull()) {
						std::string long_name;
						name_att.getValues(long_name);
						m_ui->tableWidgetVariables->item(rowIdx, 1)->setText(QString::fromStdString(long_name));
					}
					// Units
					const auto& units_att{ ncVar.getAtt("units") };
					if (!units_att.isNull()) {
						std::string units;
						ncVar.getAtt("units").getValues(units);
						m_ui->tableWidgetVariables->item(rowIdx, 2)->setText(QString::fromStdString(units));
					}
				}

			}
		}
		else {
			QMessageBox::critical(this,
								  tr("Error..."),
								  tr("Magic scan failed :( There could be a network issue (did you start your THREDDS server?), or the URL is wrong.")
								  );
		}

		m_ui->pushButtonMagicScan->setEnabled(true);

	});

	QThreadPool::globalInstance()->start(task);
}
