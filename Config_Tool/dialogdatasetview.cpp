#include "dialogdatasetview.h"
#include "ui_datasetview.h"

/***********************************************************************************/
DialogDatasetView::DialogDatasetView(QWidget* parent) :	QDialog(parent), m_ui(new Ui::DatasetView) {
	m_ui->setupUi(this);
}

/***********************************************************************************/
DialogDatasetView::~DialogDatasetView() {
	delete m_ui;
}

/***********************************************************************************/
void DialogDatasetView::SetData(const QString& datasetKey, const QJsonValueRef object) {
	setWindowTitle(tr("Editing: ") + datasetKey);
}
