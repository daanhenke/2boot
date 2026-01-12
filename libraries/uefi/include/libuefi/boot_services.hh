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

    enum class LocateSearchType
    {
        AllHandles,
        ByRegisterNotify,
        ByProtocol
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
        Status (*exit)(Handle image_handle, Status exit_status, u64 exit_data_size, Char16* exit_data);
        Status (*unload_image)(Handle image_handle);
        Status (*exit_boot_services)(Handle image_handle, u64 map_key);
        Status (*get_next_monotonic_count)(u64* count);
        Status (*stall)(u64 microseconds);
        Status (*set_watchdog_timer)(u64 timeout, u64 watchdog_code, u64 data_size, Char16* watchdog_data);
        Status (*connect_controller)(Handle controller_handle, Handle* driver_image_handle, protocol::DevicePathProtocol* remaining_device_path, bool recursive);
        Status (*disconnect_controller)(Handle controller_handle, Handle driver_image_handle, Handle child_handle);
        Status (*open_protocol)(Handle handle, const Guid* protocol, void** interface, Handle agent_handle, Handle controller_handle, u32 attributes);
        Status (*close_protocol)(Handle handle, const Guid* protocol, Handle agent_handle, Handle controller_handle);
        Status (*open_protocol_information)(Handle handle, const Guid* protocol, /* TODO: RETYPE */ void** entry_buffer, u64* entry_count);
        Status (*protocols_per_handle)(Handle handle, Guid** protocol_buffer, u64* protocol_buffer_count);
        Status (*locate_handle_buffer)(LocateSearchType search_type, const Guid* protocol, void* search_key, u64* number_of_handles, Handle** buffer);
        Status (*locate_protocol)(const Guid* protocol, void* registration, void** interface);
    };
}
