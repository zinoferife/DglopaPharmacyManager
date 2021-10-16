#pragma once
#include <wx/wx.h>
#include <wx/dataview.h>

#include "Instances.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <unordered_map>

namespace std
{
	template<>
	struct hash<wxDataViewItem>
	{
		size_t operator()(const wxDataViewItem& value) const noexcept
		{
			std::hash<std::uint32_t> h;
			return h((std::uint32_t)wxPtrToUInt(value.GetID()));
		}
	};
}
//encapsulate the simlar parts of the model in the same parts
//Products Data model, deals with the products Table 
template<typename Data>
class DataModel : public wxDataViewModel
{
	typedef typename Data::template elem_t<Data::id> id_t;
	typedef std::unordered_map<id_t, std::shared_ptr<wxDataViewItemAttr>> itemToAttr_t;
public:
	DataModel(Data& instance)
	: mData(instance), mPastLastAdded(-1){
		mData.sink<nl::notifications::add>().add_listener<DataModel, &DataModel::OnDataAdded>(this);
		mData.sink<nl::notifications::add_multiple>().add_listener<DataModel, &DataModel::OnDataAddedMultiple>(this);
		mData.sink<nl::notifications::load>().add_listener<DataModel, &DataModel::OnDataAddedMultiple>(this);
		mData.sink<nl::notifications::update>().add_listener<DataModel, &DataModel::OnDataUpdated>(this);
		mData.sink<nl::notifications::remove>().add_listener<DataModel, & DataModel::OnDeleted>(this);
		mData.sink<nl::notifications::remove_multiple>().add_listener<DataModel, & DataModel::OnMultipleDeleted>(this);
		mData.sink<nl::notifications::clear>().add_listener<DataModel, & DataModel::OnClear>(this);
	}
	virtual ~DataModel() 
	{
		mData.sink<nl::notifications::add>().remove_listener<DataModel, & DataModel::OnDataAdded>(this);
		mData.sink<nl::notifications::add_multiple>().remove_listener<DataModel, &DataModel::OnDataAddedMultiple>(this);
		mData.sink<nl::notifications::load>().remove_listener<DataModel, &DataModel::OnDataAddedMultiple>(this);
		mData.sink<nl::notifications::update>().remove_listener<DataModel, &DataModel::OnDataUpdated>(this);
		mData.sink<nl::notifications::remove>().remove_listener<DataModel, &DataModel::OnDeleted>(this);
		mData.sink<nl::notifications::remove_multiple>().remove_listener<DataModel, &DataModel::OnMultipleDeleted>(this);
		mData.sink<nl::notifications::clear>().remove_listener<DataModel, &DataModel::OnClear>(this);
	}
	virtual bool HasContainerColumns(const wxDataViewItem& item) const
	{
		return false;
	}
	virtual bool HasDefaultCompare() const
	{
		return true;
	}

	virtual int Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const
	{
		if (column > GetColumnCount()) return 0;
		auto indexIter = mIndexMap.find(item1);
		auto indexIter2 = mIndexMap.find(item2);
		if (indexIter != mIndexMap.end() && indexIter2 != mIndexMap.end())
		{
			typename Data::variant_t variant1;
			typename Data::variant_t variant2;
			mData.get_in_variant(indexIter->second, column, variant1);
			mData.get_in_variant(indexIter2->second, column, variant2);
			if (ascending)
			{
				if (variant1 < variant2) return -1;
				else if (variant1 == variant2) return 0;
				else return 1;
			}else{
				if (variant1 < variant2) return 1;
				else if (variant1 == variant2) return 0;
				else return -1;
			}
		}
		//if items are not represented in model return 0
		return 0;
	}

	virtual bool HasValue(const wxDataViewItem& item, unsigned col) const
	{
		return (mIndexMap.find(item) != mIndexMap.end());
	}
	virtual bool IsContainer(const wxDataViewItem& item) const
	{
		return false;
	}
	virtual bool IsVirtualListModel() const
	{
		return false;
	}
	virtual wxDataViewItem GetParent(const wxDataViewItem& item) const
	{
		return wxDataViewItem(0);
	}
	virtual void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
	{
		if (col < GetColumnCount())
		{
			if (item.IsOk())
			{
				auto indexIter = mIndexMap.find(item);
				if (indexIter == mIndexMap.end()) return;
				size_t index = indexIter->second;
				variant = mData.get_as_string(index, col);
			}
		}
	}
	virtual bool IsEnabled(const wxDataViewItem& item, unsigned int col) const
	{
		return (mIndexMap.find(item) != mIndexMap.end());
	}

	virtual unsigned int GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
	{
		return -1;
	}

	virtual wxString GetColumnType(unsigned int col) const
	{
		if (col < GetColumnCount())
		{
			std::string value = mData.get_column_type_name(col);
			if (value == std::string(nl::get_type_name<std::string>())){
				//wx needs type name of string as just string
				return wxString("string");
			}
			else if (value == std::string(nl::get_type_name<nl::date_time_t>())){
				//date time is keept as long long, although it is displayed as string
				return wxString("long long");
			}

			return value;
		}
		return wxString();
	}

	virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
	{
		if (col < GetColumnCount())
		{
			auto iter = mIndexMap.find(item);
			if (iter != mIndexMap.end())
			{
				const id_t& id = mData.get<Data::id>(iter->second);
				auto attrIter = mAttributeTable.find(id);
				if (attrIter != mAttributeTable.end()){
					attr = *(attrIter->second);
					return true;
				}
			}
		}

		return false;
	}
	virtual unsigned int GetColumnCount() const
	{
		return mData.get_column_count();
	}
	virtual bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
	{
		return false;
	}

	int GetDataViewItemIndex(const wxDataViewItem& item)
	{
		auto iter = mIndexMap.find(item);
		if (iter != mIndexMap.end()) return iter->second;
		return -1;
	}

	void ResetAttributes()
	{
		mAttributeTable.clear();
	}

public:
	
	void OnDataAdded(const typename Data::table_t& table, const typename Data::notification_data& data)
	{
		auto [iter, inserted] = mIndexMap.insert({ wxDataViewItem(UIntToPtr(mData.size())), mData.size() - 1 });
		if (inserted) {
			ItemAdded(wxDataViewItem(0), iter->first);
			mPastLastAdded = mData.size();
		}
	}

	void OnDataAddedMultiple(const typename Data::table_t& table, const typename Data::notification_data& data)
	{
		//assumes that the rows is added at the back of the table, maybe using relation::append_relation
		//undefined(lol) if not appened at the back 
		size_t row_added = data.count;
		if (row_added > 0)
		{
			if (!mIndexMap.empty() || mPastLastAdded != -1)
			{
				wxDataViewItemArray DataViewArray(row_added);
				for (size_t i = 0; i < row_added; i++) {
					size_t index = i + mPastLastAdded;
					auto [iter, inserted] = mIndexMap.insert(std::make_pair(wxDataViewItem(UIntToPtr(index + 1)), index));
					if (inserted) {
						DataViewArray[i] = iter->first;
					}
				}
				if (!DataViewArray.empty()) {
					ItemsAdded(wxDataViewItem(0), DataViewArray);
				}
				//(mPastLastAdded + size) is the new index of the past last element added
				mPastLastAdded += row_added;
			}
			else
			{
				wxDataViewItemArray DataViewArray(row_added);
				for (size_t i = 0; i < row_added; i++) {
					size_t index = i;
					auto [iter, inserted] = mIndexMap.insert(std::make_pair(wxDataViewItem(UIntToPtr(index + 1)), index));
					if (inserted) {
						DataViewArray[i] = iter->first;
					}
				}
				if (!DataViewArray.empty()) {
					ItemsAdded(wxDataViewItem(0), DataViewArray);
				}
				//(mPastLastAdded + size) is the new index of the past last element added
				mPastLastAdded = row_added;
			}
		}
	}

	void OnDataUpdated(const typename Data::table_t& table, const typename Data::notification_data& data)
	{
		size_t row_added = mData.get_index(data.row_iterator);
		auto iter = mIndexMap.find(wxDataViewItem(UIntToPtr(row_added + 1)));
		if (iter == mIndexMap.end()) return;
		ValueChanged(iter->first, data.column);
	}

	void OnClear(const typename Data::table_t& table, const typename Data::notification_data& data)
	{
		wxDataViewItemArray DataArray(mIndexMap.size());
		std::transform(mIndexMap.begin(), mIndexMap.end(), DataArray.begin(),
			[&](const std::pair<const wxDataViewItem, size_t>& index) -> wxDataViewItem {
			return index.first;
		});
		ItemsDeleted(wxDataViewItem(0), DataArray);
		mIndexMap.clear();
		mPastLastAdded = -1;
	}

	void OnDeleted(const typename Data::table_t& table, const typename Data::notification_data& data)
	{
		if (!mIndexMap.empty() || mPastLastAdded > 0)
		{
			//del_row is gonna swap the data in the rows just remove the last element
			ItemDeleted(wxDataViewItem(0), wxDataViewItem(wxUIntToPtr(mPastLastAdded)));
			auto it = mIndexMap.find(wxDataViewItem(wxUIntToPtr(mPastLastAdded)));
			mIndexMap.erase(it);
			mPastLastAdded--;
		}
	}

	void OnMultipleDeleted(const typename Data::table_t& table, const typename Data::notification_data& data)
	{
		if (!mIndexMap.empty() || mPastLastAdded > 0)
		{
			wxDataViewItemArray DataItemArray(data.count);
			std::generate(DataItemArray.begin(), DataItemArray.end(), [&]() {
				return wxDataViewItem(wxUIntToPtr(mPastLastAdded--));
			});
			ItemsDeleted(wxDataViewItem(0), DataItemArray);
		}
	}

//visual attributes
public:
	void AddAttribute(std::shared_ptr<wxDataViewItemAttr> attr, const std::vector<id_t>&& ids)
	{
		for (auto iter = ids.begin(); iter != ids.end(); iter++)
		{
			auto [attrIter, inserted] = mAttributeTable.insert(std::make_pair(*iter, attr));
			if (!inserted){
				//if already in table, means a change in attr
				attrIter->second = attr;
			}
		}
	}

	void AddAttribute(std::shared_ptr<wxDataViewItemAttr> attr, id_t id)
	{
		auto [iter, inserted] = mAttributeTable.insert(std::make_pair(id, attr));
		if (!inserted){
			iter->second = attr;
		}
	}

	void RemoveAttribute(const std::vector<id_t>&& ids)
	{
		for (auto iter = ids.begin(); iter != ids.end(); iter++)
		{
			mAttributeTable.erase(*iter);
		}
	}

	void RemoveAttribute(id_t id)
	{
		mAttributeTable.erase(id);
	}


	void ReloadIndices(const std::vector<size_t>&& indices)
	{
		if (indices.empty()) return;
		for (auto&& i : indices){
			auto [iter, inserted] = mIndexMap.insert({ wxDataViewItem(wxUIntToPtr(i + 1)), i });
			if (inserted) {
				ItemAdded(wxDataViewItem(0), iter->first);
				//this might not be correct
				mPastLastAdded = i + 1;
			}
		}
	}


protected:
	std::unordered_map<wxDataViewItem, size_t> mIndexMap{};
	
	
	//each row can have an attribute type, 
	itemToAttr_t mAttributeTable{};
	Data& mData;

	//one past the last added
	std::int32_t mPastLastAdded;
};
