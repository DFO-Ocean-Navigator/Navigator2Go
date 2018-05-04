#include "ui_datasetview.h"

#include "dialogdatasetview.h"
#include "htmlhighlighter.h"

#include <QDebug>
#include <QJsonValueRef>
#include <QJsonArray>

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
	m_ui->lineEditAttribution->setText(object["attribution"].toString());
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
		// FIX THIS
		m_ui->tableWidgetVariables->setItem(rowIdx, 3, new QTableWidgetItem(scaleArray[0].toString()));
		m_ui->tableWidgetVariables->setItem(rowIdx, 4, new QTableWidgetItem(scaleArray[1].toString()));

		// Hidden
		// Gonna use a checkbox
		auto* hidden = new QTableWidgetItem();
		hidden->setCheckState(variables[key]["hide"].toBool() ? Qt::Checked : Qt::Unchecked);
		m_ui->tableWidgetVariables->setItem(rowIdx, 5, hidden);
	}
}

/***********************************************************************************/
std::pair<QString, QJsonObject> DialogDatasetView::GetData() const {
	QJsonObject obj;
	obj.insert("name", m_ui->lineEditName->text());
	obj.insert("url", m_ui->lineEditURL->text());
	obj.insert("quantum", m_ui->comboBoxQuantum->currentText());
	obj.insert("climatology", m_ui->lineEditClima->text());
	obj.insert("attribution", m_ui->lineEditAttribution->text());
	obj.insert("help", m_ui->plainTextEditHelp->document()->toRawText());
	return { m_ui->lineEditKey->text(), obj };
}

/***********************************************************************************/
void DialogDatasetView::on_tableWidgetVariables_cellDoubleClicked(int row, int column) {

}
