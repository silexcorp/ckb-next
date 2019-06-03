#ifndef WINDOWINFO_H
#define WINDOWINFO_H
#include <QString>
#include <QSettings>
#include "ckbsettings.h"

class KbMode;
class KbWindowInfo : public QObject
{
    Q_OBJECT
public:
    KbWindowInfo(KbMode* parent);
    KbWindowInfo(KbMode* parent, const KbWindowInfo& other);

    // Match data
    QString windowTitle;
    // Should the window title be matched case insensitive?
    bool windowTitleCaseInsensitive;
    // Should the window title be a substring of the actual window?
    bool windowTitleSubstr;
    // Binary name
    QString program;
    // Linux specific
    QString wm_instance_name;
    QString wm_class_name;
    inline bool needsSave() const { return _needsSave; }

    void load(CkbSettings& settings);
    void save(CkbSettings& settings);
    void winInfoImport(QSettings* settings);
    void winInfoExport(QSettings* settings);

    void setNeedsSave();

private:
    bool _needsSave;
};

#endif // WINDOWINFO_H
