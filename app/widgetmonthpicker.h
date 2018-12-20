#ifndef WIDGETMONTHPICKER_H
#define WIDGETMONTHPICKER_H

#include "nodiscard.h"

#include <QDate>
#include <QWidget>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetMonthPicker;
}
class QComboBox;

/***********************************************************************************/
class WidgetMonthPicker : public QWidget {
  Q_OBJECT

public:
  explicit WidgetMonthPicker(QWidget *parent = nullptr);
  ~WidgetMonthPicker() override;

  void setStartEndDate(const QDate &start, const QDate &end);

  NODISCARD std::pair<QDate, QDate> getStartEndDate() const;

private slots:
  void on_comboBoxStartMonth_currentIndexChanged(int index);
  void on_comboBoxEndMonth_currentIndexChanged(int index);
  void on_comboBoxStartYear_currentIndexChanged(const QString &arg1);
  void on_comboBoxEndYear_currentIndexChanged(const QString &arg1);

private:
  //
  void populateMonth(const int year, QComboBox *comboBox);
  //
  void validateMonthSelection();

  Ui::WidgetMonthPicker *m_ui{nullptr}; ///< Pointer to UI widgets
  QDate m_startDate, m_endDate;
  bool m_showErrorDialog{false};
};

#endif // WIDGETMONTHPICKER_H
