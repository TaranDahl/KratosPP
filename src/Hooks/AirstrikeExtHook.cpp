﻿#include <exception>
#include <Windows.h>

#include <GeneralStructures.h>
#include <TechnoClass.h>
#include <FootClass.h>
#include <AircraftClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/TechnoExt.h>

#include <Ext/Common/CommonStatus.h>

#include <Ext/Helper/Scripts.h>

#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/TechnoType/AirstrikeData.h>

// 首次添加空袭管理器，跳过建筑检查
DEFINE_HOOK(0x41D97B, AirstrikeClass_Setup_SkipBuildingCheck, 0x7)
{
	GET(TechnoClass*, pTarget, ESI);
	GET(AirstrikeClass*, pAirstrike, EDI);
	// Debug::Log("将持有管理器 %d 的目标[%s]%d, 设置给空袭管理器 %d 的飞机\n", pTarget->Airstrike, pTarget->GetTechnoType()->ID, pTarget, pAirstrike);
	FootClass* pAircraft = pAirstrike->FirstObject;
	do
	{
		if (pAircraft)
		{
			pAircraft->SetTarget(pTarget);
		}
	} while (pAircraft && (pAircraft = pAircraft->NextTeamMember) != nullptr);
	return 0x41D98B;
}

// 接管空袭管理器向目标设置自己
DEFINE_HOOK(0x41D994, AirstrikeClass_Setup_SetToTarget, 0x6)
{
	GET(TechnoClass*, pTarget, ESI);
	GET(AirstrikeClass*, pAirstrike, EDI);
	// Debug::Log("接管将空袭管理器 %d 设置给[%s]%d\n", pAirstrike, pTarget->GetTechnoType()->ID, pTarget);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTarget, status))
	{
		status->SetAirstrike(pAirstrike);
		return 0x41D99A;
	}
	return 0;
}

// 接管空袭管理器清空旧目标
DEFINE_HOOK(0x41DA68, AirstrikeClass_Reset_Stopwhat_SkipBuildingCheck, 0x7)
{
	GET(TechnoClass*, pTarget, ESI);
	GET(AirstrikeClass*, pAirstrike, EBP);
	// Debug::Log("空袭管理器 %d 设置新目标，当前旧目标[%s]%d\n", pAirstrike, pTarget->GetTechnoType()->ID, pTarget);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTarget, status))
	{
		status->CancelAirstrike();
	}
	R->EDI(pTarget);
	return 0x41DA7C;
}

// 接管空袭管理器重设新目标
DEFINE_HOOK(0x41DA88, AirstrikeClass_Reset_Startwhat_SkipBuildingCheck, 0x7)
{
	GET(TechnoClass*, pTarget, EBX);
	GET(AirstrikeClass*, pAirstrike, EBP);
	// Debug::Log("空袭管理器 %d 设置新目标，当前新目标[%s]%d\n", pAirstrike, pTarget->GetTechnoType()->ID, pTarget);
	R->ESI(pTarget);
	return 0x41DA9C;
}

// 接管空袭管理器向目标设置自己
DEFINE_HOOK(0x41DAD4, AirstrikeClass_Reset, 0x6)
{
	GET(TechnoClass*, pTarget, ESI);
	GET(AirstrikeClass*, pAirstrike, EBP);
	// Debug::Log("接管将空袭管理器 %d 重新设置给[%s]%d\n", pAirstrike, pTarget->GetTechnoType()->ID, pTarget);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTarget, status))
	{
		status->SetAirstrike(pAirstrike);
		return 0x41DADA;
	}
	return 0;
}

DEFINE_HOOK(0x41DBD4, AirstrikeClass_ClearTarget, 0x7)
{
	enum { resetTarget = 0x41DBE8, clearTarget = 0x41DC3A };
	GET(TechnoClass*, pTarget, ESI); // AirstrikeClass->Target
	GET(AirstrikeClass*, pAirstrike, EBP);
	// Debug::Log("不满足空袭条件，空袭管理器 %d 清空目标[%s]%d\n", pAirstrike, pTarget->GetTechnoType()->ID, pTarget);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTarget, status))
	{
		status->CancelAirstrike();
	}
	// 该函数遍历所有的空袭管理器清空挂载，因为接管了目标身上的管理器，所以永久返回clearTarget
	if (pTarget->Airstrike == pAirstrike)
	{
		return resetTarget;
	}
	return clearTarget;
}

// 上一个Hook永久跳过该步骤
// DEFINE_HOOK(0x41DC10, AirstrikeClass_ResetTarget_SetToTarget, 0x6)
// {
// 	GET(TechnoClass*, pTarget, ESI);
// 	GET(AirstrikeClass*, pAirstrike, EBP);
// 	TechnoStatus* status = nullptr;
// 	if (TryGetStatus<TechnoExt>(pTarget, status))
// 	{
// 		status->SetAirstrike(pAirstrike);
// 		return 0x41DC2C;
// 	}
// 	return 0;
// }

// 绘制激光指示
DEFINE_HOOK(0x6D481D, TacticalClass_Draw_AirstrikeLaser_SkipBuildingCheck, 0x7)
{
	enum { draw = 0x6D482D, skip = 0x6D48FA };
	GET(TechnoClass*, pTechno, ESI);
	// Debug::Log("单位[%s]%d绘制空袭激光指示\n", pTechno->GetTechnoType()->ID, pTechno);
	AirstrikeData* data = INI::GetConfig<AirstrikeData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	if (data->AirstrikeDisableLine)
	{
		return skip;
	}
	return draw;
}

// 更新被空袭时的闪光变化
DEFINE_HOOK(0x70E92F, TechnoClass_Update_Airstrike_Tint_Timer, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->Airstrike && status->Airstrike->Target == pTechno)
	{
		return 0x70E96E;
	}
	return 0;
}

// 绘制SHP时无敌或者被空袭时，调整亮度
DEFINE_HOOK(0x706342, TechnoClass_DrawSHP_Tint_Airstrike, 0x7)
{
	enum { draw = 0x706377, skip = 0x706389 };
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->Airstrike && status->Airstrike->Target == pTechno)
	{
		AirstrikeData* data = INI::GetConfig<AirstrikeData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
		if (data->AirstrikeDisableBlink)
		{
			return skip;
		}
		return draw;
	}
	return 0;
}

// 绘制VXL时无敌或者被空袭时，调整亮度
DEFINE_HOOK(0x70679B, TechnoClass_DrawVXL_Tint_Airstrike, 0x5)
{
	enum { draw = 0x7067D2, skip = 0x7067E4 };
	GET(TechnoClass*, pTechno, EBP);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->Airstrike && status->Airstrike->Target == pTechno)
	{
		AirstrikeData* data = INI::GetConfig<AirstrikeData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
		if (data->AirstrikeDisableBlink)
		{
			return skip;
		}
		return draw;
	}
	return 0;
}

// 绘制VXL时无敌或者被空袭时，调整亮度
DEFINE_HOOK(0x73BFA4, UnitClass_DrawVXL_Tint_Airstrike, 0x6)
{
	enum { draw = 0x73BFAA, skip = 0x73C07C };
	GET(TechnoClass*, pTechno, EBP);
	if (pTechno->IsIronCurtained())
	{
		return draw;
	}
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->Airstrike && status->Airstrike->Target == pTechno)
	{
		AirstrikeData* data = INI::GetConfig<AirstrikeData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
		if (data->AirstrikeDisableBlink)
		{
			return skip;
		}
		return draw;
	}
	return skip;
}

