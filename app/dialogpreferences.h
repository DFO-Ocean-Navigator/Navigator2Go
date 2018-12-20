#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include "constants.h"

#include <QDialog>
#include <QSettings>

/***********************************************************************************/
// Forward Declarations
namespace Ui {
class DialogPreferences;
}
class Preferences;

/***********************************************************************************/
/// Preferences dialog window
class DialogPreferences : public QDialog {
  Q_OBJECT

public:
  DialogPreferences(Preferences &prefs, QWidget *parent = nullptr);
  ~DialogPreferences();

private slots:
  void on_pushButtonBrowseInstallDir_clicked();

  void on_pushButtonBrowseTHREDDS_clicked();

  void on_buttonBox_accepted();

private:
  void populateUI();

  Ui::DialogPreferences *m_ui{nullptr};
  Preferences &m_prefs;
};

#endif // DIALOGPREFERENCES_H
