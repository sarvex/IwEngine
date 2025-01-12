#include "iw/graphics/Loaders/ShaderLoader.h"
#include "iw/graphics/ComputeShader.h"
#include "iw/util/io/File.h"

namespace iw {
namespace Graphics {
	ShaderType TypeFromName(
		std::string& name);

	ShaderLoader::ShaderLoader(
		AssetManager& asset)
		: AssetLoader(asset)
	{}

	Shader* ShaderLoader::LoadAsset(
		std::string filepath)
	{
		std::string source = iw::ReadFile(filepath);

		Shader* shader = nullptr;

		// this loops forever if there is no new line at the end and you
		// try to include something as the last line!!!!
		// todo: append newline at end of source for a quick hack or fix algo
		
		size_t i = source.find("#shader");
		while (i < source.size()) {
			size_t start = source.find('\n', i) + 1;
			if (i != 0 && start == 0) {
				start = source.find('\0', i) + 1;
			}

			size_t end = source.find("#shader", start);

			size_t offset = i + 8;
			std::string name = source.substr(offset, start - offset - 1);
			
			i = end;

			ShaderType type = TypeFromName(name);

			std::string code;

			if(type == ShaderType::INVALID) {
				std::string include = name.substr(name.find_first_of(' ') + 1, name.find_first_of('\0') - name.find_first_of(' ') - 1);
				            name    = name.substr(0,                           name.find_first_of(' '));

				type = TypeFromName(name);

				if (type == ShaderType::INVALID) {
					LOG_WARNING << "\tInvalid shader type " << name << " from " << filepath << "@c" << offset;
					
					delete shader;
					return nullptr;
				}

				LOG_INFO << "\tGetting " << name << " shader source from " << include;
				code = iw::ReadFile(m_asset.RootPath() + include);
			}

			else {
				code = source.substr(start, end - start);
			}

			size_t j = code.find("#include");
			while (j < code.size()) {
				size_t start = code.find('\n', j) + 1;
				if (j != 0 && start == 0) {
					start = code.find('\0', j) + 1;
				}

				size_t offset = j + 9;
				std::string incldue = code.substr(offset, start - offset - 1);

				code.erase(j - 1, start - j);
				code.insert(j, iw::ReadFile(m_asset.RootPath() + incldue));

				j = code.find("#include", start);
				
				LOG_INFO << "\tGetting shader source from " << incldue;
			}

			// Only make this when we need to
			if (shader == nullptr) {
				if (type == ShaderType::COMPUTE) {
					shader = new ComputeShader();
				}

				else {
					shader = new Shader();
				}
			}

#ifdef IW_DEBUG
			std::string log = code;

			int lineNumber = 0;
			for (size_t currentIndex = 0;; currentIndex++) {
				std::stringstream ss;
				ss << ++lineNumber << "   ";

				log.insert(currentIndex, ss.str());
				currentIndex = log.find('\n', currentIndex + 1);

				if (currentIndex == std::string::npos) {
					break;
				}
			}

			LOG_INFO << log;
#endif

			shader->AddShader(type, code.c_str());
		}

		return shader;
	}

	ShaderType TypeFromName(
		std::string& name)
	{
		if (name == "Vertex") {
			return ShaderType::VERTEX;
		}

		else if (name == "Fragment") {
			return ShaderType::FRAGMENT;
		}

		else if (name == "Geometry") {
			return ShaderType::GEOMETRY;
		}

		else if (name == "Compute") {
			return ShaderType::COMPUTE;
		}

		return ShaderType::INVALID;
	}
}
}
