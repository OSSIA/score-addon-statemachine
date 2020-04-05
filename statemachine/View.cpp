#include "View.hpp"

#include <Process/Style/ScenarioStyle.hpp>

#include <QPainter>
namespace statemachine
{

View::View(QGraphicsItem* parent) : LayerView{parent}
{
}

View::~View()
{
}

void View::paint_impl(QPainter* painter) const
{
  painter->drawText(boundingRect(), "Change me");
}

void View::movedAsked(const QPointF& p)
{
  QRectF r{m_previousPoint.x(), m_previousPoint.y(), 1, 1};
  ensureVisible(mapRectFromScene(r), 30, 30);
  moved(p);

  // we use the last pos, because if not there's a larsen and crash
  m_previousPoint = p;
}
}
