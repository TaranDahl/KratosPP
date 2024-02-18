﻿#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "PaintballData.h"

#include <Ext/TechnoType/DamageText.h>

class Paintball : public StateScript<PaintballData>
{
public:
	STATE_SCRIPT(Paintball, PaintballData);

	void RGBIsPower();

	bool NeedPaint(bool& changeColor, bool& changeBright);

	virtual void OnUpdate() override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_rgbMode)
			.Process(this->_rgbIdx)
			.Process(this->_rgbTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<PaintballData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<PaintballData>::Save(stream);
		return const_cast<Paintball*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	bool _rgbMode = false;
	int _rgbIdx = 0;
	CDTimerClass _rgbTimer;
};
