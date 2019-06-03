#include "xwindowdetector.h"
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <QDebug>
#include "xwindowinfo.h"
#include <QMetaType>
#include <QFileInfo>
#include <sys/eventfd.h>
#include <unistd.h>

// Gets the current focused window, listens for focus out events on it, and repeats on new events.
const uint32_t values = XCB_EVENT_MASK_FOCUS_CHANGE;

xcb_window_t appwindow = 0;
int efd;

// The following are/must be called in the parent thread
XWindowDetector::XWindowDetector(QObject *parent) :
    QThread(parent)
{
    efd = eventfd(0, 0);
}

void XWindowDetector::terminateEventLoop()
{
    uint64_t val = 1;
    if(write(efd, &val, sizeof(uint64_t)) == -1)
        qDebug() << errno;
}

// The following are to be called inside QThread::run() (the new thread)
QString XWindowDetector::getExePathByPID(uint32_t pid)
{
    return QFileInfo(QString("/proc/") + QString::number(pid) + "/exe").canonicalFilePath();
}

void listenForWindowEvents(xcb_connection_t* conn, xcb_window_t root)
{
    xcb_change_window_attributes(conn, root, XCB_CW_EVENT_MASK, &values);
    xcb_flush(conn);
}

QString XWindowDetector::xcbGetString(xcb_connection_t* conn, xcb_window_t win, xcb_atom_enum_t atom, xcb_atom_enum_t type)
{
    QString xcb_string;
    xcb_get_property_cookie_t prop = xcb_get_property(conn, 0, win, atom, type, 0, 128);
    xcb_get_property_reply_t* reply = xcb_get_property_reply(conn, prop, NULL);
    if(!reply)
        return xcb_string;
    int len = xcb_get_property_value_length(reply);
    if(len)
        xcb_string = QString::fromUtf8((const char*)xcb_get_property_value(reply), len);
    free(reply);
    return xcb_string;
}

xcb_generic_event_t* XWindowDetector::xcbWaitForEventInterruptible(xcb_connection_t* conn, int xcbFd, fd_set* fds)
{
    FD_ZERO(fds);
    FD_SET(xcbFd, fds);
    FD_SET(efd, fds);
    if(pselect(((xcbFd > efd) ? xcbFd : efd) + 1, fds, NULL, NULL, NULL, NULL) > 0)
    {
        // If we received data from the eventfd, then we need to tell the thread to close. Otherwise, we have an event from xcb.
        if(FD_ISSET(efd, fds))
            return nullptr;
        return xcb_poll_for_event(conn);
    }
    return nullptr;
}

void XWindowDetector::run()
{
    int default_screen = 0;
    // XCB
    xcb_connection_t* conn = xcb_connect(NULL, &default_screen);
    qDebug() << "X screen" << default_screen;

    // EWMH
    xcb_ewmh_connection_t ewmh_c;
    xcb_ewmh_connection_t* ewmh = &ewmh_c;
    if(!xcb_ewmh_init_atoms_replies(ewmh, xcb_ewmh_init_atoms(conn, ewmh), NULL))
    {
        qDebug() << "Error initialising EWMH";
        return;
    }
    xcb_window_t win = 0;

    msleep(500);

    xcb_ewmh_get_active_window_reply(ewmh, xcb_ewmh_get_active_window(ewmh, default_screen), &win, NULL);
    listenForWindowEvents(conn, win);

    int xcbFd = xcb_get_file_descriptor(conn);
    fd_set fds;

    xcb_generic_event_t* evt = nullptr;
    while((evt = xcbWaitForEventInterruptible(conn, xcbFd, &fds))){
        if(evt == nullptr)
            break;
        if((evt->response_type & ~0x80) != XCB_FOCUS_OUT)
        {
            free(evt);
            continue;
        }

        // The following is needed because EWMH is not updated the moment the event is received
        // It is also used to filter out duplicate events
        int cnt = 0;
        xcb_window_t newwin = win;
        do
        {

            //qDebug() << "EWMH WAIT";
            xcb_ewmh_get_active_window_reply(ewmh, xcb_ewmh_get_active_window(ewmh, default_screen), &newwin, NULL);
            msleep(40);
            cnt++;
        } while(win == newwin && cnt < 5);
        if(cnt > 4)
        {
            free(evt);
            listenForWindowEvents(conn, win);
            continue;
        }
        win = newwin;

        // get PID if possible
        uint32_t pid = 0;
        xcb_get_property_reply_t* pid_reply = xcb_get_property_reply(conn, xcb_ewmh_get_wm_pid(ewmh, win), NULL);
        if(pid_reply)
        {
            xcb_ewmh_get_wm_pid_from_reply(&pid, pid_reply);
            free(pid_reply);
        }

        // Get WM_CLASS
        QStringList wm_class = xcbGetString(conn, win, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING).split(QChar('\0'),QString::SkipEmptyParts);

        // Get _NET_WM_NAME using EWMH
        QString window_name;
        xcb_ewmh_get_utf8_strings_reply_t _net_wm_name;
        if(xcb_ewmh_get_wm_name_from_reply(ewmh, &_net_wm_name, xcb_get_property_reply(conn, xcb_ewmh_get_wm_name(ewmh, win), NULL)))
        {
            window_name = QString::fromUtf8(_net_wm_name.strings, _net_wm_name.strings_len);
            xcb_ewmh_get_utf8_strings_reply_wipe(&_net_wm_name);
        }
        else
        {
            // Fall back to the legacy WM_NAME property
            //qDebug() << "EWMH FAIL";
            window_name = xcbGetString(conn, win, XCB_ATOM_WM_NAME, XCB_ATOM_ANY);
        }

        if(!window_name.isEmpty())
        {
            QString wm_instance_name, wm_class_name;
            if(wm_class.length() == 2)
            {
                wm_instance_name = wm_class.at(0);
                wm_class_name = wm_class.at(1);
            }
            const XWindowInfo info = {pid, window_name, getExePathByPID(pid), wm_instance_name, wm_class_name};
            emit activeWindowChanged(info);
        }

        // Set up event mask
        listenForWindowEvents(conn, win);
        free(evt);
    }
    xcb_ewmh_connection_wipe(ewmh);
    xcb_disconnect(conn);
    qDebug() << "XCB EWMH Thread Quitting...";
}
