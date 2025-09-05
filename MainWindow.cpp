#include "MainWindow.h"

#include "MediaDialog.h"
#include "MediaDetailWidgetBuilder.h"
#include "Media.h"

#include <QListView>
#include <QLineEdit>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QScrollArea>
#include <QPushButton>
#include <QButtonGroup>
#include <QRegularExpression>
#include <QDebug>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    setupConnections();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    // Actions
    m_actionOpen = new QAction(tr("Apri..."), this);
    m_actionSave = new QAction(tr("Salva"), this);
    m_actionSaveAs = new QAction(tr("Salva come..."), this);
    m_actionExit = new QAction(tr("Esci"), this);
    m_actionAdd = new QAction(tr("Aggiungi"), this);
    m_actionEdit = new QAction(tr("Modifica"), this);
    m_actionDelete = new QAction(tr("Elimina"), this);

    // Menu
    QMenu* menuFile = menuBar()->addMenu(tr("&File"));
    menuFile->addAction(m_actionOpen);
    menuFile->addAction(m_actionSave);
    menuFile->addAction(m_actionSaveAs);
    menuFile->addSeparator();
    menuFile->addAction(m_actionExit);

    // Toolbar
    QToolBar* toolbar = addToolBar(tr("Main"));
    toolbar->addAction(m_actionOpen);
    toolbar->addAction(m_actionSave);

    // central widget and main layout
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    QString sample = QCoreApplication::applicationDirPath() + "/sample_library.json";
    if (QFile::exists(sample)) {
        QString err;
        if (!m_manager.loadFromFile(sample, &err)) {
            qWarning() << "Errore caricamento sample:" << err;
        } else {
            m_currentFilePath.clear(); // non segna come salvato
            m_dirty = false;
        }
    }

    // Top row: search + filters
    QHBoxLayout* topRow = new QHBoxLayout();
    QLabel* lblSearch = new QLabel(tr("Cerca:"), this);
    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText(tr("Cerca per titolo, autore, regista..."));

    // Filter buttons
    QWidget* filterWidget = new QWidget(this);
    QHBoxLayout* filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0,0,0,0);
    QPushButton* btnAll = new QPushButton(tr("All"), this);
    QPushButton* btnBooks = new QPushButton(tr("Book"), this);
    QPushButton* btnFilms = new QPushButton(tr("Film"), this);
    QPushButton* btnArticles = new QPushButton(tr("Article"), this); // sostituisce Serie TV
    btnAll->setCheckable(true); btnBooks->setCheckable(true); btnFilms->setCheckable(true); btnArticles->setCheckable(true);
    btnAll->setChecked(true);

    // Group so only one selected at a time
    QButtonGroup* filterGroup = new QButtonGroup(this);
    filterGroup->setExclusive(true);
    filterGroup->addButton(btnAll);
    filterGroup->addButton(btnBooks);
    filterGroup->addButton(btnFilms);
    filterGroup->addButton(btnArticles);

    filterLayout->addWidget(btnAll);
    filterLayout->addWidget(btnBooks);
    filterLayout->addWidget(btnFilms);
    filterLayout->addWidget(btnArticles);

    topRow->addWidget(lblSearch);
    topRow->addWidget(m_searchLineEdit, 1);
    topRow->addWidget(filterWidget, 0);
    mainLayout->addLayout(topRow);

    // Main splitter: left list (40%) - right detail (60%)
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    // Left pane
    QWidget* leftPane = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPane);
    m_listView = new QListView(this);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setUniformItemSizes(true);
    leftLayout->addWidget(m_listView);
    splitter->addWidget(leftPane);

    // Right pane
    QWidget* rightPane = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPane);

    // Detail title
    m_detailTitle = new QLabel(tr("Seleziona un elemento"), this);
    QFont titleFont = m_detailTitle->font();
    titleFont.setPointSize(titleFont.pointSize() + 4);
    titleFont.setBold(true);
    m_detailTitle->setFont(titleFont);

    // Detail scroll area
    m_detailScroll = new QScrollArea(this);
    m_detailScroll->setWidgetResizable(true);
    m_detailContainer = new QWidget(this);
    QVBoxLayout* detailLayout = new QVBoxLayout(m_detailContainer);
    QLabel* placeholder = new QLabel(tr("Dettagli appariranno qui"), this);
    placeholder->setWordWrap(true);
    detailLayout->addWidget(placeholder);
    detailLayout->addStretch(1);
    m_detailScroll->setWidget(m_detailContainer);

    rightLayout->addWidget(m_detailTitle);
    rightLayout->addWidget(m_detailScroll, 1);
    splitter->addWidget(rightPane);

    splitter->setStretchFactor(0, 4);
    splitter->setStretchFactor(1, 6);

    mainLayout->addWidget(splitter, 1);

    // Bottom actions
    QHBoxLayout* bottomRow = new QHBoxLayout();
    bottomRow->addStretch(1);
    QPushButton* addBtn = new QPushButton(tr("Aggiungi"), this);
    QPushButton* editBtn = new QPushButton(tr("Modifica"), this);
    QPushButton* delBtn = new QPushButton(tr("Elimina"), this);
    QPushButton* saveBtn = new QPushButton(tr("Salva"), this);

    bottomRow->addWidget(addBtn);
    bottomRow->addWidget(editBtn);
    bottomRow->addWidget(delBtn);
    bottomRow->addWidget(saveBtn);
    mainLayout->addLayout(bottomRow);

    // Model setup
    m_model = new MediaListModel(&m_manager, this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setFilterRole(MediaListModel::TitleRole);
    m_listView->setModel(m_proxy);

    // Connections
    connect(m_searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    connect(filterGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
    this, [this, btnAll, btnBooks, btnFilms, btnArticles](QAbstractButton* b){
        // Se "All" selezionato: cerchiamo per testo (titolo)
        if (b == btnAll) {
            m_proxy->setFilterRole(MediaListModel::TitleRole);
            m_proxy->setFilterFixedString(m_searchLineEdit->text());
        } else {
            // Quando un tipo è selezionato filtriamo per TypeRole
            m_proxy->setFilterRole(MediaListModel::TypeRole);
            if (b == btnBooks)      m_proxy->setFilterFixedString("Book");
            else if (b == btnFilms) m_proxy->setFilterFixedString("Film");
            else if (b == btnArticles) m_proxy->setFilterFixedString("Article");
        }
    });


    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onActionAdd);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onActionEdit);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::onActionDelete);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onActionSave);

    // selection change -> enable/disable buttons
    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::onSelectionChanged);
    m_actionEdit->setEnabled(false);
    m_actionDelete->setEnabled(false);

    // update detail when selection changes
    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::onListSelectionChanged);

    // Save pointer to some widgets we may want to update later
}

void MainWindow::setupConnections() {
    connect(m_actionOpen, &QAction::triggered, this, &MainWindow::onActionOpen);
    connect(m_actionSave, &QAction::triggered, this, &MainWindow::onActionSave);
    connect(m_actionSaveAs, &QAction::triggered, this, &MainWindow::onActionSaveAs);
    connect(m_actionExit, &QAction::triggered, this, &MainWindow::onActionExit);
    connect(m_actionAdd, &QAction::triggered, this, &MainWindow::onActionAdd);
    connect(m_actionEdit, &QAction::triggered, this, &MainWindow::onActionEdit);
    connect(m_actionDelete, &QAction::triggered, this, &MainWindow::onActionDelete);
}

void MainWindow::onActionOpen() {
    QString file = QFileDialog::getOpenFileName(this, tr("Apri biblioteca"), QString(), tr("JSON Files (*.json)"));
    if (file.isEmpty()) return;
    QString err;
    if (!m_manager.loadFromFile(file, &err)) {
        QMessageBox::critical(this, tr("Errore caricamento"), err);
        return;
    }
    m_currentFilePath = file;
    m_dirty = false;
}

void MainWindow::onActionSave() {
    if (m_currentFilePath.isEmpty()) {
        onActionSaveAs();
        return;
    }
    QString err;
    if (!m_manager.saveToFile(m_currentFilePath, &err)) {
        QMessageBox::critical(this, tr("Errore salvataggio"), err);
        return;
    }
    m_dirty = false;
    statusBar()->showMessage(tr("Salvato in %1").arg(m_currentFilePath), 3000);
}

void MainWindow::onActionSaveAs() {
    QString file = QFileDialog::getSaveFileName(this, tr("Salva biblioteca"), QString(), tr("JSON Files (*.json)"));
    if (file.isEmpty()) return;
    QString err;
    if (!m_manager.saveToFile(file, &err)) {
        QMessageBox::critical(this, tr("Errore salvataggio"), err);
        return;
    }
    m_currentFilePath = file;
    m_dirty = false;
    statusBar()->showMessage(tr("Salvato in %1").arg(m_currentFilePath), 3000);
}

void MainWindow::onActionExit() {
    if (m_dirty) {
        auto r = QMessageBox::question(this, tr("Modifiche non salvate"), tr("Salvare le modifiche prima di uscire?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (r == QMessageBox::Yes) onActionSave();
        else if (r == QMessageBox::Cancel) return;
    }
    close();
}

void MainWindow::onActionAdd() {
    MediaDialog dlg(this, {});
    if (dlg.exec() != QDialog::Accepted) return;
    QSharedPointer<Media> m = dlg.getResult();
    if (!m) return;
    m_manager.add(m);
    m_dirty = true;
}

void MainWindow::onActionEdit() {
    QModelIndex idxProxy = m_listView->currentIndex();
    if (!idxProxy.isValid()) return;
    QModelIndex idx = m_proxy->mapToSource(idxProxy);
    if (!idx.isValid()) return;
    int row = idx.row();
    auto orig = m_manager.at(row);
    if (!orig) return;
    QSharedPointer<Media> working(orig->clone());
    MediaDialog dlg(this, working);
    if (dlg.exec() != QDialog::Accepted) return;
    QSharedPointer<Media> edited = dlg.getResult();
    if (!edited) return;
    m_manager.update(row, edited);
    m_dirty = true;
}

void MainWindow::onActionDelete() {
    QModelIndex idxProxy = m_listView->currentIndex();
    if (!idxProxy.isValid()) {
        QMessageBox::information(this, tr("Nessuna selezione"), tr("Nessun elemento selezionato da eliminare."));
        return;
    }
    QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(m_listView->model());
    if (!proxy) { qWarning() << "onActionDelete: model is not a proxy"; return; }
    QModelIndex srcIdx = proxy->mapToSource(idxProxy);
    if (!srcIdx.isValid()) { QMessageBox::warning(this, tr("Errore"), tr("Impossibile determinare l'elemento da eliminare.")); return; }
    int row = srcIdx.row();
    if (row < 0 || row >= m_manager.count()) { QMessageBox::warning(this, tr("Errore"), tr("Indice non valido.")); return; }

    if (QMessageBox::question(this, tr("Conferma"), tr("Eliminare l'elemento selezionato?"),
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    if (!m_manager.remove(row)) { QMessageBox::warning(this, tr("Errore"), tr("Impossibile eliminare l'elemento.")); return; }

    // aggiorna vista: scegli next selection se esiste, altrimenti pulisci dettaglio
    int newCount = m_manager.count();
    if (newCount == 0) {
        m_listView->clearSelection();
        showDetailForIndex(-1); // pulisce dettaglio
    } else {
        int nextRow = qMin(row, newCount - 1);
        QModelIndex nextIdx = m_model->index(nextRow, 0);
        QModelIndex proxyIdx = m_proxy->mapFromSource(nextIdx);
        m_listView->setCurrentIndex(proxyIdx);
        showDetailForIndex(nextRow);
    }

    m_dirty = true;
}


void MainWindow::onSearchTextChanged(const QString& txt) {
    if (!m_proxy) return;
    m_proxy->setFilterFixedString(txt);
}

void MainWindow::onListSelectionChanged(const QModelIndex& current, const QModelIndex& /*previous*/) {
    if (!current.isValid()) return;
    QModelIndex src = m_proxy->mapToSource(current);
    int row = src.row();
    showDetailForIndex(row);
}

void MainWindow::onSelectionChanged(const QModelIndex& current, const QModelIndex& /*previous*/) {
    bool hasSelection = false;
    if (current.isValid()) {
        QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(m_listView->model());
        if (proxy) {
            QModelIndex src = proxy->mapToSource(current);
            hasSelection = src.isValid();
        }
    }
    m_actionEdit->setEnabled(hasSelection);
    m_actionDelete->setEnabled(hasSelection);
}

void MainWindow::showDetailForIndex(int idx) {
    if (idx < 0 || idx >= m_manager.count()) {
        qDebug() << "showDetailForIndex: index out of range" << idx;
        m_detailTitle->setText(tr("Seleziona un elemento"));
        // clear detail container
        QLayout* layout = m_detailContainer->layout();
        if (!layout) return;
        while (QLayoutItem* it = layout->takeAt(0)) {
            if (QWidget* w = it->widget()) { w->setParent(nullptr); w->deleteLater(); }
            delete it;
        }
        QLabel* placeholder = new QLabel(tr("Dettagli appariranno qui"), m_detailContainer);
        placeholder->setWordWrap(true);
        layout->addWidget(placeholder);
        return;
    }

    auto m = m_manager.at(idx);
    if (!m) return;

    MediaDetailWidgetBuilder builder(this);
    QWidget* detail = builder.build(*m);
    if (!detail) return;

    // update title
    m_detailTitle->setText(m->getTitle());

    QLayout* layout = m_detailContainer->layout();
    if (!layout) {
        layout = new QVBoxLayout(m_detailContainer);
        m_detailContainer->setLayout(layout);
    }

    while (QLayoutItem* it = layout->takeAt(0)) {
        if (QWidget* w = it->widget()) { w->setParent(nullptr); w->deleteLater(); }
        delete it;
    }

    detail->setParent(m_detailContainer);
    layout->addWidget(detail);
    detail->show();
}
