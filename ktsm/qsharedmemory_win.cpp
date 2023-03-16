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

#include "qsharedmemory.h"
#include "qsharedmemory_p.h"
#include "qsystemsemaphore.h"
#include <windows.h>

QSharedMemoryPrivate::QSharedMemoryPrivate() :
        memory(nullptr), size(0), error(QSharedMemory::NoError),
           systemSemaphore(std::string()), lockedByMe(false), hand(nullptr)
{
}

void QSharedMemoryPrivate::setErrorString(const std::string &function)
{
    DWORD windowsError = GetLastError();
    if (windowsError == 0)
        return;
    switch (windowsError) {
    case ERROR_ALREADY_EXISTS:
        error = QSharedMemory::AlreadyExists;
        errorString = function + "1: already exists";
    break;
    case ERROR_FILE_NOT_FOUND:
        error = QSharedMemory::NotFound;
        errorString = function + ": doesn't exist";
        break;
    case ERROR_COMMITMENT_LIMIT:
        error = QSharedMemory::InvalidSize;
        errorString = function + ": invalid size";
        break;
    case ERROR_NO_SYSTEM_RESOURCES:
    case ERROR_NOT_ENOUGH_MEMORY:
        error = QSharedMemory::OutOfResources;
        errorString = function + ": out of resources";
        break;
    case ERROR_ACCESS_DENIED:
        error = QSharedMemory::PermissionDenied;
        errorString = function + ": permission denied";
        break;
    default:
        errorString = function + ": unknown error " + std::to_string(windowsError);
        error = QSharedMemory::UnknownError;
    }
}

HANDLE QSharedMemoryPrivate::handle()
{
    if (!hand) {
        const std::string function("QSharedMemory::handle");
        if (nativeKey.empty()) {
            error = QSharedMemory::KeyError;
            errorString = function + ": unable to make key";
            return nullptr;
        }
        hand = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, nativeKey.c_str());
        if (!hand) {
            setErrorString(function);
            return nullptr;
        }
    }
    return hand;
}

bool QSharedMemoryPrivate::cleanHandle()
{
    if (hand != nullptr && !CloseHandle(hand)) {
        hand = nullptr;
        setErrorString("QSharedMemory::cleanHandle");
        return false;
    }
    hand = nullptr;
    return true;
}

bool QSharedMemoryPrivate::create(int sz)
{
    const std::string function("QSharedMemory::create");
    if (nativeKey.empty()) {
        error = QSharedMemory::KeyError;
        errorString = function + ": key error";
        return false;
    }

    hand = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sz, nativeKey.c_str());
    setErrorString(function);

    // hand is valid when it already exists unlike unix so explicitly check
    return error != QSharedMemory::AlreadyExists && hand;
}

bool QSharedMemoryPrivate::attach(QSharedMemory::AccessMode mode)
{
    // Grab a pointer to the memory block
    int permissions = (mode == QSharedMemory::ReadOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS);
    memory = (void *)MapViewOfFile(handle(), permissions, 0, 0, 0);
    if (nullptr == memory) {
        setErrorString("QSharedMemory::attach");
        cleanHandle();
        return false;
    }

    // Grab the size of the memory we have been given (a multiple of 4K on windows)
    MEMORY_BASIC_INFORMATION info;
    if (!VirtualQuery(memory, &info, sizeof(info))) {
        // Windows doesn't set an error code on this one,
        // it should only be a kernel memory error.
        error = QSharedMemory::UnknownError;
        errorString = "QSharedMemory::attach: size query failed";
        return false;
    }
    size = int(info.RegionSize);

    return true;
}

bool QSharedMemoryPrivate::detach()
{
    // umap memory
    if (!UnmapViewOfFile(memory)) {
        setErrorString("QSharedMemory::detach");
        return false;
    }
    memory = nullptr;
    size = 0;

    // close handle
    return cleanHandle();
}

