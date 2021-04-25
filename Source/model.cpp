#include "model.hpp"
#include <optional>
#include <fstream>

static std::optional<std::string> LoadFileToString(const std::string& fileName)
{
    std::ifstream fileStream(fileName.c_str());
    if (!fileStream)
    {
        return {};
    }
    std::string outputStr;

    fileStream.seekg(0, std::ios::end);
    outputStr.reserve(static_cast<size_t>(fileStream.tellg()));
    fileStream.seekg(0, std::ios::beg);

    outputStr.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    return outputStr;
}

static jsonxx::Array ReadArray(jsonxx::Object& o, const std::string& key)
{
    if (!o.has<jsonxx::Array>(key))
    {
        throw JsonKeyNotFoundException(key);
    }
    return o.get<jsonxx::Array>(key);
}

static jsonxx::Object ReadObject(jsonxx::Object& o, const std::string& key)
{
    if (!o.has<jsonxx::Object>(key))
    {
        throw JsonKeyNotFoundException(key);
    }
    return o.get<jsonxx::Object>(key);
}

static int ReadNumber(jsonxx::Object& o, const std::string& key)
{
    if (!o.has<jsonxx::Number>(key))
    {
        throw JsonKeyNotFoundException(key);
    }
    return static_cast<int>(o.get<jsonxx::Number>(key));
}

static std::string ReadString(jsonxx::Object& o, const std::string& key)
{
    if (!o.has<jsonxx::String>(key))
    {
        throw JsonKeyNotFoundException(key);
    }
    return o.get<jsonxx::String>(key);
}

static bool ReadBool(jsonxx::Object& o, const std::string& key)
{
    if (!o.has<jsonxx::Boolean>(key))
    {
        throw JsonKeyNotFoundException(key);
    }
    return o.get<jsonxx::Boolean>(key);
}

static std::vector<EnumOrBasicTypeProperty> ReadObjectStructureProperties(jsonxx::Array& enumAndBasicTypes)
{
    std::vector<EnumOrBasicTypeProperty> properties;
    for (size_t j = 0; j < enumAndBasicTypes.size(); j++)
    {
        jsonxx::Object enumOrBasicType = enumAndBasicTypes.get<jsonxx::Object>(j);
        EnumOrBasicTypeProperty tmpEnumOrBasicTypeProperty;
        tmpEnumOrBasicTypeProperty.mName = ReadString(enumOrBasicType, "name");
        tmpEnumOrBasicTypeProperty.mType = ReadString(enumOrBasicType, "Type");
        tmpEnumOrBasicTypeProperty.mVisible = ReadBool(enumOrBasicType, "Visible");
        properties.push_back(tmpEnumOrBasicTypeProperty);
    }
    return properties;
}

static UP_ObjectStructure ReadObjectStructure(jsonxx::Object& objectStructure)
{
    auto tmpObjectStructure = std::make_unique<ObjectStructure>();
    tmpObjectStructure->mName = ReadString(objectStructure, "name");

    jsonxx::Array enumAndBasicTypes = ReadArray(objectStructure, "enum_and_basic_type_properties");
    tmpObjectStructure->mEnumAndBasicTypeProperties = ReadObjectStructureProperties(enumAndBasicTypes);

    return tmpObjectStructure;
}

std::vector<UP_ObjectProperty> Model::ReadProperties(const ObjectStructure* pObjStructure, jsonxx::Object& properties)
{
    std::vector<UP_ObjectProperty> tmpProperties;
    for (const EnumOrBasicTypeProperty& property : pObjStructure->mEnumAndBasicTypeProperties)
    {
        const FoundType foundTypes = FindType(property.mType);
        if (!foundTypes.mEnum && !foundTypes.mBasicType)
        {
            // corrupted schema type name has no definition
            throw ObjectPropertyTypeNotFoundException(property.mName, property.mType);
        }

        auto tmpProperty = std::make_unique<ObjectProperty>();
        tmpProperty->mName = property.mName;
        tmpProperty->mTypeName = property.mType;
        tmpProperty->mVisible = property.mVisible;
        if (foundTypes.mBasicType)
        {
            tmpProperty->mBasicTypeValue = ReadNumber(properties, property.mName);
        }
        else
        {
            tmpProperty->mEnumValue = ReadString(properties, property.mName);
        }

        tmpProperties.push_back(std::move(tmpProperty));
    }
    return tmpProperties;
}


void Model::LoadJson(const std::string& jsonFile)
{
    std::optional<std::string> jsonString = LoadFileToString(jsonFile);
    if (!jsonString)
    {
        throw IOReadException(jsonFile);
    }

    jsonxx::Object root;
    if (!root.parse(*jsonString))
    {
        throw InvalidJsonException();
    }
    
    mMapInfo.mApiVersion = ReadNumber(root, "api_version");
    mMapInfo.mGame = ReadString(root, "game");

    jsonxx::Object map = ReadObject(root, "map");

    mMapInfo.mPathBnd = ReadString(map, "path_bnd");
    mMapInfo.mPathId = ReadNumber(map, "path_id");
    mMapInfo.mXGridSize = ReadNumber(map, "x_grid_size");
    mMapInfo.mXSize = ReadNumber(map, "x_size");
    mMapInfo.mYGridSize = ReadNumber(map, "y_grid_size");
    mMapInfo.mYSize = ReadNumber(map, "y_size");

    jsonxx::Object schema = ReadObject(root, "schema");

    jsonxx::Array basicTypes = ReadArray(schema, "object_structure_property_basic_types");
    for (size_t i = 0; i < basicTypes.size(); i++)
    {
        jsonxx::Object basicType = basicTypes.get<jsonxx::Object>(i);
        auto tmpBasicType = std::make_unique<BasicType>();
        tmpBasicType->mName = ReadString(basicType, "name");
        tmpBasicType->mMaxValue = ReadNumber(basicType, "max_value");
        tmpBasicType->mMinValue = ReadNumber(basicType, "min_value");
        mBasicTypes.push_back(std::move(tmpBasicType));
    }

    jsonxx::Array enums = ReadArray(schema, "object_structure_property_enums");
    for (size_t i = 0; i < enums.size(); i++)
    {
        jsonxx::Object enumObject = enums.get<jsonxx::Object>(i);
        auto tmpEnum = std::make_unique<Enum>();
        tmpEnum->mName = ReadString(enumObject, "name");

        jsonxx::Array enumValuesArray = ReadArray(enumObject, "values");
        for (size_t j = 0; j < enumValuesArray.size(); j++)
        {
            tmpEnum->mValues.push_back(enumValuesArray.get<jsonxx::String>(j));
        }
        mEnums.push_back(std::move(tmpEnum));
    }

    jsonxx::Array objectStructures = ReadArray(schema, "object_structures");
    for (size_t i = 0; i < objectStructures.size(); i++)
    {
        jsonxx::Object objectStructure = objectStructures.get<jsonxx::Object>(i);
        auto tmpObjectStructure = ReadObjectStructure(objectStructure);
        mObjectStructures.push_back(std::move(tmpObjectStructure));
    }

    jsonxx::Array cameras = ReadArray(map, "cameras");
    for (size_t i = 0; i < cameras.size(); i++)
    {
        jsonxx::Object camera = cameras.get<jsonxx::Object>(i);

        auto tmpCamera = std::make_unique<Camera>();
        tmpCamera->mId = ReadNumber(camera, "id");
        tmpCamera->mName = ReadString(camera, "name");
        tmpCamera->mX = ReadNumber(camera, "x");
        tmpCamera->mY = ReadNumber(camera, "y");

        if (camera.has<jsonxx::Array>("map_objects"))
        {
            jsonxx::Array mapObjects = ReadArray(camera, "map_objects");

            for (size_t j = 0; j < mapObjects.size(); j++)
            {
                jsonxx::Object mapObject = mapObjects.get<jsonxx::Object>(j);
                auto tmpMapObject = std::make_unique<MapObject>();
                tmpMapObject->mName = ReadString(mapObject, "name");
                tmpMapObject->mObjectStructureType = ReadString(mapObject, "object_structures_type");

                if (mapObject.has<jsonxx::Object>("properties"))
                {
                    const ObjectStructure* pObjStructure = nullptr;
                    for (const auto& objStruct : mObjectStructures)
                    {
                        if (objStruct->mName == tmpMapObject->mObjectStructureType)
                        {
                            pObjStructure = objStruct.get();
                            break;
                        }
                    }

                    if (!pObjStructure)
                    {
                        throw JsonKeyNotFoundException(tmpMapObject->mObjectStructureType);
                    }

                    jsonxx::Object properties = ReadObject(mapObject, "properties");
                    tmpMapObject->mProperties = ReadProperties(pObjStructure, properties);
                }

                tmpCamera->mMapObjects.push_back(std::move(tmpMapObject));
            }
        }
        mCameras.push_back(std::move(tmpCamera));
    }


    jsonxx::Object collisionObject = ReadObject(map, "collisions");
    jsonxx::Array collisionsArray = ReadArray(collisionObject, "items");
    jsonxx::Array collisionStructure = ReadArray(collisionObject, "structure");

    mCollisionStructure= std::make_unique<ObjectStructure>();
    mCollisionStructure->mName = "Collision";
    mCollisionStructure->mEnumAndBasicTypeProperties = ReadObjectStructureProperties(collisionStructure);

    for (size_t i = 0; i < collisionsArray.size(); i++)
    {
        jsonxx::Object collision = collisionsArray.get<jsonxx::Object>(i);

        auto tmpCollision = std::make_unique<CollisionObject>();
        tmpCollision->mProperties = ReadProperties(mCollisionStructure.get(), collision);
        mCollisions.push_back(std::move(tmpCollision));
    }

}
