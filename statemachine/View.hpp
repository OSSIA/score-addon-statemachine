#pragma once
#include <Process/LayerView.hpp>

namespace statemachine
{
class View final : public Process::LayerView
{
public:
  explicit View(QGraphicsItem* parent);
  ~View() override;

  void pressedAsked(const QPointF& p)
  {
    m_previousPoint = p;
    pressed(p);
  }
  void movedAsked(const QPointF& p);
  void setSelectionArea(const QRectF&) const noexcept { }

private:
  void paint_impl(QPainter*) const override;
  QPointF m_previousPoint{};
};
}
