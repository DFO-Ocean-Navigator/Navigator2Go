#include "widgetlocaldata.h"
#include "ui_widgetlocaldata.h"

#include "ioutils.h"
#include "jsonio.h"

#include <QTreeWidgetItem>
#include <QDebug>

/***********************************************************************************/
WidgetLocalData::WidgetLocalData(const QString* installDir, QWidget* parent) :	QWidget{parent},
																				m_ui{new Ui::WidgetLocalData},
																				m_ONInstallDir{installDir} {
	m_ui->setupUi(this);

	updateTreeWidget();
}

/***********************************************************************************/
WidgetLocalData::~WidgetLocalData() {
	delete m_ui;
}

/***********************************************************************************/
void WidgetLocalData::updateTreeWidget() {
	const auto& offlineConfig{ m_ONInstallDir + QStringLiteral("/oceannavigator/datasetconfigOFFLINE.json") };
	if (!IO::FileExists(offlineConfig)) {
		return;
	}

	const auto& doc = IO::LoadJSONFile(offlineConfig);
	if (doc.isNull()) {
		return;
	}

	const auto& root{ doc.object() };
	m_ui->treeWidget->clear();

	const auto& keys{ root.keys() };
	for (const auto& datasetName : keys) {
		auto* parent{ new QTreeWidgetItem() };
		parent->setText(0, datasetName);

		const auto& variables{ doc[datasetName]["variables"].toObject().keys() };
		for (const auto& variable : variables) {
			auto* child{ new QTreeWidgetItem() };
			child->setText(0, variable);

			parent->addChild(child);
		}


		m_ui->treeWidget->addTopLevelItem(parent);
	}
}
