#ifndef MEDIADETAILWIDGETBUILDER_H
#define MEDIADETAILWIDGETBUILDER_H

#include <QWidget>
#include <memory>
#include "MediaVisitor.h"


class Media;

class MediaDetailWidgetBuilder : public MediaVisitor {
public:
explicit MediaDetailWidgetBuilder(QWidget* parent = nullptr);
QWidget* build(const Media& media);
void visit(const Book& b) override;
void visit(const Film& f) override;
void visit(const Article& a) override;

private:
QWidget* parent = nullptr;
QWidget* result = nullptr;
};

#endif