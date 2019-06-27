#include "ckbsettings.h"
#include "kbwindowinfo.h"
#include "kbmode.h"

KbWindowInfo::KbWindowInfo(KbMode* parent) :
    QObject(parent), enabled(false),windowTitle(QString()), windowTitleCaseInsensitive(false), windowTitleSubstr(false),
    program(QString()), wm_instance_name(QString()), wm_class_name(QString())
{
    _needsSave = true;
}

KbWindowInfo::KbWindowInfo(KbMode* parent, const KbWindowInfo &other) :
    QObject(parent), enabled(other.enabled), windowTitle(other.windowTitle), windowTitleCaseInsensitive(other.windowTitleCaseInsensitive),
    windowTitleSubstr(other.windowTitleSubstr), program(other.program), wm_instance_name(other.wm_instance_name),
    wm_class_name(other.wm_class_name)
{
    _needsSave = true;
}

void KbWindowInfo::load(CkbSettings& settings){
    _needsSave = false;

    SGroup group(settings, "WindowInfo");
    enabled = settings.value("enabled", false).toBool();
    windowTitle = settings.value("windowTitle", "").toString();
    windowTitleCaseInsensitive = settings.value("windowTitleCaseInsensitive", false).toBool();
    windowTitleSubstr = settings.value("windowTitleSubstr", false).toBool();
    program = settings.value("program", "").toString();

    // Linux specific
    wm_instance_name = settings.value("wm_instance_name", "").toString();
    wm_class_name = settings.value("wm_class_name", "").toString();
}

void KbWindowInfo::save(CkbSettings& settings){
    _needsSave = false;
    SGroup group(settings, "WindowInfo");
    settings.setValue("enabled", enabled);
    settings.setValue("windowTitle", windowTitle);
    settings.setValue("windowTitleCaseInsensitive", windowTitleCaseInsensitive);
    settings.setValue("windowTitleSubstr", windowTitleSubstr);
    settings.setValue("program", program);

    settings.setValue("wm_instance_name", wm_instance_name);
    settings.setValue("wm_class_name", wm_class_name);
}

void KbWindowInfo::winInfoImport(QSettings* settings){
    _needsSave = false;
    settings->beginGroup("WindowInfo");
    enabled = settings->value("enabled", false).toBool();
    windowTitle = settings->value("windowTitle", "").toString();
    windowTitleCaseInsensitive = settings->value("windowTitleCaseInsensitive", false).toBool();
    windowTitleSubstr = settings->value("windowTitleSubstr", false).toBool();
    program = settings->value("program", "").toString();

    // Linux specific
    wm_instance_name = settings->value("wm_instance_name", "").toString();
    wm_class_name = settings->value("wm_class_name", "").toString();
    settings->endGroup();
}

void KbWindowInfo::winInfoExport(QSettings* settings){
    _needsSave = false;
    settings->beginGroup("WindowInfo");
    settings->setValue("enabled", enabled);
    settings->setValue("windowTitle", windowTitle);
    settings->setValue("windowTitleCaseInsensitive", windowTitleCaseInsensitive);
    settings->setValue("windowTitleSubstr", windowTitleSubstr);
    settings->setValue("program", program);

    settings->setValue("wm_instance_name", wm_instance_name);
    settings->setValue("wm_class_name", wm_class_name);
    settings->endGroup();
}

void KbWindowInfo::setNeedsSave(){
    _needsSave = true;
}
