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

#include "qsystemsemaphore.h"
#include "qsystemsemaphore_p.h"

#include <windows.h>

QSystemSemaphorePrivate::QSystemSemaphorePrivate() :
        semaphore(nullptr), error(QSystemSemaphore::NoError)
{
}

void QSystemSemaphorePrivate::setErrorString(const std::string &function)
{
    auto windowsError = GetLastError();
    if (windowsError == 0)
        return;

    switch (windowsError) {
    case ERROR_NO_SYSTEM_RESOURCES:
    case ERROR_NOT_ENOUGH_MEMORY:
        error = QSystemSemaphore::OutOfResources;
        errorString = function + ": out of resources";
        break;
    case ERROR_ACCESS_DENIED:
        error = QSystemSemaphore::PermissionDenied;
        errorString = function + ": permission denied";
        break;
    default:
        errorString = function + ": unknown error " + std::to_string(windowsError);
        error = QSystemSemaphore::UnknownError;
    }
}

HANDLE QSystemSemaphorePrivate::handle(QSystemSemaphore::AccessMode)
{
    // don't allow making handles on empty keys
    if (key.empty())
        return nullptr;

    // Create it if it doesn't already exists.
    if (semaphore == nullptr) {
        semaphore = CreateSemaphore(nullptr, initialValue, MAXLONG, fileName.c_str());
        if (semaphore == nullptr)
            setErrorString("QSystemSemaphore::handle");
    }

    return semaphore;
}

void QSystemSemaphorePrivate::cleanHandle()
{
    if (semaphore && !CloseHandle(semaphore)) {
    }
    semaphore = nullptr;
}

bool QSystemSemaphorePrivate::modifySemaphore(int count)
{
    if (nullptr == handle())
        return false;

    if (count > 0) {
        if (0 == ReleaseSemaphore(semaphore, count, nullptr)) {
            setErrorString("QSystemSemaphore::modifySemaphore");
            return false;
        }
    } else {
        if (WAIT_OBJECT_0 != WaitForSingleObjectEx(semaphore, INFINITE, FALSE)) {
            setErrorString("QSystemSemaphore::modifySemaphore");

            return false;
        }
    }

    clearError();
    return true;
}

