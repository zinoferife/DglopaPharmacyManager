#include "common.h"
#include "ArtProvider.h"

ArtProvider::ArtProvider()
{
	try {
		CreateArtStore();
	}catch (std::filesystem::filesystem_error& exception){
		spdlog::get("log")->critical("Critical filesystem error {}, {:d}", exception.what(), exception.code().value());
	}catch (...){
		spdlog::get("log")->critical("Critical error in ArtProvider");
	}
}

void ArtProvider::CreateArtStore()
{
	//use c++17 filesystem classses
	std::error_code ec;
	auto path = std::filesystem::current_path() / "asserts\\icon";
	if (std::filesystem::exists(path, ec))
	{
		for (auto const& dir_entry : std::filesystem::directory_iterator(path))
		{
			if (dir_entry.is_regular_file())
			{
				auto& d_path = dir_entry.path();
				//no extension, ignore
				if (!d_path.has_extension()) continue;

				std::string filename_ext = d_path.filename().string();
				size_t found_pos = filename_ext.find_first_of(".");
				if (found_pos != std::string::npos){
					std::string filename = filename_ext.substr(0, found_pos);
					std::string extension = d_path.extension().string();
					if (extension == ".jpg"){
						auto [iter, inserted] = mImageStore.insert(std::make_pair(filename, wxBitmap(d_path.string(), wxBITMAP_TYPE_JPEG)));
						//if not inserted, ignore, the only reason is duplicate entry, i hope lol
						if (inserted){
							if (!iter->second.IsOk()){
								spdlog::get("log")->error("Cannot create art image store, error loading from {}", filename);
							}
						}
					}
					else if (extension == ".ico"){
						auto [iter, inserted] = mImageStore.insert(std::make_pair(filename, wxBitmap(d_path.string(), wxBITMAP_TYPE_ICO)));
						//if not inserted, ignore, the only reason is duplicate entry, i hope lol
						if (inserted){
							if (!iter->second.IsOk()) {
								spdlog::get("log")->error("Cannot create art image store, error loading from {}", filename);
							}
						}
					}
				}
			}

		}
	}
	else
	{
		spdlog::get("log")->error("{}, {:d}", ec.message(), ec.value());
	}
}

wxBitmap ArtProvider::CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
	std::string Data = id.ToStdString();
	auto found = mImageStore.find(Data);
	if (found == mImageStore.end()){
		spdlog::get("log")->error("Invalid art id {}", Data);
		return wxNullBitmap;
	}
	wxBitmap& resource = found->second;
	return resource;
}
