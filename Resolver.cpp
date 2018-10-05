#include "Resolver.h"
#include "Ragebot.h"
#include "Hooks.h"
#include "LagComp.h"
#include "Render.h"
int C_BaseEntity::GetSequenceActivity(int sequence) {
	auto hdr = g_ModelInfo->GetStudiomodel(this->GetModel()); if (!hdr) return -1;
	static auto getSequenceActivity = (DWORD)(U::FindPatternSig("client_panorama.dll", "55 8B EC 83 7D 08 FF 56 8B F1 74"));
	static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(getSequenceActivity);
	return GetSequenceActivity(this, hdr, sequence);
}
bool InAir(C_BaseEntity* pEntity)
{
	if (!pEntity->GetFlags() & FL_ONGROUND) {
		return true;
	}
	return false;
}
bool Fakewalking(C_BaseEntity* pEntity)
{
	for (int w = 0; w < 13; w++) {
		AnimationLayer prevlayer;
		AnimationLayer currentLayer = pEntity->GetAnimOverlay(w);
		const int activity = pEntity->GetSequenceActivity(currentLayer.m_nSequence);
		if (activity == 981 && currentLayer.m_flWeight == 1.f) 
			return true;
		prevlayer = currentLayer;
		return false;
	}
	return false;
}
bool Over120(C_BaseEntity* pEntity)
{
	for (int w = 0; w < 13; w++) {
		AnimationLayer prevlayer;
		AnimationLayer currentLayer = pEntity->GetAnimOverlay(w);
		const int activity = pEntity->GetSequenceActivity(currentLayer.m_nSequence);
		if (activity == 979) {
			if ((prevlayer.m_flCycle != currentLayer.m_flCycle) || currentLayer.m_flWeight == 1.f)
			return true;
		}
		prevlayer = currentLayer;
		return false;
	}
	return false;
}
void Resolver::ResolveYaw(C_BaseEntity* pEntity)
{
	static float PlayerYaw[64];
	static float MovingLBY[64];
	auto Index = pEntity->GetIndex();
	auto Velocity = pEntity->GetVelocity().Length2D();
	PlayerYaw[Index] = pEntity->GetEyeAngles()->y;
	if (g_Options.Ragebot.Resolver)
	{
		// moving w/o fakewalk
		if (Velocity > 1 && !Fakewalking(pEntity) && !InAir(pEntity)) {
			PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget();
			MovingLBY[Index] = *pEntity->GetLowerBodyYawTarget();
		}
		// fakewalking 
		else if (Fakewalking(pEntity) && !InAir(pEntity) && Velocity > 20 && Velocity < 50) {
			switch (Globals::missedshots % 5)
			{
			case 0: PlayerYaw[Index] = MovingLBY[Index]; break;
			case 1: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget(); break;
			case 2: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 180; break;
			case 3: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 90; break;
			case 4: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 130; break;
			case 5: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 45; break;
			case 6: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 180; break;
			case 7: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 90; break;
			case 8: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 90; break;
			case 9: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 135; break;
			case 10: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 135; break;
			case 11: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 125; break;
			case 12: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 115; break;
			case 13: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 125; break;
			case 14: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 90; break;
			case 15: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 75; break;


			}
		}
		// breaking over 120
		else if (!InAir(pEntity) && Velocity == 0 && Over120(pEntity)) {
			switch (Globals::missedshots % 5)
			{
			case 0: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 180; break;
			case 1: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 150; break;
			case 2: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 150; break;
			case 3: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 135; break;
			case 4: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 135; break;
			}
		}
		// breaking under 120 / not breaking
		else if (!InAir(pEntity) && Velocity == 0 && !Over120(pEntity)) {
			switch (Globals::missedshots % 5)
			{
			case 0: PlayerYaw[Index] = MovingLBY[Index]; break;
			case 1: PlayerYaw[Index] = MovingLBY[Index] + 20 - rand() % 50; break;
			case 2: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget(); break;
			case 3: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 90; break;
			case 4: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 90; break;
			case 6: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 135; break;
			case 5: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 135; break;
			case 7: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 125; break;
			case 8: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 115; break;
			case 9: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 125; break;
			case 10: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 90; break;
			case 11: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 75; break;
			}
		}
		// in air
		else if (InAir(pEntity)) {
			switch (Globals::missedshots % 8)
			{
			case 0: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget(); break;
			case 1: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 90; break;
			case 2: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 180; break;
			case 3: PlayerYaw[Index] = MovingLBY[Index] - 180; break;
			case 4: PlayerYaw[Index] = MovingLBY[Index]; break;
			case 5: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 140; break;
			case 6: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() + 90; break;
			case 7: PlayerYaw[Index] = *pEntity->GetLowerBodyYawTarget() - 120; break;
			}
		}
	}
	pEntity->GetEyeAngles()->y = PlayerYaw[Index];
}
void Resolver::CallFSN()
{
	for (int i = 1; i < g_Engine->GetMaxClients(); i++) {
		C_BaseEntity *pLocal = g_EntityList->GetClientEntity(g_Engine->GetLocalPlayer());
		C_BaseEntity *pEntity = g_EntityList->GetClientEntity(i);
		if (!pEntity || pEntity == pLocal || pEntity->IsDormant() || !pEntity->IsAlive() || pEntity->GetTeamNum() == pLocal->GetTeamNum()) continue;
		Resolver::ResolveYaw(pEntity);
	}
}



