#include "JsonExporterDialog.hpp"

JsonExporterDialog::JsonExporterDialog() :
  DG::ModalDialog(ACAPI_GetOwnResModule(), ExampleDialogResourceId, ACAPI_GetOwnResModule()),
  m_okButton(GetReference(), OKButtonId),
  m_cancelButton(GetReference(), CancelButtonId),
  m_elementTypeFiltersTextEdit(GetReference(), ElementTypeFiltersTextEditId),
  m_exportPathTextEdit(GetReference(), ExportPathTextEditId),
  m_userDefinedCheckbox(GetReference(), UserDefinedCheckboxId),
  m_fundamentalCheckbox(GetReference(), FundamentalCheckboxId),
  m_userLevelCheckbox(GetReference(), UserLevelCheckboxId),
  m_allPropertyCheckbox(GetReference(), AllPropertyCheckboxId),
  m_separator(GetReference(), SeparatorId),
  m_exporter()
{
  AttachToAllItems(*this);
  Attach(*this);

  FillElementTypeFilters();
  m_exportPathTextEdit.SetText("Enter file path...");
}

JsonExporterDialog::~JsonExporterDialog()
{
  Detach(*this);
  DetachFromAllItems(*this);
}


void JsonExporterDialog::ButtonClicked(const DG::ButtonClickEvent& ev)
{
  if (ev.GetSource() == &m_okButton)
  {
    // Parse and export all valid elements to JSON and close dialog
    StartExport();
    PostCloseRequest(DG::ModalDialog::Accept);
  }
  else if (ev.GetSource() == &m_cancelButton)
  {
    // Close dialog without any further action
    PostCloseRequest(DG::ModalDialog::Cancel);
  }
}

void JsonExporterDialog::FillElementTypeFilters()
{
  // Obtain all available element types and fill the type filters
  // with comma separated options
  GS::UniString allElementTypesStr;
  for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
  {
    API_ElemType elemType = API_ElemTypeID(i);
    GS::UniString elemName;
    if (ACAPI_Element_GetElemTypeName(elemType, elemName) != NoError)
    {
      continue;
    }

    allElementTypesStr += elemName;
    if (i < API_LastElemType)
    {
      allElementTypesStr += ", ";
    }
  }

  m_elementTypeFiltersTextEdit.SetText(allElementTypesStr);
}

void JsonExporterDialog::StartExport()
{
  auto elemTypes = GetElementTypes();
  auto propertyFilters = GetPropertyDefinitionFilters();
  m_exporter.Parse(elemTypes, propertyFilters);

  std::string filepath = m_exportPathTextEdit.GetText().ToCStr();
  m_exporter.Export(filepath);
}

std::vector<API_ElemTypeID> JsonExporterDialog::GetElementTypes()
{
  // Create map of element type names to ids

  std::unordered_map<std::string, API_ElemTypeID> allElemTypes;
  for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
  {
    API_ElemTypeID elemTypeId = API_ElemTypeID(i);
    API_ElemType elemType = elemTypeId;
    GS::UniString elemName;
    if (ACAPI_Element_GetElemTypeName(elemType, elemName) == NoError)
    {
      allElemTypes[elemName.ToCStr().Get()] = elemTypeId;
    }
  }

  // Extract all element names from text edit and build element types list
  std::vector<API_ElemTypeID> filterTypes;
  auto filtersText = m_elementTypeFiltersTextEdit.GetText().Split(",");

  for (GS::UniString filter : filtersText)
  {
    filter.Trim();
    std::string filterStr = filter.ToCStr();
    if (allElemTypes.find(filterStr) != allElemTypes.end())
    {
      filterTypes.push_back(allElemTypes[filterStr]);
    }
  }

  return filterTypes;
}

std::vector<API_PropertyDefinitionFilter> JsonExporterDialog::GetPropertyDefinitionFilters()
{
  // Obtain property definition filters from the option checkboxes
  if (m_allPropertyCheckbox.IsChecked())
  {
    return { API_PropertyDefinitionFilter_All }; // no need to check others
  }

  std::vector<API_PropertyDefinitionFilter> filters;
  if (m_userDefinedCheckbox.IsChecked())
  {
    filters.push_back(API_PropertyDefinitionFilter_UserDefined);
  }
  if (m_fundamentalCheckbox.IsChecked())
  {
    filters.push_back(API_PropertyDefinitionFilter_FundamentalBuiltIn);
  }
  if (m_userLevelCheckbox.IsChecked())
  {
    filters.push_back(API_PropertyDefinitionFilter_UserLevelBuiltIn);
  }

  // Default to all if none are checked
  if (filters.empty())
  {
    filters.push_back(API_PropertyDefinitionFilter_All);
  }

  return filters;
}