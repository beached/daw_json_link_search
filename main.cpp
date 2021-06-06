// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_search
//

#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>

#include <daw/daw_read_file.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <gmpxx.h>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

template<typename T>
struct data_obj_t {
	std::string type;
	std::ptrdiff_t p;
	std::ptrdiff_t alpha;
	std::ptrdiff_t r;
	std::ptrdiff_t j;
	std::ptrdiff_t M;
	std::ptrdiff_t N;
	std::ptrdiff_t stride;
	std::ptrdiff_t blockOffset;
	std::ptrdiff_t blockWidth;
	std::ptrdiff_t blockHeight;
	std::ptrdiff_t blockFill;
	std::ptrdiff_t bandOffset;
	std::ptrdiff_t bandWidth;
	bool negate;
	std::vector<T> data;
	std::vector<std::ptrdiff_t> rmin;
}; // data_obj_t

template<typename T>
struct MpqFromJSONConverter {
	T operator( )( std::string_view s ) const {
		auto v = mpq_class( static_cast<std::string>( s ) );
		if constexpr( std::is_same_v<T, mpq_class> ) {
			return v;
		} else {
			static_assert( std::is_floating_point_v<T> );
			return static_cast<T>( v.get_d( ) );
		}
	}
};

template<typename T>
struct MpqToJSONConverter {
	std::string operator( )( T const &v ) {
		if constexpr( std::is_same_v<T, mpq_class> ) {
			return v.get_str( );
		} else {
			static_assert( std::is_floating_point_v<T> );
			auto m = mpq_class( v );
			return m.get_str( );
		}
	}
};

namespace daw::json {
	template<typename T>
	struct json_data_contract<data_obj_t<T>> {
		using type = json_member_list<
		  json_string<"type">, json_number<"p", std::ptrdiff_t>,
		  json_number<"alpha", std::ptrdiff_t>, json_number<"r", std::ptrdiff_t>,
		  json_number<"j", std::ptrdiff_t>, json_number<"M", std::ptrdiff_t>,
		  json_number<"N", std::ptrdiff_t>, json_number<"stride", std::ptrdiff_t>,
		  json_number<"blockOffset", std::ptrdiff_t>,
		  json_number<"blockWidth", std::ptrdiff_t>,
		  json_number<"blockHeight", std::ptrdiff_t>,
		  json_number<"blockFill", std::ptrdiff_t>,
		  json_number<"bandOffset", std::ptrdiff_t>,
		  json_number<"bandWidth", std::ptrdiff_t>, json_bool<"negate">,
		  json_array<"data", json_custom<no_name, T, MpqFromJSONConverter<T>,
		                                 MpqToJSONConverter<T>>>,
		  json_array<"rmin", std::ptrdiff_t>>;

		static inline auto to_json_data( data_obj_t<T> const &value ) {
			return std::forward_as_tuple(
			  value.type, value.p, value.alpha, value.r, value.j, value.M, value.N,
			  value.stride, value.blockOffset, value.blockWidth, value.blockHeight,
			  value.blockFill, value.bandOffset, value.bandWidth, value.negate,
			  value.data, value.rmin );
		}
	};
} // namespace daw::json

int main( int argc, char **argv ) {
	if( argc < 1 ) {
		puts( "Must supply a filename to open\n" );
		exit( 1 );
	}

	auto const json_data1 = *daw::read_file( argv[1] );
	assert( json_data1.size( ) > 4 and "Minimum json data size is 4 '[{}]'" );

	using namespace daw::json;
	using namespace std::literals;

	// using unsigned here to optimize the parsing
	auto json_rng = json_array_range<json_delayed<no_name>>( json_data1 );

	auto pos =
	  std::find_if( json_rng.begin( ), json_rng.end( ), []( json_value jv ) {
		  using search_record_t = tuple_json_mapping<
		    json_string_raw<"type", std::string_view>,
		    json_number<"p", std::ptrdiff_t>, json_number<"alpha", std::ptrdiff_t>,
		    json_number<"r", std::ptrdiff_t>, json_number<"j", std::ptrdiff_t>>;

		  return std::tuple( "gamma2"s, 4, 1, 2, 1 ) ==
		         from_json<search_record_t>( jv ).members;
	  } );
	assert( pos != json_rng.end( ) );

	data_obj_t needle = from_json<data_obj_t<double>>( *pos );

	std::string out_str = to_json( needle );
	puts( out_str.c_str( ) );
}
