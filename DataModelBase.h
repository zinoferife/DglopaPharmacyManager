#pragma once
#include <wx/wx.h>
#include <wx/dataview.h>

#include "Instances.h"
#include <fmt/format.h>


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
		mData.sink().add_listener<DataModel, &DataModel::Notifications>(this);
		mData.update_sink().add_listener<DataModel, &DataModel::UpdateNotifications>(this);
	}
	virtual ~DataModel() 
	{
		mData.sink().remove_listener<DataModel, & DataModel::Notifications>(this);
		mData.update_sink().remove_listener<DataModel, &DataModel::UpdateNotifications>(this);
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
		if (indexIter != mIndexMap.end() && indexIter != mIndexMap.end())
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
	void Notifications(nl::notifications notif, const typename Data::table_t& table, const size_t& row_added)
	{
		switch (notif)
		{
		case nl::notifications::add:
		{
			//DataViewItem starts from one, 0 is invalid so pair(id*:0x00...1, index:0)
			auto [iter, inserted] = mIndexMap.insert(std::make_pair(wxDataViewItem(UIntToPtr(row_added + 1)), row_added));
			if (inserted) {
				ItemAdded(wxDataViewItem(0), iter->first);
				mPastLastAdded = row_added + 1;
			}
			break;
		}
		case nl::notifications::add_multiple:
		{
			//assumes that the rows is added at the back of the table, maybe using relation::append_relation
			//undefined(lol) if not appened at the back 
			if (row_added > 0)
			{
				if (!mIndexMap.empty() || mPastLastAdded != -1)
				{
					wxDataViewItemArray DataViewArray(row_added);
					for (size_t i = 0; i < row_added; i++){
						size_t index = i + mPastLastAdded;
						auto [iter, inserted] = mIndexMap.insert(std::make_pair(wxDataViewItem(UIntToPtr(index + 1)), index));
						if (inserted){
							DataViewArray[i] = iter->first;
						}
					}
					if (!DataViewArray.empty()){
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
		}
	}
	void UpdateNotifications(nl::notifications notif, const typename Data::table_t& table, size_t column, const size_t& row_added)
	{
		auto iter = mIndexMap.find(wxDataViewItem(UIntToPtr(row_added + 1)));
		if (iter == mIndexMap.end()) return;
		switch (notif)
		{
		case nl::notifications::update:
			ValueChanged(iter->first, column);
			break;
		}
	}

//attributes
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


protected:
	std::unordered_map<wxDataViewItem, size_t> mIndexMap{};
	
	
	//each row can have an attribute type, 
	itemToAttr_t mAttributeTable{};
	Data& mData;

	//one past the last added
	std::int32_t mPastLastAdded;
};
