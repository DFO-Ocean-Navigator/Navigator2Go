#include "ui_datasetview.h"
#include "dialogdatasetview.h"

#include "htmlhighlighter.h"
#include "api.h"

#include <QJsonValueRef>
#include <QJsonArray>
#include <QMessageBox>

/***********************************************************************************/
DialogDatasetView::DialogDatasetView(QWidget* parent) :	QDialog(parent), m_ui(new Ui::DatasetView) {
	m_ui->setupUi(this);

	m_highlighter = new HTMLHighlighter(m_ui->plainTextEditHelp->document());
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
	m_ui->lineEditURL->setText(object["url"].toString());
	m_ui->lineEditClima->setText(object["climatology"].toString());
	const auto idx = m_ui->comboBoxQuantum->findText(object["quantum"].toString());
	m_ui->comboBoxQuantum->setCurrentIndex(idx);
	m_ui->plainTextEditHelp->document()->setPlainText(object["help"].toString());

	// Populate variables
	const auto variables = object["variables"].toObject();
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
		const auto scaleArray = variables[key]["scale"].toArray();
		m_ui->tableWidgetVariables->setItem(rowIdx, 3, new QTableWidgetItem(QString::number(scaleArray[0].toDouble())));
		m_ui->tableWidgetVariables->setItem(rowIdx, 4, new QTableWidgetItem(QString::number(scaleArray[1].toDouble())));

		// Scale factor
		const auto factor = variables[key]["scale_factor"];
		m_ui->tableWidgetVariables->setItem(rowIdx, 5, new QTableWidgetItem(QString::number( !factor.isUndefined() ? factor.toDouble() : 1.0)));

		// Hidden
		// Gonna use a checkbox
		auto* hidden = new QTableWidgetItem();
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

	const auto nameString = datasetObj["value"].toString();
	setWindowTitle(tr("Viewing ") + nameString);

	const auto datasetIDString = datasetObj["id"].toString();

	m_ui->lineEditKey->setText(datasetIDString);
	m_ui->lineEditName->setText(nameString);
	m_ui->lineEditAttribution->setText(datasetObj["attribution"].toString());
	const auto idx = m_ui->comboBoxQuantum->findText(datasetObj["quantum"].toString());
	m_ui->comboBoxQuantum->setCurrentIndex(idx);
	m_ui->plainTextEditHelp->document()->setHtml(datasetObj["help"].toString());

	// Get variables from api
	const std::function<void(QJsonDocument)> variableReplyHandler = [&](const auto& doc) {
		const auto root = doc.array();

		for (const auto& variable : root) {
			const auto value = variable["value"].toString();

			m_ui->listWidgetVariables->addItem(value);
			m_variableMap.insert(value, variable["id"].toString());
		}
	};

	API::MakeAPIRequest(nam, "http://navigator.oceansdata.ca/api/variables/?dataset="+datasetIDString, variableReplyHandler);

	// Figure out date range
	const std::function<void(QJsonDocument)> timestampReplyHandler = [&](const auto& doc) {
		const auto root = doc.array();
		const auto quantum = datasetObj["quantum"].toString();
		const auto start = root.first()["value"].toString();
		const auto end = root.last()["value"].toString();

		const auto startDate = QDate::fromString(start, Qt::DateFormat::ISODate);
		const auto endDate =  QDate::fromString(end, Qt::DateFormat::ISODate);

		m_ui->calendarWidgetStart->setDateRange(startDate, endDate);
		m_ui->calendarWidgetStart->setSelectedDate(startDate);

		m_ui->calendarWidgetEnd->setDateRange(startDate, endDate);
		m_ui->calendarWidgetEnd->setSelectedDate(endDate);
	};

	API::MakeAPIRequest(nam, "http://navigator.oceansdata.ca/api/timestamps/?dataset="+datasetIDString, timestampReplyHandler);

	setReadOnlyUI();
}

/***********************************************************************************/
std::pair<QString, QJsonObject> DialogDatasetView::GetData() const {

	QJsonObject obj;
	obj.insert("name", m_ui->lineEditName->text());
	obj.insert("enabled", m_ui->checkBoxDatasetEnabled->isChecked());
	obj.insert("url", m_ui->lineEditURL->text());
	if (m_ui->spinBoxCache->isEnabled()) {
		obj.insert("cache", m_ui->spinBoxCache->value());
	}
	obj.insert("quantum", m_ui->comboBoxQuantum->currentText());
	obj.insert("climatology", m_ui->lineEditClima->text());
	obj.insert("attribution", m_ui->lineEditAttribution->text());
	obj.insert("help", m_ui->plainTextEditHelp->document()->toRawText());

	// Serialize variables
	QJsonObject variables;
	const auto rowCount = m_ui->tableWidgetVariables->rowCount();
	for (auto i = 0; i < rowCount; ++i) {
		QJsonObject var;

		const auto nameText = m_ui->tableWidgetVariables->item(i, 1)->text();
		var.insert("name", nameText);

		const auto unitsText = m_ui->tableWidgetVariables->item(i, 2)->text();
		var.insert("unit", unitsText);

		const auto scaleMin = m_ui->tableWidgetVariables->item(i, 3)->text().toDouble();
		const auto scaleMax = m_ui->tableWidgetVariables->item(i, 4)->text().toDouble();
		var.insert("scale", QJsonArray({scaleMin, scaleMax}));

		const auto factor = m_ui->tableWidgetVariables->item(i, 5)->text().toDouble();
		var.insert("scale_factor", factor);

		const auto isHidden = m_ui->tableWidgetVariables->item(i, 6)->checkState();
		var.insert("hide", isHidden ? "true" : "false");

		const auto keyText = m_ui->tableWidgetVariables->item(i, 0)->text().toLower();
		variables.insert(keyText, var);
	}
	obj.insert("variables", variables);

	return { m_ui->lineEditKey->text(), obj };
}

/***********************************************************************************/
DownloadData DialogDatasetView::GetDownloadData() const {

	QStringList vars;

	for (const auto& var : m_ui->listWidgetVariables->selectedItems()) {
		vars << m_variableMap[var->text()];
	}

	return {m_ui->lineEditKey->text(),
			m_ui->lineEditName->text(),
			m_ui->calendarWidgetStart->selectedDate(),
			m_ui->calendarWidgetEnd->selectedDate(),
			vars};
}

/***********************************************************************************/
void DialogDatasetView::on_tableWidgetVariables_cellDoubleClicked(int row, int column) {

}

/***********************************************************************************/
void DialogDatasetView::on_pushButtonAddVariable_clicked() {
	addEmptyVariable();
}

/***********************************************************************************/
void DialogDatasetView::addEmptyVariable() {
	m_ui->tableWidgetVariables->insertRow(m_ui->tableWidgetVariables->rowCount());

	const auto rowIdx = m_ui->tableWidgetVariables->rowCount() - 1;

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
	m_ui->tableWidgetVariables->setItem(rowIdx, 5, new QTableWidgetItem());

	// Hidden
	// Gonna use a checkbox
	auto* hidden = new QTableWidgetItem();
	hidden->setCheckState(Qt::Unchecked);
	m_ui->tableWidgetVariables->setItem(rowIdx, 6, hidden);

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
	const auto reply = QMessageBox::question(this, tr("Confirm Action"), tr("Delete selected variable?"),
									QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		m_ui->tableWidgetVariables->removeRow(m_ui->tableWidgetVariables->currentRow());
	}
}

/***********************************************************************************/
void DialogDatasetView::on_lineEditKey_editingFinished() {

	const auto text = m_ui->lineEditKey->text();
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
