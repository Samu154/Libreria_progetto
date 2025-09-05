#ifndef MEDIADIALOG_H
class QLineEdit;
class QPushButton;


class Media;


class MediaDialog : public QDialog {
Q_OBJECT
public:
explicit MediaDialog(QWidget* parent = nullptr, QSharedPointer<Media> mediaPassed = {});
~MediaDialog() override;


QSharedPointer<Media> getResult() const;


private slots:
void onTypeChanged(int index);
void onAccept();
void onReject();


private:
void buildUi();
void populateFromMedia(const QSharedPointer<Media>& media);
bool validateInputs() const;
QSharedPointer<Media> buildMediaFromForm() const;


QComboBox* m_typeCombo = nullptr;
QStackedWidget* m_stack = nullptr;


// Book
QLineEdit* m_bookTitle = nullptr;
QLineEdit* m_bookAuthor = nullptr;
QLineEdit* m_bookYear = nullptr;


// Film
QLineEdit* m_filmTitle = nullptr;
QLineEdit* m_filmDirector = nullptr;
QLineEdit* m_filmDuration = nullptr;


// Article
QLineEdit* m_articleTitle = nullptr;
QLineEdit* m_articleJournal = nullptr;
QLineEdit* m_articlePages = nullptr;


QPushButton* m_ok = nullptr;
QPushButton* m_cancel = nullptr;


QSharedPointer<Media> m_original;
QSharedPointer<Media> m_result;
};


#endif // MEDIADIALOG_H