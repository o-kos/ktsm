# ktsm - Qt5 compatible QSharedMemory implementation 

Based on Qt 5.15.2 QSharedMemory sources implementation of Qt shared memory IPC. 
Full compatible with create, attach, lock etc methods, include native key generation.
Does not depend on any Qt libraries.

## Example

Load data into shared memory
```cpp
#include "qsharedmemory.h"

QSharedMemory sm("qsharedmemory");

bool load(const std::vector<char> &buffer) {
    if (sm.isAttached()) detach(sm);

    if (!sm.create(size)) return false;

    sm.lock();
    memcpy(sm.data(), buffer.data(), size);
    sm.unlock();

    return true;
}
```

Save data from shared memory
```cpp
#include "qsharedmemory.h"

bool save(std::vector<char> &buffer) {
    if (!sm.attach()) return false;

    sm.lock();
    buffer.resize(sm.size())
	memcpy(buffer.data(), sm.data(), size);
    sm.unlock();

    sm.detach();

    return true;
}

```
