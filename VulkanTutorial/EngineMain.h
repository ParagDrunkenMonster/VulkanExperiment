#ifndef __EngineMain_h__
#define __EngineMain_h__

#include "MyWindow.h"
#include "EngineDevice.h"
#include "Mesh.h"
#include "Renderer.h"

#include <memory>
#include <vector>
#include "GameObject.h"

namespace VulkanTutorial
{
	class EngineMain
	{
	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		EngineMain();
		virtual ~EngineMain();

		EngineMain(const EngineMain&) = delete;
		EngineMain& operator = (const EngineMain&) = delete;

		EngineMain(EngineMain&&) = delete;
		EngineMain& operator = (EngineMain&&) = delete;

		void Run();

	private:
		void LoadGameObjects();

		MyWindow m_MyWindow = MyWindow("My Window", WIDTH, HEIGHT);
		EngineDevice m_EngineDevice = EngineDevice(m_MyWindow);
		Renderer m_Renderer = Renderer(m_MyWindow, m_EngineDevice);

		std::vector<GameObject> m_GameObjects;
	};
}

#endif //__EngineMain_h__
