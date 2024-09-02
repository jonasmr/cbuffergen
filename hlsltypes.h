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
	static_assert(sizeof(T) == 2 ||sizeof(T) == 4 ||sizeof(T) == 8, "only elements of 2, 4 or 8 bytes supported");
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
	static_assert(sizeof(T) == 2 ||sizeof(T) == 4 ||sizeof(T) == 8, "only elements of 2, 4 or 8 bytes supported");
	typedef hlsl_vector_type<T, LEN> ELEMENT;

	static const size_t ELEMENT_SIZE = sizeof(ELEMENT);
	static const size_t ELEMENT_ARRAY_SIZE = 16 * ((ELEMENT_SIZE + 15) / 16); 
	static const int NUM_BYTES = (ARRAY_SIZE-1) * ELEMENT_ARRAY_SIZE + sizeof(T);

	//static_assert(sizeof(T) == sizeof(uint32), "uint16 and double not yet supported");
	//static const int NUM_ELEMENTS = (ARRAY_SIZE-1) * 4 + LEN;

	char data[NUM_BYTES];

	ELEMENT& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		ELEMENT* ptr =(ELEMENT*) &data[index * ELEMENT_ARRAY_SIZE];
		return *ptr;
	}
	const ELEMENT& operator[](int index) const
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		ELEMENT* ptr = (ELEMENT*)&data[index * ELEMENT_ARRAY_SIZE];
		return *ptr;
	}
	template<typename S>
	hlsl_varray_cb& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this), "sizeof must match exactly. if you arrays of vectors with 1, 2 or 3 elements, you must assign one row a time");
		memcpy(&data[0], &other, sizeof(data));
		return *this;
	}
};



//untested
template<typename T, size_t ARRAY_SIZE>
struct hlsl_any_array_cb
{
	static_assert(sizeof(T) == 2 ||sizeof(T) == 4 ||sizeof(T) == 8, "only elements of 2, 4 or 8 bytes supported");
	static const size_t ELEMENT_SIZE = sizeof(T);
	static const size_t ELEMENT_ARRAY_SIZE = 16 * ((ELEMENT_SIZE + 15) / 16); 
	static const int NUM_BYTES = (ARRAY_SIZE-1) * ELEMENT_ARRAY_SIZE + sizeof(T);

	char data[NUM_BYTES];

	ELEMENT& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		ELEMENT* ptr = (ELEMENT*)&data[index*ELEMENT_ARRAY_SIZE];
		return *ptr;
	}
	const ELEMENT& operator[](int index) const
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		ELEMENT* ptr = (ELEMENT*)&data[index*ELEMENT_ARRAY_SIZE];
		return *ptr;
	}
	template<typename S>
	hlsl_any_array_cb& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this), "sizeof must match exactly. ");
		memcpy(&data[0], &other, sizeof(data));
		return *this;
	}

};



template<typename T, size_t LEN, size_t ROWS, size_t ARRAY_SIZE>
struct hlsl_marray
{
	static_assert(sizeof(T) == 2 ||sizeof(T) == 4 ||sizeof(T) == 8, "only elements of 2, 4 or 8 bytes supported");
	static const int MAT_SIZE = LEN * ROWS;
	static const int NUM_ELEMENTS = MAT_SIZE * ARRAY_SIZE;
	typedef hlsl_varray<T, LEN, ROWS> ELEMENT;

	T data[NUM_ELEMENTS];

	ELEMENT& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = (ELEMENT*)&data[index * MAT_SIZE];
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
	static_assert(sizeof(T) == 2 ||sizeof(T) == 4 ||sizeof(T) == 8, "only elements of 2, 4 or 8 bytes supported");
	typedef hlsl_varray_cb<T, LEN, ROWS> ELEMENT;


	static const size_t ELEMENT_SIZE = ELEMENT::NUM_BYTES;
	static const size_t ELEMENT_ARRAY_SIZE = 16 * ((ELEMENT_SIZE + 15) / 16); 
	static const int NUM_BYTES = (ARRAY_SIZE-1) * ELEMENT_ARRAY_SIZE + sizeof(T);

	char data[NUM_ELEMENTS];

	ELEMENT& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		ELEMENT* ptr = (ELEMENT*)&data[index * ELEMENT_ARRAY_SIZE];
		return *ptr;
	}
	const ELEMENT& operator[](int index) const
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		ELEMENT* ptr = (ELEMENT*)&data[index * ELEMENT_ARRAY_SIZE];
		return *ptr;
	}
};




typedef int 		hlsl_bool;
typedef float 		hlsl_float;
typedef uint32 		hlsl_uint;
typedef int32  		hlsl_int;
typedef uint16  	hlsl_uint16_t;
typedef double  	hlsl_double;

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

typedef hlsl_vector_type<hlsl_uint16_t, 1> hlsl_uint16_t1;
typedef hlsl_vector_type<hlsl_uint16_t, 2> hlsl_uint16_t2;
typedef hlsl_vector_type<hlsl_uint16_t, 3> hlsl_uint16_t3;
typedef hlsl_vector_type<hlsl_uint16_t, 4> hlsl_uint16_t4;

typedef hlsl_vector_type<hlsl_double, 1> hlsl_double1;
typedef hlsl_vector_type<hlsl_double, 2> hlsl_double2;
typedef hlsl_vector_type<hlsl_double, 3> hlsl_double3;
typedef hlsl_vector_type<hlsl_double, 4> hlsl_double4;




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

typedef hlsl_varray_cb<hlsl_uint16_t, 1, 1> hlsl_uint16_t1x1_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 1, 2> hlsl_uint16_t1x2_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 1, 3> hlsl_uint16_t1x3_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 1, 4> hlsl_uint16_t1x4_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 2, 1> hlsl_uint16_t2x1_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 2, 2> hlsl_uint16_t2x2_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 2, 3> hlsl_uint16_t2x3_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 2, 4> hlsl_uint16_t2x4_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 3, 1> hlsl_uint16_t3x1_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 3, 2> hlsl_uint16_t3x2_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 3, 3> hlsl_uint16_t3x3_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 3, 4> hlsl_uint16_t3x4_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 4, 1> hlsl_uint16_t4x1_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 4, 2> hlsl_uint16_t4x2_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 4, 3> hlsl_uint16_t4x3_cb; 
typedef hlsl_varray_cb<hlsl_uint16_t, 4, 4> hlsl_uint16_t4x4_cb; 

typedef hlsl_varray<hlsl_double, 1, 1> hlsl_double1x1; 
typedef hlsl_varray<hlsl_double, 1, 2> hlsl_double1x2; 
typedef hlsl_varray<hlsl_double, 1, 3> hlsl_double1x3; 
typedef hlsl_varray<hlsl_double, 1, 4> hlsl_double1x4; 
typedef hlsl_varray<hlsl_double, 2, 1> hlsl_double2x1; 
typedef hlsl_varray<hlsl_double, 2, 2> hlsl_double2x2; 
typedef hlsl_varray<hlsl_double, 2, 3> hlsl_double2x3; 
typedef hlsl_varray<hlsl_double, 2, 4> hlsl_double2x4; 
typedef hlsl_varray<hlsl_double, 3, 1> hlsl_double3x1; 
typedef hlsl_varray<hlsl_double, 3, 2> hlsl_double3x2; 
typedef hlsl_varray<hlsl_double, 3, 3> hlsl_double3x3; 
typedef hlsl_varray<hlsl_double, 3, 4> hlsl_double3x4; 
typedef hlsl_varray<hlsl_double, 4, 1> hlsl_double4x1; 
typedef hlsl_varray<hlsl_double, 4, 2> hlsl_double4x2; 
typedef hlsl_varray<hlsl_double, 4, 3> hlsl_double4x3; 
typedef hlsl_varray<hlsl_double, 4, 4> hlsl_double4x4; 




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













#define HLSL_VERIFY_SIZE

#ifdef HLSL_VERIFY_SIZE

#define HLSL_ALIGN_16(s) ((((s)+15)/16)*16)

static_assert(sizeof(hlsl_uint16_t1x1) == sizeof(uint16) * 1 * 1, "");
static_assert(sizeof(hlsl_uint16_t2x1) == sizeof(uint16) * 1 * 2, "");
static_assert(sizeof(hlsl_uint16_t3x1) == sizeof(uint16) * 1 * 3, "");
static_assert(sizeof(hlsl_uint16_t4x1) == sizeof(uint16) * 1 * 4, "");
static_assert(sizeof(hlsl_uint16_t1x2) == sizeof(uint16) * 2 * 1, "");
static_assert(sizeof(hlsl_uint16_t2x2) == sizeof(uint16) * 2 * 2, "");
static_assert(sizeof(hlsl_uint16_t3x2) == sizeof(uint16) * 2 * 3, "");
static_assert(sizeof(hlsl_uint16_t4x2) == sizeof(uint16) * 2 * 4, "");
static_assert(sizeof(hlsl_uint16_t1x3) == sizeof(uint16) * 3 * 1, "");
static_assert(sizeof(hlsl_uint16_t2x3) == sizeof(uint16) * 3 * 2, "");
static_assert(sizeof(hlsl_uint16_t3x3) == sizeof(uint16) * 3 * 3, "");
static_assert(sizeof(hlsl_uint16_t4x3) == sizeof(uint16) * 3 * 4, "");
static_assert(sizeof(hlsl_uint16_t1x4) == sizeof(uint16) * 4 * 1, "");
static_assert(sizeof(hlsl_uint16_t2x4) == sizeof(uint16) * 4 * 2, "");
static_assert(sizeof(hlsl_uint16_t3x4) == sizeof(uint16) * 4 * 3, "");
static_assert(sizeof(hlsl_uint16_t4x4) == sizeof(uint16) * 4 * 4, "");

static_assert(sizeof(hlsl_uint16_t1x1_cb) == sizeof(uint16) * 1, "");
static_assert(sizeof(hlsl_uint16_t2x1_cb) == sizeof(uint16) * 2, "");
static_assert(sizeof(hlsl_uint16_t3x1_cb) == sizeof(uint16) * 3, "");
static_assert(sizeof(hlsl_uint16_t4x1_cb) == sizeof(uint16) * 4, "");
static_assert(sizeof(hlsl_uint16_t1x2_cb) == sizeof(uint16) * 1 + HLSL_ALIGN_16(sizeof(uint16_t)*1) * 1, "");
static_assert(sizeof(hlsl_uint16_t2x2_cb) == sizeof(uint16) * 2 + HLSL_ALIGN_16(sizeof(uint16_t)*2) * 1, "");
static_assert(sizeof(hlsl_uint16_t3x2_cb) == sizeof(uint16) * 3 + HLSL_ALIGN_16(sizeof(uint16_t)*3) * 1, "");
static_assert(sizeof(hlsl_uint16_t4x2_cb) == sizeof(uint16) * 4 + HLSL_ALIGN_16(sizeof(uint16_t)*4) * 1, "");
static_assert(sizeof(hlsl_uint16_t1x3_cb) == sizeof(uint16) * 1 + HLSL_ALIGN_16(sizeof(uint16_t)*1) * 2, "");
static_assert(sizeof(hlsl_uint16_t2x3_cb) == sizeof(uint16) * 2 + HLSL_ALIGN_16(sizeof(uint16_t)*2) * 2, "");
static_assert(sizeof(hlsl_uint16_t3x3_cb) == sizeof(uint16) * 3 + HLSL_ALIGN_16(sizeof(uint16_t)*3) * 2, "");
static_assert(sizeof(hlsl_uint16_t4x3_cb) == sizeof(uint16) * 4 + HLSL_ALIGN_16(sizeof(uint16_t)*4) * 2, "");
static_assert(sizeof(hlsl_uint16_t1x4_cb) == sizeof(uint16) * 1 + HLSL_ALIGN_16(sizeof(uint16_t)*1) * 3, "");
static_assert(sizeof(hlsl_uint16_t2x4_cb) == sizeof(uint16) * 2 + HLSL_ALIGN_16(sizeof(uint16_t)*2) * 3, "");
static_assert(sizeof(hlsl_uint16_t3x4_cb) == sizeof(uint16) * 3 + HLSL_ALIGN_16(sizeof(uint16_t)*3) * 3, "");
static_assert(sizeof(hlsl_uint16_t4x4_cb) == sizeof(uint16) * 4 + HLSL_ALIGN_16(sizeof(uint16_t)*4) * 3, "");


static_assert(sizeof(hlsl_double1x1) == sizeof(double) * 1 * 1, "");
static_assert(sizeof(hlsl_double2x1) == sizeof(double) * 1 * 2, "");
static_assert(sizeof(hlsl_double3x1) == sizeof(double) * 1 * 3, "");
static_assert(sizeof(hlsl_double4x1) == sizeof(double) * 1 * 4, "");
static_assert(sizeof(hlsl_double1x2) == sizeof(double) * 2 * 1, "");
static_assert(sizeof(hlsl_double2x2) == sizeof(double) * 2 * 2, "");
static_assert(sizeof(hlsl_double3x2) == sizeof(double) * 2 * 3, "");
static_assert(sizeof(hlsl_double4x2) == sizeof(double) * 2 * 4, "");
static_assert(sizeof(hlsl_double1x3) == sizeof(double) * 3 * 1, "");
static_assert(sizeof(hlsl_double2x3) == sizeof(double) * 3 * 2, "");
static_assert(sizeof(hlsl_double3x3) == sizeof(double) * 3 * 3, "");
static_assert(sizeof(hlsl_double4x3) == sizeof(double) * 3 * 4, "");
static_assert(sizeof(hlsl_double1x4) == sizeof(double) * 4 * 1, "");
static_assert(sizeof(hlsl_double2x4) == sizeof(double) * 4 * 2, "");
static_assert(sizeof(hlsl_double3x4) == sizeof(double) * 4 * 3, "");
static_assert(sizeof(hlsl_double4x4) == sizeof(double) * 4 * 4, "");

static_assert(sizeof(hlsl_double1x1_cb) == sizeof(double) * 1, "");
static_assert(sizeof(hlsl_double2x1_cb) == sizeof(double) * 2, "");
static_assert(sizeof(hlsl_double3x1_cb) == sizeof(double) * 3, "");
static_assert(sizeof(hlsl_double4x1_cb) == sizeof(double) * 4, "");
static_assert(sizeof(hlsl_double1x2_cb) == sizeof(double) * 1 + HLSL_ALIGN_16(sizeof(double)*1) * 1, "");
static_assert(sizeof(hlsl_double2x2_cb) == sizeof(double) * 2 + HLSL_ALIGN_16(sizeof(double)*2) * 1, "");
static_assert(sizeof(hlsl_double3x2_cb) == sizeof(double) * 3 + HLSL_ALIGN_16(sizeof(double)*3) * 1, "");
static_assert(sizeof(hlsl_double4x2_cb) == sizeof(double) * 4 + HLSL_ALIGN_16(sizeof(double)*4) * 1, "");
static_assert(sizeof(hlsl_double1x3_cb) == sizeof(double) * 1 + HLSL_ALIGN_16(sizeof(double)*1) * 2, "");
static_assert(sizeof(hlsl_double2x3_cb) == sizeof(double) * 2 + HLSL_ALIGN_16(sizeof(double)*2) * 2, "");
static_assert(sizeof(hlsl_double3x3_cb) == sizeof(double) * 3 + HLSL_ALIGN_16(sizeof(double)*3) * 2, "");
static_assert(sizeof(hlsl_double4x3_cb) == sizeof(double) * 4 + HLSL_ALIGN_16(sizeof(double)*4) * 2, "");
static_assert(sizeof(hlsl_double1x4_cb) == sizeof(double) * 1 + HLSL_ALIGN_16(sizeof(double)*1) * 3, "");
static_assert(sizeof(hlsl_double2x4_cb) == sizeof(double) * 2 + HLSL_ALIGN_16(sizeof(double)*2) * 3, "");
static_assert(sizeof(hlsl_double3x4_cb) == sizeof(double) * 3 + HLSL_ALIGN_16(sizeof(double)*3) * 3, "");
static_assert(sizeof(hlsl_double4x4_cb) == sizeof(double) * 4 + HLSL_ALIGN_16(sizeof(double)*4) * 3, "");



static_assert(sizeof(hlsl_float1x1) == sizeof(float) * 1 * 1, "");
static_assert(sizeof(hlsl_float2x1) == sizeof(float) * 1 * 2, "");
static_assert(sizeof(hlsl_float3x1) == sizeof(float) * 1 * 3, "");
static_assert(sizeof(hlsl_float4x1) == sizeof(float) * 1 * 4, "");
static_assert(sizeof(hlsl_float1x2) == sizeof(float) * 2 * 1, "");
static_assert(sizeof(hlsl_float2x2) == sizeof(float) * 2 * 2, "");
static_assert(sizeof(hlsl_float3x2) == sizeof(float) * 2 * 3, "");
static_assert(sizeof(hlsl_float4x2) == sizeof(float) * 2 * 4, "");
static_assert(sizeof(hlsl_float1x3) == sizeof(float) * 3 * 1, "");
static_assert(sizeof(hlsl_float2x3) == sizeof(float) * 3 * 2, "");
static_assert(sizeof(hlsl_float3x3) == sizeof(float) * 3 * 3, "");
static_assert(sizeof(hlsl_float4x3) == sizeof(float) * 3 * 4, "");
static_assert(sizeof(hlsl_float1x4) == sizeof(float) * 4 * 1, "");
static_assert(sizeof(hlsl_float2x4) == sizeof(float) * 4 * 2, "");
static_assert(sizeof(hlsl_float3x4) == sizeof(float) * 4 * 3, "");
static_assert(sizeof(hlsl_float4x4) == sizeof(float) * 4 * 4, "");

static_assert(sizeof(hlsl_float1x1_cb) == sizeof(float) * 1, "");
static_assert(sizeof(hlsl_float2x1_cb) == sizeof(float) * 2, "");
static_assert(sizeof(hlsl_float3x1_cb) == sizeof(float) * 3, "");
static_assert(sizeof(hlsl_float4x1_cb) == sizeof(float) * 4, "");
static_assert(sizeof(hlsl_float1x2_cb) == sizeof(float) * 1 + HLSL_ALIGN_16(sizeof(float)*1) * 1, "");
static_assert(sizeof(hlsl_float2x2_cb) == sizeof(float) * 2 + HLSL_ALIGN_16(sizeof(float)*2) * 1, "");
static_assert(sizeof(hlsl_float3x2_cb) == sizeof(float) * 3 + HLSL_ALIGN_16(sizeof(float)*3) * 1, "");
static_assert(sizeof(hlsl_float4x2_cb) == sizeof(float) * 4 + HLSL_ALIGN_16(sizeof(float)*4) * 1, "");
static_assert(sizeof(hlsl_float1x3_cb) == sizeof(float) * 1 + HLSL_ALIGN_16(sizeof(float)*1) * 2, "");
static_assert(sizeof(hlsl_float2x3_cb) == sizeof(float) * 2 + HLSL_ALIGN_16(sizeof(float)*2) * 2, "");
static_assert(sizeof(hlsl_float3x3_cb) == sizeof(float) * 3 + HLSL_ALIGN_16(sizeof(float)*3) * 2, "");
static_assert(sizeof(hlsl_float4x3_cb) == sizeof(float) * 4 + HLSL_ALIGN_16(sizeof(float)*4) * 2, "");
static_assert(sizeof(hlsl_float1x4_cb) == sizeof(float) * 1 + HLSL_ALIGN_16(sizeof(float)*1) * 3, "");
static_assert(sizeof(hlsl_float2x4_cb) == sizeof(float) * 2 + HLSL_ALIGN_16(sizeof(float)*2) * 3, "");
static_assert(sizeof(hlsl_float3x4_cb) == sizeof(float) * 3 + HLSL_ALIGN_16(sizeof(float)*3) * 3, "");
static_assert(sizeof(hlsl_float4x4_cb) == sizeof(float) * 4 + HLSL_ALIGN_16(sizeof(float)*4) * 3, "");




#endif

#endif