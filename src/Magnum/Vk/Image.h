#ifndef Magnum_Vk_Image_h
#define Magnum_Vk_Image_h
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
 * @brief Class @ref Magnum::Vk::ImageCreateInfo, @ref Magnum::Vk::ImageCreateInfo1D, @ref Magnum::Vk::ImageCreateInfo2D, @ref Magnum::Vk::ImageCreateInfo3D, @ref Magnum::Vk::ImageCreateInfo1DArray, @ref Magnum::Vk::ImageCreateInfo2DArray, @ref Magnum::Vk::ImageCreateInfoCubeMap, @ref Magnum::Vk::ImageCreateInfoCubeMapArray, @ref Magnum::Vk::Image, enum @ref Magnum::Vk::ImageUsage, enum set @ref Magnum::Vk::ImageUsages
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Vk/Memory.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation { struct DeviceState; }

/**
@brief Image usage
@m_since_latest

@see @ref ImageUsages, @ref ImageCreateInfo
@m_enum_values_as_keywords
*/
enum class ImageUsage: UnsignedInt {
    /** Source of a transfer command */
    TransferSource = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,

    /** Destination of a transfer command */
    TransferDestination = VK_IMAGE_USAGE_TRANSFER_DST_BIT,

    /** Sampled by a shader */
    Sampled = VK_IMAGE_USAGE_SAMPLED_BIT,

    /** Shader storage */
    Storage = VK_IMAGE_USAGE_STORAGE_BIT,

    /** Color attachment */
    ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

    /** Depth/stencil attachment */
    DepthStencilAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,

    /** Transient attachment */
    TransientAttachment = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,

    /** Input attachment in a shader or framebuffer */
    InputAttachment = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
};

/**
@brief Image usages
@m_since_latest

@see @ref ImageCreateInfo
*/
typedef Containers::EnumSet<ImageUsage> ImageUsages;

CORRADE_ENUMSET_OPERATORS(ImageUsages)

/**
@brief Image create info
@m_since_latest

Wraps a @type_vk_keyword{ImageCreateInfo}. See @ref Image for usage
information.
@see @ref ImageCreateInfo1D, @ref ImageCreateInfo2D, @ref ImageCreateInfo3D,
    @ref ImageCreateInfo1DArray, @ref ImageCreateInfo2DArray,
    @ref ImageCreateInfoCubeMap, @ref ImageCreateInfoCubeMapArray
*/
class MAGNUM_VK_EXPORT ImageCreateInfo {
    public:
        /**
         * @brief Command pool flag
         *
         * Wraps @type_vk_keyword{ImageCreateFlagBits}.
         * @see @ref Flags, @ref ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** @todo sparse binding/residency/aliased */

            /** Allow creating a view of different format */
            MutableFormat = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,

            /** Allow creating a cube map view */
            CubeCompatible = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,

            /** @todo alias, 2D array compatible ... (Vulkan 1.1+) */
        };

        /**
         * @brief Command pool flags
         *
         * Type-safe wrapper for @type_vk_keyword{ImageCreateFlags}.
         * @see @ref ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param type      Image type
         * @param usages    Desired image usage. At least one flag is required.
         * @param format    Image format
         * @param size      Image size
         * @param layers    Array layer count
         * @param levels    Mip level count
         * @param samples   Sample count
         * @param flags     Image creation flags
         *
         * The following @type_vk{ImageCreateInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `imageType` to @p type
         * -    `format`
         * -    `extent` to @p size
         * -    `mipLevels` to @p levels
         * -    `arrayLayers` to @p layers
         * -    `samples`
         * -    `tiling` to @val_vk{IMAGE_TILING_OPTIMAL,ImageTiling}
         * -    `usage` to @p usages
         * -    `sharingMode` to @val_vk{SHARING_MODE_EXCLUSIVE,SharingMode}
         * -    `initialLayout` to @val_vk{IMAGE_LAYOUT_UNDEFINED,ImageLayout}
         *
         * There are various restrictions on @p size, @p layers, @p levels for
         * a particular @p type --- for common image types you're encouraged to
         * make use of @ref ImageCreateInfo1D, @ref ImageCreateInfo2D,
         * @ref ImageCreateInfo3D, @ref ImageCreateInfo1DArray,
         * @ref ImageCreateInfo2DArray, @ref ImageCreateInfoCubeMap and
         * @ref ImageCreateInfoCubeMapArray convenience classes instead of
         * this constructor.
         */
        explicit ImageCreateInfo(VkImageType type, ImageUsages usages, VkFormat format, const Vector3i& size, Int layers, Int levels, Int samples = 1, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit ImageCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit ImageCreateInfo(const VkImageCreateInfo& info);

        /** @brief Underlying @type_vk{ImageCreateInfo} structure */
        VkImageCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkImageCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkImageCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkImageCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkImageCreateInfo*() const { return &_info; }

    private:
        VkImageCreateInfo _info;
};

CORRADE_ENUMSET_OPERATORS(ImageCreateInfo::Flags)

/**
@brief Convenience constructor for 1D images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_1D,ImageType} with the last two `size` components and
`layers` set to @cpp 1 @ce.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo1D: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo1D(ImageUsages usages, VkFormat format, Int size, Int levels, Int samples = 1, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_1D, usages, format, {size, 1, 1}, 1, levels, samples, flags} {}
};

/**
@brief Convenience constructor for 2D images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_2D,ImageType} with the last `size` component and `layers`
set to @cpp 1 @ce.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo2D: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo2D(ImageUsages usages, VkFormat format, const Vector2i& size, Int levels, Int samples = 1, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size, 1}, 1, levels, samples, flags} {}
};

/**
@brief Convenience constructor for 3D images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_3D,ImageType} with `layers` set to @cpp 1 @ce.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo3D: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo3D(ImageUsages usages, VkFormat format, const Vector3i& size, Int levels, Int samples = 1, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_3D, usages, format, size, 1, levels, samples, flags} {}
};

/**
@brief Convenience constructor for 1D array images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_1D,ImageType} with the last two `size` components set to
@cpp 1 @ce and `layers` set to @cpp size.y() @ce.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo1DArray: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo1DArray(ImageUsages usages, VkFormat format, const Vector2i& size, Int levels, Int samples = 1, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_1D, usages, format, {size.x(), 1, 1}, size.y(), levels, samples, flags} {}
};

/**
@brief Convenience constructor for 2D array images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_2D,ImageType} with the last `size` component set to
@cpp 1 @ce and `layers` set to @cpp size.z() @ce.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo2DArray: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo2DArray(ImageUsages usages, VkFormat format, const Vector3i& size, Int levels, Int samples = 1, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size.xy(), 1}, size.z(), levels, samples, flags} {}
};

/**
@brief Convenience constructor for cube map images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_2D,ImageType} with the last `size` component set to
@cpp 1 @ce, `layers` set to @cpp 6 @ce and `flags` additionally having
@ref Flag::CubeCompatible.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfoCubeMap: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfoCubeMap(ImageUsages usages, VkFormat format, const Vector2i& size, Int levels, Int samples = 1, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size, 1}, 6, levels, samples, flags|Flag::CubeCompatible} {}
};

/**
@brief Convenience constructor for cube map array images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_2D,ImageType} with the last `size` component set to
@cpp 1 @ce, `layers` set to @cpp size.y() @ce and `flags` additionally having
@ref Flag::CubeCompatible.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, VkFormat, const Vector3i&, Int, Int, Int, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfoCubeMapArray: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfoCubeMapArray(ImageUsages usages, VkFormat format, const Vector3i& size, Int levels, Int samples = 1, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size.xy(), 1}, size.z(), levels, samples, flags|Flag::CubeCompatible} {}
};

/**
@brief Image
@m_since_latest

Wraps a @type_vk{Image}.
*/
class MAGNUM_VK_EXPORT Image {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device    Vulkan device the image is created on
         * @param handle    The @type_vk{Image} handle
         * @param flags     Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * an image created using a constructor, the Vulkan image is by default
         * not deleted on destruction, use @p flags for different behavior.
         * @see @ref release()
         */
        static Image wrap(Device& device, VkImage handle, HandleFlags flags = {});

        /**
         * @brief Construct an image without allocating
         * @param device    Vulkan device to create the image on
         * @param info      Image creation info
         *
         * @see @fn_vk_keyword{CreateImage}
         */
        explicit Image(Device& device, const ImageCreateInfo& info, NoAllocateT);

        /**
         * @brief Construct without creating the image
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Image(NoCreateT);

        /** @brief Copying is not allowed */
        Image(const Image&) = delete;

        /** @brief Move constructor */
        Image(Image&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Image} handle, unless the instance
         * was created using @ref wrap() without @ref HandleFlag::DestroyOnDestruction
         * specified.
         * @see @fn_vk_keyword{DestroyImage}, @ref release()
         */
        ~Image();

        /** @brief Copying is not allowed */
        Image& operator=(const Image&) = delete;

        /** @brief Move assignment */
        Image& operator=(Image&& other) noexcept;

        /** @brief Underlying @type_vk{Image} handle */
        VkImage handle() { return _handle; }
        /** @overload */
        operator VkImage() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Image memory requirements
         *
         * @see @fn_vk_keyword{GetImageMemoryRequirements2},
         *      @fn_vk_keyword{GetImageMemoryRequirements}
         */
        MemoryRequirements memoryRequirements() const;

        /**
         * @brief Bind image memory
         *
         * Assumes that @p memory type, the amount of @p memory at @p offset
         * and @p offset alignment corresponds to image memory requirements.
         * @see @ref bindDedicatedMemory(), @fn_vk_keyword{BindImageMemory2},
         *      @fn_vk_keyword{BindImageMemory}
         */
        void bindMemory(Memory& memory, UnsignedLong offset);

        /**
         * @brief Bind a dedicated image memory
         *
         * Equivalent to @ref bindMemory() with @p offset set to @cpp 0 @ce,
         * with the additional effect that @p memory ownership transfers to the
         * image and is then available through @ref dedicatedMemory().
         */
        void bindDedicatedMemory(Memory&& memory);

        /**
         * @brief Whether the image has a dedicated memory
         *
         * Returns @cpp true @ce if the image memory was bound using
         * @ref bindDedicatedMemory(), @cpp false @ce otherwise.
         * @see @ref dedicatedMemory()
         */
        bool hasDedicatedMemory() const;

        /**
         * @brief Dedicated image memory
         *
         * Expects that the image has a dedicated memory.
         * @see @ref hasDedicatedMemory()
         */
        Memory& dedicatedMemory();

        /**
         * @brief Release the underlying Vulkan image
         *
         * Releases ownership of the Vulkan image and returns its handle so
         * @fn_vk{DestroyImage} is not called on destruction. The internal
         * state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkImage release();

    private:
        friend Implementation::DeviceState;

        MAGNUM_VK_LOCAL static void getMemoryRequirementsDefault(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);
        MAGNUM_VK_LOCAL static void getMemoryRequirementsKHR(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);
        MAGNUM_VK_LOCAL static void getMemoryRequirements11(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);

        MAGNUM_VK_LOCAL static void bindMemoryImplementationDefault(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* infos);
        MAGNUM_VK_LOCAL static void bindMemoryImplementationKHR(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* infos);
        MAGNUM_VK_LOCAL static void bindMemoryImplementation11(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* infos);

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkImage _handle;
        HandleFlags _flags;
        Memory _dedicatedMemory;
};

}}

#endif
