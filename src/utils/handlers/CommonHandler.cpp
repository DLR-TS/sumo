/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    CommonHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2024
///
// Collection of functions used in handlers
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/XMLSubSys.h>

#include "CommonHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

CommonHandler::CommonHandler() {
}


CommonHandler::~CommonHandler() {}


bool
CommonHandler::isErrorCreatingElement() const {
    return myErrorCreatingElement;
}


void
CommonHandler::parseParameters(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // get key
    const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, parsedOk);
    // get SumoBaseObject parent
    CommonXMLStructure::SumoBaseObject* SumoBaseObjectParent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
    // check parent
    if ((SumoBaseObjectParent == nullptr) || (SumoBaseObjectParent->getTag() == SUMO_TAG_ROOTFILE)) {
        writeError(TL("Parameters must be defined within an object"));
    } else if (SumoBaseObjectParent->getTag() == SUMO_TAG_PARAM) {
        writeError(TL("Parameters cannot be defined within another parameter."));
    } else if ((SumoBaseObjectParent->getTag() != SUMO_TAG_NOTHING) && parsedOk) {
        // get tag str
        const std::string parentTagStr = toString(SumoBaseObjectParent->getTag());
        // circumventing empty string value
        const std::string value = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
        // show warnings if values are invalid
        if (key.empty()) {
            writeError(TLF("Error parsing key from % generic parameter. Key cannot be empty", parentTagStr));
        } else if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
            writeError(TLF("Error parsing key from % generic parameter. Key contains invalid characters", parentTagStr));
        } else {
            WRITE_DEBUG("Inserting generic parameter '" + key + "|" + value + "' into " + parentTagStr);
            // insert parameter in SumoBaseObjectParent
            SumoBaseObjectParent->addParameter(key, value);
        }
    }
}


void
CommonHandler::checkParsedParent(const SumoXMLTag currentTag, const std::vector<SumoXMLTag>& parentTags, bool& ok) {
    if (parentTags.size() > 0) {
        std::string tagsStr;
        for (auto it = parentTags.begin(); it != parentTags.end(); it++) {
            tagsStr.append(toString(*it));
            if ((it+1) != parentTags.end()) {
                if ((it+2) != parentTags.end()) {
                    tagsStr.append(", ");
                } else {
                    tagsStr.append(" or ");
                }
            }
        }
        // obtain parent
        CommonXMLStructure::SumoBaseObject* const parent = myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject();
        if (parent == nullptr) {
            ok = writeError(TLF("'%' must be defined within the definition of a %.", toString(currentTag), tagsStr));
        } else if ((parent->getTag() != SUMO_TAG_NOTHING) && std::find(parentTags.begin(), parentTags.end(), parent->getTag()) == parentTags.end()) {
            if (parent->hasStringAttribute(SUMO_ATTR_ID)) {
                ok = writeError(TLF("'%' must be defined within the definition of a '%' (found % '%').", toString(currentTag), tagsStr,
                                    toString(parent->getTag()), parent->getStringAttribute(SUMO_ATTR_ID)));
            } else {
                ok = writeError(TLF("'%' must be defined within the definition of a '%' (found %).", toString(currentTag), tagsStr,
                                    toString(parent->getTag())));
            }
        }
    }
}


bool
CommonHandler::checkListOfVehicleTypes(const SumoXMLTag tag, const std::string& id, const std::vector<std::string>& vTypeIDs) {
    for (const auto& vTypeID : vTypeIDs) {
        if (!SUMOXMLDefinitions::isValidTypeID(vTypeID)) {
            return writeError(TLF("Could not build % with ID '%' in netedit; '%' ist not a valid vType ID.", toString(tag), id, vTypeID));
        }
    }
    return true;
}


bool
CommonHandler::checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const int value, const bool canBeZero) {
    if (canBeZero) {
        if (value < 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % cannot be negative.", toString(tag), id, toString(attribute)));
        } else {
            return true;
        }
    } else {
        if (value <= 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % must be greather than zero.", toString(tag), id, toString(attribute)));
        } else {
            return true;
        }
    }
}


bool
CommonHandler::checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const double value, const bool canBeZero) {
    if (canBeZero) {
        if (value < 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % cannot be negative (%).", toString(tag), id, toString(attribute), toString(value)));
        } else {
            return true;
        }
    } else {
        if (value <= 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % must be greather than zero (%).", toString(tag), id, toString(attribute), toString(value)));
        } else {
            return true;
        }
    }
}


bool
CommonHandler::checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const SUMOTime value, const bool canBeZero) {
    if (canBeZero) {
        if (value < 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % cannot be negative (%).", toString(tag), id, toString(attribute), time2string(value)));
        } else {
            return true;
        }
    } else {
        if (value <= 0) {
            return writeError(TLF("Could not build % with ID '%' in netedit; Attribute % must be greather than zero (%).", toString(tag), id, toString(attribute), time2string(value)));
        } else {
            return true;
        }
    }
}


bool
CommonHandler::checkFileName(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const std::string &value) {
    if (SUMOXMLDefinitions::isValidFilename(value)) {
        return true;
    } else {
        return writeError(TLF("Could not build % with ID '%' in netedit; % is invalid % ()", toString(tag), id, toString(attribute), value));
    }
}


bool
CommonHandler::checkValidAdditionalID(const SumoXMLTag tag, const std::string& value) {
    if (value.empty()) {
        return writeError(TLF("Could not build %; ID cannot be empty", toString(tag)));
    } else if (!SUMOXMLDefinitions::isValidVehicleID(value)) {
        return writeError(TLF("Could not build % with ID '%' in netedit; ID contains invalid characters.", toString(tag), value));
    } else {
        return true;
    }
}


bool
CommonHandler::checkValidDetectorID(const SumoXMLTag tag, const std::string& value) {
    if (value.empty()) {
        return writeError(TLF("Could not build %; ID cannot be empty", toString(tag)));
    } else if (!SUMOXMLDefinitions::isValidDetectorID(value)) {
        return writeError(TLF("Could not build % with ID '%' in netedit; detector ID contains invalid characters.", toString(tag), value));
    } else {
        return true;
    }
}


bool
CommonHandler::checkValidDemandElementID(const SumoXMLTag tag, const std::string& value) {
    if (value.empty()) {
        return writeError(TLF("Could not build %; ID cannot be empty", toString(tag)));
    } else if (!SUMOXMLDefinitions::isValidVehicleID(value)) {
        return writeError(TLF("Could not build % with ID '%' in netedit; ID contains invalid characters.", toString(tag), value));
    } else {
        return true;
    }
}


bool
CommonHandler::writeError(const std::string& error) {
    WRITE_ERROR(error);
    myErrorCreatingElement = true;
    return false;
}


bool
CommonHandler::writeErrorInvalidPosition(const SumoXMLTag tag, const std::string& id) {
    return writeError(TLF("Could not build % with ID '%' in netedit; Invalid position over lane.", toString(tag), id));
}


bool
CommonHandler::writeErrorDuplicated(const SumoXMLTag tag, const std::string& id, const SumoXMLTag checkedTag) {
    return writeError(TLF("Could not build % with ID '%' in netedit; Found another % with the same ID.", toString(tag), id, toString(checkedTag)));
}


bool
CommonHandler::writeErrorInvalidLanes(const SumoXMLTag tag, const std::string& id) {
    return writeError(TLF("Could not build % with ID '%' in netedit; List of lanes isn't valid.", toString(tag), id));
}


bool
CommonHandler::writeErrorInvalidDistribution(const SumoXMLTag tag, const std::string& id) {
    return writeError(TLF("Could not build % with ID '%' in netedit; Distinct number of distribution values and probabilities.", toString(tag), id));
}


bool
CommonHandler::writeErrorInvalidParent(const SumoXMLTag tag, const std::string& id, const SumoXMLTag parentTag, const std::string& parentID) {
    return writeError(TLF("Could not build % with ID '%' in netedit; % parent with ID '%' doesn't exist.", toString(tag), id, toString(parentTag), parentID));
}


bool
CommonHandler::writeErrorInvalidParent(const SumoXMLTag tag, const SumoXMLTag parentTag, const std::string& parentID) {
    return writeError(TLF("Could not build % in netedit; % parent with ID '%' doesn't exist.", toString(tag), toString(parentTag), parentID));
}


bool
CommonHandler::writeErrorInvalidParent(const SumoXMLTag tag, const SumoXMLTag parentTag) {
    return writeError(TLF("Could not build % in netedit; % parent doesn't exist.", toString(tag), toString(parentTag)));
}

/****************************************************************************/