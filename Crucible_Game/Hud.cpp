#include "Hud.h"

std::pair<int, int> Hud::checkLevelClick()
{
	int stat = -1;
	std::string msg = this->levelButtons.getEMsg(oldMousePos);
	if (msg == "atk") stat = 0;
	else if (msg == "kno") stat = 1;
	else if (msg == "def") stat = 2;
	else if (msg == "agi") stat = 3;
	if (stat != -1)
		levelCache--;
	return { stat, 1 };
}

void Hud::updateExpBar(int exp, int maxExp, int level)
{
	float prc = exp / (float)maxExp;
	levelText.setString("Player Level: " + std::to_string(level));
	expText.setString(std::to_string(exp) + "/" + std::to_string(maxExp));
	expBar.setSize({ prc * 100,TILE_SIZE / 4 });
}

void Hud::levelUp() { levelCache++; }

void Hud::updateDLevelText(int level)
{
	dungeonLevelText.setString("Dungeon Level: " + std::to_string(level));
}

void Hud::setSlotSprites(std::vector<std::pair<sf::Keyboard::Key, std::string>> slotSpriteIDs, std::string lmbID, std::string rmbID)
{
	unsigned int idNum = 0;
	aSlotSprites.clear();
	for (auto id : slotSpriteIDs)
	{
		if (id.second == "")
		{
			this->aSlotSprites[id.first].second = false;
			idNum++;
			continue;
		}
		this->aSlotSprites[id.first].second = true;
		this->aSlotSprites[id.first].first.setTexture(this->game->texmgr.getRef(id.second));
		this->aSlotSprites[id.first].first.setOrigin(0, 32.f);
		this->aSlotSprites[id.first].first.setPosition(elements[A_SLOT + std::to_string(idNum)].getPosition());
		idNum++;
	}
	sf::Vector2f pos{ 0,0 };
	if (lmbID == "")
		this->lmbSprite.second = false;
	else
	{
		this->lmbSprite.second = true;
		this->lmbSprite.first.setTexture(this->game->texmgr.getRef(lmbID));
		this->lmbSprite.first.setOrigin(0, 32.f);
		this->lmbSprite.first.setTextureRect({ 0, 0, 32, 32 });
		pos = elements[A_SLOT + std::to_string(A_SLOT_COUNT - 2)].getPosition();
		this->lmbSprite.first.setPosition(pos);
	}
	if (rmbID == "")
		this->rmbSprite.second = false;
	else
	{
		this->rmbSprite.second = true;
		this->rmbSprite.first.setTexture(this->game->texmgr.getRef(rmbID));
		this->rmbSprite.first.setOrigin(0, 32.f);
		this->rmbSprite.first.setTextureRect({ 0, 0, 32, 32 });
		this->rmbSprite.first.setPosition(elements[A_SLOT + std::to_string(A_SLOT_COUNT - 1)].getPosition());
	}
}
void Hud::equipCon() {
	if (consumables.size() == 0)
		return;
	Consumable temp = consumables[0];
	consumables[0] = consumables[conAt];
	consumables[conAt] = temp;
	showCons();
}

void Hud::addConsumable(Consumable * con)
{
	for (int i = 0; i < consumables.size(); i++)
	{
		Consumable& _con = consumables[i];
		if (con->name == _con.name && con->ilvl == _con.ilvl)
		{
			_con.add();
			return;
		}
	}
	con->setSpritePos(conPos);
	consumables.push_back(*con);
}

bool Hud::hoverCon(sf::Vector2f mPos)
{
	if (consumables.size() <= 0)
		return false;
	sf::Vector2f size = { slotW, slotW };
	sf::Vector2f pos = conPos;
	if (conHover)
	{
		int diff = (consumables.size() - 1) * slotW;
		size.y += diff;
		pos.y -= diff;
	}
	if (
		mPos.x >= pos.x &&
		mPos.y >= pos.y &&
		mPos.x <= pos.x + size.x &&
		mPos.y <= pos.y + size.y
		)
	{
		conAt = std::floor(((conPos.y + slotW) - mPos.y) / slotW);
		if (conAt >= consumables.size()) conAt = consumables.size() - 1;
		conHoverText.setString(consumables[conAt].name + "\n" + consumables[conAt].getEffectString());
		conHoverText.setPosition({ conPos.x + slotW + 4.f,conPos.y - (slotW*conAt) });
		return true;
	}
	return false;
}

void Hud::showCons()
{
	for (int i = 0; i < consumables.size(); i++)
	{
		consumables[i].setSpritePos(conPos - sf::Vector2f(0, slotW*i));
	}
	conHover = true;
}

void Hud::draw(float dt)
{
	for (auto element : elements)
	{
		this->game->window.draw(element.second);
	}
	for (int i = 0; i < A_SLOT_COUNT; i++)
	{
		this->game->window.draw(slotText[i]);
	}
	for (auto slot : aSlotSprites)
	{
		if (slot.second.second)
			this->game->window.draw(slot.second.first);
	}
	if (rmbSprite.second)
	{
		this->game->window.draw(rmbSprite.first);
	}
	if (lmbSprite.second)
	{
		this->game->window.draw(lmbSprite.first);
	}
	for (int cd = 0; cd < A_SLOT_COUNT; cd++)
	{
		if (cooldowns[cd].active)
		{
			sf::Vector2f pos = elements[A_SLOT + std::to_string(cd)].getPosition();
			pos.y += cooldowns[cd].bounds.top;
			cdSprite.setPosition(pos);
			cdSprite.setTextureRect(cooldowns[cd].bounds);
			this->game->window.draw(cdSprite);
		}
	}
	this->game->window.draw(conText);
	this->game->window.draw(hpText);
	if (consumables.size() > 0)
	{
		consumables[0].draw(this->game->window);
		if (conHover)
		{
			for (int i = 1; i < consumables.size(); i++)
			{
				consumables[i].draw(this->game->window);
			}
			this->game->window.draw(conHoverText);
		}
	}
	else
		this->game->window.draw(conBack);

	this->game->window.draw(expBack);
	this->game->window.draw(expBar);
	this->game->window.draw(expText);
	this->game->window.draw(levelText);
	this->game->window.draw(dungeonLevelText);

	if (levelCache > 0)
		this->levelButtons.draw(this->game->window, dt);

	sf::Vector2f offset = { 0,0 };
	if (showState != ShowState::SHOW_NONE)
	{
		this->game->window.draw(msgBack);
		if (showState == Hud::ShowState::SHOW_MSG)
		{
			for (int i = this->game->gameMsgs.size()-1; i >= 0; i--)
			{
				this->game->gameMsgs[i].setPosition(msgStart + offset);
				this->game->window.draw(this->game->gameMsgs[i]);
				offset.y -= tSize;
			}
			this->game->window.draw(msgTitle);
		}
	}
}
void Hud::updateCD(unsigned int slot, float ticks)
{
	if (cooldowns[slot].active)
	{
		cooldowns[slot].timer -= ticks;
	}
}
void Hud::checkHover(sf::View view)
{
	sf::Vector2f mousePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), view);

	if (levelCache > 0)
	{
		this->levelButtons.highlight(this->levelButtons.getElement(mousePos));
	}
	if (showState == Hud::ShowState::SHOW_NONE)
	{
		if (oldMousePos != mousePos)
		{
			if (hoverCon(mousePos))
				showCons();
			else
				conHover = false;
		}
	}
	oldMousePos = mousePos;
}
void Hud::update(float dt)
{
	if (levelCache > 0) {
		levelButtons.update(dt);
	}
	for (int cd = 0; cd < A_SLOT_COUNT; cd++)
	{
		if (cooldowns[cd].active)
		{
			//cooldowns[cd].timer -= dt;
			if (cooldowns[cd].timer < 0)
			{
				cooldowns[cd].active = false;
				cooldowns[cd].timer = 0;
			}
			else
			{
				float remaining = cooldowns[cd].timer / cooldowns[cd].totalTime;
				int newH = (1.f - remaining)*slotW;
				cooldowns[cd].bounds = sf::IntRect(
					cooldowns[cd].bounds.left,
					newH,
					slotW,
					(int)(slotW * remaining)
				);
			}
		}
	}
}

void Hud::updateHealth(int at, int max, float percent)
{
	sf::IntRect hRect = elements[H_GLOBE].getTextureRect();
	int newH = (1.f - percent)*(float)hRect.height;
	elements[H_POOL].setTextureRect({ 0, newH,hRect.width,(int)(hRect.height * percent) });
	sf::Vector2f pos = elements[H_GLOBE].getPosition();
	elements[H_POOL].setPosition(pos.x, pos.y + newH);

	hpText.setString(std::to_string(at) + "/" + std::to_string(max));
}

Consumable::ConEffect Hud::useConsumable() {
	Consumable::ConEffect eff{};
	if (consumables.size() > 0)
	{
		eff = consumables[0].getEffect();
		if (consumables[0].use())
		{
			consumables.erase(consumables.begin());
			showCons();
		}
	}
	return eff;
}

void Hud::handleInput()
{
}

Hud::~Hud()
{
}
//
//void Inventory::updateAutoScroll()
//{
//	Item* itm = eqItems[2].first.getItem();
//	if (itm != nullptr)
//	{
//		Ability* autoAttack = new Ability(this->game, game->texmgr.getRef("slash"), "slash_icon",
//			{ 0,3,0.1f }, { 32,32 }, Ability::ID::SLASH, 10, 1, "Default Attack",
//			"Melee targeted attack");
//		autoAttack->setInfo(Ability::AbInfo(Ability::AbPrmType::MELEE, Ability::AbSecType::TARG, 0, 1));
//		autoAttack->addEffect(itm->getEffectFromMAH());
//		autoScroll = new Scroll("Slash",
//			{ 0,0,0,0 },
//			Item::SlotType::SCR,
//			autoAttack);
//	}
//}

std::vector<Ability*> Inventory::getScrollAbilities()
{
	std::vector<Ability*> abilities;
	for (int i = 0; i < eqScrolls.size(); i++)
	{
		Item* itm = eqScrolls[i].first.getItem();
		if (itm != nullptr)
			abilities.push_back(itm->getAbility());
	}
	return abilities;
}

bool Inventory::pickupItem(Item * item)
{
	int slot = firstEmptySlot(itemSlots);
	if (slot != -1)
	{
		itemSlots[slot].setItem(item);
		return true;
	}
	return false;
}
bool Inventory::pickupScroll(Scroll * item)
{
	int slot = firstEmptySlot(scrollSlots);
	if (slot != -1)
	{
		scrollSlots[slot].setItem(item);
		return true;
	}
	return false;
}
void Inventory::draw()
{
	this->game->window.draw(slider);
	this->game->window.draw(invTitle);
	this->game->window.draw(deleteButton.first);
	this->game->window.draw(deleteButton.second);
	for (int i = slotScIndex; i < maxScHeight + slotScIndex; i++)
	{
		if (i >= itemSlots.size())
			break;
		itemTextBack.setPosition(itemSlots[i].position);
		this->game->window.draw(itemSlots[i].slotBack);
		for (auto text : itemSlots[i].itemText)
		{
			this->game->window.draw(text);
		}
	}
	for (auto slot : eqItems)
	{
		this->game->window.draw(slot.first.slotBack);
		for (auto text : slot.first.itemText)
		{
			this->game->window.draw(text);
		}
		this->game->window.draw(slot.second);
	}
	for (int i = slotScIndex; i < maxScHeight + slotScIndex; i++)
	{
		if (i >= scrollSlots.size())
			break;
		itemTextBack.setPosition(scrollSlots[i].position);
		this->game->window.draw(scrollSlots[i].slotBack);
		for (auto text : scrollSlots[i].itemText)
		{
			this->game->window.draw(text);
		}
	}
	for (auto slot : eqScrolls)
	{
		this->game->window.draw(slot.first.slotBack);
		for (auto text : slot.first.itemText)
		{
			this->game->window.draw(text);
		}
		this->game->window.draw(slot.second);
	}
	if (showInfo)
	{
		switch (hovType)
		{
		case Hovering::EQI:
		{
			if (eqItems[hovering].first.nameInfoText.size() > 0)
			{
				this->game->window.draw(eqItems[hovering].first.infoBack);

				for (auto text : eqItems[hovering].first.nameInfoText)
				{
					this->game->window.draw(text);
				}
				for (auto text : eqItems[hovering].first.baseInfoText)
				{
					this->game->window.draw(text);
				}
				this->game->window.draw(eqItems[hovering].first.seperatorText);
				for (auto text : eqItems[hovering].first.buffInfoText)
				{
					this->game->window.draw(text);
				}
				this->game->window.draw(eqItems[hovering].first.slotTypeText);
			}
			break;
		}
		case Hovering::ITM:
		{
			if (itemSlots[hovering].nameInfoText.size() > 0)
			{
				this->game->window.draw(itemSlots[hovering].infoBack);

				for (auto text : itemSlots[hovering].nameInfoText)
				{
					this->game->window.draw(text);
				}
				for (auto text : itemSlots[hovering].baseInfoText)
				{
					this->game->window.draw(text);
				}
				this->game->window.draw(itemSlots[hovering].seperatorText);
				for (auto text : itemSlots[hovering].buffInfoText)
				{
					this->game->window.draw(text);
				}
				this->game->window.draw(itemSlots[hovering].slotTypeText);
			}
			break;
		}
		case Hovering::EQS:
		{
			if (eqScrolls[hovering].first.nameInfoText.size() > 0)
			{
				this->game->window.draw(eqScrolls[hovering].first.infoBack);

				for (auto text : eqScrolls[hovering].first.nameInfoText)
				{
					this->game->window.draw(text);
				}
				for (auto text : eqScrolls[hovering].first.baseInfoText)
				{
					this->game->window.draw(text);
				}
				for (auto text : eqScrolls[hovering].first.buffInfoText)
				{
					this->game->window.draw(text);
				}
				this->game->window.draw(eqScrolls[hovering].first.slotTypeText);
			}
			break;
		}
		case Hovering::SCR:
		{
			if (scrollSlots[hovering].nameInfoText.size() > 0)
			{
				this->game->window.draw(scrollSlots[hovering].infoBack);

				for (auto text : scrollSlots[hovering].nameInfoText)
				{
					this->game->window.draw(text);
				}
				for (auto text : scrollSlots[hovering].baseInfoText)
				{
					this->game->window.draw(text);
				}
				for (auto text : scrollSlots[hovering].buffInfoText)
				{
					this->game->window.draw(text);
				}
				this->game->window.draw(scrollSlots[hovering].slotTypeText);
			}
			break;
		}
		}
	}
}

void Inventory::update(sf::Vector2f mousePos)
{
	mousePos.y = mousePos.y >= MSG_BACK_H - invSlotH ? MSG_BACK_H - invSlotH : mousePos.y;
	showInfo = false;
	float offset = 0.f;
	int index = 0;
	for (int s = slotScIndex; s < scrollSlots.size(); s++)
	{
		/*if (s >= slots.size())
		break;*/
		scrollSlots[s].updatePos({
			scrollSlots[s].start.x + (xOffset * (int)scrollSlots[s].equipped),
			scrollSlots[s].start.y + (spacing * index)
			});
		for (int i = 0; i < scrollSlots[s].itemText.size(); i++)
		{
			int textL = scrollSlots[s].itemText[i].getString().getSize();
			offset = (((scrollSlots[s].maxNameChar + 1) - textL) / 2.f)*(charWidth);
			scrollSlots[s].itemText[i].setPosition(scrollSlots[s].position.x + offset, scrollSlots[s].position.y + (i*tSize));
		}
		if (scrollSlots[s].isHovering(mousePos))
		{
			hovType = Hovering::SCR;
			hoveringEq = false;
			hovering = s;
			showInfo = true;
		}
		else if (isHoveringDelete(mousePos))
		{
			delHover = true;
		}
		else
			delHover = false;
		index++;
	}
	for (int s = 0; s < eqScrolls.size(); s++)
	{
		for (int i = 0; i < eqScrolls[s].first.itemText.size(); i++)
		{
			int textL = eqScrolls[s].first.itemText[i].getString().getSize();
			offset = (((eqScrolls[s].first.maxNameChar + 1) - textL) / 2.f)*(charWidth);
			eqScrolls[s].first.itemText[i].setPosition(eqScrolls[s].first.position.x + offset, eqScrolls[s].first.position.y + (i*tSize));
		}
		if (eqScrolls[s].first.isHovering(mousePos))
		{
			hovType = Hovering::EQS;
			hoveringEq = true;
			hovering = s;
			showInfo = true;
		}
		index++;
	}
	index = 0;
	for (int s = slotScIndex; s < itemSlots.size(); s++)
	{
		/*if (s >= slots.size())
			break;*/
		itemSlots[s].updatePos({
			itemSlots[s].start.x + (xOffset * (int)itemSlots[s].equipped),
			itemSlots[s].start.y + (spacing * index)
			});
		for (int i = 0; i < itemSlots[s].itemText.size(); i++)
		{
			int textL = itemSlots[s].itemText[i].getString().getSize();
			offset = (((itemSlots[s].maxNameChar + 1) - textL) / 2.f)*(charWidth);
			itemSlots[s].itemText[i].setPosition(itemSlots[s].position.x + offset, itemSlots[s].position.y + (i*tSize));
		}
		if (itemSlots[s].isHovering(mousePos))
		{
			hovType = Hovering::ITM;
			hoveringEq = false;
			hovering = s;
			showInfo = true;
		}
		else if (isHoveringDelete(mousePos))
		{
			delHover = true;
		}
		else
			delHover = false;
		index++;
	}
	for (int s = 0; s < eqItems.size(); s++)
	{
		for (int i = 0; i < eqItems[s].first.itemText.size(); i++)
		{
			int textL = eqItems[s].first.itemText[i].getString().getSize();
			offset = (((eqItems[s].first.maxNameChar + 1) - textL) / 2.f)*(charWidth);
			eqItems[s].first.itemText[i].setPosition(eqItems[s].first.position.x + offset, eqItems[s].first.position.y + (i*tSize));
		}
		if (eqItems[s].first.isHovering(mousePos))
		{
			hovType = Hovering::EQI;
			hoveringEq = true;
			hovering = s;
			showInfo = true;
		}
		index++;
	}
	if (showInfo) {
		int textL;
		int ySpacing = tSize + 4;
		mousePos += {2, 2};
		if (mousePos.x + infoWidth > game->windowSize.x)
			mousePos.x = game->windowSize.x - infoWidth;
		switch (hovType)
		{
		case Hovering::EQI:
		{
			int nSize = eqItems[hovering].first.nameInfoText.size() + 3;
			int ln = nSize + eqItems[hovering].first.baseInfoText.size() + 2;
			float slotH = (ySpacing + 2) * ((float)ln + eqItems[hovering].first.buffInfoText.size());
			if (mousePos.y + slotH > game->windowSize.y)
				mousePos.y = game->windowSize.y - slotH;
			for (int i = 0; i < eqItems[hovering].first.nameInfoText.size(); i++)
			{
				textL = eqItems[hovering].first.nameInfoText[i].getString().getSize();
				offset = (((eqItems[hovering].first.maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				eqItems[hovering].first.nameInfoText[i].setPosition(mousePos.x + offset, mousePos.y + (i*ySpacing));
			}
			textL = eqItems[hovering].first.slotTypeText.getString().getSize();
			offset = (((eqItems[hovering].first.maxInfoChar + 1) - textL) / 2.f)*(charWidth);
			eqItems[hovering].first.slotTypeText.setPosition(mousePos.x + offset, mousePos.y + ((nSize - 2)*ySpacing));
			int baseSize = eqItems[hovering].first.baseInfoText.size();
			for (int i = 0; i < baseSize; i++)
			{
				textL = eqItems[hovering].first.baseInfoText[i].getString().getSize();
				offset = (((eqItems[hovering].first.maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				eqItems[hovering].first.baseInfoText[i].setPosition(mousePos.x + offset, mousePos.y + ((i + nSize)*ySpacing));
			}
			eqItems[hovering].first.seperatorText.setPosition(mousePos.x + (infoWidth / 2) - (charWidth * 2), mousePos.y + ((baseSize + nSize)*ySpacing));
			nSize += eqItems[hovering].first.baseInfoText.size() + 2;
			for (int i = 0; i < eqItems[hovering].first.buffInfoText.size(); i++)
			{
				textL = eqItems[hovering].first.buffInfoText[i].getString().getSize();
				offset = (((eqItems[hovering].first.maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				eqItems[hovering].first.buffInfoText[i].setPosition(mousePos.x + offset, mousePos.y + ((i + nSize)*ySpacing));
			}
			itemInfoBack.setPosition(mousePos);
			eqItems[hovering].first.infoBack.setPosition(mousePos);
			eqItems[hovering].first.infoBack.setSize({ infoWidth, (ySpacing + 2) * ((float)nSize + eqItems[hovering].first.buffInfoText.size()) });
			break;
		}
		case Hovering::ITM:
		{
			int nSize = itemSlots[hovering].nameInfoText.size() + 3;
			int ln = nSize + itemSlots[hovering].baseInfoText.size() + 2;
			float slotH = (ySpacing + 2) * ((float)ln + itemSlots[hovering].buffInfoText.size());
			if (mousePos.y + slotH > game->windowSize.y)
				mousePos.y = game->windowSize.y - slotH;
			for (int i = 0; i < itemSlots[hovering].nameInfoText.size(); i++)
			{
				textL = itemSlots[hovering].nameInfoText[i].getString().getSize();
				offset = (((itemSlots[hovering].maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				itemSlots[hovering].nameInfoText[i].setPosition(mousePos.x + offset, mousePos.y + (i*ySpacing));
			}

			textL = itemSlots[hovering].slotTypeText.getString().getSize();
			offset = (((itemSlots[hovering].maxInfoChar + 1) - textL) / 2.f)*(charWidth);
			itemSlots[hovering].slotTypeText.setPosition(mousePos.x + offset, mousePos.y + ((nSize - 2)*ySpacing));
			int baseSize = itemSlots[hovering].baseInfoText.size();
			for (int i = 0; i < baseSize; i++)
			{
				textL = itemSlots[hovering].baseInfoText[i].getString().getSize();
				offset = (((itemSlots[hovering].maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				itemSlots[hovering].baseInfoText[i].setPosition(mousePos.x + offset, mousePos.y + ((i + nSize)*ySpacing));
			}
			itemSlots[hovering].seperatorText.setPosition(mousePos.x + (infoWidth / 2) - (charWidth * 2), mousePos.y + ((baseSize + nSize)*ySpacing));
			nSize += itemSlots[hovering].baseInfoText.size() + 2;
			for (int i = 0; i < itemSlots[hovering].buffInfoText.size(); i++)
			{
				textL = itemSlots[hovering].buffInfoText[i].getString().getSize();
				offset = (((itemSlots[hovering].maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				itemSlots[hovering].buffInfoText[i].setPosition(mousePos.x + offset, mousePos.y + ((i + nSize)*ySpacing));
			}
			itemInfoBack.setPosition(mousePos);
			itemSlots[hovering].infoBack.setPosition(mousePos);
			itemSlots[hovering].infoBack.setSize({ infoWidth, (ySpacing + 2) * ((float)nSize + itemSlots[hovering].buffInfoText.size()) });
			break;
		}
		case Hovering::EQS:
		{
			int nSize = eqScrolls[hovering].first.nameInfoText.size() + 3;
			int ln = nSize + eqScrolls[hovering].first.baseInfoText.size() + 2;
			float slotH = (ySpacing + 2) * ((float)ln + eqScrolls[hovering].first.buffInfoText.size()) + tSize;
			if (mousePos.y + slotH > game->windowSize.y)
				mousePos.y = game->windowSize.y - slotH;
			for (int i = 0; i < eqScrolls[hovering].first.nameInfoText.size(); i++)
			{
				textL = eqScrolls[hovering].first.nameInfoText[i].getString().getSize();
				offset = (((eqScrolls[hovering].first.maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				eqScrolls[hovering].first.nameInfoText[i].setPosition(mousePos.x + offset, mousePos.y + (i*ySpacing));
			}
			textL = eqScrolls[hovering].first.slotTypeText.getString().getSize();
			offset = (((eqScrolls[hovering].first.maxInfoChar + 1) - textL) / 2.f)*(charWidth);
			eqScrolls[hovering].first.slotTypeText.setPosition(mousePos.x + offset, mousePos.y + ((nSize - 2)*ySpacing));
			/*	for (int i = 0; i < eqScrolls[hovering].first.baseInfoText.size(); i++)
				{
					textL = eqScrolls[hovering].first.baseInfoText[i].getString().getSize();
					offset = (((eqScrolls[hovering].first.maxInfoChar + 1) - textL) / 2.f)*(charWidth);
					eqScrolls[hovering].first.baseInfoText[i].setPosition(mousePos.x + offset, mousePos.y + ((i + nSize)*ySpacing));
				}
				nSize += eqScrolls[hovering].first.baseInfoText.size() + 1;*/
			for (int i = 0; i < eqScrolls[hovering].first.buffInfoText.size(); i++)
			{
				textL = eqScrolls[hovering].first.buffInfoText[i].getString().getSize();
				offset = (((eqScrolls[hovering].first.maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				eqScrolls[hovering].first.buffInfoText[i].setPosition(mousePos.x + offset, mousePos.y + ((i + nSize)*ySpacing));
			}
			itemInfoBack.setPosition(mousePos);
			eqScrolls[hovering].first.infoBack.setPosition(mousePos);
			eqScrolls[hovering].first.infoBack.setSize({ infoWidth, (ySpacing + 2) * ((float)nSize + eqScrolls[hovering].first.buffInfoText.size()) });
			break;
		}
		case Hovering::SCR:
		{
			int nSize = scrollSlots[hovering].nameInfoText.size() + 3;
			int ln = nSize + scrollSlots[hovering].baseInfoText.size() + 2;
			float slotH = (ySpacing + 2) * ((float)ln + scrollSlots[hovering].buffInfoText.size());
			if (mousePos.y + slotH > game->windowSize.y)
				mousePos.y = game->windowSize.y - slotH;
			for (int i = 0; i < scrollSlots[hovering].nameInfoText.size(); i++)
			{
				textL = scrollSlots[hovering].nameInfoText[i].getString().getSize();
				offset = (((scrollSlots[hovering].maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				scrollSlots[hovering].nameInfoText[i].setPosition(mousePos.x + offset, mousePos.y + (i*ySpacing));
			}
			textL = scrollSlots[hovering].slotTypeText.getString().getSize();
			offset = (((scrollSlots[hovering].maxInfoChar + 1) - textL) / 2.f)*(charWidth);
			scrollSlots[hovering].slotTypeText.setPosition(mousePos.x + offset, mousePos.y + ((nSize - 2)*ySpacing));
			/*	for (int i = 0; i < scrollSlots[hovering].baseInfoText.size(); i++)
				{
					textL = scrollSlots[hovering].baseInfoText[i].getString().getSize();
					offset = (((scrollSlots[hovering].maxInfoChar + 1) - textL) / 2.f)*(charWidth);
					scrollSlots[hovering].baseInfoText[i].setPosition(mousePos.x + offset, mousePos.y + ((i + nSize)*ySpacing));
				}
				nSize += scrollSlots[hovering].baseInfoText.size() + 1;*/
			for (int i = 0; i < scrollSlots[hovering].buffInfoText.size(); i++)
			{
				textL = scrollSlots[hovering].buffInfoText[i].getString().getSize();
				offset = (((scrollSlots[hovering].maxInfoChar + 1) - textL) / 2.f)*(charWidth);
				scrollSlots[hovering].buffInfoText[i].setPosition(mousePos.x + offset, mousePos.y + ((i + nSize)*ySpacing));
			}
			itemInfoBack.setPosition(mousePos);
			scrollSlots[hovering].infoBack.setPosition(mousePos);
			scrollSlots[hovering].infoBack.setSize({ infoWidth, (ySpacing + 2) * ((float)nSize + scrollSlots[hovering].buffInfoText.size()) });
			break;
		}
		default:
			break;
		}
	}
}

void PlayerInfo::draw()
{
	for (int i = 0; i < info.size(); i++)
	{
		this->game->window.draw(info[i]);
	}
	for (int i = 0; i < this->abilities.size(); i++)
	{
		for (int j = 0; j < this->abilities[i].size(); j++)
		{
			this->game->window.draw(this->abilities[i][j]);
		}
	}
	this->game->window.draw(title);
}
