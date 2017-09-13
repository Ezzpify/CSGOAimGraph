#pragma once

extern struct globalVars {
	float realTime;
	int frameCount;
	float absFrameTime;

	float unknown;
	float curTime;
	float frameTime;
	int maxClients;
	int tickCount;
	float intervalPerTick;
	float interpAmount;
	int simTicksThisFrame;
} *globals;