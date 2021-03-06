#include "LogStatusLabel.h"
#include <QTextDocument>

LogStatusLabel::LogStatusLabel(QStatusBar* parent) : QLabel(parent)
{
    this->setFrameStyle(QFrame::Sunken | QFrame::Panel); //sunken style
    this->setStyleSheet("QLabel { background-color : #C0C0C0; }");

    this->setTextFormat(Qt::PlainText);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(Bridge::getBridge(), SIGNAL(addMsgToLog(QString)), this, SLOT(logUpdate(QString)));
    connect(Bridge::getBridge(), SIGNAL(addMsgToStatusBar(QString)), this, SLOT(logUpdate(QString)));
    QApplication* app = (QApplication*)QApplication::instance();
    connect(app, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
}

void LogStatusLabel::logUpdate(QString message)
{
    if(!message.length())
        return;
    labelText += message.replace("\r\n", "\n");
    QStringList lineList = labelText.split('\n');
    labelText = lineList.last(); //if the last character is a newline this will be an empty string
    QString finalLabel;
    for(int i = 0; i < lineList.length(); i++)
    {
        const QString & line = lineList[lineList.size() - i - 1];
        if(line.length()) //set the last non-empty string from the split
        {
            finalLabel = line;
            break;
        }
    }
    setText(finalLabel);
}

void LogStatusLabel::focusChanged(QWidget* old, QWidget* now)
{
    if(old && now && QString(now->metaObject()->className()) == QString("CPUWidget"))
    {
        old->setFocus();
        return;
    }

    auto findTitle = [](QWidget * w, void* & hwnd) -> QString
    {
        if(!w)
            return "(null)";
        if(!w->windowTitle().length())
        {
            auto p = w->parentWidget();
            if(p && p->windowTitle().length())
            {
                hwnd = (void*)p->winId();
                return p->windowTitle();
            }
        }
        hwnd = (void*)w->winId();
        return w->windowTitle();
    };
    auto className = [](QWidget * w, void* & hwnd) -> QString
    {
        if(!w)
            return "(null)";
        hwnd = (void*)w->winId();
        return w->metaObject()->className();
    };

    ACTIVEVIEW activeView;
    memset(&activeView, 0, sizeof(ACTIVEVIEW));
    strncpy_s(activeView.title, findTitle(now, activeView.titleHwnd).toUtf8().constData(), _TRUNCATE);
    strncpy_s(activeView.className, className(now, activeView.classHwnd).toUtf8().constData(), _TRUNCATE);
    Bridge::getBridge()->activeView = activeView;

    /*QString oldTitle = findTitle(old);
    QString oldClass = className(old);
    QString nowTitle = findTitle(now);
    QString nowClass = className(now);
    printf("[FOCUS] old: %s%s, now: %s%s\n",
           oldTitle.toUtf8().constData(), oldClass.toUtf8().constData(),
           nowTitle.toUtf8().constData(), nowClass.toUtf8().constData());*/
}
