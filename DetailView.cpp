#include "common.h"
#include "DetailView.h"
BEGIN_EVENT_TABLE(DetailView, wxPropertyGridManager)
EVT_TOOL(DetailView::ID_UPDATE, DetailView::OnUpdate)
END_EVENT_TABLE()


static constexpr std::array<std::string_view, 3> cls{ "POM", "P", "OTC" };
static inline int findClassIndex(const std::string& cl) {
	//how to do this
	for (int i = 0; i < cls.size(); i++) {
		if (cl == cls[i]) return i;
	}
	return -1;
}

DetailView::DetailView(typename Products::elem_t<Products::id> product_id, wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style)
: wxPropertyGridManager(parent, id, position, size, style), mCreated(false){
	CreatePropertyGrid(product_id);
}

DetailView::~DetailView()
{
	Clear();
}

void DetailView::CreatePropertyGrid(typename Products::elem_t<Products::id> product_id)
{
	auto product = ProductInstance::instance().find_on<Products::id>(product_id);
	if (product == ProductInstance::instance().end())
	{
		wxMessageBox("Invalid Product selected", "DETAILS", wxICON_WARNING | wxOK);
		return;
	}
	//assume ProductView has loaded the value into the ProductDetail table
	auto product_detail = ProductDetailsInstance::instance().find_on<ProductDetails::id>(product_id);
	if (product_detail == ProductDetailsInstance::instance().end()){
		//load from database instead of failing
		wxMessageBox("Invalid product selected, no product details", "DETAILS", wxICON_WARNING | wxOK);
		return;
	}
	CreateToolBar();
	auto page = AddPage("Product", wxArtProvider::GetBitmap("file"));
	page->Append(new wxPropertyCategory("Product"))->SetHelpString(fmt::format("Product details for {}", nl::row_value<Products::name>(*product)));
	page->Append(new wxStringProperty("Product name", wxPG_LABEL, nl::row_value<Products::name>(*product)))->SetHelpString("The name of the product");
	page->Append(new wxIntProperty("Product package size", wxPG_LABEL, nl::row_value<Products::package_size>(*product)))->SetHelpString("The package size eg the number of tablets in a single pack");
	auto stock = page->Append(new wxIntProperty("Product stock count", wxPG_LABEL, nl::row_value<Products::stock_count>(*product)));
	stock->SetHelpString("The stock count");
	stock->Enable(false);
	auto f = 0.0f;
	try {
		f = std::atof(nl::row_value<Products::unit_price>(*product).c_str());
	}
	catch (...) {
		spdlog::get("log")->critical("Formart of Unit price is wrong  for {}", nl::row_value<Products::name>(*product));
	}
	auto unit_price = page->Append(new wxFloatProperty("Product unit price", wxPG_LABEL, static_cast<double>(f))); 
	unit_price->SetHelpString("The current sale price for the product");
	//TODO: set a float validator for the unitprice string property

	page->Append(new wxPropertyCategory("Product details"));
	
	wxPGChoices* choices = new wxPGChoices();
	choices->Add("POM", 0);
	choices->Add("P", 1);
	choices->Add("OTC", 2);

	page->Append(new wxEnumProperty("Product class", wxPG_LABEL, *choices,
		findClassIndex(nl::row_value<ProductDetails::p_class>(*product_detail))))->SetHelpString("Pharamacy products are divided into POM(prescription only medicines), P(general pharmacy) and OTC(over the counter), select the class for this product.");
	page->Append(new wxStringProperty("Product active ingredient",
		wxPG_LABEL, nl::row_value<ProductDetails::active_ing>(*product_detail)))->SetHelpString("The active ingredient in this medication.");
	page->Append(new wxLongStringProperty("Product description",
		wxPG_LABEL, nl::row_value<ProductDetails::description>(*product_detail)))->SetHelpString("A detail description for this medication.");
	page->Append(new wxLongStringProperty("Product direction for use", wxPG_LABEL,
		nl::row_value<ProductDetails::dir_for_use>(*product_detail)))->SetHelpString("A detail direction of how to use the product.");
	std::stringstream stream(nl::row_value<ProductDetails::health_conditions>(*product_detail), std::ios::in);
	wxArrayString conditions;
	std::string value;
	while (!stream.eof()){
		std::getline(stream, value, ',');
		conditions.push_back(value);
	}
	page->Append(new wxArrayStringProperty("Health conditions", wxPG_LABEL, conditions))->SetHelpString("Some conditions that the product can be used for, aid look up by disease conditions.");
	page->Append(new wxPropertyCategory("Product settings"));
	page->Append(new wxIntProperty("Minimum stock count", wxPG_LABEL, nl::row_value<ProductDetails::stock_limit>(*product_detail)))->SetHelpString("Minimum stock count that should be flagged as low stock for this product.");

	CreatePropertyProductCallback(product);
	CreatePropertyProductDetailCallback(product_detail);
	CreateGridPageArt();
	mCreated = true;
}

void DetailView::CreateToolBar()
{
	auto tool = GetToolBar();
	if (tool){
		tool->SetWindowStyleFlag(wxTB_HORZ_TEXT);
		tool->AddTool(ID_UPDATE, "Update", wxArtProvider::GetBitmap("save"));
		tool->SetBackgroundColour(*wxWHITE);
		tool->Realize();
		Update();
	}
}

void DetailView::CreateGridPageArt()
{
	auto grid = GetPage("Product")->GetGrid();
	grid->SetBackgroundColour(*wxWHITE);
	grid->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->SetCaptionTextColour(*wxBLACK);
}

void DetailView::CreatePropertyProductCallback(Products::iterator iter)
{
	mPropertyToValueCallback.insert({ "Product name", std::bind(
		StringProperty<Products>, std::placeholders::_1, Products::name, ProductInstance::instance(), iter)});
	mPropertyToValueCallback.insert({ "Product package size", std::bind(
		IntProperty<Products>, std::placeholders::_1, Products::package_size, ProductInstance::instance(), iter) });
	mPropertyToValueCallback.insert({ "Product stock count", std::bind(
		LongLongProperty<Products>, std::placeholders::_1, Products::stock_count, ProductInstance::instance(), iter) });
	mPropertyToValueCallback.insert({ "Product unit price", std::bind(
		FloatToStringProperty<Products>, std::placeholders::_1, Products::unit_price, ProductInstance::instance(), iter) });
}

void DetailView::CreatePropertyProductDetailCallback(ProductDetails::iterator iter)
{
	mPropertyToValueCallback.insert({ "Product class", std::bind(EnumProperty<ProductDetails, decltype(cls)>, 
		std::placeholders::_1, ProductDetails::p_class, ProductDetailsInstance::instance(), iter, cls)});
	mPropertyToValueCallback.insert({ "Product active ingredient", std::bind(StringProperty<ProductDetails>,
		std::placeholders::_1, ProductDetails::active_ing, ProductDetailsInstance::instance(), iter) });
	mPropertyToValueCallback.insert({ "Product description", std::bind(StringProperty<ProductDetails>,
		std::placeholders::_1, ProductDetails::description, ProductDetailsInstance::instance(), iter) });
	mPropertyToValueCallback.insert({ "Product direction for use", std::bind(StringProperty<ProductDetails>,
		std::placeholders::_1, ProductDetails::dir_for_use, ProductDetailsInstance::instance(), iter) });
	mPropertyToValueCallback.insert({ "Health conditions", std::bind(ArrayStringProperty<ProductDetails>,
		std::placeholders::_1, ProductDetails::health_conditions, ProductDetailsInstance::instance(), iter) });
	mPropertyToValueCallback.insert({ "Minimum stock count", std::bind(IntProperty<ProductDetails>,
		std::placeholders::_1, ProductDetails::stock_limit, ProductDetailsInstance::instance(), iter) });
}


void DetailView::LoadDataIntoGrid(typename Products::elem_t<Products::id> product_id)
{
	auto product = ProductInstance::instance().find_on<Products::id>(product_id);
	if (product == ProductInstance::instance().end())
	{
		wxMessageBox("Invalid Product selected", "DETAILS", wxICON_WARNING | wxOK);
		return;
	}
	ResetModifiedFlag();
	//assume ProductView has loaded the value into the ProductDetail table
	auto product_detail = ProductDetailsInstance::instance().find_on<ProductDetails::id>(product_id);
	if (product_detail == ProductDetailsInstance::instance().end()) {
		wxMessageBox("Invalid product selected, no product details", "DETAILS", wxICON_WARNING | wxOK);
		return;
	}
	Freeze();
	auto page = GetPage("Product");
	if (page){
		page->GetProperty("Product")->SetHelpString(fmt::format("Product details for {}", nl::row_value<Products::name>(*product)));
		page->GetProperty("Product name")->SetValue(nl::row_value<Products::name>(*product));
		page->GetProperty("Product package size")->SetValue(wxLongLong(nl::row_value<Products::package_size>(*product)));
		page->GetProperty("Product stock count")->SetValue(wxLongLong(nl::row_value<Products::stock_count>(*product)));
		page->GetProperty("Product unit price")->SetValue(nl::row_value<Products::unit_price>(*product));
		page->GetProperty("Product class")->SetValue(nl::row_value<ProductDetails::p_class>(*product_detail));
		page->GetProperty("Product active ingredient")->SetValue(nl::row_value<ProductDetails::active_ing>(*product_detail));
		page->GetProperty("Product description")->SetValue(nl::row_value<ProductDetails::description>(*product_detail));
		page->GetProperty("Product direction for use")->SetValue(nl::row_value<ProductDetails::dir_for_use>(*product_detail));
		
		std::stringstream stream(nl::row_value<ProductDetails::health_conditions>(*product_detail), std::ios::in);
		wxArrayString conditions;
		std::string value;
		while (!stream.eof()) {
			std::getline(stream, value, ',');
			conditions.push_back(value);
		}
		page->GetProperty("Health conditions")->SetValue(conditions);
		page->GetProperty("Minimum stock count")->SetValue((int)nl::row_value<ProductDetails::stock_limit>(*product_detail));

		mPropertyToValueCallback.clear();
		CreatePropertyProductCallback(product);
		CreatePropertyProductDetailCallback(product_detail);
	}
	else{
		spdlog::get("log")->error("Cannot get page products");
	}
	Thaw();
	Update();
}

void DetailView::OnUpdate(wxCommandEvent& evt)
{
	auto grid = GetPage("Product")->GetGrid();
	if (grid->IsAnyModified()){
		wxPropertyGridIterator iter = grid->GetIterator();
		while (!iter.AtEnd()){
			if (!iter.GetProperty()->IsCategory() && iter.GetProperty()->HasFlag(wxPG_PROP_MODIFIED)){
				spdlog::get("log")->info("The {} is modified", iter.GetProperty()->GetName().ToStdString());
				auto callback_iter = mPropertyToValueCallback.find(iter.GetProperty()->GetName().ToStdString());
				if (callback_iter != mPropertyToValueCallback.end()) {
					callback_iter->second(iter.GetProperty()->GetValue());
				}
				iter.GetProperty()->SetFlagRecursively(wxPG_PROP_MODIFIED, false);
			}
			iter.Next();
		}
		wxMessageBox(fmt::format("{} has been updated",
			grid->GetProperty("Product name")->GetValue().GetString().ToStdString()), "PRODUCT EDIT", wxICON_INFORMATION | wxOK);
		grid->ClearModifiedStatus();
		Update();
	}
}

void DetailView::ResetModifiedFlag()
{
	auto grid = GetPage("Product")->GetGrid();
	wxPropertyGridIterator iter = grid->GetIterator();
	while (!iter.AtEnd()){
		iter.GetProperty()->SetFlagRecursively(wxPG_PROP_MODIFIED, false);
		iter.Next();
	}
	grid->ClearModifiedStatus();
}
