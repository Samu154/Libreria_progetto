#ifndef MEDIADETAILWIDGETBUILDER_H
#define MEDIADETAILWIDGETBUILDER_H


#include <QWidget>
#include <memory>
#include "MediaVisitor.h"


class Media; // forward


class MediaDetailWidgetBuilder : public MediaVisitor {
public:
explicit MediaDetailWidgetBuilder(QWidget* parent = nullptr);


// Entry point: costruisce il widget per un qualsiasi Media
QWidget* build(const Media& media);


// Implementazioni visitor per i tipi concreti
void visit(const Book& b) override;
void visit(const Film& f) override;
void visit(const Article& a) override;


private:
QWidget* parent = nullptr;
QWidget* result = nullptr; // impostato nelle visit(...)
};


#endif // MEDIADETAILWIDGETBUILDER_H