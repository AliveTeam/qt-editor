#include "model.hpp"
#include <jsonxx.h>
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

    jsonxx::Array collisions = ReadArray(map, "collisions");
    for (size_t i = 0; i < collisions.size(); i++)
    {
        jsonxx::Object collision = collisions.get<jsonxx::Object>(i);

        if (mMapInfo.mGame == "AO")
        {
            auto tmpCollision = std::make_unique<CollisionAO>();
            tmpCollision->mNext = ReadNumber(collision, "next");
            tmpCollision->mPrevious = ReadNumber(collision, "previous");
            tmpCollision->mType = ReadNumber(collision, "type");
            tmpCollision->mPos.mX1 = ReadNumber(collision, "x1");
            tmpCollision->mPos.mX2 = ReadNumber(collision, "x2");
            tmpCollision->mPos.mY1 = ReadNumber(collision, "y1");
            tmpCollision->mPos.mY2 = ReadNumber(collision, "y2");
            mCollisions.push_back(std::move(tmpCollision));
        }
        else if (mMapInfo.mGame == "AE")
        {
            auto tmpCollision = std::make_unique<CollisionAE>();
            tmpCollision->mLength = ReadNumber(collision, "length");
            tmpCollision->mNext1 = ReadNumber(collision, "next");
            tmpCollision->mNext2 = ReadNumber(collision, "next2");
            tmpCollision->mPrevious1 = ReadNumber(collision, "previous");
            tmpCollision->mPrevious2 = ReadNumber(collision, "previous2");
            tmpCollision->mType = ReadNumber(collision, "type");
            tmpCollision->mPos.mX1 = ReadNumber(collision, "x1");
            tmpCollision->mPos.mX2 = ReadNumber(collision, "x2");
            tmpCollision->mPos.mY1 = ReadNumber(collision, "y1");
            tmpCollision->mPos.mY2 = ReadNumber(collision, "y2");
            mCollisions.push_back(std::move(tmpCollision));
        }
        else
        {
            throw InvalidGameException(mMapInfo.mGame);
        }
    }

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
        auto tmpObjectStructure = std::make_unique<ObjectStructure>();
        tmpObjectStructure->mName = ReadString(objectStructure, "name");

        jsonxx::Array enumAndBasicTypes = ReadArray(objectStructure, "enum_and_basic_type_properties");
        for (size_t j = 0; j < enumAndBasicTypes.size(); j++)
        {
            jsonxx::Object enumOrBasicType = enumAndBasicTypes.get<jsonxx::Object>(j);
            EnumOrBasicTypeProperty tmpEnumOrBasicTypeProperty;
            tmpEnumOrBasicTypeProperty.mName = ReadString(enumOrBasicType, "name");
            tmpEnumOrBasicTypeProperty.mType = ReadString(enumOrBasicType, "Type");
            tmpEnumOrBasicTypeProperty.mVisible = ReadBool(enumOrBasicType, "Visible");
            tmpObjectStructure->mEnumAndBasicTypeProperties.push_back(tmpEnumOrBasicTypeProperty);
        }
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
                tmpMapObject->mWidth = ReadNumber(mapObject, "width");
                tmpMapObject->mHeight = ReadNumber(mapObject, "height");
                tmpMapObject->mXPos = ReadNumber(mapObject, "xpos");
                tmpMapObject->mYPos = ReadNumber(mapObject, "ypos");
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
                    for (const EnumOrBasicTypeProperty& property : pObjStructure->mEnumAndBasicTypeProperties)
                    {
                        const FoundType foundTypes = FindType(property.mType);
                        if (!foundTypes.mEnum && !foundTypes.mBasicType)
                        {
                            // corrupted schema type name has no definition
                            throw ObjectPropertyTypeNotFoundException(property.mName, property.mType);
                        }
                        
                        auto tmpProperty = std::make_unique<MapObjectProperty>();
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

                        tmpMapObject->mProperties.push_back(std::move(tmpProperty));
                    }
                }

                tmpCamera->mMapObjects.push_back(std::move(tmpMapObject));
            }
        }
        mCameras.push_back(std::move(tmpCamera));
    }
}
