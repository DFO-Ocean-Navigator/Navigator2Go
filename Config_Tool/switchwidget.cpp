#include "switchwidget.h"

#include <QObject>
#include <QPropertyAnimation>
#include <QPainter>
#include <QMouseEvent>

/***********************************************************************************/
SwitchWidget::SwitchWidget(QWidget* parent) : QAbstractButton{parent}, m_anim{new QPropertyAnimation{this, "offset", this}} {
	const auto offset = m_height / 2;
	setOffset(offset);
	m_y = offset;
	setBrush(QColor("009688"));
	setMaximumWidth(m_width);
	setCheckable(true);
}

/***********************************************************************************/
QSize SwitchWidget::sizeHint() const {
	return {2 * (m_height + m_margin), m_height + 2 * m_margin};
}

/***********************************************************************************/
void SwitchWidget::setOffset(const int o) {
	m_x = o;
	update();
}

/***********************************************************************************/
void SwitchWidget::paintEvent(QPaintEvent* e) {
	QPainter p{this};
	p.fillRect(rect(), QColor(90, 102, 117));
	QRect switchRect;
	QColor switchColor;
	QString text;
	const auto halfWidth = width() / 2;

	if (isChecked()) {
		switchRect = QRect(halfWidth, 0, halfWidth, height());
		switchColor = QColor(61, 174, 233);
		text = tr("On");
	} else {
		switchRect = QRect(0, 0, halfWidth, height());
		switchColor = QColor(Qt::darkGray);
		text = tr("Off");
	}
	p.fillRect(switchRect, switchColor);
	p.drawText(switchRect, Qt::AlignCenter, text);
}

/***********************************************************************************/
void SwitchWidget::mouseReleaseEvent(QMouseEvent* e) {

	QAbstractButton::mouseReleaseEvent(e);
}

/***********************************************************************************/
void SwitchWidget::enterEvent(QEvent* e) {
	setCursor(Qt::PointingHandCursor);
	QAbstractButton::enterEvent(e);
}
