#ifndef MEDIAVISITOR_H
#define MEDIAVISITOR_H


class Book;
class Film;
class Article;

class MediaVisitor {
public:
    virtual ~MediaVisitor() = default;
    virtual void visit(const Book& b) = 0;
    virtual void visit(const Film& f) = 0;
    virtual void visit(const Article& a) = 0;
};

#endif