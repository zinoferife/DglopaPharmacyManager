#pragma once
#include <wx/dnd.h>
#include <type_traits>
#include "DataObject.h"

template<typename window_t, typename... Args>
class DropTarget : public wxDropTarget
{
public:
	typedef typename DataObject<Args...> DataObject_t;
	typedef typename DataObject<Args...>::row_t row_t;
	typedef window_t wxWindow_t;
	static_assert(std::is_base_of_v<wxWindow, window_t>, "win_t is not a valid wxWindow type");
	DropTarget(wxWindow_t* owner) : wxDropTarget(new DataObject<Args...>),
		mOwner(owner) {

	}

	virtual ~DropTarget() {
		mOwner = nullptr;
	}

	virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def) override
	{
		return OnDragOver(x, y, def);
	}

	virtual void OnLeave() override
	{
		//what to if leaving
	}

	virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) override {
		mPosition.x = x;
		mPosition.y = y;
		if (!GetData()) {
			//cannot drop data
			spdlog::get("log")->error("Cannot drop data, failed to get drag and drop data");
			return wxDragNone;
		}
		DataObject_t* ptr = (DataObject_t*)GetDataObject();
		if (ptr) {
			try {
				if (mOwner) {
					mOwner->DropData(ptr->GetValue());
				}
			}
			catch (std::exception& ex) {
				spdlog::get("log")->error("Drop error: {}",
					ex.what());
				return wxDragNone;
			}
			return def;
		}
		return wxDragNone;
	}
	const wxPoint& GetCoords() const { mPosition; }


	private:
		wxPoint mPosition;
		wxWindow_t* mOwner;

};

template<typename wxWindow_t, typename... Args>
auto CreateDropTarget(wxWindow_t* owner, const std::tuple<Args...>& tuple) -> DropTarget<wxWindow_t, Args...>*
{
	return new DropTarget<wxWindow_t, Args...>(owner);
}