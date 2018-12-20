#ifndef DIALOGTHREDDSLOGS_H
#define DIALOGTHREDDSLOGS_H

#include <QDialog>
#include <QTimer>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class DialogTHREDDSLogs;
}

/***********************************************************************************/
/// Dialog window to view THREDDS log files.
class DialogTHREDDSLogs : public QDialog {
  Q_OBJECT

public:
  DialogTHREDDSLogs(const QString &threddsLogFolder, QWidget *parent = nullptr);
  ~DialogTHREDDSLogs();

private slots:
  void on_pushButtonOpenLogFolder_clicked();

  void on_comboBoxLogFiles_currentIndexChanged(const QString &arg1);

  void on_pushButtonReloadLogFile_clicked();

private:
  /// Loads a given log file into the UI.
  void loadLogFile(const QString &filename);

  Ui::DialogTHREDDSLogs *m_ui{nullptr};

  QTimer m_reloadLogFileTimer{this}; ///< Timer to reload the current log file.

  const QString m_logFolder; ///< Folder containing THREDDS log files.
};

#endif // DIALOGTHREDDSLOGS_H
