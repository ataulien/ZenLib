#pragma once
#include <string.h>
#include <iostream>
#include <string>

namespace ZMath
{    
	static const float Pi = 3.14159265359f;


    constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1)
    {
      return exp < 1 ? result : ipow(base * base, exp / 2, (exp % 2) ? result * base : result);
    }

    struct t_float2
    {
        t_float2(){}
        t_float2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        union
        {
            struct
            {
                float x;
                float y;
            };

            float v[2];
        };

		std::string toString() const
		{
			std::string out;
			out = "[" + std::to_string(x)
				+ ", " + std::to_string(y) + "]";

			return out;
		}

		friend std::ostream& operator<< (std::ostream &out, t_float2 &v);
    };

    std::ostream& operator<< (std::ostream &out, t_float2 &v);

    struct t_float3
    {
        t_float3(){}
        t_float3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        union
        {
            struct
            {
                float x;
                float y;
                float z;
            };


            float v[3];
        };

		std::string toString() const
		{
			std::string out;
			out = "[" + std::to_string(x)
				+ ", " + std::to_string(y)
				+ ", " + std::to_string(z) + "]";

			return out;
		}

		friend std::ostream& operator<< (std::ostream &out, t_float3 &v);
    };

    std::ostream& operator<< (std::ostream &out, t_float3 &v);

    struct t_float4
    {
        t_float4(){}
		t_float4(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

        union
        {
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };

	    float v[4];

        };

		/**
		 * @brief Converts the given ABGR8-Color to float4
		 */
		void fromABGR8(uint32_t argb)
		{
			unsigned char a = argb >> 24;
			unsigned char b = (argb >> 16) & 0xFF;
			unsigned char g = (argb >> 8) & 0xFF;
			unsigned char r = argb & 0xFF;

			x = r / 255.0f;
			y = g / 255.0f;
			z = b / 255.0f;
			w = a / 255.0f;
		}

		/**
		* @brief Converts the stored color to ARGB8
		*/
		uint32_t toABGR8()
		{
			unsigned char b[] = { static_cast<unsigned char>(w * 255.0f), 
				static_cast<unsigned char>(z * 255.0f), 
				static_cast<unsigned char>(y * 255.0f),
				static_cast<unsigned char>(x * 255.0f)};

			return *reinterpret_cast<uint32_t*>(b);
		}

		std::string toString() const
		{
			std::string out;
			out = "[" + std::to_string(x)
				+ ", " + std::to_string(y)
				+ ", " + std::to_string(z)
				+ ", " + std::to_string(w) + "]";

			return out;
		}

		friend std::ostream& operator<< (std::ostream &out, t_float4 &v);
    };

    std::ostream& operator<< (std::ostream &out, t_float4 &v);

    template<typename T, typename... S>
    struct t_vector : public T
    {
        t_vector(S... x) : T(x...){}
        t_vector(){}

        t_vector(const T& v)
        {
            T::_glmt_vector = v._glmt_vector;
        }

        // Comparision operators
        bool operator == (const t_vector<T, S...>& v) const
        {
            return memcmp(T::v, v.v, sizeof(v.v)) == 0;
        }

        bool operator != (const t_vector<T, S...>& v) const
        {
            return !(*this == v);
        }

        t_vector<T,S...> operator *(float s) const
        {
            t_vector<T,S...> rs;

            for(size_t i=0;i<sizeof(T) / sizeof(float);i++) // Fixme: Doesn't work for double-vectors! Also, not very nice solution.
            {
                rs.v[i] = T::v[i] * s;
            }

            return rs;
        }

        t_vector<T, S...>& operator*= (float s) { *this = (*this) * s; return *this; }
        
    };

    typedef t_vector<t_float2, float, float> float2;
    typedef t_vector<t_float3, float, float, float> float3;
    typedef t_vector<t_float4, float, float, float, float> float4;

    //------------------------------------------------------------------------------
    // 4x4 Matrix (assumes right-handed cooordinates)
    struct Matrix
    {
        Matrix(){}

		Matrix(float* pm)
		{
			memcpy(m, pm, sizeof(m));
		}

        Matrix(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33)
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[0][2] = m02;
            m[0][3] = m03;

            m[1][0] = m10;
            m[1][1] = m11;
            m[1][2] = m12;
            m[1][3] = m13;

            m[2][0] = m20;
            m[2][1] = m21;
            m[2][2] = m22;
            m[2][3] = m23;

            m[3][0] = m30;
            m[3][1] = m31;
            m[3][2] = m32;
            m[3][3] = m33;
        }

        void Transpose()
        {
            Matrix mm = *this;

            m[0][0] = mm.m[0][0];
            m[0][1] = mm.m[1][0];
            m[0][2] = mm.m[2][0];
            m[0][3] = mm.m[3][0];
            m[1][0] = mm.m[0][1];
            m[1][1] = mm.m[1][1];
            m[1][2] = mm.m[2][1];
            m[1][3] = mm.m[3][1];
            m[2][0] = mm.m[0][2];
            m[2][1] = mm.m[1][2];
            m[2][2] = mm.m[2][2];
            m[2][3] = mm.m[3][2];
            m[3][0] = mm.m[0][3];
            m[3][1] = mm.m[1][3];
            m[3][2] = mm.m[2][3];
            m[3][3] = mm.m[3][3];
        }

        // Properties
        float3 Up() const { return float3( _21, _22, _23); }
        void Up( const float3& v ) { _21 = v.x; _22 = v.y; _23 = v.z; }

        float3 Down() const { return float3( -_21, -_22, -_23); }
        void Down( const float3& v ) { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

        float3 Right() const { return float3( _11, _12, _13 ); }
        void Right( const float3& v ) { _11 = v.x; _12 = v.y; _13 = v.z; }

        float3 Left() const { return float3( -_11, -_12, -_13 ); }
        void Left( const float3& v ) { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

        float3 Forward() const  { return float3( -_31, -_32, -_33 ); }
        void Forward( const float3& v ) { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

        float3 Backward() const { return float3( _31, _32, _33 ); }
        void Backward( const float3& v ) { _31 = v.x; _32 = v.y; _33 = v.z; }

        float3 Translation() const { return float3( _41, _42, _43 ); }
        float3 TranslationT() const { return float3( _14, _24, _34 ); }
        void Translation( const float3& v ) { _41 = v.x; _42 = v.y; _43 = v.z; }

        static Matrix CreateIdentity()
        {
                return Matrix(1,0,0,0,
                                       0,1,0,0,
                                       0,0,1,0,
                                       0,0,0,1);
        }
        
        union
        {
            struct
            {
                float _11, _12, _13, _14;
                float _21, _22, _23, _24;
                float _31, _32, _33, _34;
                float _41, _42, _43, _44;
            };
		float m[4][4];
		float4 v[4];
                float mv[16];
        };

		std::string toString()
		{
			std::string out;
			out = "[";
			for(size_t i = 0; i < 16; i++)
			{
				out += std::to_string(mv[i]);

				// Only add "," when not at the last value
				if(i != 15)
					out += ", ";
			}
			out += "]";

			return out;
		}

		friend std::ostream& operator<< (std::ostream &out, Matrix &v);
    };


    std::ostream& operator<< (std::ostream &out, Matrix &m);
}
