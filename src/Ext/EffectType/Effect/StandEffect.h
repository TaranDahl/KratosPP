﻿#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <TechnoClass.h>

#include "../EffectScript.h"
#include "StandData.h"


/// @brief EffectScript
/// GameObject
///		|__ AttachEffect
///				|__ AttachEffectScript#0
///						|__ EffectScript#0
///						|__ EffectScript#1
///				|__ AttachEffectScript#1
///						|__ EffectScript#0
///						|__ EffectScript#1
///						|__ EffectScript#2
class StandEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(Stand);

	void UpdateLocation(LocationMark LocationMark);

	void OnTechnoDelete(EventSystem* sender, Event e, void* args);

	virtual void ExtChanged() override;

	virtual void Clean() override
	{
		EffectScript::Clean();

		pStand = nullptr;
		masterIsRocket = false;
		masterIsSpawned = false;
		standIsBuilding = false;
		onStopCommand = false;
		notBeHuman = false;
		onReceiveDamageDestroy = false;
		onRocketExplosion = false;

		_lastLocationMark = {};
		_isMoving = false;
		_walkRateTimer = {};

	}

	virtual void OnStart() override;

	virtual void End(CoordStruct location) override;

	virtual void OnPause() override;
	virtual void OnRecover() override;

	virtual bool IsAlive() override;

	virtual void OnGScreenRender(CoordStruct location) override;

	virtual void OnPut(CoordStruct* pCoord, DirType faceDir) override;
	virtual void OnUpdate() override;
	virtual void OnWarpUpdate() override;
	virtual void OnTemporalEliminate(TemporalClass* pTemporal) override;
	virtual void OnRemove() override;
	virtual void OnReceiveDamageDestroy() override;
	virtual void OnGuardCommand() override;
	virtual void OnStopCommand() override;

	virtual void OnRocketExplosion() override;

	TechnoClass* pStand = nullptr;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->pStand)

			.Process(this->masterIsRocket)
			.Process(this->masterIsSpawned)
			.Process(this->standIsBuilding)
			.Process(this->onStopCommand)
			.Process(this->notBeHuman)
			.Process(this->onReceiveDamageDestroy)
			.Process(this->onRocketExplosion)

			.Process(this->_lastLocationMark)
			.Process(this->_isMoving)
			.Process(this->_walkRateTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		EffectScript::Load(stream, registerForChange);
		EventSystems::General.AddHandler(Events::ObjectUnInitEvent, this, &StandEffect::OnTechnoDelete);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		EffectScript::Save(stream);
		return const_cast<StandEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void CreateAndPutStand();
	TechnoStatus* SetupStandStatus();
	void SetLocation(CoordStruct location);
	void SetFacing(DirStruct dir, bool forceSetTurret);
	void ForceFacing(DirStruct dir);

	void ExplodesOrDisappear(bool peaceful);

	void UpdateStateBullet();
	void UpdateStateTechno(bool masterIsDead);

	void RemoveStandIllegalTarget();

	bool masterIsRocket = false;
	bool masterIsSpawned = false;
	bool standIsBuilding = false;
	bool onStopCommand = false;
	bool notBeHuman = false;
	bool onReceiveDamageDestroy = false;
	bool onRocketExplosion = false;

	LocationMark _lastLocationMark{};
	bool _isMoving = false;
	CDTimerClass _walkRateTimer{};
};
