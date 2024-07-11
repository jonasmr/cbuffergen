#pragma once

#ifdef __cplusplus
#define HLSL_ASSERT(expr) do{if(!(expr))__debugbreak();}while(0)

template<typename T, size_t LEN>
struct hlsl_vector_type;
template<typename T, size_t LEN, size_t ARRAY_SIZE>
struct hlsl_varray;
template<typename T, size_t LEN, size_t ARRAY_SIZE>
struct hlsl_varray_cb;
template<typename T, size_t LEN, size_t ROWS, size_t ARRAY_SIZE>
struct hlsl_marray;
template<typename T, size_t LEN, size_t ROWS, size_t ARRAY_SIZE>
struct hlsl_marray_cb;


template<typename T>
struct hlsl_vector_type<T, 1>
{
	union
	{
		T data[1];
		struct
		{
			T x;
		};
	};
	template<typename S>
	const hlsl_vector_type& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this));
		memcpy(&data[0], &other, sizeof(data));
		return *this;
	}
	template<typename S>
	S get() const
	{
		static_assert(sizeof(S) == sizeof(*this));
		return *(S*)&data[0];
	}
	T& operator[](int i)
	{
		static_assert(i < 1);
		return data[i];
	}
	const T& operator[](int i) const
	{
		static_assert(i < 4);
		return data[i];
	}

};
template<typename T>
struct hlsl_vector_type<T, 2>
{
	union
	{
		T data[2];
		struct
		{
			T x;
			T y;
		};
	};
	template<typename S>
	hlsl_vector_type& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this));
		memcpy(&data[0], &other, sizeof(data));
		return *this;
	}
	template<typename S>
	S get() const 
	{
		static_assert(sizeof(S) == sizeof(*this));
		return *(S*)&data[0];
	}
	T& operator[](int i)
	{
		static_assert(i < 2);
		return data[i];
	}
	const T& operator[](int i) const
	{
		static_assert(i < 2);
		return data[i];
	}

};

template<typename T>
struct hlsl_vector_type<T, 3>
{
	union
	{
		T data[3];
		struct
		{
			T x;
			T y;
			T z;
		};
	};
	template<typename S>
	hlsl_vector_type& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this));
		memcpy(&data[0], &other, sizeof(data));
		return *this;
	}
	template<typename S>
	S get() const
	{
		static_assert(sizeof(S) == sizeof(*this));
		return *(S*)&data[0];
	}
	T& operator[](int i)
	{
		static_assert(i < 3);
		return data[i];
	}
	const T& operator[](int i) const
	{
		static_assert(i < 3);
		return data[i];
	}
};

template<typename T>
struct hlsl_vector_type<T, 4>
{
	union
	{
		T data[4];
		struct
		{
			T x;
			T y;
			T z;
			T w;
		};
	};

	template<typename S>
	hlsl_vector_type& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this), "vector assign must match exactly");
		memcpy(&data[0], &other, sizeof(data));
		return *this;
	}
	template<typename S>
	S get() const 
	{
		static_assert(sizeof(S) == sizeof(*this));
		return *(S*)&data[0];
	}
	T& operator[](int i)
	{
		static_assert(i < 4);
		return data[i];
	}
	const T& operator[](int i) const
	{
		static_assert(i < 4);
		return data[i];
	}
};

template<typename T, size_t LEN, size_t ARRAY_SIZE>
struct hlsl_varray
{
	static_assert(sizeof(T) == sizeof(uint32), "uint16 and double not yet supported");
	static const int NUM_ELEMENTS = ARRAY_SIZE * LEN;

	T data[NUM_ELEMENTS];

	hlsl_vector_type<T, LEN>& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index*LEN];
		return *(hlsl_vector_type<T, LEN>*)ptr;
	}
	const hlsl_vector_type<T, LEN>& operator[](int index) const
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index*LEN];
		return *(hlsl_vector_type<T, LEN>*)ptr;
	}
	template<typename S>
	hlsl_varray& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this), "sizeof must match exactly. if you arrays of vectors with 1, 2 or 3 elements, you must assign one row a time");
		memcpy(&data[0], &other, sizeof(data));
		return *this;
	}

};


template<typename T, size_t LEN, size_t ARRAY_SIZE>
struct hlsl_varray_cb
{
	static_assert(sizeof(T) == sizeof(uint32), "uint16 and double not yet supported");
	static const int NUM_ELEMENTS = (ARRAY_SIZE-1) * 4 + LEN;
	typedef hlsl_vector_type<T, LEN> ELEMENT;

	T data[NUM_ELEMENTS];

	ELEMENT& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index*4];
		return *(ELEMENT*)ptr;
	}
	const ELEMENT& operator[](int index) const
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index*4];
		return *(ELEMENT*)ptr;
	}
	template<typename S>
	hlsl_varray_cb& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this), "sizeof must match exactly. if you arrays of vectors with 1, 2 or 3 elements, you must assign one row a time");
		memcpy(&data[0], &other, sizeof(data));
		return *this;
	}

};

template<typename T, size_t LEN, size_t ROWS, size_t ARRAY_SIZE>
struct hlsl_marray
{
	static_assert(sizeof(T) == sizeof(uint32), "uint16 and double not yet supported");
	static const int MAT_SIZE = LEN * ROWS;
	static const int NUM_ELEMENTS = MAT_SIZE * ARRAY_SIZE;
	typedef hlsl_varray<T, LEN, ROWS> ELEMENT;

	T data[NUM_ELEMENTS];

	ELEMENT& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index * MAT_SIZE];
		return *(ELEMENT*)ptr;
	}
	const ELEMENT& operator[](int index) const
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index * MAT_SIZE];
		return *(ELEMENT*)ptr;
	}
};

template<typename T, size_t LEN, size_t ROWS, size_t ARRAY_SIZE>
struct hlsl_marray_cb
{
	static_assert(sizeof(T) == sizeof(uint32), "uint16 and double not yet supported");
	static const int PADDED_MAT_SIZE = 4 * ROWS;
	static const int LAST_MAT_SIZE = (4 * (ROWS-1)) + LEN;
	static const int NUM_ELEMENTS = PADDED_MAT_SIZE * (ARRAY_SIZE-1) + LAST_MAT_SIZE;
	typedef hlsl_varray_cb<T, LEN, ROWS> ELEMENT;

	T data[NUM_ELEMENTS];

	ELEMENT& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index * PADDED_MAT_SIZE];
		return *(ELEMENT*)ptr;
	}
	const ELEMENT& operator[](int index) const
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index * PADDED_MAT_SIZE];
		return *(ELEMENT*)ptr;
	}
};




typedef int 		hlsl_bool;
typedef float 		hlsl_float;
typedef uint32 		hlsl_uint;
typedef int32  		hlsl_int;

typedef hlsl_vector_type<hlsl_float, 1> hlsl_float1;
typedef hlsl_vector_type<hlsl_float, 2> hlsl_float2;
typedef hlsl_vector_type<hlsl_float, 3> hlsl_float3;
typedef hlsl_vector_type<hlsl_float, 4> hlsl_float4;

typedef hlsl_vector_type<hlsl_uint, 1> hlsl_uint1;
typedef hlsl_vector_type<hlsl_uint, 2> hlsl_uint2;
typedef hlsl_vector_type<hlsl_uint, 3> hlsl_uint3;
typedef hlsl_vector_type<hlsl_uint, 4> hlsl_uint4;

typedef hlsl_vector_type<hlsl_int, 1> hlsl_int1;
typedef hlsl_vector_type<hlsl_int, 2> hlsl_int2;
typedef hlsl_vector_type<hlsl_int, 3> hlsl_int3;
typedef hlsl_vector_type<hlsl_int, 4> hlsl_int4;

typedef hlsl_vector_type<hlsl_int, 1> hlsl_bool1;
typedef hlsl_vector_type<hlsl_int, 2> hlsl_bool2;
typedef hlsl_vector_type<hlsl_int, 3> hlsl_bool3;
typedef hlsl_vector_type<hlsl_int, 4> hlsl_bool4;


typedef hlsl_varray_cb<hlsl_float, 1, 1> hlsl_float1x1_cb; 
typedef hlsl_varray_cb<hlsl_float, 1, 2> hlsl_float1x2_cb; 
typedef hlsl_varray_cb<hlsl_float, 1, 3> hlsl_float1x3_cb; 
typedef hlsl_varray_cb<hlsl_float, 1, 4> hlsl_float1x4_cb; 
typedef hlsl_varray_cb<hlsl_float, 2, 1> hlsl_float2x1_cb; 
typedef hlsl_varray_cb<hlsl_float, 2, 2> hlsl_float2x2_cb; 
typedef hlsl_varray_cb<hlsl_float, 2, 3> hlsl_float2x3_cb; 
typedef hlsl_varray_cb<hlsl_float, 2, 4> hlsl_float2x4_cb; 
typedef hlsl_varray_cb<hlsl_float, 3, 1> hlsl_float3x1_cb; 
typedef hlsl_varray_cb<hlsl_float, 3, 2> hlsl_float3x2_cb; 
typedef hlsl_varray_cb<hlsl_float, 3, 3> hlsl_float3x3_cb; 
typedef hlsl_varray_cb<hlsl_float, 3, 4> hlsl_float3x4_cb; 
typedef hlsl_varray_cb<hlsl_float, 4, 1> hlsl_float4x1_cb; 
typedef hlsl_varray_cb<hlsl_float, 4, 2> hlsl_float4x2_cb; 
typedef hlsl_varray_cb<hlsl_float, 4, 3> hlsl_float4x3_cb; 
typedef hlsl_varray_cb<hlsl_float, 4, 4> hlsl_float4x4_cb; 

typedef hlsl_varray<hlsl_float, 1, 1> hlsl_float1x1; 
typedef hlsl_varray<hlsl_float, 1, 2> hlsl_float1x2; 
typedef hlsl_varray<hlsl_float, 1, 3> hlsl_float1x3; 
typedef hlsl_varray<hlsl_float, 1, 4> hlsl_float1x4; 
typedef hlsl_varray<hlsl_float, 2, 1> hlsl_float2x1; 
typedef hlsl_varray<hlsl_float, 2, 2> hlsl_float2x2; 
typedef hlsl_varray<hlsl_float, 2, 3> hlsl_float2x3; 
typedef hlsl_varray<hlsl_float, 2, 4> hlsl_float2x4; 
typedef hlsl_varray<hlsl_float, 3, 1> hlsl_float3x1; 
typedef hlsl_varray<hlsl_float, 3, 2> hlsl_float3x2; 
typedef hlsl_varray<hlsl_float, 3, 3> hlsl_float3x3; 
typedef hlsl_varray<hlsl_float, 3, 4> hlsl_float3x4; 
typedef hlsl_varray<hlsl_float, 4, 1> hlsl_float4x1; 
typedef hlsl_varray<hlsl_float, 4, 2> hlsl_float4x2; 
typedef hlsl_varray<hlsl_float, 4, 3> hlsl_float4x3; 
typedef hlsl_varray<hlsl_float, 4, 4> hlsl_float4x4; 


typedef hlsl_varray_cb<hlsl_uint, 1, 1> hlsl_uint1x1_cb; 
typedef hlsl_varray_cb<hlsl_uint, 1, 2> hlsl_uint1x2_cb; 
typedef hlsl_varray_cb<hlsl_uint, 1, 3> hlsl_uint1x3_cb; 
typedef hlsl_varray_cb<hlsl_uint, 1, 4> hlsl_uint1x4_cb; 
typedef hlsl_varray_cb<hlsl_uint, 2, 1> hlsl_uint2x1_cb; 
typedef hlsl_varray_cb<hlsl_uint, 2, 2> hlsl_uint2x2_cb; 
typedef hlsl_varray_cb<hlsl_uint, 2, 3> hlsl_uint2x3_cb; 
typedef hlsl_varray_cb<hlsl_uint, 2, 4> hlsl_uint2x4_cb; 
typedef hlsl_varray_cb<hlsl_uint, 3, 1> hlsl_uint3x1_cb; 
typedef hlsl_varray_cb<hlsl_uint, 3, 2> hlsl_uint3x2_cb; 
typedef hlsl_varray_cb<hlsl_uint, 3, 3> hlsl_uint3x3_cb; 
typedef hlsl_varray_cb<hlsl_uint, 3, 4> hlsl_uint3x4_cb; 
typedef hlsl_varray_cb<hlsl_uint, 4, 1> hlsl_uint4x1_cb; 
typedef hlsl_varray_cb<hlsl_uint, 4, 2> hlsl_uint4x2_cb; 
typedef hlsl_varray_cb<hlsl_uint, 4, 3> hlsl_uint4x3_cb; 
typedef hlsl_varray_cb<hlsl_uint, 4, 4> hlsl_uint4x4_cb; 

typedef hlsl_varray<hlsl_uint, 1, 1> hlsl_uint1x1; 
typedef hlsl_varray<hlsl_uint, 1, 2> hlsl_uint1x2; 
typedef hlsl_varray<hlsl_uint, 1, 3> hlsl_uint1x3; 
typedef hlsl_varray<hlsl_uint, 1, 4> hlsl_uint1x4; 
typedef hlsl_varray<hlsl_uint, 2, 1> hlsl_uint2x1; 
typedef hlsl_varray<hlsl_uint, 2, 2> hlsl_uint2x2; 
typedef hlsl_varray<hlsl_uint, 2, 3> hlsl_uint2x3; 
typedef hlsl_varray<hlsl_uint, 2, 4> hlsl_uint2x4; 
typedef hlsl_varray<hlsl_uint, 3, 1> hlsl_uint3x1; 
typedef hlsl_varray<hlsl_uint, 3, 2> hlsl_uint3x2; 
typedef hlsl_varray<hlsl_uint, 3, 3> hlsl_uint3x3; 
typedef hlsl_varray<hlsl_uint, 3, 4> hlsl_uint3x4; 
typedef hlsl_varray<hlsl_uint, 4, 1> hlsl_uint4x1; 
typedef hlsl_varray<hlsl_uint, 4, 2> hlsl_uint4x2; 
typedef hlsl_varray<hlsl_uint, 4, 3> hlsl_uint4x3; 
typedef hlsl_varray<hlsl_uint, 4, 4> hlsl_uint4x4; 


typedef hlsl_varray_cb<hlsl_int, 1, 1> hlsl_int1x1_cb; 
typedef hlsl_varray_cb<hlsl_int, 1, 2> hlsl_int1x2_cb; 
typedef hlsl_varray_cb<hlsl_int, 1, 3> hlsl_int1x3_cb; 
typedef hlsl_varray_cb<hlsl_int, 1, 4> hlsl_int1x4_cb; 
typedef hlsl_varray_cb<hlsl_int, 2, 1> hlsl_int2x1_cb; 
typedef hlsl_varray_cb<hlsl_int, 2, 2> hlsl_int2x2_cb; 
typedef hlsl_varray_cb<hlsl_int, 2, 3> hlsl_int2x3_cb; 
typedef hlsl_varray_cb<hlsl_int, 2, 4> hlsl_int2x4_cb; 
typedef hlsl_varray_cb<hlsl_int, 3, 1> hlsl_int3x1_cb; 
typedef hlsl_varray_cb<hlsl_int, 3, 2> hlsl_int3x2_cb; 
typedef hlsl_varray_cb<hlsl_int, 3, 3> hlsl_int3x3_cb; 
typedef hlsl_varray_cb<hlsl_int, 3, 4> hlsl_int3x4_cb; 
typedef hlsl_varray_cb<hlsl_int, 4, 1> hlsl_int4x1_cb; 
typedef hlsl_varray_cb<hlsl_int, 4, 2> hlsl_int4x2_cb; 
typedef hlsl_varray_cb<hlsl_int, 4, 3> hlsl_int4x3_cb; 
typedef hlsl_varray_cb<hlsl_int, 4, 4> hlsl_int4x4_cb; 

typedef hlsl_varray<hlsl_int, 1, 1> hlsl_int1x1; 
typedef hlsl_varray<hlsl_int, 1, 2> hlsl_int1x2; 
typedef hlsl_varray<hlsl_int, 1, 3> hlsl_int1x3; 
typedef hlsl_varray<hlsl_int, 1, 4> hlsl_int1x4; 
typedef hlsl_varray<hlsl_int, 2, 1> hlsl_int2x1; 
typedef hlsl_varray<hlsl_int, 2, 2> hlsl_int2x2; 
typedef hlsl_varray<hlsl_int, 2, 3> hlsl_int2x3; 
typedef hlsl_varray<hlsl_int, 2, 4> hlsl_int2x4; 
typedef hlsl_varray<hlsl_int, 3, 1> hlsl_int3x1; 
typedef hlsl_varray<hlsl_int, 3, 2> hlsl_int3x2; 
typedef hlsl_varray<hlsl_int, 3, 3> hlsl_int3x3; 
typedef hlsl_varray<hlsl_int, 3, 4> hlsl_int3x4; 
typedef hlsl_varray<hlsl_int, 4, 1> hlsl_int4x1; 
typedef hlsl_varray<hlsl_int, 4, 2> hlsl_int4x2; 
typedef hlsl_varray<hlsl_int, 4, 3> hlsl_int4x3; 
typedef hlsl_varray<hlsl_int, 4, 4> hlsl_int4x4; 

typedef hlsl_varray_cb<hlsl_int, 1, 1> hlsl_bool1x1_cb; 
typedef hlsl_varray_cb<hlsl_int, 1, 2> hlsl_bool1x2_cb; 
typedef hlsl_varray_cb<hlsl_int, 1, 3> hlsl_bool1x3_cb; 
typedef hlsl_varray_cb<hlsl_int, 1, 4> hlsl_bool1x4_cb; 
typedef hlsl_varray_cb<hlsl_int, 2, 1> hlsl_bool2x1_cb; 
typedef hlsl_varray_cb<hlsl_int, 2, 2> hlsl_bool2x2_cb; 
typedef hlsl_varray_cb<hlsl_int, 2, 3> hlsl_bool2x3_cb; 
typedef hlsl_varray_cb<hlsl_int, 2, 4> hlsl_bool2x4_cb; 
typedef hlsl_varray_cb<hlsl_int, 3, 1> hlsl_bool3x1_cb; 
typedef hlsl_varray_cb<hlsl_int, 3, 2> hlsl_bool3x2_cb; 
typedef hlsl_varray_cb<hlsl_int, 3, 3> hlsl_bool3x3_cb; 
typedef hlsl_varray_cb<hlsl_int, 3, 4> hlsl_bool3x4_cb; 
typedef hlsl_varray_cb<hlsl_int, 4, 1> hlsl_bool4x1_cb; 
typedef hlsl_varray_cb<hlsl_int, 4, 2> hlsl_bool4x2_cb; 
typedef hlsl_varray_cb<hlsl_int, 4, 3> hlsl_bool4x3_cb; 
typedef hlsl_varray_cb<hlsl_int, 4, 4> hlsl_bool4x4_cb; 

typedef hlsl_varray<hlsl_int, 1, 1> hlsl_bool1x1; 
typedef hlsl_varray<hlsl_int, 1, 2> hlsl_bool1x2; 
typedef hlsl_varray<hlsl_int, 1, 3> hlsl_bool1x3; 
typedef hlsl_varray<hlsl_int, 1, 4> hlsl_bool1x4; 
typedef hlsl_varray<hlsl_int, 2, 1> hlsl_bool2x1; 
typedef hlsl_varray<hlsl_int, 2, 2> hlsl_bool2x2; 
typedef hlsl_varray<hlsl_int, 2, 3> hlsl_bool2x3; 
typedef hlsl_varray<hlsl_int, 2, 4> hlsl_bool2x4; 
typedef hlsl_varray<hlsl_int, 3, 1> hlsl_bool3x1; 
typedef hlsl_varray<hlsl_int, 3, 2> hlsl_bool3x2; 
typedef hlsl_varray<hlsl_int, 3, 3> hlsl_bool3x3; 
typedef hlsl_varray<hlsl_int, 3, 4> hlsl_bool3x4; 
typedef hlsl_varray<hlsl_int, 4, 1> hlsl_bool4x1; 
typedef hlsl_varray<hlsl_int, 4, 2> hlsl_bool4x2; 
typedef hlsl_varray<hlsl_int, 4, 3> hlsl_bool4x3; 
typedef hlsl_varray<hlsl_int, 4, 4> hlsl_bool4x4; 


#define hlsl_float1_cb_array(s) hlsl_varray_cb<hlsl_float, 1, s>
#define hlsl_float2_cb_array(s) hlsl_varray_cb<hlsl_float, 2, s>
#define hlsl_float3_cb_array(s) hlsl_varray_cb<hlsl_float, 3, s>
#define hlsl_float4_cb_array(s) hlsl_varray_cb<hlsl_float, 4, s>

#define hlsl_uint1_cb_array(s) hlsl_varray_cb<hlsl_uint, 1, s>
#define hlsl_uint2_cb_array(s) hlsl_varray_cb<hlsl_uint, 2, s>
#define hlsl_uint3_cb_array(s) hlsl_varray_cb<hlsl_uint, 3, s>
#define hlsl_uint4_cb_array(s) hlsl_varray_cb<hlsl_uint, 4, s>

#define hlsl_int1_cb_array(s) hlsl_varray_cb<hlsl_int, 1, s>
#define hlsl_int2_cb_array(s) hlsl_varray_cb<hlsl_int, 2, s>
#define hlsl_int3_cb_array(s) hlsl_varray_cb<hlsl_int, 3, s>
#define hlsl_int4_cb_array(s) hlsl_varray_cb<hlsl_int, 4, s>

#define hlsl_bool1_cb_array(s) hlsl_varray_cb<hlsl_bool, 1, s>
#define hlsl_bool2_cb_array(s) hlsl_varray_cb<hlsl_bool, 2, s>
#define hlsl_bool3_cb_array(s) hlsl_varray_cb<hlsl_bool, 3, s>
#define hlsl_bool4_cb_array(s) hlsl_varray_cb<hlsl_bool, 4, s>

#define hlsl_float1x1_cb_array(s) hlsl_marray_cb<hlsl_float, 1, 1, s>
#define hlsl_float1x2_cb_array(s) hlsl_marray_cb<hlsl_float, 1, 2, s>
#define hlsl_float1x3_cb_array(s) hlsl_marray_cb<hlsl_float, 1, 3, s>
#define hlsl_float1x4_cb_array(s) hlsl_marray_cb<hlsl_float, 1, 4, s>
#define hlsl_float2x1_cb_array(s) hlsl_marray_cb<hlsl_float, 2, 1, s>
#define hlsl_float2x2_cb_array(s) hlsl_marray_cb<hlsl_float, 2, 2, s>
#define hlsl_float2x3_cb_array(s) hlsl_marray_cb<hlsl_float, 2, 3, s>
#define hlsl_float2x4_cb_array(s) hlsl_marray_cb<hlsl_float, 2, 4, s>
#define hlsl_float3x1_cb_array(s) hlsl_marray_cb<hlsl_float, 3, 1, s>
#define hlsl_float3x2_cb_array(s) hlsl_marray_cb<hlsl_float, 3, 2, s>
#define hlsl_float3x3_cb_array(s) hlsl_marray_cb<hlsl_float, 3, 3, s>
#define hlsl_float3x4_cb_array(s) hlsl_marray_cb<hlsl_float, 3, 4, s>
#define hlsl_float4x1_cb_array(s) hlsl_marray_cb<hlsl_float, 4, 1, s>
#define hlsl_float4x2_cb_array(s) hlsl_marray_cb<hlsl_float, 4, 2, s>
#define hlsl_float4x3_cb_array(s) hlsl_marray_cb<hlsl_float, 4, 3, s>
#define hlsl_float4x4_cb_array(s) hlsl_marray_cb<hlsl_float, 4, 4, s>

#define hlsl_uint1x1_cb_array(s) hlsl_marray_cb<hlsl_uint, 1, 1, s>
#define hlsl_uint1x2_cb_array(s) hlsl_marray_cb<hlsl_uint, 1, 2, s>
#define hlsl_uint1x3_cb_array(s) hlsl_marray_cb<hlsl_uint, 1, 3, s>
#define hlsl_uint1x4_cb_array(s) hlsl_marray_cb<hlsl_uint, 1, 4, s>
#define hlsl_uint2x1_cb_array(s) hlsl_marray_cb<hlsl_uint, 2, 1, s>
#define hlsl_uint2x2_cb_array(s) hlsl_marray_cb<hlsl_uint, 2, 2, s>
#define hlsl_uint2x3_cb_array(s) hlsl_marray_cb<hlsl_uint, 2, 3, s>
#define hlsl_uint2x4_cb_array(s) hlsl_marray_cb<hlsl_uint, 2, 4, s>
#define hlsl_uint3x1_cb_array(s) hlsl_marray_cb<hlsl_uint, 3, 1, s>
#define hlsl_uint3x2_cb_array(s) hlsl_marray_cb<hlsl_uint, 3, 2, s>
#define hlsl_uint3x3_cb_array(s) hlsl_marray_cb<hlsl_uint, 3, 3, s>
#define hlsl_uint3x4_cb_array(s) hlsl_marray_cb<hlsl_uint, 3, 4, s>
#define hlsl_uint4x1_cb_array(s) hlsl_marray_cb<hlsl_uint, 4, 1, s>
#define hlsl_uint4x2_cb_array(s) hlsl_marray_cb<hlsl_uint, 4, 2, s>
#define hlsl_uint4x3_cb_array(s) hlsl_marray_cb<hlsl_uint, 4, 3, s>
#define hlsl_uint4x4_cb_array(s) hlsl_marray_cb<hlsl_uint, 4, 4, s>


#define hlsl_int1x1_cb_array(s) hlsl_marray_cb<hlsl_int, 1, 1, s>
#define hlsl_int1x2_cb_array(s) hlsl_marray_cb<hlsl_int, 1, 2, s>
#define hlsl_int1x3_cb_array(s) hlsl_marray_cb<hlsl_int, 1, 3, s>
#define hlsl_int1x4_cb_array(s) hlsl_marray_cb<hlsl_int, 1, 4, s>
#define hlsl_int2x1_cb_array(s) hlsl_marray_cb<hlsl_int, 2, 1, s>
#define hlsl_int2x2_cb_array(s) hlsl_marray_cb<hlsl_int, 2, 2, s>
#define hlsl_int2x3_cb_array(s) hlsl_marray_cb<hlsl_int, 2, 3, s>
#define hlsl_int2x4_cb_array(s) hlsl_marray_cb<hlsl_int, 2, 4, s>
#define hlsl_int3x1_cb_array(s) hlsl_marray_cb<hlsl_int, 3, 1, s>
#define hlsl_int3x2_cb_array(s) hlsl_marray_cb<hlsl_int, 3, 2, s>
#define hlsl_int3x3_cb_array(s) hlsl_marray_cb<hlsl_int, 3, 3, s>
#define hlsl_int3x4_cb_array(s) hlsl_marray_cb<hlsl_int, 3, 4, s>
#define hlsl_int4x1_cb_array(s) hlsl_marray_cb<hlsl_int, 4, 1, s>
#define hlsl_int4x2_cb_array(s) hlsl_marray_cb<hlsl_int, 4, 2, s>
#define hlsl_int4x3_cb_array(s) hlsl_marray_cb<hlsl_int, 4, 3, s>
#define hlsl_int4x4_cb_array(s) hlsl_marray_cb<hlsl_int, 4, 4, s>

#define hlsl_bool1x1_cb_array(s) hlsl_marray_cb<hlsl_bool, 1, 1, s>
#define hlsl_bool1x2_cb_array(s) hlsl_marray_cb<hlsl_bool, 1, 2, s>
#define hlsl_bool1x3_cb_array(s) hlsl_marray_cb<hlsl_bool, 1, 3, s>
#define hlsl_bool1x4_cb_array(s) hlsl_marray_cb<hlsl_bool, 1, 4, s>
#define hlsl_bool2x1_cb_array(s) hlsl_marray_cb<hlsl_bool, 2, 1, s>
#define hlsl_bool2x2_cb_array(s) hlsl_marray_cb<hlsl_bool, 2, 2, s>
#define hlsl_bool2x3_cb_array(s) hlsl_marray_cb<hlsl_bool, 2, 3, s>
#define hlsl_bool2x4_cb_array(s) hlsl_marray_cb<hlsl_bool, 2, 4, s>
#define hlsl_bool3x1_cb_array(s) hlsl_marray_cb<hlsl_bool, 3, 1, s>
#define hlsl_bool3x2_cb_array(s) hlsl_marray_cb<hlsl_bool, 3, 2, s>
#define hlsl_bool3x3_cb_array(s) hlsl_marray_cb<hlsl_bool, 3, 3, s>
#define hlsl_bool3x4_cb_array(s) hlsl_marray_cb<hlsl_bool, 3, 4, s>
#define hlsl_bool4x1_cb_array(s) hlsl_marray_cb<hlsl_bool, 4, 1, s>
#define hlsl_bool4x2_cb_array(s) hlsl_marray_cb<hlsl_bool, 4, 2, s>
#define hlsl_bool4x3_cb_array(s) hlsl_marray_cb<hlsl_bool, 4, 3, s>
#define hlsl_bool4x4_cb_array(s) hlsl_marray_cb<hlsl_bool, 4, 4, s>


#endif