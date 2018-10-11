#pragma once

#include "Module.h"

class ModuleWindow : public Module
{
public:

	// Virtual functions

	bool start() override;

	bool preUpdate() override;

	bool postUpdate() override;

	bool cleanUp() override;

private:

	int width = 600;
	int height = 800;

public:

	int GetWidth() { return width; }
	int GetHeight() { return height; }

};