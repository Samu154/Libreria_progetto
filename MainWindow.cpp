#include "MainWindow.h"
#include "MediaDialog.h"
#include "MediaDetailWidgetBuilder.h"
#include "Media.h"

#include <QListView>
#include <QLineEdit>
#include <QStackedWidget>
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
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    setupConnections();

    // connect selection changed after model set
    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::onListSelectionChanged);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    // Actions
    m_actionOpen = new QAction(tr("Apri..."), this);
    m_actionSave = new QAction(tr("Salva..."), this);
    m_actionExit = new QAction(tr("Esci"), this);
    m_actionAdd = new QAction(tr("Aggiungi"), this);
    m_actionEdit = new QAction(tr("Modifica"), this);
    m_actionDelete = new QAction(tr("Elimina"), this);

    // Menu
    QMenu* menuFile = menuBar()->addMenu(tr("&File"));
    menuFile->addAction(m_actionOpen);
    menuFile->addAction(m_actionSave);
    menuFile->addSeparator();
    menuFile->addAction(m_actionExit);

    // Toolbar
    QToolBar* toolbar = addToolBar(tr("Main"));
    toolbar->addAction(m_actionAdd);
    toolbar->addAction(m_actionEdit);
    toolbar->addAction(m_actionDelete);

    // central widget layout
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // search row
    QHBoxLayout* searchLayout = new QHBoxLayout();
    QLabel* lbl = new QLabel(tr("Cerca:"), this);
    m_searchLineEdit = new QLineEdit(this);
    searchLayout->addWidget(lbl);
    searchLayout->addWidget(m_searchLineEdit);
    mainLayout->addLayout(searchLayout);

    // splitter with list and details
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    QWidget* leftPane = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPane);
    m_listView = new QListView(this);
    leftLayout->addWidget(m_listView);
    splitter->addWidget(leftPane);

    QWidget* rightPane = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPane);
    m_stackedWidget = new QStackedWidget(this);

    // placeholder page with layout where we'll insert detail widget
    QWidget* placeholderPage = new QWidget(this);
    QVBoxLayout* detailLayout = new QVBoxLayout(placeholderPage);
    m_detailPlaceholder = new QWidget(this);
    detailLayout->addWidget(m_detailPlaceholder);
    // keep pointer to layout via object name for later access
    placeholderPage->setLayout(detailLayout);

    m_stackedWidget->addWidget(placeholderPage);
    rightLayout->addWidget(m_stackedWidget);
    splitter->addWidget(rightPane);

    mainLayout->addWidget(splitter);

    statusBar();

    // model setup
    m_model = new MediaListModel(&m_manager, this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setFilterRole(MediaListModel::TitleRole);
    m_listView->setModel(m_proxy);
}

void MainWindow::setupConnections() {
    connect(m_actionOpen, &QAction::triggered, this, &MainWindow::onActionOpen);
    connect(m_actionSave, &QAction::triggered, this, &MainWindow::onActionSave);
    connect(m_actionExit, &QAction::triggered, this, &MainWindow::onActionExit);
    connect(m_actionAdd, &QAction::triggered, this, &MainWindow::onActionAdd);
    connect(m_actionEdit, &QAction::triggered, this, &MainWindow::onActionEdit);
    connect(m_actionDelete, &QAction::triggered, this, &MainWindow::onActionDelete);
    connect(m_searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
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
    QString file = QFileDialog::getSaveFileName(this, tr("Salva biblioteca"), QString(), tr("JSON Files (*.json)"));
    if (file.isEmpty()) return;
    QString err;
    if (!m_manager.saveToFile(file, &err)) {
        QMessageBox::critical(this, tr("Errore salvataggio"), err);
        return;
    }
    m_currentFilePath = file;
    m_dirty = false;
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
    if (!idxProxy.isValid()) return;
    QModelIndex idx = m_proxy->mapToSource(idxProxy);
    int row = idx.row();
    if (QMessageBox::question(this, tr("Conferma"), tr("Eliminare l'elemento selezionato?"), QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
        return;
    if (!m_manager.remove(row)) {
        QMessageBox::warning(this, tr("Errore"), tr("Impossibile eliminare l'elemento"));
        return;
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

void MainWindow::showDetailForIndex(int idx) {
    auto m = m_manager.at(idx);
    if (!m) return;
    MediaDetailWidgetBuilder builder(this);
    QWidget* detail = builder.build(*m);
    if (!detail) return;

    // find layout inside the placeholder and replace content
    QLayout* layout = m_detailPlaceholder->parentWidget()->layout();
    if (!layout) return;
    // remove old widgets
    while (QLayoutItem* it = layout->takeAt(0)) {
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }
    layout->addWidget(detail);
}