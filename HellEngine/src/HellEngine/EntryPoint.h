#pragma once

#ifdef HELL_PLATFORM_WINDOWS

extern HellEngine::Application* HellEngine::CreateApplication();

int main(int argc, char** argv)
{
	HellEngine::Log::Init();
	printf("Welcome to Hell\n\n");
	HELL_CORE_WARN("Welcome To Hell");
	HELL_INFO("Welcome to Hell");

	auto app = HellEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif
