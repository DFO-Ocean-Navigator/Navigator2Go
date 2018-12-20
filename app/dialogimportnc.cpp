#include "dialogimportnc.h"
#include "ui_dialogimportnc.h"

#include "xmlio.h"

#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>

/***********************************************************************************/
DialogImportNC::DialogImportNC(const QString &threddsContentDir,
                               QWidget *parent)
    : QDialog{parent}, m_ui{new Ui::DialogImportNC} {
  m_ui->setupUi(this);

  m_ui->tableWidget->horizontalHeader()->setStretchLastSection(
      true); // Resize columns to widget width
  m_ui->tableWidget->setHorizontalHeaderLabels({tr("File"), tr("Append to")});

  const auto &doc{
      IO::readXML(threddsContentDir + QStringLiteral("/catalog.xml"))};
  m_datasetList = IO::getTHREDDSDatasetList(*doc);
}

/***********************************************************************************/
DialogImportNC::~DialogImportNC() { delete m_ui; }

/***********************************************************************************/
QVector<NetCDFImportDesc> DialogImportNC::GetImportList() const {
  QVector<NetCDFImportDesc> imports;

  for (auto i = 0; i < m_ui->tableWidget->rowCount(); ++i) {
    // Get a load of dis...
    const auto &dataset{
        qobject_cast<QComboBox *>(m_ui->tableWidget->cellWidget(i, 1))
            ->currentText()};

    imports.push_back({m_ui->tableWidget->item(i, 0)->text(), dataset});
  }

  return imports;
}

/***********************************************************************************/
bool DialogImportNC::ShouldRemoveSourceNCFiles() const {
  return m_ui->checkBoxRemoveSourceFiles->isChecked();
}

/***********************************************************************************/
void DialogImportNC::on_pushButtonAdd_clicked() { addDataset(); }

/***********************************************************************************/
void DialogImportNC::on_pushButtonDelete_clicked() {
  if (QMessageBox::question(this, tr("Confirm action..."),
                            tr("Remove selected datasets to import?")) ==
      QMessageBox::Cancel) {
    return;
  }

  // 2 ways to select a row: only one way is possible at a time

  // If the user selects the entire row by clicking on the row index on the
  // left-side get row index and delete it.
  const auto &selectedRows{m_ui->tableWidget->selectionModel()->selectedRows()};
  if (!selectedRows.empty()) {
    for (const auto &row : selectedRows) {
      m_ui->tableWidget->removeRow(row.row());
    }
    return;
  }

  // If the user selects a cell on a particular row, get the row index of that
  // cell and delete the row.
  const auto &selectedItems{m_ui->tableWidget->selectedItems()};
  if (!selectedItems.empty()) {
    for (const auto &item : selectedItems) {
      m_ui->tableWidget->removeRow(item->row());
    }
  }
}

/***********************************************************************************/
void DialogImportNC::on_tableWidget_doubleClicked(const QModelIndex &index) {
  addDataset();
}

/***********************************************************************************/
void DialogImportNC::addDataset() {
  auto file{QFileDialog::getOpenFileName(
      this, tr("Select NetCDF file to import..."), QDir::currentPath(),
      QStringLiteral("NetCDF Files (*.nc)"))};
  if (file.isEmpty()) {
    return;
  }

  // Check for duplicates
  for (auto i = 0; i < m_ui->tableWidget->rowCount(); ++i) {

    if (const auto *item{m_ui->tableWidget->item(i, 0)}; file == item->text()) {

      QMessageBox::information(
          this, tr("Duplicate entry"),
          tr("This file is already listed in the import queue!"));
      return;
    }
  }

  // Add new row
  m_ui->tableWidget->insertRow(m_ui->tableWidget->rowCount());
  const auto rowIdx{m_ui->tableWidget->rowCount() - 1};

  // Set left cell (filename)
  m_ui->tableWidget->setItem(rowIdx, 0, new QTableWidgetItem(file));

  // Set right cell (combobox)
  auto *box{new QComboBox()};
  box->addItems(m_datasetList);
  m_ui->tableWidget->setCellWidget(rowIdx, 1, box);
}
