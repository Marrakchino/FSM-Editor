#include <fsm-editor/FSMEditor.h>
#include <fsm-editor/ExportVisitor.h>

#include <QGridLayout>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

FSMEditor::FSMEditor(ExportVisitor& visitor)
  : fsmView_(&scene_, this)
  , visitor_(visitor)
{
  makeLuaEditor();
  addWidget(makeViewPanel());
  addWidget(editor_);

  connect(&scene_, SIGNAL(command(QUndoCommand*)), SLOT(stackCommand(QUndoCommand*)));
}

void FSMEditor::zoomIn()
{
  fsmView_.scale(1.2, 1.2);
}

void FSMEditor::zoomOut()
{
  fsmView_.scale(0.8, 0.8);
}

void FSMEditor::stackCommand(QUndoCommand* command)
{
  undoStack_.push(command);
}

void FSMEditor::makeLuaEditor()
{
  editor_ = new QPlainTextEdit(this);
  editor_->setPlainText("function sample code");
  connect(&scene_, SIGNAL(codeChanged(const QString&)), SLOT(displaySetCode(const QString&)));
  connect(editor_, SIGNAL(textChanged()), SLOT(transferCodeChanged()));
}

void FSMEditor::displaySetCode(const QString& code)
{
  if (editor_->toPlainText() != code)
  {
    editor_->blockSignals(true);
    editor_->setPlainText(code);
    editor_->blockSignals(false);
  }
}

void FSMEditor::transferCodeChanged()
{
  scene_.updateCode(editor_->toPlainText());
}

QWidget* FSMEditor::makeViewPanel()
{
  QWidget* viewPanel = new QWidget(this);
  QLayout* vLayout = new QVBoxLayout;
  QToolBar* toolbar = new QToolBar(this);
  createSceneActions(toolbar);
  vLayout->addWidget(toolbar);
  vLayout->addWidget(&fsmView_);
  viewPanel->setLayout(vLayout);
  return viewPanel;
}

void FSMEditor::createSceneActions(QToolBar* toolbar)
{
  QAction* zoomIn = toolbar->addAction("+");
  QAction* zoomOut = toolbar->addAction("-");
  toolbar->addSeparator();
  QAction* undo = undoStack_.createUndoAction(toolbar);
  undo->setShortcut(QKeySequence::Undo);
  toolbar->addAction(undo);
  QAction* redo = undoStack_.createRedoAction(toolbar);
  redo->setShortcut(QKeySequence::Redo);
  toolbar->addAction(redo);
  toolbar->addSeparator();
  QAction* exportAction = toolbar->addAction("Export");
  exportAction->setShortcut(QKeySequence::Save);
  connect(zoomIn, SIGNAL(triggered()), SLOT(zoomIn()));
  connect(zoomOut, SIGNAL(triggered()), SLOT(zoomOut()));
  connect(exportAction, SIGNAL(triggered()), SLOT(save()));
}

void FSMEditor::save()
{
  QFile file = QFileDialog::getSaveFileName();
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  QTextStream out(&file);
  out << scene_.generateExport(visitor_);
}
