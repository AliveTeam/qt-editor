#pragma once

#include "relive_api.hpp"
#include "file_api.hpp"
#include <functional>
#include <QString>

// TODO: Implement ReliveAPI::FileIO override that supports unicode from utf8 on windows

// TODO: Add more context to each of these errors
template<typename ApiCall>
bool ExecApiCall(ApiCall apiCall, std::function<void(const QString)> onFailure)
{
    try
    {
        return apiCall();
    }
    catch (const ReliveAPI::IOReadException&)
    {
        onFailure(QString("IO read failure"));
        return false;
    }
    catch (const ReliveAPI::IOWriteException&)
    {
        onFailure(QString("IO write failure"));
        return false;
    }
    catch (const ReliveAPI::IOReadPastEOFException&)
    {
        onFailure(QString("IO read past EOF"));
        return false;
    }
    catch (const ReliveAPI::EmptyPropertyNameException&)
    {
        onFailure(QString("Empty property name"));
        return false;
    }
    catch (const ReliveAPI::EmptyTypeNameException&)
    {
        onFailure(QString("Empty type name"));
        return false;
    }
    catch (const ReliveAPI::DuplicatePropertyKeyException&)
    {
        onFailure(QString("Duplicated property key"));
        return false;
    }
    catch (const ReliveAPI::DuplicatePropertyNameException& e)
    {
        onFailure(QString("Duplicated property name: ") + e.PropertyName().c_str());
        return false;
    }
    catch (const ReliveAPI::DuplicateEnumNameException& e)
    {
        onFailure(QString("Duplicated enum name: ") + e.EnumTypeName().c_str());
        return false;
    }
    catch (const ReliveAPI::PropertyNotFoundException&)
    {
        onFailure(QString("Property not found"));
        return false;
    }
    catch (const ReliveAPI::InvalidGameException& e)
    {
        onFailure(QString("Invalid game name: ") + e.GameName().c_str());
        return false;
    }
    catch (const ReliveAPI::InvalidJsonException&)
    {
        onFailure(QString("Invalid json, can't parse"));
        return false;
    }
    catch (const ReliveAPI::JsonVersionTooNew&)
    {
        onFailure(QString("Json version too new"));
        return false;
    }
    catch (const ReliveAPI::JsonVersionTooOld&)
    {
        onFailure(QString("Json version too old"));
        return false;
    }
    catch (const ReliveAPI::BadCameraNameException& e)
    {
        onFailure(QString("Bad camera name: ") + e.CameraName().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonNeedsUpgradingException& e)
    {
        onFailure(QString("Json needs upgrading from ") + QString::number(e.YourJsonVersion()) + " to " + QString::number(e.CurrentApiVersion()) + " you can export with the previous editor to .lvl and re export to json with this version to fix this");
        return false;
    }
    catch (const ReliveAPI::OpenPathException&)
    {
        onFailure(QString("Open path failure"));
        return false;
    }
    catch (const ReliveAPI::CameraOutOfBoundsException&)
    {
        onFailure(QString("Camera out of bounds"));
        return false;
    }
    catch (const ReliveAPI::UnknownStructureTypeException& e)
    {
        onFailure(QString("Unknown structure record: ") + e.StructureTypeName().c_str());
        return false;
    }
    catch (const ReliveAPI::WrongTLVLengthException&)
    {
        onFailure(QString("TLV length is wrong"));
        return false;
    }
    catch (const ReliveAPI::JsonKeyNotFoundException& e)
    {
        onFailure(QString("Missing json key: ") + e.Key().c_str());
        return false;
    }
    catch (const ReliveAPI::Exception&)
    {
        onFailure(QString("Unknown API exception"));
        return false;
    }
    return true;
}
