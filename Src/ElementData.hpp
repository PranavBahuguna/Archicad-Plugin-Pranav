#pragma once

/**
 * @brief Defines the key info for a given element
 */
struct ElementData
{
  API_Guid elemGuid;
  API_ElemTypeID elemTypeId;
  GS::UniString layerName;
  GS::Array<API_Property> properties;
};