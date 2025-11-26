#include "io.hpp"

// Ensure the data directory is present for data storage.
int ensure_data_dir(WorldInfo* info, filesys::path& file_path) {
	if (info->rank != 0) {
		return 0;
	}

	try {
		// Gather the directory of the file path.
		filesys::path dir = file_path.parent_path();

		// Ensure the directory is present.
		if (filesys::create_directories(dir)) {
			std::cout << "Created directory: " << dir << std::endl;
		} else {
			std::cout << "Directory already exists: " << dir << std::endl;
		}
	} catch (const filesys::filesystem_error& err) {
		std::cerr << "[ERROR] Unable to ensure directory: " << err.path1() <<
			err.what() << std::endl;

		return 1;
	}

	return 0;
}
