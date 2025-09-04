#ifndef MEDIAVISITOR_H
#define MEDIAVISITOR_H

class Book;
class Film;
class Article;

class MediaVisitor {
public:
    virtual ~MediaVisitor() = default;

    // Called when visiting a Book
    virtual void visit(const Book& b) = 0;

    // Called when visiting a Film
    virtual void visit(const Film& f) = 0;

    // Called when visiting an Article
    virtual void visit(const Article& a) = 0;
};

#endif // MEDIAVISITOR_H
