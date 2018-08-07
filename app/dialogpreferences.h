#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include "constants.h"
#include "preferences.h"

#include <QDialog>
#include <QSettings>

/***********************************************************************************/
// Forward Declarations
namespace Ui {
class DialogPreferences;
}

/***********************************************************************************/
/// Preferences dialog window
class DialogPreferences : public QDialog {
	Q_OBJECT

public:
	explicit DialogPreferences(QWidget* parent = nullptr);
	~DialogPreferences();

	/// Sets the UI state from a Preferences object.
	void SetPreferences(Preferences& settings);

	/// Serializes UI values into a Preferences object.
	NODISCARD auto GetPreferences() const -> Preferences;

private slots:
	void on_pushButtonBrowseInstallDir_clicked();

	void on_pushButtonBroweseTHREDDS_clicked();

private:
	Ui::DialogPreferences* m_ui{nullptr};


};

#endif // DIALOGPREFERENCES_H
