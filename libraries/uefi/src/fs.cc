#include <libuefi/ext/fs.hh>

#include <libuefi/globals.hh>
#include <libuefi/protocol/loaded_image.hh>
#include <libuefi/protocol/simple_filesystem.hh>

#include <libuefi/ext/string.hh>

namespace uefi
{
    protocol::FileProtocol* gPrimaryDriveRoot = nullptr;

    bool InitializeFilesystem()
    {
        if (gPrimaryDriveRoot != nullptr)
        {
            gPrimaryDriveRoot->close(gPrimaryDriveRoot);
        }

        const auto* BS = gSystemTable->boot_services;

        protocol::LoadedImageProtocol* loaded_image = nullptr;
        if (BS->handle_protocol(gImageHandle, &guids::LoadedImageProtocolGuid, reinterpret_cast<void**>(&loaded_image)))
        {
            return false;
        }

        protocol::SimpleFilesystemProtocol* simple_filesystem = nullptr;
        if (BS->handle_protocol(loaded_image->device_handle, &guids::SimpleFilesystemProtocolGuid, reinterpret_cast<void**>(&simple_filesystem)))
        {
            return false;
        }

        if (simple_filesystem->open_volume(simple_filesystem, &gPrimaryDriveRoot))
        {
            return false;
        }

        return true;
    }

    bool ReadFile(const char* path, void** buffer, u64* size)
    {
        const auto* BS = gSystemTable->boot_services;

        const auto path_length = String8Length(path) + 1;
        Char16* path_buffer = nullptr;
        if (BS->allocate_pool(MemoryType::LoaderData, path_length * sizeof(Char16), reinterpret_cast<void**>(&path_buffer)))
        {
            return false;
        }
        String8ToString16(path, path_buffer, path_length);

        protocol::FileProtocol* file = nullptr;
        if (gPrimaryDriveRoot->open(gPrimaryDriveRoot, &file, path_buffer, protocol::FileOpenMode::Read, 0))
        {
             PrintString8("Failed to find file\r\n");
           BS->free_pool(path_buffer);
            return false;
        }
        BS->free_pool(path_buffer);

        u64 file_info_size = 0;
        if (const auto result = file->get_info(file, &guids::FileInfoGuid, &file_info_size, nullptr);
            result != status::Success && result != status::BufferTooSmall)
        {
            file->close(file);
            return false;
        }

        protocol::FileInfo* file_info = nullptr;
        if (BS->allocate_pool(MemoryType::LoaderData, file_info_size, reinterpret_cast<void**>(&file_info)))
        {
            file->close(file);
            return false;
        }

        if (file->get_info(file, &guids::FileInfoGuid, &file_info_size, file_info))
        {
            BS->free_pool(file_info);
            file->close(file);
            return false;
        }

        auto file_size = file_info->file_size;
        BS->free_pool(file_info);

        if (file_size == 0)
        {
            file->close(file);
            return false;
        }

        void* file_buffer = nullptr;
        if (BS->allocate_pool(MemoryType::LoaderData, file_size, reinterpret_cast<void**>(&file_buffer)))
        {
            file->close(file);
            return false;
        }

        size_t total = 0;
        while (total < file_size)
        {
            auto chunk = file_size - total;
            if (const auto result = file->read(file, &chunk, static_cast<u8*>(file_buffer) + total); result != status::Success)
            {
                BS->free_pool(file_buffer);
                file->close(file);
                return false;
            }
            if (chunk == 0) break;
            total += chunk;
        }

        file->close(file);

        if (total != file_size)
        {
            BS->free_pool(file_buffer);
            return false;
        }

        *buffer = file_buffer;
        *size = file_size;
        return true;
    }
}
