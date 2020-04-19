#pragma once

namespace RoMan
{
	using RendererID = uint32_t;

	struct RenderAPICapabilities
	{
		std::string Vendor;
		std::string Renderer;
		std::string Version;

		int MaxSamples;
		float MaxAnisotropy;
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		static void Init();
		static void ShutDown();

		static void SetClearColor(float r, float g, float b, float a);
		static void Clear(float r, float g, float b, float a);

		static void DrawIndexed(unsigned int count, bool depthTest = true);

		static RenderAPICapabilities& GetCapabilities()
		{
			static RenderAPICapabilities capabilities;
			return capabilities;
		}

		inline static API GetAPI() { return s_API; }

	private:
		static void LoadRequiredAssets();
	private:
		static API s_API;
	};
}