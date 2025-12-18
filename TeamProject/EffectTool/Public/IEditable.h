#pragma once

NS_BEGIN(EffectTool)
class IEditable
{
protected:
	IEditable();
	virtual ~IEditable() DEFAULT;

public:
	virtual void ImportToJson(nlohmann::ordered_json& json) PURE;
	virtual void ExportToJson(nlohmann::ordered_json& json) PURE;

};
NS_END
