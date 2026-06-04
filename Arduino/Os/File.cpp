// ======================================================================
// \title Os/Arduino/File.cpp
// \brief Arduino implementation for Os::File
// ======================================================================
#include "Arduino/Os/File.hpp"
#include <Fw/Types/Assert.hpp>

namespace Os {
namespace Arduino {

ArduinoFile::~ArduinoFile() {
    this->close();
}

ArduinoFile::Status ArduinoFile::open(const char* filepath, ArduinoFile::Mode open_mode, OverwriteType overwrite) {
#if defined(ARDUINO_ARCH_ESP32)
    // ESP32 core: fs::FS::open() takes a POSIX-style mode *string* ("r"/"w"/"a"),
    // not the integer O_* flags used by the SdFat-based cores. On this core
    // FILE_READ/FILE_WRITE/FILE_APPEND are themselves the mode strings.
    // Map the F' open mode onto the closest available string mode. The "create"
    // argument requests creation of the file/parent path when it does not exist.
    const char* mode = FILE_READ;
    bool create = false;

    switch (open_mode) {
        case OPEN_READ:
            mode = FILE_READ;  // "r"
            break;
        case OPEN_WRITE:
        case OPEN_SYNC_WRITE:
        case OPEN_CREATE:
            // No truncate-vs-keep or sync distinction is available through the
            // string-mode API; all map to write ("w") with creation enabled.
            mode = FILE_WRITE;  // "w"
            create = true;
            break;
        case OPEN_APPEND:
            mode = FILE_APPEND;  // "a"
            create = true;
            break;
        default:
            FW_ASSERT(0, open_mode);
            break;
    }

    this->m_handle.m_fd = SD.open(filepath, mode, create);
#else
    // SdFat-based cores (e.g. Adafruit SAMD / Feather M4): SD.open() takes
    // integer O_* flags, and FILE_READ/FILE_WRITE are integer constants.
    PlatformIntType flags = 0;

    switch (open_mode) {
        case OPEN_READ:
            flags = O_RDONLY;
            break;
        case OPEN_WRITE:
            flags = O_WRONLY | O_CREAT;
            break;
        case OPEN_SYNC_WRITE:
            flags = O_WRONLY | O_CREAT | O_SYNC;
            break;
        case OPEN_CREATE:
            flags = O_WRONLY | O_CREAT | O_TRUNC | ((overwrite == ArduinoFile::OverwriteType::OVERWRITE) ? 0 : O_EXCL);
            break;
        case OPEN_APPEND:
            flags = FILE_WRITE;
            break;
        default:
            FW_ASSERT(0, open_mode);
            break;
    }

    this->m_handle.m_fd = SD.open(filepath, flags);
#endif

    if (!this->m_handle.m_fd) {
        return Status::OTHER_ERROR;
    }

    this->m_handle.opened = true;
    return Status::OP_OK;
}

void ArduinoFile::close() {
    if (this->m_handle.opened) {
        this->m_handle.m_fd.close();
        this->m_handle.opened = false;
    }
}

ArduinoFile::Status ArduinoFile::size(FwSizeType& size_result) {
    if (!this->m_handle.opened) {
        size_result = 0;
        return Status::NOT_OPENED;
    }

    size_result = this->m_handle.m_fd.size();

    return Status::OP_OK;
}

ArduinoFile::Status ArduinoFile::position(FwSizeType& position_result) {
    if (!this->m_handle.opened) {
        position_result = 0;
        return Status::NOT_OPENED;
    }

    position_result = this->m_handle.m_fd.position();

    return Status::OP_OK;
}

ArduinoFile::Status ArduinoFile::preallocate(FwSizeType offset, FwSizeType length) {
    Status status = Status::NOT_SUPPORTED;
    return status;
}

ArduinoFile::Status ArduinoFile::seek(FwSignedSizeType offset, SeekType seekType) {
    if (!this->m_handle.opened) {
        return Status::NOT_OPENED;
    }

    if (this->m_handle.m_fd.seek(offset)) {
        return Status::OP_OK;
    }

    return Status::OTHER_ERROR;
}

ArduinoFile::Status ArduinoFile::flush() {
    Status status = Status::NOT_SUPPORTED;
    return status;
}

ArduinoFile::Status ArduinoFile::read(U8* buffer, FwSizeType& size, ArduinoFile::WaitType wait) {
    FW_ASSERT(buffer);

    if (!this->m_handle.opened) {
        size = 0;
        return Status::NOT_OPENED;
    }

    if (size <= 0) {
        size = 0;
        return Status::BAD_SIZE;
    }

    if (this->m_handle.m_fd.available()) {
        size = this->m_handle.m_fd.read(static_cast<U8*>(buffer), size);
    } else {
        size = 0;
    }

    return OP_OK;
}

ArduinoFile::Status ArduinoFile::write(const U8* buffer, FwSizeType& size, ArduinoFile::WaitType wait) {
    if (!this->m_handle.opened) {
        size = 0;
        return Status::NOT_OPENED;
    }

    if (size <= 0) {
        size = 0;
        return Status::BAD_SIZE;
    }

    size = this->m_handle.m_fd.write(buffer, size);

    if (size == 0) {
        return Status::OTHER_ERROR;
    }

    return OP_OK;
}

FileHandle* ArduinoFile::getHandle() {
    return &this->m_handle;
}

}  // namespace Arduino
}  // namespace Os
