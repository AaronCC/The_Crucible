#include "EditorState.h"
namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	bool _ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

}
void EditorState::draw(const float dt)
{
	ImGui::SFML::Render(this->game->window);
}
void EditorState::update(const float dt)
{
	ImGui::SFML::Update(this->game->window, deltaClock.restart());

	ImGui::Begin("Editor"); // begin window

	ImGui::Columns(2, NULL, false);
	//static int mode = 0;
	//ImGui::Combo("Mode", &mode, "Edit\0New\0");

	static int file_int = 0;

	ImGui::_ListBox("##", &file_int, iGen.filePaths);

	ImGui::BeginGroup();
	{
		ImGui::NextColumn();
		if (ImGui::Button("Save File")) {
			iGen.exportAffixes(iGen.filePaths[file_int]);
		}
		static char newFile[255] = "NULL";
		if (ImGui::Button("Create File")) {
			iGen.writeFile(newFile);
			iGen.getFilePaths();
		}
		ImGui::InputText("File Name##", newFile, 20);
		if (ImGui::Button("Set As Default")) {
		
		}
	}
	ImGui::EndGroup();

	ImGui::Columns(1);
	if (file_int != old_file)
		iGen.loadAffixes(iGen.filePaths[file_int]);

	static int type = 0;
	ImGui::Combo("Type", &type, "Item Affix\0Item Base\0Ability Affix\0Ability Base\0Enemy Ability Base\0Enemy Base\0");

	std::map<Helper::Affix, std::vector<std::pair<Helper::AffVal, int>>> afList;
	char buff[255];
	std::string str_buff;

	static int afInt = 0;
	static int ps = 0;
	static int nameInt = 0;

	Helper::Affix af_selected;
	this->editing = (EType)type;
	if (editing == I_AFFIX)
	{
		ImGui::Combo("Suffix | Prefix", &ps, "Suffixes\0Prefixes\0");
		if (ps == 1)
		{
			afList = iGen.prefixes;
			ImGui::ListBox("Affix Type", &afInt, helper.c_prefix_buffnames, IM_ARRAYSIZE(helper.c_prefix_buffnames), 4);
		}
		else if (ps == 0)
		{
			afList = iGen.suffixes;
			ImGui::ListBox("Affix Type", &afInt, helper.c_suffix_buffnames, IM_ARRAYSIZE(helper.c_suffix_buffnames), 4);
		}
		if (ps == 0)
			af_selected = (Helper::Affix)afInt;
		else if (ps == 1)
			af_selected = (Helper::Affix)(afInt + helper.sRange.second);
		
		ItemGenerator::AfBase* afBase = &iGen.afBases[af_selected];
		if (!helper.isDamage[af_selected])
		{
			afBase->dtype = 5;
			ImGui::PushID((int)af_selected);
			ImGui::InputInt("Value##", &afBase->offset);
			ImGui::PopID();
		}
		else
		{
			ImGui::PushID((int)af_selected);
			ImGui::InputInt("Range##", &afBase->range);
			ImGui::InputInt("Offset##", &afBase->offset);
			ImGui::Combo("Damage Type##", &afBase->dtype, "Physical\0Fire\0Cold\0Lightning\0Poison");
			ImGui::PopID();
		}
		if (ImGui::CollapsingHeader("Names"))
		{
			std::vector<int> erase_buf;
			for (int i = 0; i < iGen.nameTable[af_selected].size(); ++i) {
				ImGui::PushID(i);
				if (ImGui::Button("Delete##")) {
					erase_buf.push_back(i);
				}ImGui::SameLine(72);
				ImGui::InputText("##", iGen.nameTable[af_selected][i], IM_ARRAYSIZE(iGen.nameTable[af_selected][i]));
				ImGui::PopID();
			}
			for (auto i : erase_buf)
			{
				iGen.nameTable[af_selected].erase(iGen.nameTable[af_selected].begin() + i);
			}
			if (ImGui::Button("New Name", { 64,20 })) {
				iGen.nameTable[af_selected].push_back("NULL");
			}
		}
	}
	else if (editing == I_BASE)
	{

		static int slot_type = 0;
		static int baseNum = 0;
		char* typeStrings = { "Head\0Body\0Main-hand\0Off-hand\0Ring\0Amulet\0Cloak\0Belt\0Scroll" };
		ImGui::Combo("Slot Type", &slot_type, typeStrings);
		std::vector<std::string> base_names;
		for (int i = 0; i < iGen.bases[(Item::SlotType)slot_type].size(); ++i)
		{
			base_names.push_back(iGen.bases[(Item::SlotType)slot_type][i].second.id);
		}
		ImGui::PushID(slot_type);
		ImGui::_ListBox("##", &baseNum, base_names);
		ImGui::PopID();
		if (iGen.bases[(Item::SlotType)slot_type].size() > 0)
		{
			BaseItem* base = &iGen.bases[(Item::SlotType)slot_type][baseNum].second;
			ImGui::PushID(baseNum);
			//ImGui::Text(base->id); 
			ImGui::InputText("Name##", base->id, 20);
			ImGui::ListBox("Affix Type", &base->init_af, helper.c_buffnames, IM_ARRAYSIZE(helper.c_buffnames), 4);
			if (helper.isDamage[(AF)base->init_af])
			{
				ImGui::InputInt("Min Damage##", &base->init_v1);
				ImGui::InputInt("Max Damage##", &base->init_v2);
			}
			else
			{
				ImGui::InputInt("Value##", &base->init_v1);
				base->init_v2 = -1;
			}
			if (slot_type == (int)Item::SlotType::MAH)
				ImGui::Checkbox("Two Handed##", &base->twoh);
			ImGui::InputInt("Item Level##", &iGen.bases[(Item::SlotType)slot_type][baseNum].first);
			ImGui::PopID();
		}

		if (ImGui::Button("New Base", { 74,20 })) {
			iGen.bases[(Item::SlotType)slot_type].push_back({ 0,BaseItem((Item::SlotType)slot_type,"NULL",false) });
		}

	}

	old_file = file_int;

	ImGui::End(); // end window
}

void EditorState::handleInput()
{
	sf::Event event;
	while (this->game->window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed) {
			this->game->window.close();
		}


	}
}
