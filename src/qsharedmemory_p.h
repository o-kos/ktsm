/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSHAREDMEMORY_P_H
#define QSHAREDMEMORY_P_H

#include "qglobal.h"
#include "qsharedmemory.h"
#include "qsystemsemaphore.h"

#include <cassert>
#include <string>

#if !defined(_WIN32)
#  include <sys/sem.h>
#endif

class QSharedMemoryLocker
{

public:
    inline explicit QSharedMemoryLocker(QSharedMemory *sharedMemory) : q_sm(sharedMemory)
    {
        assert(q_sm);
    }

    inline ~QSharedMemoryLocker()
    {
        if (q_sm)
            q_sm->unlock();
    }

    inline bool lock()
    {
        if (q_sm && q_sm->lock())
            return true;
        q_sm = nullptr;
        return false;
    }

private:
    QSharedMemory *q_sm;
};

class QSharedMemoryPrivate
{
public:
    QSharedMemoryPrivate();

    void *memory;
    int size;
    std::string key;
    std::string nativeKey;
    QSharedMemory::SharedMemoryError error;
    std::string errorString;
    QSystemSemaphore systemSemaphore;
    bool lockedByMe;

    static int createUnixKeyFile(const std::string &fileName);
    static std::string makePlatformSafeKey(const std::string &key, const std::string &prefix = "qipc_sharedmemory_");
#ifdef __WIN32
    Qt::HANDLE handle();
#elif defined(QT_POSIX_IPC)
    int handle();
#endif
    bool initKey();
    bool cleanHandle();
    bool create(int size);
    bool attach(QSharedMemory::AccessMode mode);
    bool detach();

    void setErrorString(const std::string& function);

    bool tryLocker(QSharedMemoryLocker *locker, const std::string &function) {
        if (!locker->lock()) {
            errorString = function + ": unable to lock";
            error = QSharedMemory::LockError;
            return false;
        }
        return true;
    }

private:
#ifdef __WIN32
    Qt::HANDLE hand;
#elif defined(QT_POSIX_IPC)
    int hand;
#else
    key_t unix_key;
#endif
};

#endif // QSHAREDMEMORY_P_H
