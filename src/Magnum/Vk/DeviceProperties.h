#ifndef Magnum_Vk_DeviceProperties_h
#define Magnum_Vk_DeviceProperties_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::Vk::DeviceProperties, enum @ref Magnum::Vk::DeviceType, @ref Magnum::Vk::QueueFlag, @ref Magnum::Vk::MemoryHeapFlag, enum set @ref Magnum::Vk::QueueFlags, @ref Magnum::Vk::MemoryHeapFlags, function @ref Magnum::Vk::enumerateDevices(), @ref Magnum::Vk::pickDevice(), @ref Magnum::Vk::tryPickDevice()
 * @m_since_latest
 */

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation { struct InstanceState; }

/**
@brief Physical device type
@m_since_latest

Wraps a @type_vk_keyword{PhysicalDeviceType}.
@see @ref DeviceProperties::type()
@m_enum_values_as_keywords
*/
enum class DeviceType: Int {
    /** Anything that does not match any other available types */
    Other = VK_PHYSICAL_DEVICE_TYPE_OTHER,

    /**
     * Typically a device embedded in or tightly coupled with the host
     */
    IntegratedGpu = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,

    /**
     * Typically a separate processor connected to the host via an
     * interlink
     */
    DiscreteGpu = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,

    /** Typically a virtual node in a virtualization environment */
    VirtualGpu = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,

    /** Typically running on the same processors as the host */
    Cpu = VK_PHYSICAL_DEVICE_TYPE_CPU
};

/**
@debugoperatorclassenum{DeviceProperties,DeviceType}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, DeviceType value);

/**
@brief Queue flag
@m_since_latest

Wraps a @type_vk_keyword{QueueFlagBits}.
@see @ref QueueFlags, @ref DeviceProperties::queueFamilyFlags()
@m_enum_values_as_keywords
*/
enum class QueueFlag: UnsignedInt {
    /** Supports graphics operations */
    Graphics = VK_QUEUE_GRAPHICS_BIT,

    /** Supports compute operations */
    Compute = VK_QUEUE_COMPUTE_BIT,

    /** Supports transfer operations */
    Transfer = VK_QUEUE_TRANSFER_BIT,

    /** Supports sparse memory management operations */
    SparseBinding = VK_QUEUE_SPARSE_BINDING_BIT,

    /** Supports protected memory operations */
    Protected = VK_QUEUE_PROTECTED_BIT
};

/**
@debugoperatorclassenum{DeviceProperties,QueueFlag}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, QueueFlag value);

/**
@brief Queue flags
@m_since_latest

Type-safe wrapper for @type_vk_keyword{QueueFlags}.
@see @ref DeviceProperties::queueFamilyFlags()
*/
typedef Containers::EnumSet<QueueFlag> QueueFlags;

CORRADE_ENUMSET_OPERATORS(QueueFlags)

/**
@debugoperatorclassenum{DeviceProperties,QueueFlags}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, QueueFlags value);

/**
@brief Memory heap flag
@m_since_latest

Wraps a @type_vk_keyword{MemoryHeapFlagBits}.
@see @ref QueueFlags, @ref DeviceProperties::queueFamilyFlags()
@m_enum_values_as_keywords
*/
enum class MemoryHeapFlag: UnsignedInt {
    /** Corresponds to device-local memory */
    DeviceLocal = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,

    /** @todo MultiInstance, Vk 1.1 */
};

/**
@debugoperatorclassenum{DeviceProperties,MemoryHeapFlag}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, MemoryHeapFlag value);

/**
@brief Memory heap flags
@m_since_latest

Type-safe wrapper for @type_vk_keyword{MemoryHeapFlags}.
@see @ref DeviceProperties::memoryHeapFlags(), @ref MemoryFlags
*/
typedef Containers::EnumSet<MemoryHeapFlag> MemoryHeapFlags;

CORRADE_ENUMSET_OPERATORS(MemoryHeapFlags)

/**
@debugoperatorclassenum{DeviceProperties,MemoryHeapFlags}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, MemoryHeapFlags value);

/* MemoryFlag/MemoryFlags are in Memory.h since those are used mainly in
   contexts where DeviceProperties isn't present */

/**
@brief Physical device properties
@m_since_latest

Wraps a @type_vk_keyword{PhysicalDevice} along with its (lazy-populated)
properties such as @type_vk_keyword{PhysicalDeviceProperties2} and
@type_vk_keyword{GetPhysicalDeviceQueueFamilyProperties2}.

See the @ref Vk-Device-usage "Device class docs" for an example of using this
class for enumerating available devices and picking one of them.

@see @ref pickDevice(), @ref enumerateDevices()
*/
class MAGNUM_VK_EXPORT DeviceProperties {
    public:
        /**
         * @brief Wrap existing Vulkan physical device
         * @param instance      Vulkan instance
         * @param handle        The @type_vk{PhysicalDevice} handle
         *
         * The @p handle is expected to be originating from @p instance. Unlike
         * with other handle types, the @type_vk{PhysicalDevice} handles don't
         * have to be destroyed at the end. so there's no equivalent of e.g.
         * @ref Instance::release() or @ref Instance::handleFlags().
         */
        static DeviceProperties wrap(Instance& instance, VkPhysicalDevice handle) {
            return DeviceProperties{instance, handle};
        }

        /**
         * @brief Construct without populating the contents
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit DeviceProperties(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        DeviceProperties(const DeviceProperties&) = delete;

        /** @brief Move constructor */
        DeviceProperties(DeviceProperties&&) noexcept;

        ~DeviceProperties();

        /** @brief Copying is not allowed */
        DeviceProperties& operator=(const DeviceProperties&) = delete;

        /** @brief Move assignment */
        DeviceProperties& operator=(DeviceProperties&&) noexcept;

        /** @brief Underlying @type_vk{PhysicalDevice} handle */
        VkPhysicalDevice handle() { return _handle; }
        /** @overload */
        operator VkPhysicalDevice() { return _handle; }

        /**
         * @brief Raw device properties
         *
         * Populated lazily on first request. If Vulkan 1.1 or the
         * @vk_extension{KHR,get_physical_device_properties2} extension is not
         * enabled on the originating instance, only the Vulkan 1.0 subset of
         * device properties is queried, with the `pNext` member being
         * @cpp nullptr @ce.
         * @see @fn_vk_keyword{GetPhysicalDeviceProperties2},
         *      @fn_vk_keyword{GetPhysicalDeviceProperties}
         */
        const VkPhysicalDeviceProperties2& properties();

        /**
         * @brief API version
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Version apiVersion() {
            return Version(properties().properties.apiVersion);
        }

        /**
         * @brief Driver version
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Version driverVersion() {
            return Version(properties().properties.driverVersion);
        }

        /**
         * @brief Device type
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        DeviceType type() {
            return DeviceType(properties().properties.deviceType);
        }

        /**
         * @brief Device name
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Containers::StringView name();

        /**
         * @brief Enumerate device extensions
         * @param layers        Additional layers to list extensions from
         *
         * Expects that all listed layers are supported --- however they don't
         * need to be enabled on the instance.
         * @see @ref LayerProperties::isSupported(),
         *      @fn_vk_keyword{EnumerateDeviceExtensionProperties}
         */
        ExtensionProperties enumerateExtensionProperties(Containers::ArrayView<const Containers::StringView> layers = {});

        /** @overload */
        ExtensionProperties enumerateExtensionProperties(std::initializer_list<Containers::StringView> layers);

        /**
         * @brief Queue family properties
         *
         * Populated lazily on first request. If Vulkan 1.1 or the
         * @vk_extension{KHR,get_physical_device_properties2} extension is not
         * enabled on the originating instance, only the Vulkan 1.0 subset of
         * device properties is queried.
         * @see @fn_vk_keyword{GetPhysicalDeviceQueueFamilyProperties2},
         *      @fn_vk_keyword{GetPhysicalDeviceQueueFamilyProperties}
         */
        Containers::ArrayView<const VkQueueFamilyProperties2> queueFamilyProperties();

        /**
         * @brief Queue family count
         *
         * Convenience access to @ref queueFamilyProperties() internals,
         * populated lazily on first request.
         */
        UnsignedInt queueFamilyCount();

        /**
         * @brief Queue count in given family
         * @param queueFamily   Queue family index, expected to be smaller than
         *      @ref queueFamilyCount()
         *
         * Convenience access to @ref queueFamilyProperties() internals,
         * populated lazily on first request.
         */
        UnsignedInt queueFamilySize(UnsignedInt queueFamily);

        /**
         * @brief Queue family flags
         * @param queueFamily   Queue family index, expected to be smaller than
         *      @ref queueFamilyCount()
         *
         * Convenience access to @ref queueFamilyProperties() internals,
         * populated lazily on first request.
         */
        QueueFlags queueFamilyFlags(UnsignedInt queueFamily);

        /**
         * @brief Pick a queue family satisfying given flags
         * @return Queue family index for use in @ref queueFamilySize(),
         *      @ref queueFamilyFlags() and @ref DeviceCreateInfo::addQueues()
         *
         * Queries queue family properties using @ref queueFamilyProperties()
         * and tries to find the first that contains all @p flags. If it is not
         * found, exits. See @ref tryPickQueueFamily() for an alternative that
         * doesn't exit on failure.
         */
        UnsignedInt pickQueueFamily(QueueFlags flags);

        /**
         * @brief Try to pick a queue family satisfying given flags
         *
         * Compared to @ref pickQueueFamily() the function returns
         * @ref Containers::NullOpt if a desired family isn't found instead of
         * exiting.
         */
        Containers::Optional<UnsignedInt> tryPickQueueFamily(QueueFlags flags);

        /**
         * @brief Device memory properties
         *
         * Populated lazily on first request. If Vulkan 1.1 or the
         * @vk_extension{KHR,get_physical_device_properties2} extension is not
         * enabled on the originating instance, only the Vulkan 1.0 subset of
         * device properties is queried.
         * @see @fn_vk_keyword{GetPhysicalDeviceMemoryProperties2},
         *      @fn_vk_keyword{GetPhysicalDeviceMemoryProperties}
         */
        const VkPhysicalDeviceMemoryProperties2& memoryProperties();

        /**
         * @brief Memory heap count
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        UnsignedInt memoryHeapCount();

        /**
         * @brief Memory heap size
         * @param heap   Memory heap index, expected to be smaller than
         *      @ref memoryHeapCount()
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        UnsignedLong memoryHeapSize(UnsignedInt heap);

        /**
         * @brief Memory heap size
         * @param heap   Memory heap index, expected to be smaller than
         *      @ref memoryHeapCount()
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        MemoryHeapFlags memoryHeapFlags(UnsignedInt heap);

        /**
         * @brief Memory type count
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        UnsignedInt memoryCount();

        /**
         * @brief Memory type flags
         * @param memory    Memory type index, expected to be smaller than
         *      @ref memoryCount()
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        MemoryFlags memoryFlags(UnsignedInt memory);

        /**
         * @brief Memory heap index
         * @param memory    Memory type index, expected to be smaller than
         *      @ref memoryCount()
         * @return Memory heap index, always smaller than @ref memoryHeapCount()
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        UnsignedInt memoryHeapIndex(UnsignedInt memory);

        /**
         * @brief Pick a memory type satisfying given flags
         * @param requiredFlags     Memory flags that should be present in
         *      picked memory type. Can be an empty set, but picking such
         *      memory is probably not very useful.
         * @param preferredFlags    If there's more than one memory type
         *      matching @p requiredFlags, prefer one that has most of these
         *      as well. Defaults to an empty set.
         * @param memories          Bits indicating which memory types should
         *      be considered (bit @cpp 0 @ce indicates memory type @cpp 0 @ce
         *      should be considered etc.). Expected to have at least one bit
         *      of the first @ref memoryCount() bits set, otherwise the
         *      function will always fail. Defaults to all bits set, meaning
         *      all memory types are considered. Corresponds to
         *      @ref MemoryRequirements::memories().
         *
         * Queries memory properties using @ref memoryProperties() and out of
         * memory types set in @p memoryBits tries to find one that contains
         * all @p requiredFlags and most of @p optionalFlags. If it is not
         * found, exits. See @ref tryPickMemory() for an alternative that
         * doesn't exit on failure.
         *
         * @m_class{m-note m-success}
         *
         * @par
         *      The @p preferredFlags can be used for example to ask for a
         *      @ref MemoryFlag::HostVisible bit on a
         *      @ref MemoryFlag::DeviceLocal memory --- on discrete GPUs this
         *      combination is usually not possible so you get just a
         *      device-only memory, but on integrated GPUs it can be used to
         *      avoid a need for a copy through a temporary staging buffer.
         */
        UnsignedInt pickMemory(MemoryFlags requiredFlags, MemoryFlags preferredFlags = {}, UnsignedInt memories = ~UnsignedInt{});

        /**
         * @overload
         *
         * Equivalent to calling @ref pickMemory(MemoryFlags, MemoryFlags, UnsignedInt)
         * with empty @p preferredFlags.
         */
        UnsignedInt pickMemory(MemoryFlags requiredFlags, UnsignedInt memories);

        /**
         * @brief Try to pick a memory type satisfying given flags
         *
         * Compared to @ref pickMemory() the function returns
         * @ref Containers::NullOpt if a desired memory type isn't found
         * instead of exiting.
         */
        Containers::Optional<UnsignedInt> tryPickMemory(MemoryFlags requiredFlags, MemoryFlags preferredFlags = {}, UnsignedInt memories = ~UnsignedInt{});

        /**
         * @overload
         *
         * Equivalent to calling @ref tryPickMemory(MemoryFlags, MemoryFlags, UnsignedInt)
         * with empty @p preferredFlags.
         */
        Containers::Optional<UnsignedInt> tryPickMemory(MemoryFlags requiredFlags, UnsignedInt memories);

    private:
        friend Implementation::InstanceState;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* The DAMN THING lists this among friends, which is AN IMPLEMENTATION
           DETAIL */
        friend DeviceCreateInfo;
        friend MAGNUM_VK_EXPORT Containers::Array<DeviceProperties> enumerateDevices(Instance&);
        #endif

        explicit DeviceProperties(Instance& instance, VkPhysicalDevice handle);

        MAGNUM_VK_LOCAL static void getPropertiesImplementationDefault(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);
        MAGNUM_VK_LOCAL static void getPropertiesImplementationKHR(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);
        MAGNUM_VK_LOCAL static void getPropertiesImplementation11(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);

        MAGNUM_VK_LOCAL static void getQueueFamilyPropertiesImplementationDefault(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties);
        MAGNUM_VK_LOCAL static void getQueueFamilyPropertiesImplementationKHR(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties);
        MAGNUM_VK_LOCAL static void getQueueFamilyPropertiesImplementation11(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties);

        MAGNUM_VK_LOCAL static void getMemoryPropertiesImplementationDefault(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties);
        MAGNUM_VK_LOCAL static void getMemoryPropertiesImplementationKHR(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties);
        MAGNUM_VK_LOCAL static void getMemoryPropertiesImplementation11(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties);

        /* Can't be a reference because of the NoCreate constructor */
        Instance* _instance;

        VkPhysicalDevice _handle;
        struct State;
        Containers::Pointer<State> _state;
};

/**
@brief Enumerate physical devices
@m_since_latest

@see @fn_vk_keyword{EnumeratePhysicalDevices}
*/
MAGNUM_VK_EXPORT Containers::Array<DeviceProperties> enumerateDevices(Instance& instance);

/**
@brief Pick a physical device
@m_since_latest

Calls @ref enumerateDevices() and selects a device based on preferences
specified through the `--magnum-device` @ref Vk-Instance-command-line "command-line option".
If a device is not found, exits. See @ref tryPickDevice() for an alternative
that doesn't exit on failure. See @ref Device for general usage information.
*/
MAGNUM_VK_EXPORT DeviceProperties pickDevice(Instance& instance);

/**
@brief Try to pick a physical device
@m_since_latest

Compared to @ref pickDevice() the function returns @ref Containers::NullOpt if
a device isn't found instead of exiting.
*/
MAGNUM_VK_EXPORT Containers::Optional<DeviceProperties> tryPickDevice(Instance& instance);

}}

#endif
