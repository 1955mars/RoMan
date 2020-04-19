#pragma once

#include "RenderCommandQueue.h"
#include "RendererAPI.h"

#include "Camera.h"

namespace RoMan
{
	class ShaderLibrary;

	class Renderer
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		//Commands
		static void Clear();
		static void Clear(float r, float g, float b, float a = 1.0f);
		static void SetClearColor(float r, float g, float b, float a);

		static void DrawIndexed(uint32_t count, bool depthTest = true);

		static void ClearMagenta();

		static void Init();

		static const Scope<ShaderLibrary>& GetShaderLibrary() { return Get().m_ShaderLibrary; }

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void* Submit(RenderCommandFn fn, unsigned int size)
		{
			return s_Instance->m_CommandQueue.Allocate(fn, size);
		}

		void WaitAndRender();

		//static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		inline static Renderer& Get() { return *s_Instance; }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;

		static Renderer* s_Instance;

		RenderCommandQueue m_CommandQueue;
		Scope<ShaderLibrary> m_ShaderLibrary;
	};
}


#define RM_RENDER_PASTE2(a, b) a ## b
#define RM_RENDER_PASTE(a, b) RM_RENDER_PASTE2(a, b)
#define RM_RENDER_UNIQUE(x) RM_RENDER_PASTE(x, __LINE__)

#define RM_RENDER(code) \
    struct RM_RENDER_UNIQUE(RMRenderCommand) \
    {\
        static void Execute(void*)\
        {\
            code\
        }\
    };\
	{\
		auto mem = ::RoMan::Renderer::Submit(RM_RENDER_UNIQUE(RMRenderCommand)::Execute, sizeof(RM_RENDER_UNIQUE(RMRenderCommand)));\
		new (mem) RM_RENDER_UNIQUE(RMRenderCommand)();\
	}\

#define RM_RENDER_1(arg0, code) \
	do {\
    struct RM_RENDER_UNIQUE(RMRenderCommand) \
    {\
		RM_RENDER_UNIQUE(RMRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0) \
		: arg0(arg0) {}\
		\
        static void Execute(void* argBuffer)\
        {\
			auto& arg0 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg0;\
            code\
        }\
		\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
    };\
	{\
		auto mem = ::RoMan::Renderer::Submit(RM_RENDER_UNIQUE(RMRenderCommand)::Execute, sizeof(RM_RENDER_UNIQUE(RMRenderCommand)));\
		new (mem) RM_RENDER_UNIQUE(RMRenderCommand)(arg0);\
	} } while(0)

#define RM_RENDER_2(arg0, arg1, code) \
    struct RM_RENDER_UNIQUE(RMRenderCommand) \
    {\
		RM_RENDER_UNIQUE(RMRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1) \
		: arg0(arg0), arg1(arg1) {}\
		\
        static void Execute(void* argBuffer)\
        {\
			auto& arg0 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg0;\
			auto& arg1 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg1;\
            code\
        }\
		\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;\
    };\
	{\
		auto mem = ::RoMan::Renderer::Submit(RM_RENDER_UNIQUE(RMRenderCommand)::Execute, sizeof(RM_RENDER_UNIQUE(RMRenderCommand)));\
		new (mem) RM_RENDER_UNIQUE(RMRenderCommand)(arg0, arg1);\
	}\

#define RM_RENDER_3(arg0, arg1, arg2, code) \
    struct RM_RENDER_UNIQUE(RMRenderCommand) \
    {\
		RM_RENDER_UNIQUE(RMRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2) \
		: arg0(arg0), arg1(arg1), arg2(arg2) {}\
		\
        static void Execute(void* argBuffer)\
        {\
			auto& arg0 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg0;\
			auto& arg1 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg1;\
			auto& arg2 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg2;\
            code\
        }\
		\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2;\
    };\
	{\
		auto mem = ::RoMan::Renderer::Submit(RM_RENDER_UNIQUE(RMRenderCommand)::Execute, sizeof(RM_RENDER_UNIQUE(RMRenderCommand)));\
		new (mem) RM_RENDER_UNIQUE(RMRenderCommand)(arg0, arg1, arg2);\
	}\

#define RM_RENDER_4(arg0, arg1, arg2, arg3, code) \
    struct RM_RENDER_UNIQUE(RMRenderCommand) \
    {\
		RM_RENDER_UNIQUE(RMRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg3)>::type>::type arg3)\
		: arg0(arg0), arg1(arg1), arg2(arg2), arg3(arg3) {}\
		\
        static void Execute(void* argBuffer)\
        {\
			auto& arg0 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg0;\
			auto& arg1 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg1;\
			auto& arg2 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg2;\
			auto& arg3 = ((RM_RENDER_UNIQUE(RMRenderCommand)*)argBuffer)->arg3;\
            code\
        }\
		\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg3)>::type>::type arg3;\
    };\
	{\
		auto mem = Renderer::Submit(RM_RENDER_UNIQUE(RMRenderCommand)::Execute, sizeof(RM_RENDER_UNIQUE(RMRenderCommand)));\
		new (mem) RM_RENDER_UNIQUE(RMRenderCommand)(arg0, arg1, arg2, arg3);\
	}

#define RM_RENDER_S(code) auto self = this;\
	RM_RENDER_1(self, code)

#define RM_RENDER_S1(arg0, code) auto self = this;\
	RM_RENDER_2(self, arg0, code)

#define RM_RENDER_S2(arg0, arg1, code) auto self = this;\
	RM_RENDER_3(self, arg0, arg1, code)

#define RM_RENDER_S3(arg0, arg1, arg2, code) auto self = this;\
	RM_RENDER_4(self, arg0, arg1, arg2, code)