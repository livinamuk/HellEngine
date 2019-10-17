#include "hellpch.h"
#include "File.h"
#include <filesystem>
#include <sstream> 
#include <HellEngine/Util.h> 

namespace HellEngine {

	std::vector<std::string> File::MapList;

	File::File()
	{
	}


	File::~File()
	{
	}

	void File::LoadMapNames()
	{
		// Look for maps
		std::string path = "res/maps/";
		for (const auto & entry : std::filesystem::directory_iterator(path))
		{
			std::stringstream ss;
			ss << entry.path();
			std::string filename = ss.str();
			filename = Util::RemoveCharactersFromEnd(filename, 1);
			filename = Util::RemoveCharactersFromBeginning(filename, path.length() + 1);
			MapList.push_back(filename);
		}
	}

	House File::LoadMap(std::string filename)
	{
		// Load file
		std::string fileName = "res/maps/" + filename;
		FILE* pFile = fopen(fileName.c_str(), "rb");
		char buffer[65536];
		rapidjson::FileReadStream is(pFile, buffer, sizeof(buffer));
		rapidjson::Document document;
		document.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);

		// Check for errors
		if (document.HasParseError())
			std::cout << "Error  : " << document.GetParseError() << '\n' << "Offset : " << document.GetErrorOffset() << '\n';

		// Hardcode type names
		static const char* kTypeNames[] =
		{ "Null", "False", "True", "Object", "Array", "String", "Number" };

		House house;
		house.position = glm::vec3(0, 0, 0);

		// Rooms
		if (document.HasMember("ROOMS"))
		{
			const rapidjson::Value& rooms = document["ROOMS"];
			for (rapidjson::SizeType i = 0; i < rooms.Size(); i++)
			{
				glm::vec3 cornerA = ReadVec3(rooms[i], "CornerA");
				glm::vec3 cornerB = ReadVec3(rooms[i], "CornerB");
				std::string floorMaterialName = ReadString(rooms[i], "FloorMaterial");
				std::string ceilingMaterialName = ReadString(rooms[i], "CeilingMaterial");
				std::string wallMaterialName = ReadString(rooms[i], "WallMaterial");
				bool rotateFloor = ReadBool(rooms[i], "RotateFloorTexture");
				bool rotateCeiling = ReadBool(rooms[i], "RotateCeilingTexture");
				glm::vec3 lightPosition = ReadVec3(rooms[i], "LightPosition");
				glm::vec3 lightColor = ReadVec3(rooms[i], "LightColor");
				float LightAttConstant = ReadFloat(rooms[i], "LightAttConstant");
				float LightAttLinear = ReadFloat(rooms[i], "LightAttLinear");
				float LightAttExp = ReadFloat(rooms[i], "LightAttExp");
				float LightStrength = ReadFloat(rooms[i], "LightStrength");
				Light light = Light(lightPosition, lightColor, LightAttConstant, LightAttLinear, LightAttExp, LightStrength);
				house.AddRoom(cornerA, cornerB, wallMaterialName, floorMaterialName, ceilingMaterialName, rotateFloor, rotateCeiling, light);
			}
		}
		// Doors
		if (document.HasMember("DOORS"))
		{
			const rapidjson::Value& rooms = document["DOORS"];
			for (rapidjson::SizeType i = 0; i < rooms.Size(); i++)
			{
				glm::vec3 position = ReadVec3(rooms[i], "Position");
				std::string axis = ReadString(rooms[i], "Axis");
				std::string floorMaterialName = ReadString(rooms[i], "FloorMaterial");
				bool rotateFloor = ReadBool(rooms[i], "RotateFloorTexture");
				bool initiallyOpen = ReadBool(rooms[i], "InitiallyOpen");
				bool initiallyLocked = ReadBool(rooms[i], "InitiallyLocked");
				float maxOpenAngle = ReadFloat(rooms[i], "MaxOpenAngle");
				house.AddDoor(position.x, position.z, Util::StringToAxis(axis), floorMaterialName, initiallyOpen, initiallyLocked, maxOpenAngle, rotateFloor);
			}
		}
		return house;
	}

	void File::SaveMap(std::string filename, House * house)
	{
		rapidjson::Document document;
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
		rapidjson::Value roomsArray(rapidjson::kArrayType);
		rapidjson::Value doorsArray(rapidjson::kArrayType);	
		document.SetObject();

		// Save rooms
		for (Room & room : house->rooms)
		{
			rapidjson::Value roomObject(rapidjson::kObjectType);
			SaveVec3(&roomObject, "CornerA", room.cornerA, allocator);
			SaveVec3(&roomObject, "CornerB", room.cornerB, allocator);
			SaveString(&roomObject, "WallMaterial", room.wallMaterialName, allocator);
			SaveString(&roomObject, "FloorMaterial", room.floor.material->name, allocator);
			SaveString(&roomObject, "CeilingMaterial", room.ceiling.material->name, allocator);
			SaveBool(&roomObject, "RotateFloorTexture", room.floor.rotateTexture, allocator);
			SaveBool(&roomObject, "RotateCeilingTexture", room.ceiling.rotateTexture, allocator);
			SaveVec3(&roomObject, "LightPosition", room.light.position, allocator);;
			SaveVec3(&roomObject, "LightColor", room.light.color, allocator);
			SaveFloat(&roomObject, "LightAttConstant", room.light.attConstant, allocator);
			SaveFloat(&roomObject, "LightAttLinear", room.light.attLinear, allocator);
			SaveFloat(&roomObject, "LightAttExp", room.light.attExp, allocator);
			SaveFloat(&roomObject, "LightStrength", room.light.strength, allocator);
			roomsArray.PushBack(roomObject, allocator);
		}		
		
		// Save doors
		for (Door & door : house->doors)
		{
			rapidjson::Value doorObject(rapidjson::kObjectType);
			SaveVec3(&doorObject, "Position", door.position, allocator);
			SaveString(&doorObject, "Axis", Util::AxisToString(door.axis), allocator);
			SaveString(&doorObject, "FloorMaterial", door.floor.material->name, allocator);
			SaveBool(&doorObject, "RotateFloorTexture", door.floor.rotateTexture, allocator);
			SaveBool(&doorObject, "InitiallyOpen", door.initiallyOpen, allocator);
			SaveBool(&doorObject, "InitiallyLocked", door.initiallyLocked, allocator);
			SaveFloat(&doorObject, "MaxOpenAngle", door.maxOpenAngle, allocator);
			doorsArray.PushBack(doorObject, allocator);
		}

		// Add arrays
		document.AddMember("ROOMS", roomsArray, allocator);
		document.AddMember("DOORS", doorsArray, allocator);

		// Convert JSON document to string
		rapidjson::StringBuffer strbuf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
		document.Accept(writer);

		// Save it
		std::string data = strbuf.GetString();
		std::ofstream out("res/maps/" + filename);
		out << data;
		out.close();
	}

	void File::SaveVec3(rapidjson::Value* object, std::string elementName, glm::vec3 vector, rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value array(rapidjson::kArrayType);
		rapidjson::Value name(elementName.c_str(), allocator);
		array.PushBack(rapidjson::Value().SetFloat(vector.x), allocator);
		array.PushBack(rapidjson::Value().SetFloat(vector.y), allocator);
		array.PushBack(rapidjson::Value().SetFloat(vector.z), allocator);
		object->AddMember(name, array, allocator);
	}

	void File::SaveString(rapidjson::Value* object, std::string elementName, std::string string, rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value name(elementName.c_str(), allocator);
		rapidjson::Value value(rapidjson::kObjectType);
		value.SetString(string.c_str(), static_cast<rapidjson::SizeType>(string.length()), allocator);
		object->AddMember(name, value, allocator);
	}

	void File::SaveBool(rapidjson::Value* object, std::string elementName, bool boolean, rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value name(elementName.c_str(), allocator);
		rapidjson::Value value(rapidjson::kObjectType);
		value.SetBool(boolean);
		object->AddMember(name, value, allocator);
	}

	void File::SaveFloat(rapidjson::Value* object, std::string elementName, float number, rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value name(elementName.c_str(), allocator);
		rapidjson::Value value(rapidjson::kObjectType);
		value.SetFloat(number);
		object->AddMember(name, value, allocator);
	}


	glm::vec3 File::ReadVec3(const rapidjson::Value & value, std::string name)
	{
		glm::vec3 v = glm::vec3(0, 0, 0);
		if (value.HasMember(name.c_str())) {
			const rapidjson::Value& element = value[name.c_str()];
			v.x = element[0].GetFloat();
			v.y = element[1].GetFloat();
			v.z = element[2].GetFloat();
		}
		else
			std::cout << "'" << name << "' NOT FOUND IN MAP FILE\n";
		return v;
	}

	std::string File::ReadString(const rapidjson::Value & value, std::string name)
	{
		std::string s = "NOT FOUND";
		if (value.HasMember(name.c_str()))
			s = value[name.c_str()].GetString();
		else
			std::cout << "'" << name << "' NOT FOUND IN MAP FILE\n";
		return s;
	}

	bool File::ReadBool(const rapidjson::Value & value, std::string name)
	{
		bool b = false;
		if (value.HasMember(name.c_str()))
			b = value[name.c_str()].GetBool();
		else
			std::cout << "'" << name << "' NOT FOUND IN MAP FILE\n";
		return b;
	}

	float File::ReadFloat(const rapidjson::Value & value, std::string name)
	{
		float f = -1;
		
		if (value.HasMember(name.c_str()))
			f = value[name.c_str()].GetFloat();
		else
			std::cout << "'" << name << "' NOT FOUND IN MAP FILE\n";
		return f;
	}
}
