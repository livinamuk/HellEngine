#pragma once

#ifdef HELL_ENGINE_PLATFORM_WINDOWS

extern HellEngine::Application* HellEngine::CreateApplication();

int main(int argc, char** argv)
{
	printf("Welcome to Hell\n");
	auto app = HellEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif
