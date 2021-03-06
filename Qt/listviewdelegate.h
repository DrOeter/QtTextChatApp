#ifndef LISTVIEWDELEGATE_H_
#define LISTVIEWDELEGATE_H_

#include <QAbstractItemDelegate>
#include <QPainter>

class ListViewDelegate : public QAbstractItemDelegate{
  int d_radius;
  int d_toppadding;
  int d_bottompadding;
  int d_leftpadding;
  int d_rightpadding;
  int d_verticalmargin;
  int d_horizontalmargin;
  int d_pointerwidth;
  int d_pointerheight;

 public:
  inline ListViewDelegate(QObject *parent = nullptr);
 protected:
  inline void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const;
  inline QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const;
};

inline ListViewDelegate::ListViewDelegate(QObject *parent)
  :
  QAbstractItemDelegate(parent),
  d_radius(5),
  d_toppadding(5),
  d_bottompadding(3),
  d_leftpadding(5),
  d_rightpadding(5),
  d_verticalmargin(15),
  d_horizontalmargin(10),
  d_pointerwidth(10),
  d_pointerheight(17)
  //d_widthfraction(0.7)
{}

inline void ListViewDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const{
  double d_widthfraction = 0.7;
  QTextDocument bodydoc;
  QTextOption textOption(bodydoc.defaultTextOption());
  textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  bodydoc.setDefaultTextOption(textOption);
  bodydoc.setDefaultFont(QFont("Roboto", 11));
  QString bodytext(index.data(Qt::DisplayRole).toString());
  bodydoc.setPlainText(bodytext);

  QFont myFont("Roboto", 11);

  QFontMetrics fm(myFont);
  int width=fm.horizontalAdvance(bodytext);

  if(width < 434) d_widthfraction = ((double)width / 434) * 0.7;
  else d_widthfraction = 0.7;

  if(d_widthfraction < 0.182258) d_widthfraction = 0.182258;

  //qInfo() << d_widthfraction << "\n";

  qreal contentswidth = option.rect.width() * d_widthfraction - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
  bodydoc.setTextWidth(contentswidth);
  qreal bodyheight = bodydoc.size().height();

  bool outgoing = index.data(Qt::UserRole + 1).toString() == "Outgoing";

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);

  // uncomment to see the area provided to paint this item
  //painter->drawRect(option.rect);

  painter->translate(option.rect.left() + d_horizontalmargin, option.rect.top() + ((index.row() == 0) ? d_verticalmargin : 0));

  // background color for chat bubble
  QColor bgcolor("#DD1212");
  if (outgoing)
    bgcolor = "#DDDDDD";

  // create chat bubble
  QPainterPath pointie;

  // left bottom
  pointie.moveTo(0, bodyheight + d_toppadding + d_bottompadding);

  // right bottom
  pointie.lineTo(0 + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - d_radius,
                 bodyheight + d_toppadding + d_bottompadding);
  pointie.arcTo(0 + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - 2 * d_radius,
                bodyheight + d_toppadding + d_bottompadding - 2 * d_radius,
                2 * d_radius, 2 * d_radius, 270, 90);

  // right top
  pointie.lineTo(0 + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding, 0 + d_radius);
  pointie.arcTo(0 + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - 2 * d_radius, 0,
                2 * d_radius, 2 * d_radius, 0, 90);

  // left top
  pointie.lineTo(0 + d_pointerwidth + d_radius, 0);
  pointie.arcTo(0 + d_pointerwidth, 0, 2 * d_radius, 2 * d_radius, 90, 90);

  // left bottom almost (here is the pointie)
  pointie.lineTo(0 + d_pointerwidth, bodyheight + d_toppadding + d_bottompadding - d_pointerheight);
  pointie.closeSubpath();

  // rotate bubble for outgoing messages
  if (outgoing)
  {
    painter->translate(option.rect.width() - pointie.boundingRect().width() - d_horizontalmargin - d_pointerwidth, 0);
    painter->translate(pointie.boundingRect().center());
    painter->rotate(180);
    painter->translate(-pointie.boundingRect().center());
  }

  // now paint it!
  painter->setPen(QPen(bgcolor));
  painter->drawPath(pointie);
  painter->fillPath(pointie, QBrush(bgcolor));

  // rotate back or painter is going to paint the text rotated...
  if (outgoing)
  {
    painter->translate(pointie.boundingRect().center());
    painter->rotate(-180);
    painter->translate(-pointie.boundingRect().center());
  }

  // set text color used to draw message body
  QAbstractTextDocumentLayout::PaintContext ctx;
  if (outgoing)
    ctx.palette.setColor(QPalette::Text, QColor("black"));
  else
    ctx.palette.setColor(QPalette::Text, QColor("white"));

  // draw body text
  painter->translate((outgoing ? 0 : d_pointerwidth) + d_leftpadding, 0);
  bodydoc.documentLayout()->draw(painter, ctx);

  painter->restore();
}

inline QSize ListViewDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const{
  double d_widthfraction = 0.7;
  QTextDocument bodydoc;
  QTextOption textOption(bodydoc.defaultTextOption());
  textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  bodydoc.setDefaultTextOption(textOption);
  bodydoc.setDefaultFont(QFont("Roboto", 11));
  QString bodytext(index.data(Qt::DisplayRole).toString());
  bodydoc.setPlainText(bodytext);

  QFont myFont("Roboto", 11);


  QFontMetrics fm(myFont);
  int width=fm.horizontalAdvance(bodytext);

  if(width < 434) d_widthfraction = ((double)width / 434) * 0.7;
  else d_widthfraction = 0.7;

  if(d_widthfraction < 0.182258) d_widthfraction = 0.182258;

  //qInfo() << d_widthfraction << "\n";

  // the width of the contents are the (a fraction of the window width) minus (margins + padding + width of the bubble's tail)
  qreal contentswidth = option.rect.width() * d_widthfraction - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;

  // set this available width on the text document
  bodydoc.setTextWidth(contentswidth);

  QSize size(bodydoc.idealWidth() + d_horizontalmargin + d_pointerwidth + d_leftpadding + d_rightpadding,
             bodydoc.size().height() + d_bottompadding + d_toppadding + d_verticalmargin + 1); // I dont remember why +1, haha, might not be necessary

  if (index.row() == 0) // have extra margin at top of first item
    size += QSize(0, d_verticalmargin);

  return size;
}

#endif
