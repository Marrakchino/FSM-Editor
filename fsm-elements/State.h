#pragma once

#include <functional>

#include <QGraphicsRectItem>
#include <QUndoCommand>

class FSMScene;
class Transition;

class State : public QGraphicsRectItem
{
  using super = QGraphicsRectItem;
public:
  State(QString title, const QPointF& position, std::function<void(QUndoCommand*)>&& pushStack);

  virtual ~State();

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

  void shrinkTextToFit(QPainter * painter);

  static bool fitInRect(const QRectF& rect, const QRect& bounding);

  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

  void setSilentMove(bool silent);

  QString title() const;

  virtual void keyPressEvent(QKeyEvent *event) override;
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

  FSMScene* scene() const;

private:
  static const qreal WIDTH;
  static const qreal HEIGHT;
  static const qreal H_MARGIN;
  static const qreal V_MARGIN;
  static const QColor PEN_COLOR;

private:
  QString title_;
  QString content_;
  std::function<void(QUndoCommand*)> pushStack_;
  bool silent_;
  QList<Transition*> transitions_;
};
