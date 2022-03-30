#pragma once

#include "relive_api.hpp"
#include <functional>
#include <QString>

// TODO: Add more context to each of these errors
template<typename ApiCall>
bool ExecApiCall(ApiCall apiCall, std::function<void(const QString)> onFailure)
{
    try
    {
        apiCall();
    }
    catch (const ReliveAPI::IOReadException& e)
    {
        onFailure(QString("IO read failure: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::IOWriteException& e)
    {
        onFailure(QString("IO write failure: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::UnknownEnumValueException& e)
    {
        onFailure(QString("Unknown enum value: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::IOReadPastEOFException& e)
    {
        onFailure(QString("IO read past EOF: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::EmptyPropertyNameException& e)
    {
        onFailure(QString("Empty property name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::EmptyTypeNameException& e)
    {
        onFailure(QString("Empty type name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::DuplicatePropertyKeyException& e)
    {
        onFailure(QString("Duplicated property key: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::DuplicatePropertyNameException& e)
    {
        onFailure(QString("Duplicated property name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::DuplicateEnumNameException& e)
    {
        onFailure(QString("Duplicated enum name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::PropertyNotFoundException& e)
    {
        onFailure(QString("Property not found: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::InvalidGameException& e)
    {
        onFailure(QString("Invalid game name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::InvalidJsonException& e)
    {
        onFailure(QString("Invalid json, can't parse: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonVersionTooNew& e)
    {
        onFailure(QString("Json version too new: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonVersionTooOld& e)
    {
        onFailure(QString("Json version too old: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::BadCameraNameException& e)
    {
        onFailure(QString("Bad camera name: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonNeedsUpgradingException& e)
    {
        onFailure(QString("Json needs upgrading: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::OpenPathException& e)
    {
        onFailure(QString("Open path failure: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::CollisionsCountChangedException& e)
    {
        onFailure(QString("Collision count changed: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::CameraOutOfBoundsException& e)
    {
        onFailure(QString("Camera out of bounds: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::UnknownStructureTypeException& e)
    {
        onFailure(QString("Unknown structure record: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::WrongTLVLengthException& e)
    {
        onFailure(QString("TLV length is wrong: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::JsonKeyNotFoundException& e)
    {
        onFailure(QString("Missing json key: ") + e.what().c_str());
        return false;
    }
    catch (const ReliveAPI::Exception& e)
    {
        onFailure(QString("Error: ") + e.what().c_str());
        return false;
    }
    return true;
}
