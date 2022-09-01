#pragma once
#include <wx/dataobj.h>
#include <spdlog/spdlog.h>

#include <tuple>
#include <type_traits>


//represents a data object that is dragged from one table view to another
template<typename... Args>
class DataObject : public wxDataObject
{
public:
	typedef std::tuple<Args...> row_t;
	static constexpr size_t col_count = sizeof...(Args);

	//makes sure the types are copy constructable
	static_assert(std::conjunction_v<std::is_copy_constructible<Args>...>, "Argument types must be copyable");
	//holds a of the data
	//the data should 
	DataObject(const row_t& value = {}) : mValue(value) {
		mFormat.SetId("TableRow");
	}

	virtual ~DataObject() { }

	virtual size_t GetFormatCount(Direction dir) const override
	{
		//only supports the row_t format. 
		return 1;
	}

	virtual void GetAllFormats(wxDataFormat* formats, Direction dir) const override
	{
		formats[0] = mFormat;
	}

	virtual size_t GetDataSize(const wxDataFormat& format) const
	{
		//returns the row size
		return (format == mFormat) ? sizeof(row_t) : 0;
		
	}


	virtual wxDataFormat GetPreferredFormat(Direction WXUNUSED(dir)) const override
	{
		return mFormat;
	}

	virtual bool GetDataHere(const wxDataFormat& format, void* buffer) const override
	{
		if (buffer != NULL && format == mFormat) {
			//buffer is a pointer to sizeof(tuple_t) size
			//copy construct the data into ther 
			//placement is preventing deleting ??
			//causing memory leaks
			row_t* p = new (buffer) row_t(mValue);
			if (p) {
				//so that p isnt optimised away use p
				return true;
			}
		}
		return false;
	}

	virtual bool SetData(const wxDataFormat& format, size_t len, const void* buf) override
	{
		size_t size = sizeof(row_t);
		const char* bufc = (const char*)buf;
		if (buf != NULL && len == sizeof(row_t) && format == mFormat) {
			mValue = *((const row_t*)(buf));
			return true;
		}
		return false;
	}

	const row_t& GetValue() const { return mValue; }


private:
	//represents the value
	row_t mValue;
	wxDataFormat mFormat;

};

template<typename... Args>
auto CreateDataObject(const std::tuple<Args...>& t) -> DataObject<Args...>*
{
	return new DataObject<Args...>(t);
}

