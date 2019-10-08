#ifndef SWITCH_H
#define SWITCH_H

#include "nodiscard.h"

#include <QAbstractButton>

/***********************************************************************************/
// Forward Declarations
class QPropertyAnimation;

/***********************************************************************************/
/// A toggleable switch widget.
class SwitchWidget : public QAbstractButton {
  Q_OBJECT
  Q_PROPERTY(int offset READ offset WRITE setOffset)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)

public:
  explicit SwitchWidget(QWidget *parent = nullptr);

  QSize sizeHint() const override;

  void setBrush(const QBrush &brsh) { m_brush = brsh; }

  NODISCARD auto brush() const noexcept { return m_brush; }

  NODISCARD auto offset() const noexcept { return m_x; }

  void setOffset(const int o);

signals:
  void toggled(const bool checked);

protected:
  void paintEvent(QPaintEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void enterEvent(QEvent *e) override;

private:
  bool m_switch{true};
  qreal m_opacity{0.0};
  int m_x, m_y, m_width{50}, m_height{16}, m_margin{3};
  QBrush m_thumb{"#d5d5d5"}, m_track, m_brush;
  QPropertyAnimation *m_anim{nullptr};
};

#endif // SWITCH_H
