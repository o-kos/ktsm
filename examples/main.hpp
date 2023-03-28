#include <vector>
#include <iostream>
#include <fstream>

int handle_error(const std::string &msg)
{
    std::printf("Error: %s\n", msg.c_str());
    return 1;
}

void detach(QSharedMemory &sm)
{
    if (!sm.detach())
        handle_error("unable to detach from shared memory");
}

int load(const char *key, const std::string &path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return handle_error("unable to load file");

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    file.read(buffer.data(), std::streamsize(size));

    QSharedMemory sm(key);
    if (sm.isAttached()) detach(sm);

    if (!sm.create(int(sizeof(size) + size)))
        return handle_error("unable to create system memory share");

    sm.lock();
    memcpy(sm.data(), &size, sizeof(size));
    memcpy((void*)((const char*)sm.data() + sizeof(size)), buffer.data(), size);
    sm.unlock();

    std::cout << "Loaded " << size << " bytes from " << path << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}

int save(const char *key, const std::string &path) {
    QSharedMemory sm(key);
    if (!sm.attach())
        return handle_error("unable to attach to shared memory");

    sm.lock();
    std::ofstream file(path, std::ios::binary);
    std::streamsize size;
    memcpy(&size, sm.constData(), sizeof(size));
    if (!file.write((const char*)sm.constData() + sizeof(size), size))
        return handle_error("unable to write file");
    sm.unlock();
    std::cout << "Saved " << size << " bytes to " << path << std::endl;

    sm.detach();

    return 0;
}

int parseArgs(int argc, char *argv[]) {
    auto usage = [&]() {
        std::cerr << "Usage: " << argv[0] << " [load|save] <key> <filename>" << std::endl;
        return 1;
    };
    if (argc != 4) return usage();

    std::string cmd(argv[1]);
    if (cmd == "load") {
        return load(argv[2], argv[3]);
    } else if (cmd == "save") {
        return save(argv[2], argv[3]);
    } else {
        return usage();
    }
}

int sm_main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    int ret = parseArgs(argc, argv);
    if (ret) return ret;

    return sm_main(argc, argv);
}
