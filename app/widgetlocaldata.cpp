#include "widgetlocaldata.h"
#include "ui_widgetlocaldata.h"

#include "ioutils.h"
#include "jsonio.h"
#include "xmlio.h"

#include <QTreeWidgetItem>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>

/***********************************************************************************/
WidgetLocalData::WidgetLocalData(const QString* installDir, QWidget* parent) :	QWidget{parent},
																				m_ui{new Ui::WidgetLocalData},
																				m_ONInstallDir{installDir} {
	m_ui->setupUi(this);

	m_ui->treeWidget->header()->setStretchLastSection(true);

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


		auto* delButton{ new QPushButton() };
		delButton->setText(tr("Delete"));
		delButton->setIcon(QIcon(QStringLiteral(":/icons/icons/delete.png")));
		QObject::connect(delButton, &QPushButton::clicked, this, &WidgetLocalData::buttonHandler);

		m_ui->treeWidget->setItemWidget(parent, 1, delButton);
	}
}

/***********************************************************************************/
void WidgetLocalData::buttonHandler() {
	// Clicking the delete button will also select an entire row
	// in the tree widget, so we need to get the selected row dataset name
	const auto& selectedItems{ m_ui->treeWidget->selectedItems() };
	const auto& dsName{ selectedItems.at(0)->text(0) }; // Column 0 has dataset name

	// Confirm delete action
	QMessageBox box;
	box.setWindowTitle(tr("Confirm action"));
	box.setText(tr("Are your sure you wish to delete ") + dsName + QStringLiteral("?"));
	box.setIcon(QMessageBox::Warning);
	box.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

	if (box.exec() == QMessageBox::StandardButton::No) {
		return;
	}

	// Remove from UI
	delete selectedItems.at(0);

	deleteDataset(dsName);
}

/***********************************************************************************/
void WidgetLocalData::deleteDataset(const QString& datasetName) {

	// Delete from datasetconfig.json
	auto doc{ IO::LoadJSONFile(*m_ONInstallDir, false) };
	auto root{ doc.object() };

	// Find JSON object and erase it
	const auto& ds{ root.find(datasetName) };
	root.erase(ds);

	IO::WriteJSONFile(*m_ONInstallDir + QStringLiteral("/datasetconfigOFFLINE.json"), root);

	// Delete from THREDDS catalogs and the files too
	IO::removeDataset(QStringLiteral("/opt/thredds_content/thredds/"),
					  datasetName,
					  QStringLiteral("/opt/thredds_content/data/") + datasetName
					  );

	emit refreshRequested();
}
