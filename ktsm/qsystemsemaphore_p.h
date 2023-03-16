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

#ifndef QSYSTEMSEMAPHORE_P_H
#define QSYSTEMSEMAPHORE_P_H

#include "qsystemsemaphore.h"

#include "qsharedmemory_p.h"
#include <sys/types.h>
#ifdef QT_POSIX_IPC
#   include <semaphore.h>
#endif

#include <string>

class QSystemSemaphorePrivate
{
public:
    QSystemSemaphorePrivate();
    std::string makeKeyFileName()
    {
        return QSharedMemoryPrivate::makePlatformSafeKey(key, "qipc_systemsem_");
    }

    inline void setError(QSystemSemaphore::SystemSemaphoreError e, const std::string &message)
    { error = e; errorString = message; }
    inline void clearError()
    { setError(QSystemSemaphore::NoError, std::string()); }

#ifdef __WIN32
    Qt::HANDLE handle(QSystemSemaphore::AccessMode mode = QSystemSemaphore::Open);
    void setErrorString(const std::string &function);
#elif defined(QT_POSIX_IPC)
    bool handle(QSystemSemaphore::AccessMode mode = QSystemSemaphore::Open);
    void setErrorString(const std::string &function);
#else
    key_t handle(QSystemSemaphore::AccessMode mode = QSystemSemaphore::Open);
    void setErrorString(const std::string &function);
#endif
    void cleanHandle();
    bool modifySemaphore(int count);

    std::string key;
    std::string fileName;
    int initialValue{};
#ifdef __WIN32
    Qt::HANDLE semaphore;
    Qt::HANDLE semaphoreLock{};
#elif defined(QT_POSIX_IPC)
    sem_t *semaphore;
    bool createdSemaphore;
#else
    key_t unix_key;
    int semaphore;
    bool createdFile;
    bool createdSemaphore;
#endif
    std::string errorString;
    QSystemSemaphore::SystemSemaphoreError error;
};

#endif // QSYSTEMSEMAPHORE_P_H

