#pragma once


#include <iostream>


enum class FieldType {
	PASSAGE,
	WALL,
	ENTRANCE,
	EXIT,
	ITEM
};

enum class ItemType {
	SWORD,
	SHIELD,
	HAMMER,
	FOG_OF_WAR
};

class MatrixField {
protected:
	FieldType fieldType;
	char symbol;
	bool walkable;
public:
	MatrixField(FieldType type, char sym, bool walkable)
		: fieldType(type), symbol(sym), walkable(walkable) {}

	virtual ~MatrixField() = default;

	virtual FieldType getFieldType() const = 0;
	virtual char getSymbol() const = 0;
	virtual bool isWalkable() const = 0;

	virtual void onEnter() = 0;
};

class Passage : public MatrixField {
public:
	Passage() : MatrixField(FieldType::PASSAGE, '.', true) {}

	FieldType getFieldType() const override {
		return FieldType::PASSAGE;
	}

	char getSymbol() const override {
		return '.';
	}

	bool isWalkable() const override {
		return true;
	}
};

class Wall : public MatrixField {
public:
	Wall() : MatrixField(FieldType::WALL, '#', false) {}

	FieldType getFieldType() const override {
		return FieldType::WALL;
	}

	char getSymbol() const override {
		return '#';
	}

	bool isWalkable() const override {
		return false;
	}
};

class Entrance : public MatrixField {
public:
	Entrance() : MatrixField(FieldType::ENTRANCE, 'U', true) {}

	FieldType getFieldType() const override {
		return FieldType::ENTRANCE;
	}

	char getSymbol() const override {
		return 'U';
	}

	bool isWalkable() const override {
		return true;
	}
};

class Exit : public MatrixField {
public:
	Exit() : MatrixField(FieldType::EXIT, 'I', true) {}

	FieldType getFieldType() const override {
		return FieldType::EXIT;
	}

	char getSymbol() const override {
		return 'I';
	}

	bool isWalkable() const override {
		return true;
	}

	void onEnter() override {
		std::cout << "You have exited the maze!" << std::endl;
	}
};

class Item : public MatrixField {
protected:
	ItemType itemType;
public:
	Item(ItemType type, char sym, bool walkable)
		: MatrixField(FieldType::ITEM, sym, walkable), itemType(type) {}

	virtual ~Item() = default;

	virtual ItemType getItemType() const = 0;

	FieldType getFieldType() const override {
		return FieldType::ITEM;
	}
};

class Sword : public Item {
public:
	Sword() : Item(ItemType::SWORD, 'M', true) {}

	ItemType getItemType() const override {
		return ItemType::SWORD;
	}

	char getSymbol() const override {
		return 'M';
	}

	bool isWalkable() const override {
		return true;
	}

	void onEnter() override {
		std::cout << "You have picked up a sword!" << std::endl;
	}
};

class Shield : public Item {
public:
	Shield() : Item(ItemType::SHIELD, 'S', true) {}

	ItemType getItemType() const override {
		return ItemType::SHIELD;
	}

	char getSymbol() const override {
		return 'S';
	}

	bool isWalkable() const override {
		return true;
	}

	void onEnter() override {
		std::cout << "You have picked up a shield!" << std::endl;
	}
};

class Hammer : public Item {
public:
	Hammer() : Item(ItemType::HAMMER, 'C', true) {}

	ItemType getItemType() const override {
		return ItemType::HAMMER;
	}

	char getSymbol() const override {
		return 'C';
	}

	bool isWalkable() const override {
		return true;
	}

	void onEnter() override {
		std::cout << "You have picked up a hammer!" << std::endl;
	}
};

class FogOfWar : public Item {
public:
	FogOfWar() : Item(ItemType::FOG_OF_WAR, 'F', true) {}

	ItemType getItemType() const override {
		return ItemType::FOG_OF_WAR;
	}

	char getSymbol() const override {
		return 'F';
	}

	bool isWalkable() const override {
		return true;
	}

	void onEnter() override {
		std::cout << "You have picked up a fog of war!" << std::endl;
	}
};
