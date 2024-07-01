#pragma once

template<typename T, size_t LEN>
struct hlsl_vector_type<T, LEN>;


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
	const T& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this));
		memcpy(&data[0], &other, sizeof(data));
	}
	template<typename S>
	S get<S>()
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
	const T& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this));
		memcpy(&data[0], &other, sizeof(data));
	}
	template<typename S>
	S get<S>()
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
	const T& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this));
		memcpy(&data[0], &other, sizeof(data));
	}
	template<typename S>
	S get<S>()
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
	const T& operator = (const S& other)
	{
		static_assert(sizeof(S) == sizeof(*this));
		memcpy(&data[0], &other, sizeof(data));
	}
	template<typename S>
	S get<S>()
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
struct hlsl_array_cb<T, LEN, ARRAY_SIZE>
{
	static_assert(sizeof(T) == sizeof(uint32), "uint16 and double not yet supported");
	static const int NUM_ELEMENTS = (ARRAY_SIZE-1) * 4 + LEN;

	T data[NUM_ELEMENTS];

	hlsl_vector_type<T, LEN>& operator[](int index)
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index*4];
		return *(hlsl_vector_type<T, LEN>*)ptr;
	}
	const hlsl_vector_type<T, LEN>& operator[](int index) const
	{
		HLSL_ASSERT(index < ARRAY_SIZE);
		T* ptr = &data[index*4];
		return *(hlsl_vector_type<T, LEN>*)ptr;
	}
};



typedef int hlsl_bool;

typedef hlsl_vector_type<float, 1> hlsl_float;
typedef hlsl_vector_type<float, 1> hlsl_float1;
typedef hlsl_vector_type<float, 2> hlsl_float2;
typedef hlsl_vector_type<float, 3> hlsl_float3;
typedef hlsl_vector_type<float, 4> hlsl_float4;

typedef hlsl_vector_type<uint32, 1> hlsl_uint;
typedef hlsl_vector_type<uint32, 1> hlsl_uint1;
typedef hlsl_vector_type<uint32, 2> hlsl_uint2;
typedef hlsl_vector_type<uint32, 3> hlsl_uint3;
typedef hlsl_vector_type<uint32, 4> hlsl_uint4;

typedef hlsl_vector_type<int32, 1> hlsl_int;
typedef hlsl_vector_type<int32, 1> hlsl_int1;
typedef hlsl_vector_type<int32, 2> hlsl_int2;
typedef hlsl_vector_type<int32, 3> hlsl_int3;
typedef hlsl_vector_type<int32, 4> hlsl_int4;

typedef hlsl_vector_type<int32, 1> hlsl_bool;
typedef hlsl_vector_type<int32, 1> hlsl_bool1;
typedef hlsl_vector_type<int32, 2> hlsl_bool2;
typedef hlsl_vector_type<int32, 3> hlsl_bool3;
typedef hlsl_vector_type<int32, 4> hlsl_bool4;


typedef hlsl_array_cb<float, 1, 1> hlsl_float1x1_cb; 
typedef hlsl_array_cb<float, 1, 2> hlsl_float1x2_cb; 
typedef hlsl_array_cb<float, 1, 3> hlsl_float1x3_cb; 
typedef hlsl_array_cb<float, 1, 4> hlsl_float1x4_cb; 
typedef hlsl_array_cb<float, 2, 1> hlsl_float2x1_cb; 
typedef hlsl_array_cb<float, 2, 2> hlsl_float2x2_cb; 
typedef hlsl_array_cb<float, 2, 3> hlsl_float2x3_cb; 
typedef hlsl_array_cb<float, 2, 4> hlsl_float2x4_cb; 
typedef hlsl_array_cb<float, 3, 1> hlsl_float3x1_cb; 
typedef hlsl_array_cb<float, 3, 2> hlsl_float3x2_cb; 
typedef hlsl_array_cb<float, 3, 3> hlsl_float3x3_cb; 
typedef hlsl_array_cb<float, 3, 4> hlsl_float3x4_cb; 
typedef hlsl_array_cb<float, 4, 1> hlsl_float4x1_cb; 
typedef hlsl_array_cb<float, 4, 2> hlsl_float4x2_cb; 
typedef hlsl_array_cb<float, 4, 3> hlsl_float4x3_cb; 
typedef hlsl_array_cb<float, 4, 4> hlsl_float4x4_cb; 

typedef hlsl_array<float, 1, 1> hlsl_float1x1; 
typedef hlsl_array<float, 1, 2> hlsl_float1x2; 
typedef hlsl_array<float, 1, 3> hlsl_float1x3; 
typedef hlsl_array<float, 1, 4> hlsl_float1x4; 
typedef hlsl_array<float, 2, 1> hlsl_float2x1; 
typedef hlsl_array<float, 2, 2> hlsl_float2x2; 
typedef hlsl_array<float, 2, 3> hlsl_float2x3; 
typedef hlsl_array<float, 2, 4> hlsl_float2x4; 
typedef hlsl_array<float, 3, 1> hlsl_float3x1; 
typedef hlsl_array<float, 3, 2> hlsl_float3x2; 
typedef hlsl_array<float, 3, 3> hlsl_float3x3; 
typedef hlsl_array<float, 3, 4> hlsl_float3x4; 
typedef hlsl_array<float, 4, 1> hlsl_float4x1; 
typedef hlsl_array<float, 4, 2> hlsl_float4x2; 
typedef hlsl_array<float, 4, 3> hlsl_float4x3; 
typedef hlsl_array<float, 4, 4> hlsl_float4x4; 


typedef hlsl_array_cb<uint32, 1, 1> hlsl_uint1x1_cb; 
typedef hlsl_array_cb<uint32, 1, 2> hlsl_uint1x2_cb; 
typedef hlsl_array_cb<uint32, 1, 3> hlsl_uint1x3_cb; 
typedef hlsl_array_cb<uint32, 1, 4> hlsl_uint1x4_cb; 
typedef hlsl_array_cb<uint32, 2, 1> hlsl_uint2x1_cb; 
typedef hlsl_array_cb<uint32, 2, 2> hlsl_uint2x2_cb; 
typedef hlsl_array_cb<uint32, 2, 3> hlsl_uint2x3_cb; 
typedef hlsl_array_cb<uint32, 2, 4> hlsl_uint2x4_cb; 
typedef hlsl_array_cb<uint32, 3, 1> hlsl_uint3x1_cb; 
typedef hlsl_array_cb<uint32, 3, 2> hlsl_uint3x2_cb; 
typedef hlsl_array_cb<uint32, 3, 3> hlsl_uint3x3_cb; 
typedef hlsl_array_cb<uint32, 3, 4> hlsl_uint3x4_cb; 
typedef hlsl_array_cb<uint32, 4, 1> hlsl_uint4x1_cb; 
typedef hlsl_array_cb<uint32, 4, 2> hlsl_uint4x2_cb; 
typedef hlsl_array_cb<uint32, 4, 3> hlsl_uint4x3_cb; 
typedef hlsl_array_cb<uint32, 4, 4> hlsl_uint4x4_cb; 

typedef hlsl_array<uint32, 1, 1> hlsl_uint1x1; 
typedef hlsl_array<uint32, 1, 2> hlsl_uint1x2; 
typedef hlsl_array<uint32, 1, 3> hlsl_uint1x3; 
typedef hlsl_array<uint32, 1, 4> hlsl_uint1x4; 
typedef hlsl_array<uint32, 2, 1> hlsl_uint2x1; 
typedef hlsl_array<uint32, 2, 2> hlsl_uint2x2; 
typedef hlsl_array<uint32, 2, 3> hlsl_uint2x3; 
typedef hlsl_array<uint32, 2, 4> hlsl_uint2x4; 
typedef hlsl_array<uint32, 3, 1> hlsl_uint3x1; 
typedef hlsl_array<uint32, 3, 2> hlsl_uint3x2; 
typedef hlsl_array<uint32, 3, 3> hlsl_uint3x3; 
typedef hlsl_array<uint32, 3, 4> hlsl_uint3x4; 
typedef hlsl_array<uint32, 4, 1> hlsl_uint4x1; 
typedef hlsl_array<uint32, 4, 2> hlsl_uint4x2; 
typedef hlsl_array<uint32, 4, 3> hlsl_uint4x3; 
typedef hlsl_array<uint32, 4, 4> hlsl_uint4x4; 


typedef hlsl_array_cb<int32, 1, 1> hlsl_int1x1_cb; 
typedef hlsl_array_cb<int32, 1, 2> hlsl_int1x2_cb; 
typedef hlsl_array_cb<int32, 1, 3> hlsl_int1x3_cb; 
typedef hlsl_array_cb<int32, 1, 4> hlsl_int1x4_cb; 
typedef hlsl_array_cb<int32, 2, 1> hlsl_int2x1_cb; 
typedef hlsl_array_cb<int32, 2, 2> hlsl_int2x2_cb; 
typedef hlsl_array_cb<int32, 2, 3> hlsl_int2x3_cb; 
typedef hlsl_array_cb<int32, 2, 4> hlsl_int2x4_cb; 
typedef hlsl_array_cb<int32, 3, 1> hlsl_int3x1_cb; 
typedef hlsl_array_cb<int32, 3, 2> hlsl_int3x2_cb; 
typedef hlsl_array_cb<int32, 3, 3> hlsl_int3x3_cb; 
typedef hlsl_array_cb<int32, 3, 4> hlsl_int3x4_cb; 
typedef hlsl_array_cb<int32, 4, 1> hlsl_int4x1_cb; 
typedef hlsl_array_cb<int32, 4, 2> hlsl_int4x2_cb; 
typedef hlsl_array_cb<int32, 4, 3> hlsl_int4x3_cb; 
typedef hlsl_array_cb<int32, 4, 4> hlsl_int4x4_cb; 

typedef hlsl_array<int32, 1, 1> hlsl_int1x1; 
typedef hlsl_array<int32, 1, 2> hlsl_int1x2; 
typedef hlsl_array<int32, 1, 3> hlsl_int1x3; 
typedef hlsl_array<int32, 1, 4> hlsl_int1x4; 
typedef hlsl_array<int32, 2, 1> hlsl_int2x1; 
typedef hlsl_array<int32, 2, 2> hlsl_int2x2; 
typedef hlsl_array<int32, 2, 3> hlsl_int2x3; 
typedef hlsl_array<int32, 2, 4> hlsl_int2x4; 
typedef hlsl_array<int32, 3, 1> hlsl_int3x1; 
typedef hlsl_array<int32, 3, 2> hlsl_int3x2; 
typedef hlsl_array<int32, 3, 3> hlsl_int3x3; 
typedef hlsl_array<int32, 3, 4> hlsl_int3x4; 
typedef hlsl_array<int32, 4, 1> hlsl_int4x1; 
typedef hlsl_array<int32, 4, 2> hlsl_int4x2; 
typedef hlsl_array<int32, 4, 3> hlsl_int4x3; 
typedef hlsl_array<int32, 4, 4> hlsl_int4x4; 

typedef hlsl_array_cb<int32, 1, 1> hlsl_bool1x1_cb; 
typedef hlsl_array_cb<int32, 1, 2> hlsl_bool1x2_cb; 
typedef hlsl_array_cb<int32, 1, 3> hlsl_bool1x3_cb; 
typedef hlsl_array_cb<int32, 1, 4> hlsl_bool1x4_cb; 
typedef hlsl_array_cb<int32, 2, 1> hlsl_bool2x1_cb; 
typedef hlsl_array_cb<int32, 2, 2> hlsl_bool2x2_cb; 
typedef hlsl_array_cb<int32, 2, 3> hlsl_bool2x3_cb; 
typedef hlsl_array_cb<int32, 2, 4> hlsl_bool2x4_cb; 
typedef hlsl_array_cb<int32, 3, 1> hlsl_bool3x1_cb; 
typedef hlsl_array_cb<int32, 3, 2> hlsl_bool3x2_cb; 
typedef hlsl_array_cb<int32, 3, 3> hlsl_bool3x3_cb; 
typedef hlsl_array_cb<int32, 3, 4> hlsl_bool3x4_cb; 
typedef hlsl_array_cb<int32, 4, 1> hlsl_bool4x1_cb; 
typedef hlsl_array_cb<int32, 4, 2> hlsl_bool4x2_cb; 
typedef hlsl_array_cb<int32, 4, 3> hlsl_bool4x3_cb; 
typedef hlsl_array_cb<int32, 4, 4> hlsl_bool4x4_cb; 

typedef hlsl_array<int32, 1, 1> hlsl_bool1x1; 
typedef hlsl_array<int32, 1, 2> hlsl_bool1x2; 
typedef hlsl_array<int32, 1, 3> hlsl_bool1x3; 
typedef hlsl_array<int32, 1, 4> hlsl_bool1x4; 
typedef hlsl_array<int32, 2, 1> hlsl_bool2x1; 
typedef hlsl_array<int32, 2, 2> hlsl_bool2x2; 
typedef hlsl_array<int32, 2, 3> hlsl_bool2x3; 
typedef hlsl_array<int32, 2, 4> hlsl_bool2x4; 
typedef hlsl_array<int32, 3, 1> hlsl_bool3x1; 
typedef hlsl_array<int32, 3, 2> hlsl_bool3x2; 
typedef hlsl_array<int32, 3, 3> hlsl_bool3x3; 
typedef hlsl_array<int32, 3, 4> hlsl_bool3x4; 
typedef hlsl_array<int32, 4, 1> hlsl_bool4x1; 
typedef hlsl_array<int32, 4, 2> hlsl_bool4x2; 
typedef hlsl_array<int32, 4, 3> hlsl_bool4x3; 
typedef hlsl_array<int32, 4, 4> hlsl_bool4x4; 



