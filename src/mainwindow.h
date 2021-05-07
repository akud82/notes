/*********************************************************************************************
* Mozila License
* Just a meantime project to see the ability of qt, the framework that my OS might be based on
* And for those linux users that beleive in the power of notes
*********************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <QGroupBox>
#include <QPushButton>
#include <vector>
#include <QToolButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QSettings>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QProgressDialog>
#include <QAction>

#include <QAutostart>
#include <QTreeWidget>

#include "notedata.h"
#include "notemodel.h"
#include "noteview.h"
#include "updaterwindow.h"
#include "dbmanager.h"
#include "markdownhighlighter.h"
#include "folderview.h"
#include "foldermodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class tst_MainWindow;

public:

    enum class ShadowType{
        Linear = 0,
        Radial
        };

    enum class ShadowSide{
        Left = 0,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
        };

    enum class StretchSide{
        None = 0,
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
        };

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    Q_ENUM(ShadowType)
    Q_ENUM(ShadowSide)
    Q_ENUM(StretchSide)
#endif

    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow() Q_DECL_OVERRIDE;

    void setMainWindowVisibility(bool state);

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent*) Q_DECL_OVERRIDE;
    bool eventFilter(QObject* object, QEvent* event) Q_DECL_OVERRIDE;

private:

    Ui::MainWindow* ui;

    QTimer* m_autoSaveTimer;
    QSettings* m_settingsDatabase;
    QToolButton* m_clearButton;
    QPushButton* m_greenMaximizeButton;
    QPushButton* m_redCloseButton;
    QPushButton* m_yellowMinimizeButton;
    QHBoxLayout m_trafficLightLayout;
    QPushButton* m_newNoteButton;
    QPushButton* m_newFolderButton;
    QPushButton* m_newTagsButton;
    QPushButton* m_trashButton;
    QPushButton* m_dotsButton;
    QPushButton* m_sidebarButton;
    QTextEdit* m_textEdit;
    QLineEdit* m_searchEdit;
    QLabel* m_editorDateLabel;
    QSplitter *m_splitter;
    QSplitter *m_splitterMain;
    QTreeWidget *m_tagsTree;
    QTreeWidget *m_rootTree;
    QSystemTrayIcon* m_trayIcon;
    QAction* m_restoreAction;
    QAction* m_quitAction;
    QMenu* m_trayIconMenu;

    FolderView *m_foldersView;
    FolderModel* m_foldersModel;

    NoteView* m_noteView;
    NoteModel* m_noteModel;

    NoteModel* m_deletedNotesModel;

    QSortFilterProxyModel* m_notesProxyModel;
    QSortFilterProxyModel* m_foldersProxyModel;
    QSortFilterProxyModel* m_tagsProxyModel;

    QModelIndex m_currentSelectedNoteProxy;
    QModelIndex m_selectedNoteBeforeSearchingInSource;
    QQueue<QString> m_searchQueue;
    DBManager* m_dbManager;
    QThread* m_dbThread;
    MarkdownHighlighter *m_highlighter;

    UpdaterWindow m_updater;
    StretchSide m_stretchSide;
    Autostart m_autostart;
    int m_mousePressX;
    int m_mousePressY;
    int m_noteCounter;
    int m_folderCounter;
    int m_tagCounter;
    int m_trashCounter;
    int m_layoutMargin;
    int m_shadowWidth;
    int m_noteListWidth;
    int m_foldersListWidth;
    bool m_canMoveWindow;
    bool m_canStretchWindow;
    bool m_isTemp;
    bool m_isListViewScrollBarHidden;
    bool m_isContentModified;
    bool m_isOperationRunning;
    bool m_dontShowUpdateWindow;
    bool m_alwaysStayOnTop;
    bool m_useNativeWindowFrame;

    void setupTreeView();
    void setupMainWindow();
    void setupFonts();
    void setupTrayIcon();
    void setupKeyboardShortcuts();
    void setupPushButtons();
    void setupSplitters();
    void setupLine();
    void setupRightFrame();
    void setupTitleBarButtons();
    void setupSignalsSlots();
    void autoCheckForUpdates();
    void setupSearchEdit();
    void setupTextEdit();
    void setupDatabases();
    void setupModelView();
    void initializeSettingsDatabase();
    void createNewNoteIfEmpty();
    void setLayoutForScrollArea();
    void setButtonsAndFieldsEnabled(bool doEnable);
    void restoreStates();
    QString getFirstLine(const QString& str);
    QString getNoteDateEditor (QString dateEdited);
    NoteData* generateNote(const int noteID, bool isTemp);
    FolderData* generateFolder(const int folderID);
    TagData* generateTag(const int tagID);
    QDateTime getQDateTime(QString date);
    void showNoteInEditor(const QModelIndex& noteIndex);
    void sortNotesList(QStringList &stringNotesList);
    void saveNoteToDB(const QModelIndex& noteIndex);
    void removeNoteFromDB(const QModelIndex& noteIndex);
    void selectFirstNote();
    void moveNoteToTop();
    void clearSearch();
    void highlightSearch() const;
    void findNotesContain(const QString &keyword);
    void selectNote(const QModelIndex& noteIndex);
    void checkMigration();
    void executeImport(const bool replace);
    void migrateNote(QString notePath);
    void migrateTrash(QString trashPath);

    void dropShadow(QPainter& painter, ShadowType type, ShadowSide side);
    void fillRectWithGradient(QPainter& painter, const QRect& rect, QGradient& gradient);
    double gaussianDist(double x, const double center, double sigma) const;

    void setMargins(QMargins margins);

private slots:
    void initData();
    void onFolderSelect();
    void onRootSelect();
    void loadFolders(QList<FolderData*> folderList, int folderCounter);
    void loadTags(QList<TagData*> tagList, int tagCounter);
    void loadNotes(QList<NoteData*> noteList, int noteCounter);
    void onNewFolderButtonPressed();
    void onNewFolderButtonClicked();
    void onNewTagButtonPressed();
    void onNewTagButtonClicked();
    void onNewNoteButtonPressed();
    void onNewNoteButtonClicked();
    void onTrashButtonPressed();
    void onTrashButtonClicked();
    void onDotsButtonPressed();
    void onDotsButtonClicked();
    void onNotePressed(const QModelIndex &index);
    void onTextEditTextChanged();
    void onSearchEditTextChanged(const QString& keyword);
    void onClearButtonClicked();
    void onGreenMaximizeButtonPressed();
    void onYellowMinimizeButtonPressed();
    void onRedCloseButtonPressed();
    void onGreenMaximizeButtonClicked();
    void onYellowMinimizeButtonClicked();
    void onRedCloseButtonClicked();
    void createNewNote();    
    void deleteNote(const QModelIndex& noteIndex, bool isFromUser=true);    
    void deleteSelectedNote();
    void deleteSelectedFolder();
    void createNewFolder();
    void deleteFolder(const QModelIndex& folderIndex);
    void createNewTag();
    void deleteTag(const QModelIndex& tagIndex);
    void setFocusOnCurrentNote();
    void selectNoteDown();
    void selectNoteUp();
    void setFocusOnText();
    void fullscreenWindow();
    void maximizeWindow();
    void minimizeWindow();
    void QuitApplication();
    void checkForUpdates (const bool clicked);
    void collapseNoteList();
    void expandNoteList();
    void toggleNoteList();
    void importNotesFile(const bool clicked);
    void exportNotesFile(const bool clicked);
    void restoreNotesFile (const bool clicked);
    void stayOnTop(bool checked);
    void askBeforeSettingNativeWindowFrame();
    void setUseNativeWindowFrame(bool useNativeWindowFrame);
    void toggleStayOnTop();
    void onSearchEditReturnPressed();
    void folderSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void rootSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

signals:
    void requestOpenDBManager(QString path, bool doCreate);

    void requestFoldersList();
    void requestCreateUpdateFolder(FolderData* note);
    void requestDeleteFolder(FolderData* note);
    void requestForceLastFolderIndexValue(int index);

    void requestTagsList();
    void requestCreateUpdateTag(TagData* note);
    void requestDeleteTag(TagData* note);
    void requestForceLastTagIndexValue(int index);

    void requestNotesList(FolderModel::FolderType type, int folderId);
    void requestCreateUpdateNote(NoteData* note);
    void requestDeleteNote(NoteData* note);   
    void requestForceLastNoteIndexValue(int index);

    void requestRestoreNotes(QList<NoteData *> noteList);
    void requestImportNotes(QList<NoteData *> noteList);
    void requestExportNotes(QString fileName);
    void requestMigrateNotes(QList<NoteData *> noteList);
    void requestMigrateTrash(QList<NoteData *> noteList);

};

#endif // MAINWINDOW_H
