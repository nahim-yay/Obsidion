
#pragma once

#include <iostream>

#include "vertex.h"
#include "shader.h"



namespace jvk
{
	const std::string sky_vertex_shader   = "res/shaders/sky_vert.spv";
	const std::string sky_fragment_shader = "res/shaders/sky_frag.spv";





    static vk::VertexInputBindingDescription get_sky_binding_description()
    {
        vk::VertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = vk::VertexInputRate::eVertex;


        return binding_description;
    }


    static std::array<vk::VertexInputAttributeDescription, 2> get_sky_attribute_descriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 2> attribute_descriptions{};
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attribute_descriptions[0].offset = offsetof(Vertex, position);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 2;
        attribute_descriptions[1].format = vk::Format::eR32G32Sfloat;
        attribute_descriptions[1].offset = offsetof(Vertex, tex_coord);


        return attribute_descriptions;
    }





	void create_sky_descriptorset_layout()
	{
        vk::DescriptorSetLayoutBinding ubo_layout_binding{};
        ubo_layout_binding.binding = 0;
        ubo_layout_binding.descriptorCount = 1;
        ubo_layout_binding.descriptorType = vk::DescriptorType::eUniformBuffer;
        ubo_layout_binding.pImmutableSamplers = nullptr;
        ubo_layout_binding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutBinding sampler_layout_binding{};
        sampler_layout_binding.binding = 1;
        sampler_layout_binding.descriptorCount = 1;
        sampler_layout_binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        sampler_layout_binding.pImmutableSamplers = nullptr;
        sampler_layout_binding.stageFlags = vk::ShaderStageFlagBits::eFragment;


        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {ubo_layout_binding, sampler_layout_binding};


		vk::DescriptorSetLayoutCreateInfo layout_info{};
		layout_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
		layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
		layout_info.pBindings = bindings.data();

		try
		{
			sky_descriptorset_layout = logical_device.createDescriptorSetLayout(layout_info);
		}
		catch(vk::SystemError err)
		{
		    throw std::runtime_error("failed to create descriptor set layout!");
		}
	}


    void create_sky_descriptor_pool()
    {
        vk::DescriptorPoolSize pool_size{};
        pool_size.type = vk::DescriptorType::eUniformBuffer;
        pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        vk::DescriptorPoolCreateInfo pool_info{};
        pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
        pool_info.poolSizeCount = 1;
        pool_info.pPoolSizes = &pool_size;
        pool_info.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);



		sky_descriptor_pool = logical_device.createDescriptorPool(pool_info);
            // throw std::runtime_error("failed to create descriptor pool!");
    }

    void create_sky_descriptor_sets()
    {
        std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, sky_descriptorset_layout);
        vk::DescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
        alloc_info.descriptorPool = sky_descriptor_pool;
        alloc_info.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        alloc_info.pSetLayouts = layouts.data();

        sky_descriptor_sets.resize(MAX_FRAMES_IN_FLIGHT);

    	if (logical_device.allocateDescriptorSets(&alloc_info, sky_descriptor_sets.data()) != vk::Result::eSuccess)
        {
	        throw std::runtime_error("failed to allocate descriptor sets!");
		}

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::DescriptorBufferInfo buffer_info{};
            buffer_info.buffer = sky_uniform_buffers[i];
            buffer_info.offset = 0;
            buffer_info.range = sizeof(SceneData);

            vk::DescriptorImageInfo image_info{};
            image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            image_info.imageView = texture_image_view;
            image_info.sampler = texture_sampler;

            

            std::array<vk::WriteDescriptorSet, 2> descriptor_writes{};

            descriptor_writes[0].sType = vk::StructureType::eWriteDescriptorSet;
            descriptor_writes[0].dstSet = sky_descriptor_sets[i];
            descriptor_writes[0].dstBinding = 0;
            descriptor_writes[0].dstArrayElement = 0;
            descriptor_writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptor_writes[0].descriptorCount = 1;
            descriptor_writes[0].pBufferInfo = &buffer_info;

            descriptor_writes[1].sType = vk::StructureType::eWriteDescriptorSet;
            descriptor_writes[1].dstSet = sky_descriptor_sets[i];
            descriptor_writes[1].dstBinding = 1;
            descriptor_writes[1].dstArrayElement = 0;
            descriptor_writes[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptor_writes[1].descriptorCount = 1;
            descriptor_writes[1].pBufferInfo = &buffer_info;

            logical_device.updateDescriptorSets(static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
        }
    }







	// void create_sky_renderpass()
	// {
	// 	vk::AttachmentDescription color_attachment{};
	// 	color_attachment.format = swapchain_image_format;
	// 	color_attachment.samples = vk::SampleCountFlagBits::e1;
	// 	color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
	// 	color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
	// 	color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	// 	color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	// 	color_attachment.initialLayout = vk::ImageLayout::eUndefined;
	// 	color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	// 	vk::AttachmentReference color_attachment_ref{};
	// 	color_attachment_ref.attachment = 0;
	// 	color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;



	// 	vk::SubpassDescription subpass{};
	// 	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
 //        subpass.colorAttachmentCount = 1;
 //        subpass.pColorAttachments = &color_attachment_ref;
 //        subpass.pDepthStencilAttachment = &depth_attachment_ref;


 //        vk::SubpassDependency color_dependency{};
 //        color_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
 //        color_dependency.dstSubpass = 0;
 //        color_dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
 //        color_dependency.srcAccessMask = {};
 //        color_dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
 //        color_dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;



	// 	vk::RenderPassCreateInfo renderpass_info{};
	// 	renderpass_info.sType = vk::StructureType::eRenderPassCreateInfo;
	// 	renderpass_info.attachmentCount = 1;
	// 	renderpass_info.pAttachments = &color_attachment;
	// 	renderpass_info.subpassCount = 1;
	// 	renderpass_info.pSubpasses = &subpass;
	// 	renderpass_info.dependencyCount = 1;
	// 	renderpass_info.pDependencies = &color_dependency;


	// 	try
	// 	{
	// 		render_pass = logical_device.createRenderPass(renderpass_info);
	// 	}
	// 	catch(vk::SystemError err)
	// 	{
	// 		throw std::runtime_error("failed to create render pass!");
	// 	}
	// }





	void create_sky_graphics_pipeline()
	{
		auto vert_shader_code = read_file(sky_vertex_shader);
		auto frag_shader_code = read_file(sky_fragment_shader);

		vk::ShaderModule vert_shader_module = create_shader_module(vert_shader_code);
		vk::ShaderModule frag_shader_module = create_shader_module(frag_shader_code);

		vk::PipelineShaderStageCreateInfo vert_shader_stage_info{};
		vert_shader_stage_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
		vert_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
		vert_shader_stage_info.module = vert_shader_module;
		vert_shader_stage_info.pName = "main";

		vk::PipelineShaderStageCreateInfo frag_shader_stage_info{};
		frag_shader_stage_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
		frag_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
		frag_shader_stage_info.module = frag_shader_module;
		frag_shader_stage_info.pName = "main";

		vk::PipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };



		std::vector<vk::DynamicState> dynamic_states =
		{
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		vk::PipelineDynamicStateCreateInfo dynamic_state{};
		dynamic_state.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
		dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		dynamic_state.pDynamicStates = dynamic_states.data();

		auto binding_description = get_sky_binding_description();
		auto attribute_descriptions = get_sky_attribute_descriptions();

		vk::PipelineVertexInputStateCreateInfo vertex_input_info{};
		vertex_input_info.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
		vertex_input_info.vertexBindingDescriptionCount = 1;
		vertex_input_info.pVertexBindingDescriptions = &binding_description; // Optional
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data(); // Optional

		vk::PipelineInputAssemblyStateCreateInfo input_assembly{};
		input_assembly.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
		input_assembly.topology = vk::PrimitiveTopology::eTriangleList;
		input_assembly.primitiveRestartEnable = VK_FALSE;



		vk::PipelineViewportStateCreateInfo viewport_state{};
		viewport_state.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
		viewport_state.viewportCount = 1;
		// viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		// viewport_state.pScissors = &scissor;

		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


		vk::PipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		vk::PipelineColorBlendAttachmentState color_blend_attachment{};
		color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		color_blend_attachment.blendEnable = VK_FALSE;
		color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
		color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
		color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
		color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
		color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
		color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional


		color_blend_attachment.blendEnable = VK_TRUE;
		color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
		color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
		color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
		color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;

		vk::PipelineColorBlendStateCreateInfo color_blending{};
		color_blending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = vk::LogicOp::eCopy; // Optional
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &color_blend_attachment;
		color_blending.blendConstants[0] = 0.0f; // Optional
		color_blending.blendConstants[1] = 0.0f; // Optional
		color_blending.blendConstants[2] = 0.0f; // Optional
		color_blending.blendConstants[3] = 0.0f; // Optional


		vk::PipelineLayoutCreateInfo pipeline_layout_info{};
		pipeline_layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
		pipeline_layout_info.setLayoutCount = 1; // Optional
		pipeline_layout_info.pSetLayouts = &sky_descriptorset_layout; // Optional
		pipeline_layout_info.pushConstantRangeCount = 1; // Optional

		vk::PushConstantRange push_constant_info;
		push_constant_info.offset = 0;
		push_constant_info.size = sizeof(MeshData);
		push_constant_info.stageFlags = vk::ShaderStageFlagBits::eVertex;

		pipeline_layout_info.pPushConstantRanges = &push_constant_info; // Optional


		// Create Pipeline Layout
		try
		{
			pipeline_layout = logical_device.createPipelineLayout(pipeline_layout_info);
		}
		catch(vk::SystemError err)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}


		// DEPTH STENCIL INFO
        vk::PipelineDepthStencilStateCreateInfo depth_stencil_info = {};
        depth_stencil_info.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
        depth_stencil_info.pNext = nullptr;
        depth_stencil_info.depthTestEnable = VK_FALSE;
        depth_stencil_info.depthWriteEnable = VK_FALSE;
        depth_stencil_info.depthCompareOp = vk::CompareOp::eLessOrEqual;
        depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_info.minDepthBounds = 0.0f; // Optional
        depth_stencil_info.maxDepthBounds = 1.0f; // Optional
        depth_stencil_info.stencilTestEnable = VK_FALSE;



		// Graphics Pipeline Info
		vk::GraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pDepthStencilState = &depth_stencil_info;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.layout = pipeline_layout;
		pipeline_info.renderPass = render_pass;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipeline_info.basePipelineIndex = -1; // Optional



		// Create The Graphics Pipeline
		try
		{
			pipeline = logical_device.createGraphicsPipeline(nullptr, pipeline_info).value;
		}
		catch(vk::SystemError err)
		{
		    throw std::runtime_error("failed to create graphics pipeline!");
		}



		// Destroy Shader Modules
		logical_device.destroyShaderModule(frag_shader_module);
		logical_device.destroyShaderModule(vert_shader_module);
	}
}