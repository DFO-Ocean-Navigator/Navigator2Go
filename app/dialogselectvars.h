#ifndef DIALOGSELECTVARS_H
#define DIALOGSELECTVARS_H

#include "nodiscard.h"

#include <QDialog>

#include <netcdf4/ncFile.h>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class DialogSelectVars;
}

/***********************************************************************************/
/// Dialog window to select variables to add to a dataset config file.
/** Opened by Magic Scan button. */
class DialogSelectVars : public QDialog {
  Q_OBJECT

public:
  DialogSelectVars(const netCDF::NcFile &ds, QWidget *parent = nullptr);
  ~DialogSelectVars();

  /// Returns a list of variable keys to be added to dataset config file.
  NODISCARD QStringList GetSelectedVars() const;

private slots:
  void on_tableWidget_cellClicked(int row, int column);

private:
  Ui::DialogSelectVars *m_ui{nullptr};
};

#endif // DIALOGSELECTVARS_H
