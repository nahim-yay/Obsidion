

#pragma once


#include <iostream>

#include "includes.h"
#include "cubemap.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"





namespace jvk
{
    void transition_image_layout(vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout)
    {
        vk::CommandBuffer command_buffer = begin_single_time_commands();

        vk::ImageMemoryBarrier barrier{};
        barrier.sType = vk::StructureType::eImageMemoryBarrier;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 6;

        vk::PipelineStageFlags source_stage;
        vk::PipelineStageFlags destination_stage;

        if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
            destination_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            source_stage = vk::PipelineStageFlagBits::eTransfer;
            destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        command_buffer.pipelineBarrier(
            source_stage, destination_stage,
            {},
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        end_single_time_commands(command_buffer);
    }






    void copy_buffer_to_image(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
    {
        vk::CommandBuffer command_buffer = begin_single_time_commands();

        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 6;
        region.imageOffset = { { 0, 0, 0 } };
        region.imageExtent = { { width, height, 1 } };

        command_buffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        end_single_time_commands(command_buffer);
    }






    vk::ImageView create_image_view(vk::Image image, vk::Format format)
    {
        vk::ImageViewCreateInfo view_info{};
        view_info.sType = vk::StructureType::eImageViewCreateInfo;
        view_info.image = image;
        view_info.viewType = vk::ImageViewType::eCube;
        view_info.format = format;
        view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 6;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        vk::ImageView image_view;
        image_view = logical_device.createImageView(view_info);

        return image_view;
    }

    void create_image(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk:: Image &image, vk::DeviceMemory &image_memory)
    {
        vk::ImageCreateInfo image_info{};
        image_info.sType = vk::StructureType::eImageViewCreateInfo;
        image_info.imageType = vk::ImageType::e2D;
        image_info.extent.width = width;
        image_info.extent.height = height;
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 6;
        image_info.format = format;
        image_info.tiling = tiling;
        image_info.initialLayout = vk::ImageLayout::eUndefined;
        image_info.usage = usage;
        image_info.samples = vk::SampleCountFlagBits::e1;
        image_info.sharingMode = vk::SharingMode::eExclusive;
        image_info.flags = vk::ImageCreateFlagBits::eCubeCompatible;

        image = logical_device.createImage(image_info);

        // ALLOCATE MEMORY
        vk::MemoryRequirements mem_requirements;
        logical_device.getImageMemoryRequirements(image, &mem_requirements);

        vk::MemoryAllocateInfo alloc_info{};
        alloc_info.sType = vk::StructureType::eMemoryAllocateInfo;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

        if (logical_device.allocateMemory(&alloc_info, nullptr, &image_memory) != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        logical_device.bindImageMemory(image, image_memory, 0);
    }







    void load_texture_image(char **file, int width, int height, int channels, void *pixels)
    {
        pixels = stbi_load(*file, &width, &height, &channels, STBI_rgb_alpha);

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }
    }


    void create_cubemap(CubeMap &cubemap)
    {
    	load_texture_image(cubemap.faces + 0, cubemap.width, cubemap.height, cubemap.channels, &cubemap.texture_data[0]);
    	load_texture_image(cubemap.faces + 1, cubemap.width, cubemap.height, cubemap.channels, &cubemap.texture_data[1]);
    	load_texture_image(cubemap.faces + 2, cubemap.width, cubemap.height, cubemap.channels, &cubemap.texture_data[2]);
    	load_texture_image(cubemap.faces + 3, cubemap.width, cubemap.height, cubemap.channels, &cubemap.texture_data[3]);
    	load_texture_image(cubemap.faces + 4, cubemap.width, cubemap.height, cubemap.channels, &cubemap.texture_data[4]);
    	load_texture_image(cubemap.faces + 5, cubemap.width, cubemap.height, cubemap.channels, &cubemap.texture_data[5]);



        //Calculate the image size and the layer size.
        const vk::DeviceSize image_size = cubemap.width * cubemap.height * 4 * 6; //4 since I always load my textures with an alpha channel, and multiply it by 6 because the image must have 6 layers.
        const vk::DeviceSize layer_size = image_size / 6; //This is just the size of each layer.

        //Set up the staging buffer.
        vk::Buffer staging_buffer;
        vk::DeviceMemory staging_buffer_memory;

        //Create the staging buffer.
        create_buffer(
        	image_size,
        	vk::BufferUsageFlagBits::eTransferSrc,
        	vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        	staging_buffer,
        	staging_buffer_memory
        );

        //Map the memory.
        void* data;
        if (logical_device.mapMemory(staging_buffer_memory, 0, image_size, {}, &data) != vk::Result::eSuccess)
        {
            std::cout << "Map Memory Failed!\n";
        }
        //Copy the data into the staging buffer.
        memcpy(data, cubemap.texture_data, image_size);
        logical_device.unmapMemory(staging_buffer_memory);

        for (int i = 0; i < 6; i++)
        {
            stbi_image_free(cubemap.texture_data[i]);
        }


        create_image(
            cubemap.width, cubemap.height,
            vk::Format::eR8G8B8A8Srgb,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            texture_image,
            texture_image_memory);

        transition_image_layout(texture_image, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        copy_buffer_to_image(staging_buffer, texture_image, cubemap.width, cubemap.height);
        transition_image_layout(texture_image, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        logical_device.destroyBuffer(staging_buffer, nullptr);
        logical_device.freeMemory(staging_buffer_memory, nullptr);
    }





    void create_texture_sampler()
    {
        vk::PhysicalDeviceProperties properties = physical_device.getProperties();

        vk::SamplerCreateInfo sampler_info{};
        sampler_info.sType = vk::StructureType::eSamplerCreateInfo;
        sampler_info.magFilter = vk::Filter::eLinear;
        sampler_info.minFilter = vk::Filter::eLinear;
        sampler_info.addressModeU = vk::SamplerAddressMode::eRepeat;
        sampler_info.addressModeV = vk::SamplerAddressMode::eRepeat;
        sampler_info.addressModeW = vk::SamplerAddressMode::eRepeat;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = vk::CompareOp::eAlways;
        sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;

        texture_sampler = logical_device.createSampler(sampler_info);
    }




    void create_texture_image_view()
    {
        texture_image_view = create_image_view(texture_image, vk::Format::eR8G8B8A8Srgb);
    }
}