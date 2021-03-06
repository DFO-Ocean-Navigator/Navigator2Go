#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"

#include "ioutils.h"
#include "preferences.h"

#include <QFileDialog>
#include <QMessageBox>

/***********************************************************************************/
DialogPreferences::DialogPreferences(Preferences &prefs, QWidget *parent)
    : QDialog{parent}, m_ui{new Ui::DialogPreferences}, m_prefs{prefs} {
  m_ui->setupUi(this);

  populateUI();
}

/***********************************************************************************/
DialogPreferences::~DialogPreferences() { delete m_ui; }

/***********************************************************************************/
void DialogPreferences::on_pushButtonBrowseInstallDir_clicked() {
#ifdef QT_DEBUG
  const constexpr auto path = "/home/nabil/";
#else
  const constexpr auto path = "/opt/";
#endif
  const auto &dir{QFileDialog::getExistingDirectory(
      this, tr("Open Ocean Navigator Installation Folder..."), path,
      QFileDialog::ShowDirsOnly)};

  if (!dir.isEmpty()) {
    m_ui->lineEditInstallDir->setText(dir);
  }
}

/***********************************************************************************/
void DialogPreferences::on_pushButtonBrowseTHREDDS_clicked() {
  const auto &dir{QFileDialog::getExistingDirectory(
      this, tr("Open THREDDS catalog.xml folder..."))};

  if (!dir.isEmpty()) {
    if (IO::FileExists(dir + QStringLiteral("/catalog.xml"))) {
      m_ui->lineEditTHREDDSDataLocation->setText(dir);
      return;
    }

    QMessageBox::critical(this, tr("Error..."),
                          tr("catalog.xml not found in this directory: ") +
                              dir);
  }
}

/***********************************************************************************/
void DialogPreferences::on_buttonBox_accepted() {
  if (!m_ui->lineEditInstallDir->text().isEmpty()) {
    m_prefs.ONInstallDir = m_ui->lineEditInstallDir->text();
  }

  if (!m_ui->lineEditRemoteURL->text().isEmpty()) {
    m_prefs.RemoteURL = m_ui->lineEditRemoteURL->text();
  }

  if (!m_ui->lineEditTHREDDSDataLocation->text().isEmpty()) {
    m_prefs.THREDDSCatalogLocation = m_ui->lineEditTHREDDSDataLocation->text();
  }

  m_prefs.DataDownloadFormat = m_ui->comboBoxDownloadFormat->currentText();
  m_prefs.UpdateRemoteListOnStart =
      m_ui->switchUpdateDoryDatasetsOnStart->isChecked();
  m_prefs.CheckForUpdatesOnStart =
      m_ui->switchWidgetCheckForUpdates->isChecked();

  if (m_ui->switchWidgetAdvancedUI->isChecked() != m_prefs.AdvancedUI) {
    QMessageBox::information(this, tr("Settings changed..."),
                             tr("To apply your preference for Advanced UI, "
                                "please restart this program."));
    m_prefs.AdvancedUI = m_ui->switchWidgetAdvancedUI->isChecked();
  }
}

/***********************************************************************************/
void DialogPreferences::populateUI() {
  const QStringList downloadFormats{
      QStringLiteral("NETCDF4"), QStringLiteral("NETCDF4_CLASSIC"),
      QStringLiteral("NETCDF3_64BIT"), QStringLiteral("NETCDF3_CLASSIC"),
      QStringLiteral("NETCDF3_NC")};

  m_ui->comboBoxDownloadFormat->addItems(downloadFormats);

  m_ui->lineEditInstallDir->setText(m_prefs.ONInstallDir);
  m_ui->lineEditRemoteURL->setText(m_prefs.RemoteURL);
  m_ui->lineEditTHREDDSDataLocation->setText(m_prefs.THREDDSCatalogLocation);
  m_ui->comboBoxDownloadFormat->setCurrentText(m_prefs.DataDownloadFormat);
  m_ui->switchUpdateDoryDatasetsOnStart->setChecked(
      m_prefs.UpdateRemoteListOnStart);
  m_ui->switchWidgetCheckForUpdates->setChecked(m_prefs.CheckForUpdatesOnStart);
  m_ui->switchWidgetAdvancedUI->setChecked(m_prefs.AdvancedUI);
}
