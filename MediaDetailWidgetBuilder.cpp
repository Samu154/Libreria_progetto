#include "MediaDetailWidgetBuilder.h"
#include "Media.h"
#include "Book.h"
#include <QFormLayout>
#include <QLabel>


MediaDetailWidgetBuilder::MediaDetailWidgetBuilder(QWidget* parent)
: parent(parent) {}


QWidget* MediaDetailWidgetBuilder::build(const Media& media) {
result = nullptr;
media.accept(*this); // doppio dispatch polimorfico
return result; // sarà impostato nella visit del tipo concreto
}


void MediaDetailWidgetBuilder::visit(const Book& b) {
QWidget* w = new QWidget(parent);
auto* layout = new QFormLayout(w);
layout->addRow("Titolo:", new QLabel(b.getTitle()));
layout->addRow("Autore:", new QLabel(b.getAuthor()));
layout->addRow("Anno:", new QLabel(QString::number(b.getYear())));
result = w;
}

void MediaDetailWidgetBuilder::visit(const Film& f) {
    QWidget* w = new QWidget(parent);
auto* layout = new QFormLayout(w);
layout->addRow("Titolo:", new QLabel(f.getTitle()));
layout->addRow("Regista:", new QLabel(f.getDirector()));
layout->addRow("Durata (min):", new QLabel(QString::number(f.getDuration())));
result = w;
}

void MediaDetailWidgetBuilder::visit(const Article& a) {
    QWidget* w = new QWidget(parent);
auto* layout = new QFormLayout(w);
layout->addRow("Titolo:", new QLabel(a.getTitle()));
layout->addRow("Rivista:", new QLabel(a.getJournal()));
layout->addRow("Pagine:", new QLabel(QString::number(a.getPages())));
result = w;
}