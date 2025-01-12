#pragma once

#include "../Serial.h"

namespace meta
{
	//
	// Strings
	//

	template<> void serial_write(serial_writer* writer, const std::string& instance)
	{
		writer->write_length(instance.size());
		writer->write_string(instance.data(), instance.size());
	}

	template<> void serial_read(serial_reader* reader, std::string& instance)
	{
		instance.resize(reader->read_length());
		reader->read_string(instance.data(), instance.size());
	}

	//
	// Vectors
	//

	// g++ doesnt find these
	// clang does find these

	template<typename _t> void serial_write(serial_writer* writer, const std::vector<_t>& instance)
	{
		writer->write_length(instance.size());
		writer->write_array(meta::get_class<_t>(), (void*)instance.data(), instance.size());
	}

	template<typename _t> void serial_read(serial_reader* reader, std::vector<_t>& instance)
	{
		instance.resize(reader->read_length());
		reader->read_array(meta::get_class<_t>(), instance.data(), instance.size());
	}

	// typenames, todo: add all of the common ones

	template<> std::string type_name(tag<int>)   { return "int"; }
	template<> std::string type_name(tag<float>) { return "float"; }
	template<> std::string type_name(tag<std::string>) { return "std::string"; }

	template<typename _i>
	std::string type_name(tag<std::vector<_i>>)
	{
		std::stringstream ss;
		ss << "std::vector<" << type_name(tag<_i>{}) << ">"; 
		return ss.str();
	}
}