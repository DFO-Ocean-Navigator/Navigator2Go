#include "ui_datasetview.h"
#include "dialogdatasetview.h"

#include "htmlhighlighter.h"

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
}

/***********************************************************************************/
std::pair<QString, QJsonObject> DialogDatasetView::GetData() const {

	QJsonObject obj;
	obj.insert("name", m_ui->lineEditName->text());
	obj.insert("enabled", m_ui->checkBoxDatasetEnabled->checkState() ? true : false);
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

		const auto keyText = m_ui->tableWidgetVariables->item(i, 0)->text();
		variables.insert(keyText, var);
	}
	obj.insert("variables", variables);

	return { m_ui->lineEditKey->text(), obj };
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

	// Hidden
	// Gonna use a checkbox
	auto* hidden = new QTableWidgetItem();
	hidden->setCheckState(Qt::Unchecked);
	m_ui->tableWidgetVariables->setItem(rowIdx, 5, hidden);

}

/***********************************************************************************/
void DialogDatasetView::checkInputEmpty(const QString& inputLabel, const QString& inputText) {
	if (inputText.isEmpty()) {
		QMessageBox::critical(this, tr("Alert"), inputLabel + tr(" cannot be empty!"), QMessageBox::Ok);
	}
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
