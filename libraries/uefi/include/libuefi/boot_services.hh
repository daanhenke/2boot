#pragma once

#include <libuefi/primitives.hh>

#include "protocol/device_path.hh"

namespace uefi
{
    enum class AllocateType
    {
        AnyPages,
        MaxAddress,
        Address
    };

    enum class MemoryType
    {
        ReservedMemoryType,
        LoaderCode,
        LoaderData,
        BootServicesCode,
        BootServicesData,
        RuntimeServicesCode,
        RuntimeServicesData,
        ConventionalMemory,
        UnusableMemory,
        ACPIReclaimMemory,
        ACPIMemoryNVS,
        MemoryMappedIO,
        MemoryMappedIOPortSpace,
        PALCode,
        PersistentMemory
    };

    using BootServices = struct
    {
        TableHeader Header;

        void (*stub_0)();
        void (*stub_1)();
        Status (*allocate_pages)(AllocateType type, MemoryType memory_type, u64 pages, PhysicalAddress* memory);
        Status (*free_pages)(PhysicalAddress memory, u64 pages);
        void (*stub_4)();
        Status (*allocate_pool)(MemoryType pool_type, u64 size, void** buffer);
        Status (*free_pool)(void* buffer);
        void (*stub_7)();
        void (*stub_8)();
        Status (*wait_for_event)(u64 number_of_events, Event* events, u64* index);
        Status (*signal_event)(Event event);
        Status (*close_event)(Event event);
        Status (*check_event)(Event event);
        void (*stub_9)();
        void (*stub_10)();
        void (*stub_11)();
        Status (*handle_protocol)(Handle handle, const Guid* protocol, void** interface);
        void* reserved;
        void (*stub_12)();
        void (*stub_13)();
        void (*stub_14)();
        void (*stub_15)();
        Status (*load_image)(bool boot_policy, Handle parent_image_handle, const protocol::DevicePathProtocol* device_path, void* source_buffer, u64 source_size, Handle* image_handle);
        Status (*start_image)(Handle image_handle, u64* exit_data_size, Char16** exit_data);
    };
}
